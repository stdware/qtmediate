#include "qmdisplaystring.h"

#include <QCoreApplication>
#include <QDebug>

// The internal class should be transparent in an anonymous namespace.
namespace {
    class BaseString;
}

class QMDisplayStringData {
public:
    QMDisplayString *q;
    BaseString *str;
    QVariantHash properties;

    explicit QMDisplayStringData(const QString &s, QMDisplayString *q);
    explicit QMDisplayStringData(QMDisplayString::GetText func, QMDisplayString *q);
    explicit QMDisplayStringData(QMDisplayString::GetTextEx func, void *userdata,
                                 QMDisplayString *q);
    explicit QMDisplayStringData(BaseString *str, const QVariantHash &properties,
                                 QMDisplayString *q);
    ~QMDisplayStringData();
};

namespace {

    class BaseString {
    public:
        explicit BaseString(QMDisplayString::TranslatePolicy policy, QMDisplayStringData *q)
            : p(policy), q(q){};
        virtual ~BaseString() = default;

        virtual QString text() const = 0;
        virtual BaseString *clone(QMDisplayStringData *q) const = 0;

        QMDisplayString::TranslatePolicy p;
        QMDisplayStringData *q;
    };

    class PlainString : public BaseString {
    public:
        explicit PlainString(QString s, QMDisplayStringData *q)
            : BaseString(QMDisplayString::TranslateIgnored, q), s(std::move(s)){};

        QString text() const override {
            return s;
        }
        BaseString *clone(QMDisplayStringData *q) const override {
            return new PlainString(s, q);
        }

        QString s;
    };

    class CallbackString : public BaseString {
    public:
        explicit CallbackString(QMDisplayString::GetText func, QMDisplayStringData *q)
            : BaseString(QMDisplayString::TranslateAlways, q), func(std::move(func)){};

        QString text() const override {
            return func();
        }

        BaseString *clone(QMDisplayStringData *q) const override {
            return new CallbackString(func, q);
        }

        QMDisplayString::GetText func;
    };

    class CallbackExString : public BaseString {
    public:
        explicit CallbackExString(QMDisplayString::GetTextEx func, QMDisplayStringData *q)
            : BaseString(QMDisplayString::TranslateAlwaysEx, q), func(std::move(func)){};

        QString text() const override {
            return func(*(q->q));
        }
        BaseString *clone(QMDisplayStringData *q) const override {
            return new CallbackExString(func, q);
        }

        QMDisplayString::GetTextEx func;
    };

}

QMDisplayStringData::QMDisplayStringData(const QString &s, QMDisplayString *q)
    : q(q), str(new PlainString(s, this)) {
}

QMDisplayStringData::QMDisplayStringData(QMDisplayString::GetText func, QMDisplayString *q)
    : q(q), str(func ? decltype(str)(new CallbackString(std::move(func), this))
                     : decltype(str)(new PlainString({}, this))) {
}

QMDisplayStringData::QMDisplayStringData(QMDisplayString::GetTextEx func, void *userdata,
                                         QMDisplayString *q)
    : q(q), str(func ? decltype(str)(new CallbackExString(std::move(func), this))
                     : decltype(str)(new PlainString({}, this))) {
}

QMDisplayStringData::QMDisplayStringData(BaseString *str, const QVariantHash &properties,
                                         QMDisplayString *q)
    : q(q), str(str->clone(this)), properties(properties) {
}

QMDisplayStringData::~QMDisplayStringData() {
    delete str;
}

/*!
    \class QMDisplayString
    \brief A wrapper of QString that always returns a translated string.
*/

/*!
    \typedef QMDisplayString::GetText
    \brief Translation callback
*/

/*!
    \typedef QMDisplayString::GetTextEx
    \brief Advanced translation callback
*/

/*!
    Constructs from a plain string.
*/
QMDisplayString::QMDisplayString(const QString &s) : d(new QMDisplayStringData(s, this)) {
}

/*!
    Constructs from a translation callback, you should call QCoreApplication::tr() in this callback.
*/
QMDisplayString::QMDisplayString(const QMDisplayString::GetText &func)
    : d(new QMDisplayStringData(func, this)) {
}

/*!
    Constructs from a translation callback that provides the QMDisplayString instance, you may use
    the property map in this callback.
*/
QMDisplayString::QMDisplayString(const GetTextEx &func, void *userdata)
    : d(new QMDisplayStringData(func, userdata, this)) {
}

/*!
    Destructor.
*/
QMDisplayString::~QMDisplayString() {
    delete d;
}

QMDisplayString::QMDisplayString(const QMDisplayString &other)
    : d(new QMDisplayStringData(other.d->str, other.d->properties, this)) {
}

QMDisplayString::QMDisplayString(QMDisplayString &&other) noexcept : d(other.d) {
    other.d = nullptr;
    d->q = this;
}

/*!
    Sets the QMDisplayString as the given plain string.
*/
QMDisplayString &QMDisplayString::operator=(const QString &s) {
    setPlainString(s);
    return *this;
}

QMDisplayString &QMDisplayString::operator=(const QMDisplayString &other) {
    if (&other == this) {
        return *this;
    }

    d = new QMDisplayStringData(other.d->str, other.d->properties, this);
    return *this;
}

QMDisplayString &QMDisplayString::operator=(QMDisplayString &&other) noexcept {
    if (&other == this) {
        return *this;
    }

    d = other.d;
    other.d = nullptr;
    d->q = this;
    return *this;
}

/*!
    Returns the plain string or translated string.
*/
QString QMDisplayString::text() const {
    return d->str->text();
}

/*!
    Returns the translation policy.
*/
QMDisplayString::TranslatePolicy QMDisplayString::translatePolicy() const {
    return d->str->p;
}

/*!
    Assigns translation callback, the translation policy may be changed.
*/
void QMDisplayString::setTranslateCallback(const QMDisplayString::GetText &func) {
    if (!func) {
        setPlainString({});
        return;
    }

    if (d->str->p != TranslateAlways) {
        delete d->str;
        d->str = new CallbackString(func, d);
    } else {
        auto str = static_cast<CallbackString *>(d->str);
        str->func = func;
    }
}

/*!
    Assigns translation callback, the translation policy may be changed.
*/
void QMDisplayString::setTranslateCallback(const QMDisplayString::GetTextEx &func) {
    if (!func) {
        setPlainString({});
        return;
    }

    if (d->str->p != TranslateAlwaysEx) {
        delete d->str;
        d->str = new CallbackExString(func, d);
    } else {
        auto *str = static_cast<CallbackExString *>(d->str);
        str->func = func;
    }
}

/*!
    Assigns plain string, the translation policy may be changed.
*/
void QMDisplayString::setPlainString(const QString &s) {
    if (d->str->p != TranslateIgnored) {
        delete d->str;
        d->str = new PlainString(s, d);
    } else {
        auto str = static_cast<PlainString *>(d->str);
        str->s = s;
    }
}

/*!
    Gets the property of the given key.
*/
QVariant QMDisplayString::property(const QString &key) const {
    return d->properties.value(key);
}

/*!
    Sets the property of the given key.
*/
void QMDisplayString::setProperty(const QString &key, const QVariant &value) {
    auto &properties = d->properties;
    auto it = properties.find(key);
    if (it == properties.end()) {
        if (!value.isValid())
            return;
        properties.insert(key, value);
    } else {
        if (!value.isValid())
            properties.erase(it);
        else
            it.value() = value;
    }
}

/*!
    Returns the property hash map.
*/
QVariantHash QMDisplayString::propertyMap() const {
    return d->properties;
}

/*!
    \fn QMDisplayString(Func func)

    Template constructor, \c func must be the type of QMDisplayString::GetText.
*/