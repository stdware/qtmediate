#ifndef QMCSS_H
#define QMCSS_H

#include <QVariant>
#include <QMetaType>

#include <QMWidgets/qmwidgetsglobal.h>

class QM_WIDGETS_EXPORT QMCssType {
public:
    static bool registerMetaTypeName(int id, const QByteArray &name);
    static bool unregisterMetaTypeName(int id);
    static bool unregisterMetaTypeName(const QByteArray &name);

    static QByteArray metaTypeName(int id);
    static int metaTypeId(const QByteArray &name);

    static QVariant parse(const QString &s);

    template <class From, class To>
    static inline void unregisterConverterFunction() {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QMetaType::unregisterConverterFunction(qMetaTypeId<From>(), qMetaTypeId<To>());
#else
        // QMetaType::unregisterConverterFunction(QMetaType::fromType<From>(),
        //                                        QMetaType::fromType<To>());
#endif
    }
};

namespace QMCss {

    QM_WIDGETS_EXPORT void applyLayoutInfo(const QStringList &sl, QWidget *w);

}

#define Q_LAYOUT_PROPERTY_DELCARE_2                                                                \
private:                                                                                           \
    Q_PROPERTY(                                                                                    \
        QStringList layoutInfo READ layoutInfo WRITE setLayoutInfo NOTIFY layoutInfoChanged)       \
                                                                                                   \
    void setLayoutInfo(const QStringList &layoutInfo) {                                            \
        QMCss::applyLayoutInfo(layoutInfo, this);                                                  \
    }                                                                                              \
                                                                                                   \
    QStringList layoutInfo() const {                                                               \
        return {};                                                                                 \
    }

#endif // QMCSS_H
