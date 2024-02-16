#ifndef CCOMBOBOX_H
#define CCOMBOBOX_H

#include <QComboBox>

#include <QMWidgets/qmwidgetsglobal.h>

class QM_WIDGETS_EXPORT CComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit CComboBox(QWidget *parent = nullptr);
    ~CComboBox();

    bool enableWheel() const;
    void setEnableWheel(bool enableWheel);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    bool m_enableWheel;
};

#endif // CCOMBOBOX_H
