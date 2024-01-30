#ifndef QMCOREAPPEXTENSION_H
#define QMCOREAPPEXTENSION_H

#include <QLoggingCategory>
#include <QObject>
#include <QSettings>

#include <QMCore/qmglobal.h>

#define qAppExt QMCoreAppExtension::instance()

class QMCoreAppExtensionPrivate;

class QM_CORE_EXPORT QMCoreAppExtension : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMCoreAppExtension)
public:
    explicit QMCoreAppExtension(QObject *parent = nullptr);
    ~QMCoreAppExtension();

    static QMCoreAppExtension *instance();

    enum MessageBoxFlag {
        NoIcon,
        Information,
        Question,
        Warning,
        Critical,
    };
    Q_ENUM(MessageBoxFlag)

    virtual void showMessage(QObject *parent, MessageBoxFlag flag, const QString &title,
                             const QString &text) const;

public:
    bool isAboutToQuit() const;

    QString appDataDir() const;
    void setAppDataDir(const QString &dir);

    QString tempDir() const;
    void setTempDir(const QString &dir);

    QString libDir() const;
    void setLibDir(const QString &dir);

    QString shareDir() const;
    void setShareDir(const QString &dir);

    QString appShareDir() const;
    void setAppShareDir(const QString &dir);

    QString appPluginsDir() const;
    void setAppPluginsDir(const QString &dir);

    bool createDataAndTempDirs() const;

public:
    static QString configurationPath(QSettings::Scope scope = QSettings::UserScope);
    static QString configurationBasePrefix();

protected:
    QMCoreAppExtension(QMCoreAppExtensionPrivate &d, QObject *parent = nullptr);

    QScopedPointer<QMCoreAppExtensionPrivate> d_ptr;
};

QM_CORE_EXPORT Q_DECLARE_LOGGING_CATEGORY(qAppExtLog)

#endif // QMCOREAPPEXTENSION_H