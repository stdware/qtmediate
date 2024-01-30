#ifndef QMSVGX_P_H
#define QMSVGX_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QtMediate API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include <QMWidgets/qmsvgx.h>
#include <QMWidgets/private/qmbuttonstate_p.h>

namespace QMPrivate {

    enum IconEngineHook {
        Create = 0x1000,
        GetState,
        SetState,
        GetColor,
        SetColor,
    };

    QM_WIDGETS_EXPORT QString serializeSvgxArgs(const QHash<QM::ButtonState, QString> &fileMap,
                                                const QHash<QM::ButtonState, QString> &colorMap);

    QM_WIDGETS_EXPORT bool deserializeSvgxArgs(const QString &s,
                                               QHash<QM::ButtonState, QString> *fileMap,
                                               QHash<QM::ButtonState, QString> *colorMap);

}

#endif // QMSVGX_P_H
