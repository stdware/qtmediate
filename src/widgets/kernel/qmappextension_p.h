#ifndef QMAPPEXTENSION_P_H
#define QMAPPEXTENSION_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QtMediate API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include <QMCore/private/qmcoreappextension_p.h>
#include <QMWidgets/qmappextension.h>

class QMAppExtensionPrivate : public QMCoreAppExtensionPrivate {
    Q_DECLARE_PUBLIC(QMAppExtension)
public:
    QMAppExtensionPrivate();
    ~QMAppExtensionPrivate();

    void init();

    QMCoreDecoratorV2 *createDecorator(QObject *parent) override;

    static int globalIconCacheSerialNum;
};

#endif // QMAPPEXTENSION_P_H
