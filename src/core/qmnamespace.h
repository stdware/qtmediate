#ifndef QMNAMESPACE_H
#define QMNAMESPACE_H

#include <QObject>

#include <QMCore/qmglobal.h>

namespace QM {

    Q_NAMESPACE_EXPORT(QM_CORE_EXPORT)

    enum ButtonState {
        ButtonNormal,
        ButtonHover,
        ButtonPressed,
        ButtonDisabled,
        ButtonNormalChecked,
        ButtonHoverChecked,
        ButtonPressedChecked,
        ButtonDisabledChecked,
    };
    Q_ENUM_NS(ButtonState)

}

#endif // QMNAMESPACE_H
