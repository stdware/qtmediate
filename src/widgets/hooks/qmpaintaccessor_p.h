#ifndef QMPAINTACCESSORPRIVATE_H
#define QMPAINTACCESSORPRIVATE_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QtMediate API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include <QMWidgets/qmpaintaccessor.h>

class QMPaintAccessorPrivate {
    Q_DECLARE_PUBLIC(QMPaintAccessor)
public:
    QMPaintAccessorPrivate();
    virtual ~QMPaintAccessorPrivate();

    void init();

    QMPaintAccessor *q_ptr;
};

#endif // QMPAINTACCESSORPRIVATE_H