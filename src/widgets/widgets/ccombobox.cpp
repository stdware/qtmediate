#include "ccombobox.h"

#include <QListView>

CComboBox::CComboBox(QWidget *parent) : QComboBox(parent), m_enableWheel(false) {
    setView(new QListView());
}

CComboBox::~CComboBox() {
}

bool CComboBox::enableWheel() const {
    return m_enableWheel;
}

void CComboBox::setEnableWheel(bool enableWheel) {
    m_enableWheel = enableWheel;
}

void CComboBox::wheelEvent(QWheelEvent *event) {
    if (m_enableWheel) {
        QComboBox::wheelEvent(event);
    }
}
