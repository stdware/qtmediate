#ifndef CTABBUTTON_P_H
#define CTABBUTTON_P_H

#include <QMWidgets/CTabButton.h>
#include <QMWidgets/private/CPushButton_p.h>

class CTabButtonPrivate : public CPushButtonPrivate {
    Q_DECLARE_PUBLIC(CTabButton)
public:
    CTabButtonPrivate();
    ~CTabButtonPrivate();

    void init();

    double spaceRatio;
};

#endif // CTABBUTTON_P_H
