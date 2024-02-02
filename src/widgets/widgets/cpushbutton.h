#ifndef CPUSHBUTTON_H
#define CPUSHBUTTON_H

#include <QPushButton>

#include <QMWidgets/qmwidgetsglobal.h>

class CPushButtonPrivate;

class QM_WIDGETS_EXPORT CPushButton : public QPushButton {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CPushButton)
public:
    explicit CPushButton(QWidget *parent = nullptr);
    explicit CPushButton(const QString &text, QWidget *parent = nullptr);
    CPushButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr);
    ~CPushButton();

protected:
    void paintEvent(QPaintEvent *event) override;

    virtual void initStyleOptionEx(QStyleOptionButton *opt);

protected:
    CPushButton(CPushButtonPrivate &d, QWidget *parent = nullptr);

    QScopedPointer<CPushButtonPrivate> d_ptr;
};

#endif // CPUSHBUTTON_H
