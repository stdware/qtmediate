#ifndef CTOOLBAR_H
#define CTOOLBAR_H

#include <QToolBar>
#include <QToolButton>

#include <QMWidgets/qmwidgetsglobal.h>

class QM_WIDGETS_EXPORT CToolBar : public QToolBar {
    Q_OBJECT
public:
    explicit CToolBar(QWidget *parent = nullptr);
    ~CToolBar();

    virtual QToolButton *createButton(QWidget *parent);

protected:
    void actionEvent(QActionEvent *event) override;
};

#endif // CTOOLBAR_H
