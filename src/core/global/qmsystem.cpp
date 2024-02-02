#include "qmsystem.h"

#include <QCoreApplication>
#include <QDirIterator>
#include <QProcess>
#include <QStandardPaths>

#ifdef Q_OS_WINDOWS
#  include <ShlObj.h>
#else
#  include <dlfcn.h>
#  include <limits.h>
#  include <unistd.h>
#endif

static const char Slash = '/';


#define Q_D_EXPLORE(str)                                                                           \
    QFileInfoList d;                                                                               \
    {                                                                                              \
        QDir dir;                                                                                  \
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);                          \
        dir.setPath(str);                                                                          \
        d = dir.entryInfoList();                                                                   \
    }

namespace QM {

    /*!
        \fn bool isPathRelative(const QString &path)

        Returns if the given path is a relative path.
    */

    /*!
        \fn bool isPathExist(const QString &path)

        Returns \c true if the given path is absolute and exists.
    */

    /*!
        \fn bool isFileExist(const QString &path)

        Returns \c true if the given path is absolute and is a file.
    */

    /*!
        \fn bool isDirExist(const QString &path)

        Returns \c true if the given path is absolute and is a directory.
    */

    /*!
        \fn bool isSamePath(const QString &path)

        Returns \c true if the canonical form of two given paths are identical.
    */

    /*!
        \fn QString PathFindSuffix(const QString &path)

        Get the suffix of the given path.
    */

    /*!
        \fn QString PathFindDirPath(const QString &path)

        Get the absolute directory path of the given path.
    */

    /*!
        Returns the file name if the path is not the root, otherwise returns the path itself.
    */
    QString PathFindFileName(const QString &path) {
        QFileInfo info(path);
        if (info.isRoot()) {
            return path;
        }
        return info.fileName();
    }

    /*!
        Get the next file or directory name after the given \c dir in the \c path string.
    */
    QString PathFindNextDir(const QString &path, const QString &dir) {
        if (!path.startsWith(dir)) {
            return "";
        }
        QString suffix = path.mid(dir.size());
        if (suffix.startsWith(Slash)) {
            suffix = suffix.mid(1);
        }
        int slashIndex = suffix.indexOf(Slash);
        if (slashIndex < 0) {
            return suffix;
        }
        return suffix.mid(0, slashIndex);
    }

    /*!
        Combine two files at binary level.
    */
    bool combine(const QString &fileName1, const QString &fileName2, const QString &newName) {
        QFile file1(fileName1);
        QFile file2(fileName2);
        QFile file3(newName);

        if (file1.open(QIODevice::ReadOnly) && file2.open(QIODevice::ReadOnly) &&
            file3.open(QIODevice::WriteOnly)) {
        } else {
            return false;
        }

        auto bytes1 = file1.readAll();
        file1.close();
        
        auto bytes2 = file2.readAll();
        file2.close();

        file3.write(bytes1);
        file3.write(bytes2);
        file3.close();

        return true;
    }

    /*!
        Reveal a file or directory in the system file manager.
    */
    void reveal(const QString &filename) {
        QFileInfo info(filename);
#if defined(Q_OS_WINDOWS)
        if (info.isFile()) {
            QStringList cmds;
            cmds << "/e,"
                 << "/select," << QDir::toNativeSeparators(filename);
            QProcess::startDetached("explorer.exe", cmds);
        } else if (info.isDir()) {
            QStringList cmds;
            cmds << "/e,"
                 << "/root," << QDir::toNativeSeparators(filename);
            QProcess::startDetached("explorer.exe", cmds);
        }
#elif defined(Q_OS_MAC)
        if (info.isDir()) {
            QString dirname = filename;
            if (!dirname.endsWith(Slash)) {
                dirname.append(Slash);
            }
            QProcess::startDetached("bash", {"-c", "open \'" + dirname + "\'"});
        } else if (info.isFile()) {
            QStringList scriptArgs;
            scriptArgs << QLatin1String("-e")
                       << QString::fromLatin1(
                              "tell application \"Finder\" to reveal POSIX file \"%1\"")
                              .arg(filename);
            QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
            scriptArgs.clear();
            scriptArgs << QLatin1String("-e")
                       << QLatin1String("tell application \"Finder\" to activate");
            QProcess::execute("/usr/bin/osascript", scriptArgs);
        }
#else
        if (info.isDir()) {
            QProcess::startDetached("bash", {"-c", "xdg-open \'" + filename + "\'"});
        } else if (info.isFile()) {
            QString arg = info.absolutePath();
            QProcess::startDetached("bash", {"-c", "xdg-open \'" + arg + "\'"});
        }
#endif
    }

    /*!
        Removes all files whose name is prefixed with the given string in a directory.
    */
    int rmPreStr(const QString &dirname, const QString &prefix) {
        if (!isDirExist(dirname)) {
            return 0;
        }

        Q_D_EXPLORE(dirname)

        int cnt = 0;
        QFileInfo cur;
        for (auto it = d.rbegin(); it != d.rend(); ++it) {
            cur = *it;
            if (prefix.isEmpty() || cur.fileName().startsWith(prefix)) {
                QFile file(cur.filePath());
                if (file.remove()) {
                    cnt++;
                }
            }
        }
        return cnt;
    }

    /*!
        Removes all files whose name is strictly prefixed with the given number in a directory.
    */
    int rmPreNum(const QString &dirname, int prefix) {
        if (!isDirExist(dirname)) {
            return 0;
        }

        Q_D_EXPLORE(dirname)

        int cnt = 0;
        for (auto it = d.rbegin(); it != d.rend(); ++it) {
            const auto &cur = *it;
            QString num = QString::number(prefix);
            QString filename = cur.fileName();
            if (filename.startsWith(num) &&
                (filename.size() == num.size() || !filename.at(num.size()).isNumber())) {
                QFile file(cur.filePath());
                if (file.remove()) {
                    cnt++;
                }
            }
        }

        return cnt;
    }

    /*!
        Removes the path separators at the end of the string.
    */
    QString removeTailSlashes(const QString &dirname) {
        QString path = dirname;
        while (!path.isEmpty() && (path.endsWith('/') || path.endsWith('\\'))) {
            path.chop(1);
        }
        return path;
    }

    /*!
        Returns the standard AppData location.

        \li On Windows, returns <tt>\%UserProfile\%/AppData</tt>
        \li On Mac/Linux, returns <tt>\%HOME\%/.config</tt>
    */
    QString appDataPath() {
        static const auto path = []() {
            QString path;
            QString slashName;
#ifdef Q_OS_WINDOWS
            path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#elif defined(Q_OS_MAC)
            path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config";
#else
            path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#endif
            slashName = Slash + qApp->applicationName();
            if (path.endsWith(slashName)) {
                path.chop(slashName.size());
            }
            slashName = Slash + qApp->organizationName();
            if (path.endsWith(slashName)) {
                path.chop(slashName.size());
            }
            return path;
        }();
        return path;
    }


    /*!
        \fn int unitDpi()

        Returns the system unit dpi value.

        \li on Mac, returns 72
        \li on Windows/Linux, returns 96
    */

    /*!
        Returns \c true if running with Administrator/Root privilege.
    */
    bool isUserRoot() {
#ifdef Q_OS_WINDOWS
        return ::IsUserAnAdmin();
#else
        return geteuid() == 0;
#endif
    }

}