#ifndef CTABBUTTON_P_H
#define CTABBUTTON_P_H

#include <QMWidgets/ctabbutton.h>
#include <QMWidgets/private/cpushbutton_p.h>

class CTabButtonPrivate : public CPushButtonPrivate {
    Q_DECLARE_PUBLIC(CTabButton)
public:
    CTabButtonPrivate();
    ~CTabButtonPrivate();

    void init();

    double spaceRatio;
};

#endif // CTABBUTTON_P_H
