#ifndef QMARGINSIMPL_H
#define QMARGINSIMPL_H

#include <QMargins>
#include <QVariant>

#include <QMWidgets/qmwidgetsglobal.h>

namespace QMarginsImpl {

    QM_WIDGETS_EXPORT QMargins fromStringList(const QStringList &stringList);

    QM_WIDGETS_EXPORT QMargins fromString(const QString &s);

    QM_WIDGETS_EXPORT const char *metaFunctionName();

}

Q_DECLARE_METATYPE(QMargins)

#endif // QMARGINSIMPL_H
