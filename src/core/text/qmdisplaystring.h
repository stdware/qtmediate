#ifndef QMDISPLAYSTRING_H
#define QMDISPLAYSTRING_H

#include <functional>

#include <QPair>
#include <QSharedDataPointer>
#include <QString>
#include <QVariant>

#include <QMCore/qmglobal.h>

class QMDisplayStringData;

class QM_CORE_EXPORT QMDisplayString {
public:
    enum TranslatePolicy {
        TranslateIgnored = 0,
        TranslateAlways = 1,
        TranslateAlwaysEx = 3,
    };

    using GetText = std::function<QString()>;
    using GetTextEx = std::function<QString(const QMDisplayString &)>;

    QMDisplayString() : QMDisplayString(QString()){};
    QMDisplayString(const QString &s);
    QMDisplayString(const GetText &func);
    explicit QMDisplayString(const GetTextEx &func, void *userdata = nullptr);
    ~QMDisplayString();

    QMDisplayString(const QMDisplayString &other);
    QMDisplayString(QMDisplayString &&other) noexcept;

    QMDisplayString &operator=(const QString &s);
    QMDisplayString &operator=(const QMDisplayString &other);
    QMDisplayString &operator=(QMDisplayString &&other) noexcept;

    QString text() const;
    TranslatePolicy translatePolicy() const;

    void setTranslateCallback(const GetText &func);
    void setTranslateCallback(const GetTextEx &func);
    void setPlainString(const QString &s);

    QVariant property(const QString &key) const;
    void setProperty(const QString &key, const QVariant &value);
    QVariantHash propertyMap() const;

    inline operator QString() const;

    template <class Func>
    inline QMDisplayString(Func func) : QMDisplayString(GetText(func)){};

private:
    QMDisplayStringData *d;

    friend class QMDisplayStringData;
};

inline QMDisplayString::operator QString() const {
    return text();
}

Q_DECLARE_METATYPE(QMDisplayString)

Q_DECLARE_TYPEINFO(QMDisplayString, Q_MOVABLE_TYPE);

#endif // QMDISPLAYSTRING_H
