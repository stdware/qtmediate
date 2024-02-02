#ifndef CPUSHBUTTON_P_H
#define CPUSHBUTTON_P_H

#include <QMWidgets/cpushbutton.h>

class CPushButtonPrivate {
    Q_DECLARE_PUBLIC(CPushButton)
public:
    CPushButtonPrivate();
    virtual ~CPushButtonPrivate();

    void init();

    CPushButton *q_ptr;
};

#endif // CPUSHBUTTON_P_H
