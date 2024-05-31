#ifndef QMCSS_P_H
#define QMCSS_P_H

#include <QColor>
#include <QHash>
#include <QSize>
#include <QStringList>
#include <QVariant>

#include <QMWidgets/qmcss.h>

namespace QMCss {

    QM_WIDGETS_EXPORT QList<int> parseSizeValueList(const QString &s);

    QM_WIDGETS_EXPORT QList<double> parseSizeFValueList(const QString &s);

    QM_WIDGETS_EXPORT QStringList parseStringValueList(const QString &s,
                                                       QChar separator = QChar(','));

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

#endif // QMCSS_P_H
