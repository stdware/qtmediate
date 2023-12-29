#ifndef QMNAMESPACE_H
#define QMNAMESPACE_H

#include <QObject>

#include <QMCore/qmglobal.h>

namespace QM {

    Q_NAMESPACE_EXPORT(QM_CORE_EXPORT)

    enum Direction {
        Forward,
        Backward,
    };
    Q_ENUM_NS(Direction)

    enum Priority {
        Primary = 16,
        Secondary = 32,
    };
    Q_ENUM_NS(Priority)

    enum DockCorner {
        LeftTopCorner = 0x1,
        LeftBottomCorner,
        TopLeftCorner,
        TopRightCorner,
        RightTopCorner,
        RightBottomCorner,
        BottomLeftCorner,
        BottomRightCorner,
    };
    Q_ENUM_NS(DockCorner)

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
