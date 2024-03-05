#include "qmcoreappextension.h"
#include "qmcoreappextension_p.h"

#include <QCoreApplication>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageLogger>
#include <QSettings>

#include "qmsystem.h"

#include "qmconf.h"

#ifdef Q_OS_WINDOWS
#  include <Windows.h>
#elif defined(Q_OS_MACOS)
#  include <CoreFoundation/CoreFoundation.h>
#endif

Q_LOGGING_CATEGORY(qAppExtLog, "qtmediate")

#ifdef Q_OS_MAC
#  define QT_CONFIG_FILE_DIR  appUpperDir() + "/Resources"
#  define QT_CONFIG_BASE_DIR  appUpperDir()
#  define DEFAULT_LIBRARY_DIR "Frameworks"
#  define DEFAULT_SHARE_DIR   "Resources"
#else
#  define QT_CONFIG_FILE_DIR  QCoreApplication::applicationDirPath()
#  define QT_CONFIG_BASE_DIR  QCoreApplication::applicationDirPath()
#  define DEFAULT_LIBRARY_DIR "lib"
#  define DEFAULT_SHARE_DIR   "share"
#endif

static QMCoreAppExtension *m_instance = nullptr;

static QString appUpperDir() {
    static QString dir = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/..");
    return dir;
}

QMCoreAppExtensionPrivate::QMCoreAppExtensionPrivate() {
    isAboutToQuit = false;
}

QMCoreAppExtensionPrivate::~QMCoreAppExtensionPrivate() {
}

void QMCoreAppExtensionPrivate::init() {
    Q_Q(QMCoreAppExtension);

    // Basic directories
    appDataDir = QM::appDataPath() + "/" + qApp->organizationName() + "/" + qApp->applicationName();
    tempDir = QDir::tempPath() + "/" + qApp->organizationName() + "/" + qApp->applicationName();

    libDir = appUpperDir() + "/" + DEFAULT_LIBRARY_DIR;
    shareDir = appUpperDir() + "/" + DEFAULT_SHARE_DIR;

    configVars.addHash(QMSimpleVarExp::systemValues());
    configVars.add("DEFAULT_APPDATA", appDataDir);
    configVars.add("DEFAULT_TEMP", tempDir);

    // Read configurations
    if (readConfiguration(QMCoreAppExtension::configurationPath(QSettings::SystemScope))) {
        qCDebug(qAppExtLog) << "system configuration file found";
    } else {
        qCDebug(qAppExtLog) << "system configuration file not found";
    }

    if (readConfiguration(QMCoreAppExtension::configurationPath(QSettings::UserScope))) {
        qCDebug(qAppExtLog) << "user configuration file found";
    } else {
        qCDebug(qAppExtLog) << "user configuration file not found";
    }

    // Create instances
    s_dec = createDecorator(q);
    qCDebug(qAppExtLog) << s_dec->metaObject()->className() << "initializing.";

    QObject::connect(qApp, &QCoreApplication::aboutToQuit, this,
                     &QMCoreAppExtensionPrivate::_q_applicationAboutToQuit);

    // Add plugin paths
    for (const auto &path : qAsConst(pluginPaths))
        QCoreApplication::addLibraryPath(path);

    // Add translation paths
    for (const auto &path : qAsConst(translationPaths))
        s_dec->addTranslationPath(path);

    // Set default app share dir and app plugins dir
    appShareDir = shareDir
#ifndef Q_OS_MAC
                  + "/" + qApp->applicationName()
#endif
        ;

    appPluginsDir =
#ifdef Q_OS_MAC
        appUpperDir() + "/Plugins"
#else
        libDir + "/" + QCoreApplication::applicationName() + "/plugins"
#endif
        ;
}

bool QMCoreAppExtensionPrivate::readConfiguration(const QString &fileName) {
    // Read
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    auto data = file.readAll();
    file.close();

    // Parse
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    auto obj = doc.object();

    QJsonValue value;

    value = obj.value("AppFont");
    if (!value.isUndefined()) {
        if (value.isString()) {
            appFont.insert("Family", value.toString());
        } else if (value.isObject()) {
            appFont = value.toObject();
        }
    }

    QString prefix = QMCoreAppExtension::configurationBasePrefix();
    value = obj.value("Prefix");
    if (value.isString()) {
        auto dir = configVars.parse(value.toString());
        if (QM::isPathRelative(dir)) {
            dir = QT_CONFIG_BASE_DIR + "/" + dir;
        }

        QFileInfo info(dir);
        if (info.isDir()) {
            prefix = info.canonicalFilePath();
        }
    }

    auto getDir = [prefix, this](QString path) {
        path = configVars.parse(path);
        if (QM::isPathRelative(path)) {
            path = prefix + "/" + path;
        }
        QFileInfo info(path);
        if (!info.isDir())
            return QString();
        return info.canonicalFilePath();
    };

    auto getDirs = [getDir](QStringList &paths, const QJsonValue &value) {
        switch (value.type()) {
            case QJsonValue::String: {
                auto dir = getDir(value.toString());
                if (!dir.isEmpty()) {
                    paths << dir;
                }
                break;
            }
            case QJsonValue::Array: {
                for (const auto &item : value.toArray()) {
                    if (!item.isString())
                        continue;

                    auto dir = getDir(item.toString());
                    if (!dir.isEmpty()) {
                        paths << dir;
                    }
                }
                break;
            }
            default:
                break;
        }
    };

    value = obj.value("Temp");
    if (value.isString()) {
        auto dir = getDir(value.toString());
        if (!dir.isEmpty()) {
            tempDir = dir;
        }
    }

    value = obj.value("Libraries");
    if (value.isString()) {
        auto dir = getDir(value.toString());
        if (!dir.isEmpty()) {
            libDir = dir;
        }
    }

    value = obj.value("Share");
    if (value.isString()) {
        auto dir = getDir(value.toString());
        if (!dir.isEmpty()) {
            shareDir = dir;
        }
    }

    getDirs(pluginPaths, obj.value("Plugins"));
    getDirs(translationPaths, obj.value("Translations"));
    getDirs(themePaths, obj.value("Themes"));
    getDirs(fontPaths, obj.value("Fonts"));

    return true;
}

QMCoreDecoratorV2 *QMCoreAppExtensionPrivate::createDecorator(QObject *parent) {
    return new QMCoreDecoratorV2(parent);
}

#if defined(Q_OS_WINDOWS) || defined(Q_OS_MAC)

void QMCoreAppExtensionPrivate::osMessageBox_helper(void *winHandle,
                                                    QMCoreAppExtension::MessageBoxFlag flag,
                                                    const QString &title,
                                                    const QString &text) const {
#  ifdef Q_OS_WINDOWS
    int winFlag;
    switch (flag) {
        case QMCoreAppExtension::NoIcon:
            winFlag = MB_OK;
            break;
        case QMCoreAppExtension::Critical:
            winFlag = MB_ICONERROR;
            break;
        case QMCoreAppExtension::Warning:
            winFlag = MB_ICONWARNING;
            break;
        case QMCoreAppExtension::Question:
            winFlag = MB_ICONQUESTION;
            break;
        case QMCoreAppExtension::Information:
            winFlag = MB_ICONINFORMATION;
            break;
    };

    ::MessageBoxW(static_cast<HWND>(winHandle), text.toStdWString().data(),
                  title.toStdWString().data(),
                  MB_OK
#    ifdef QTMEDIATE_WIN32_MSGBOX_TOPMOST
                      | MB_TOPMOST
#    endif
                      | MB_SETFOREGROUND | winFlag);
#  else
    // https://web.archive.org/web/20111127025605/http://jorgearimany.blogspot.com/2010/05/messagebox-from-windows-to-mac.html
    CFOptionFlags result;
    int level = 0;
    switch (flag) {
        case QMCoreAppExtension::Critical:
            level = 2;
            break;
        case QMCoreAppExtension::Warning:
            level = 1;
            break;
        case QMCoreAppExtension::Question:
            level = 3;
            break;
        default:
            level = 0;
            break;
    };
    CFUserNotificationDisplayAlert(
        0,                  // no timeout
        level,              // change it depending message_type flags ( MB_ICONASTERISK.... etc.)
        NULL,               // icon url, use default, you can change it depending message_type flags
        NULL,               // not used
        NULL,               // localization of strings
        title.toCFString(), // header text
        text.toCFString(),  // message text
        NULL,               // default "ok" text in button
        NULL,               // alternate button title
        NULL,               // other button title, null--> no other button
        &result             // response flags
    );
#  endif
}

#endif

void QMCoreAppExtensionPrivate::_q_applicationAboutToQuit() {
    isAboutToQuit = true;
}

/*!
    \class QMCoreAppExtension

    The QMCoreAppExtension class is the global resources manager for \c QtMediate framework in
    a console application.
*/

/*!
    Constructor.
*/
QMCoreAppExtension::QMCoreAppExtension(QObject *parent)
    : QMCoreAppExtension(*new QMCoreAppExtensionPrivate(), parent) {
}

/*!
    Destructor.
*/
QMCoreAppExtension::~QMCoreAppExtension() {
    m_instance = nullptr;
}

/*!
    Returns a pointer to the application's QMCoreAppExtension instance.
*/
QMCoreAppExtension *QMCoreAppExtension::instance() {
    return m_instance;
}

/*!
    \enum QMCoreAppExtension::MessageBoxFlag
    \brief Message level enumeration.

    \var QMCoreAppExtension::NoIcon
    \brief Normal level.

    \var QMCoreAppExtension::Information
    \brief Information level.

    \var QMCoreAppExtension::Question
    \brief Question level.

    \var QMCoreAppExtension::Warning
    \brief Warning level.

    \var QMCoreAppExtension::Critical
    \brief Error level.
*/

/*!
    Shows system message box if supported.
*/
void QMCoreAppExtension::showMessage(QObject *parent, MessageBoxFlag flag, const QString &title,
                                     const QString &text) const {
    Q_UNUSED(parent);

#if defined(Q_OS_WINDOWS) || defined(Q_OS_MAC)
    Q_D(const QMCoreAppExtension);
    d->osMessageBox_helper(nullptr, flag, title, text);
#else
    switch (flag) {
        case Critical:
        case Warning:
            fputs(qPrintable(text), stderr);
            break;
        default:
            fputs(qPrintable(text), stdout);
            break;
    };
#endif
}

/*!
    Returns \c true if QCoreApplication::aboutToQuit() has emitted, otherwise returns false.
*/
bool QMCoreAppExtension::isAboutToQuit() const {
    Q_D(const QMCoreAppExtension);
    return d->isAboutToQuit;
}

/*!
    Returns application data directory.

    \li On Mac/Linux, the default path is <tt>\%HOME\%/.config/\%ORG\%/\%AppName\%</tt>
    \li On Windows, the default path is <tt>\%UserProfile\%/AppData/Local/\%ORG\%/\%AppName\%</tt>
 */
QString QMCoreAppExtension::appDataDir() const {
    Q_D(const QMCoreAppExtension);
    return d->appDataDir;
}

/*!
    Sets the application data directory.
*/
void QMCoreAppExtension::setAppDataDir(const QString &dir) {
    Q_D(QMCoreAppExtension);
    d->appDataDir = dir;
}

/*!
    Returns the application temporary directory.

    \li On Mac/Linux, the default path is <tt>\%TMPDIR\%</tt>
    \li On Windows, the default path is <tt>\%TEMP\%</tt>
*/
QString QMCoreAppExtension::tempDir() const {
    Q_D(const QMCoreAppExtension);
    return d->tempDir;
}

/*!
    Sets the application temporary directory.
*/
void QMCoreAppExtension::setTempDir(const QString &dir) {
    Q_D(QMCoreAppExtension);
    d->tempDir = dir;
}

/*!
    Returns the library directory.

    \li On Mac/Linux, the default path is <tt>\%AppPath\%/../Frameworks</tt>
    \li On Windows, the default path is <tt>\%AppPath\%/../lib</tt>
*/
QString QMCoreAppExtension::libDir() const {
    Q_D(const QMCoreAppExtension);
    return d->libDir;
}

/*!
    Sets the library directory.
*/
void QMCoreAppExtension::setLibDir(const QString &dir) {
    Q_D(QMCoreAppExtension);
    d->libDir = dir;
}

/*!
    Returns the share directory.

    \li On Mac, the default path is <tt>\%AppPath\%/../Resources</tt>
    \li On Windows/Linux, the default path is <tt>\%AppPath\%/../share</tt>
*/
QString QMCoreAppExtension::shareDir() const {
    Q_D(const QMCoreAppExtension);
    return d->shareDir;
}

/*!
    Sets the share directory.
*/
void QMCoreAppExtension::setShareDir(const QString &dir) {
    Q_D(QMCoreAppExtension);
    d->shareDir = dir;
}

/*!
    Returns the application's share directory.

    \li On Mac, the default path is <tt>\%ShareDir\%</tt>
    \li On Windows/Linux, the default path is <tt>\%ShareDir\%/\%AppName\%</tt>
*/
QString QMCoreAppExtension::appShareDir() const {
    Q_D(const QMCoreAppExtension);
    return d->appShareDir;
}

/*!
    Sets the application's share directory.
*/
void QMCoreAppExtension::setAppShareDir(const QString &dir) {
    Q_D(QMCoreAppExtension);
    d->appShareDir = dir;
}

/*!
    Returns the application's plugins directory.

    \li On Mac, the default path is <tt>\%AppPath\%/../Plugins</tt>
    \li On Windows/Linux, the default path is <tt>\%LibDir\%/\%AppName\%/plugins</tt>
*/
QString QMCoreAppExtension::appPluginsDir() const {
    Q_D(const QMCoreAppExtension);
    return d->appPluginsDir;
}

/*!
    Sets the application's plugins directory.
*/
void QMCoreAppExtension::setAppPluginsDir(const QString &dir) {
    Q_D(QMCoreAppExtension);
    d->appPluginsDir = dir;
}

/*!
    Creates data and temp directories for further use, returns true if success.
*/
bool QMCoreAppExtension::createDataAndTempDirs() const {
    static const auto &func = [](const QString &path) {
        qCDebug(qAppExtLog) << "qmcorehost:" << (QM::isDirExist(path) ? "find" : "create")
                            << "directory" << path;
        return QM::mkDir(path);
    };

    if (!func(appDataDir())) {
        return false;
    }

    if (!func(tempDir())) {
        return false;
    }

    return true;
}

/*!
    Returns the \c QtMediate configuration file that will be read.
*/
QString QMCoreAppExtension::configurationPath(QSettings::Scope scope) {
    if (scope == QSettings::SystemScope) {
        static QString dir = QT_CONFIG_FILE_DIR + QStringLiteral("/qtmediate.json");
        return dir;
    }

    static QString dir = QM::appDataPath() + QStringLiteral("/ChorusKit/") +
                         qApp->applicationName() + QStringLiteral("/qtmediate.json");
    return dir;
}

/*!
    Returns the \c QtMediate configuration file directory, it's the same as where <tt>qt.conf</tt>
    locates.

    \li On Mac, the default path is <tt>\%AppPath\%/../Resources</tt>
    \li On Windows/Linux, the default path is <tt>\%AppPath\%</tt>
*/
QString QMCoreAppExtension::configurationBasePrefix() {
    return QT_CONFIG_BASE_DIR;
}

/*!
    \internal
*/
QMCoreAppExtension::QMCoreAppExtension(QMCoreAppExtensionPrivate &d, QObject *parent)
    : QObject(parent), d_ptr(&d) {
    m_instance = this;

    d.q_ptr = this;
    d.init();
}