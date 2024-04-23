#ifndef QMCOREDECORATORV2_H
#define QMCOREDECORATORV2_H

#include <QObject>

#include <QMCore/qmnamespace.h>

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

    template <class Object>
    void installLocale(Object *o) {
        static_assert(std::is_base_of<QObject, Object>::value, "T should inherit from QObject");
        installLocale(o, std::bind(&Object::reloadStrings, o));
    }

    template <class Func>
    void installLocale(typename QtPrivate::FunctionPointer<Func>::Object *o, Func slot) {
        static_assert(std::is_base_of<QObject, decltype(*o)>::value,
                      "T should inherit from QObject");
        installLocale(o, std::bind(slot, o));
    }

    template <class Object>
    void installLocale(QObject *o, Object *receiver) {
        installLocale(o, std::bind(&Object::reloadStrings, receiver));
    }

    template <class Func>
    void installLocale(QObject *o, typename QtPrivate::FunctionPointer<Func>::Object *receiver,
                       Func slot) {
        installLocale(o, std::bind(slot, receiver));
    }

Q_SIGNALS:
    void localeChanged(const QString &locale);

protected:
    QMCoreDecoratorV2(QMCoreDecoratorV2Private &d, QObject *parent = nullptr);

    QScopedPointer<QMCoreDecoratorV2Private> d_ptr;
};

#endif // QMCOREDECORATORV2_H
