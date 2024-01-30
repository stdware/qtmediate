#ifndef QMCOREDECORATORV2_H
#define QMCOREDECORATORV2_H

#include <QObject>

#include <QMCore/qmglobal.h>

#define qIDec QMCoreDecoratorV2::instance()

class QMCoreDecoratorV2Private;

class QM_CORE_EXPORT QMCoreDecoratorV2 : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMCoreDecoratorV2)
public:
    explicit QMCoreDecoratorV2(QObject *parent = nullptr);
    ~QMCoreDecoratorV2();

    static QMCoreDecoratorV2 *instance();

public:
    void addTranslationPath(const QString &path);
    void removeTranslationPath(const QString &path);

    QStringList locales() const;
    QString locale() const;
    void setLocale(const QString &locale);
    void refreshLocale();

    void installLocale(QObject *o, const std::function<void()> &updater);

Q_SIGNALS:
    void localeChanged(const QString &locale);

protected:
    QMCoreDecoratorV2(QMCoreDecoratorV2Private &d, QObject *parent = nullptr);

    QScopedPointer<QMCoreDecoratorV2Private> d_ptr;
};

#endif // QMCOREDECORATORV2_H
