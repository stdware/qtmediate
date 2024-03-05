#ifndef QMCSS_H
#define QMCSS_H

#include <QColor>
#include <QHash>
#include <QSize>
#include <QStringList>
#include <QVariant>

#include <QMWidgets/qmwidgetsglobal.h>

namespace QMCss {

    QM_WIDGETS_EXPORT QList<int> parseSizeValueList(const QString &s);

    QM_WIDGETS_EXPORT QList<double> parseSizeFValueList(const QString &s);

    QM_WIDGETS_EXPORT QStringList parseStringValueList(const QString &s);

    QM_WIDGETS_EXPORT bool parseBoolean(const QString &s, bool *ok = nullptr);

    QM_WIDGETS_EXPORT QColor parseColor(const QString &s);

    QM_WIDGETS_EXPORT QString colorName(const QColor &color);

    QM_WIDGETS_EXPORT int indexOfEqSign(const QString &s);

    enum FallbackOption {
        FO_Value,
        FO_Reference,
    };
    QM_WIDGETS_EXPORT QHash<QString, QString> parseArgList( //
        const QString &s,                                   //
        const QStringList &expectedKeys,                    //
        const QHash<QString, QPair<QString, FallbackOption>> &fallbacks);

    QM_WIDGETS_EXPORT bool parseButtonStateList(const QString &s, QString arr[],
                                                bool resolveFallback);

}

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
// #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//         QMetaType::unregisterConverterFunction(qMetaTypeId<From>(), qMetaTypeId<To>());
// #else
//         QMetaType::unregisterConverterFunction(QMetaType::fromType<From>(),
//                                                QMetaType::fromType<To>());
// #endif
    }
};

#endif // QMCSS_H
