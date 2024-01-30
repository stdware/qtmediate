#ifndef QMEQUALBOXLAYOUT_H
#define QMEQUALBOXLAYOUT_H

#include <QBoxLayout>

#include <QMWidgets/qmwidgetsglobal.h>

class QMEqualBoxLayoutPrivate;

class QM_WIDGETS_EXPORT QMEqualBoxLayout : public QBoxLayout {
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMEqualBoxLayout)
public:
    QMEqualBoxLayout(Direction direction, QWidget *parent = nullptr);
    ~QMEqualBoxLayout();

    void addWidget2(QWidget *widget, int stretch = 0, Qt::Alignment alignment = {});
    void insertWidget2(int index, QWidget *widget, int stretch = 0, Qt::Alignment alignment = {});

    void addItem(QLayoutItem *item) override;
    QLayoutItem *takeAt(int index) override;

protected:
    QScopedPointer<QMEqualBoxLayoutPrivate> d_ptr;
};

#endif // QMEQUALBOXLAYOUT_H
