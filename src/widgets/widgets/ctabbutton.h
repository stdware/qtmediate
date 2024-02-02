#ifndef CTABBUTTON_H
#define CTABBUTTON_H

#include <QMWidgets/cpushbutton.h>

class CTabButtonPrivate;

class QM_WIDGETS_EXPORT CTabButton : public CPushButton {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CTabButton)
    Q_PROPERTY(double spaceRatio READ spaceRatio WRITE setSpaceRatio NOTIFY spaceChanged)
public:
    explicit CTabButton(QWidget *parent = nullptr);
    explicit CTabButton(const QString &text, QWidget *parent = nullptr);
    CTabButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr);
    ~CTabButton();

public:
    double spaceRatio() const;
    void setSpaceRatio(double ratio);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void spaceChanged();

protected:
    void initStyleOptionEx(QStyleOptionButton *opt) override;

protected:
    CTabButton(CTabButtonPrivate &d, QWidget *parent = nullptr);
};

#endif // CTABBUTTON_H
