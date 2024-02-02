#include "qmbuttonstate_p.h"

static void InitClickStateIndexes(int arr[]) {
    arr[QM::ButtonNormal] = QM::ButtonNormal;
    arr[QM::ButtonHover] = QM::ButtonNormal;
    arr[QM::ButtonPressed] = QM::ButtonNormal;
    arr[QM::ButtonDisabled] = QM::ButtonNormal;
    arr[QM::ButtonNormalChecked] = QM::ButtonNormal;
    arr[QM::ButtonHoverChecked] = QM::ButtonNormal;
    arr[QM::ButtonPressedChecked] = QM::ButtonNormal;
    arr[QM::ButtonDisabledChecked] = QM::ButtonNormal;
}

static void UpdateClickStateIndex(int i, int arr[]) {
    switch (static_cast<QM::ButtonState>(i)) {
        case QM::ButtonHover:
        case QM::ButtonDisabled: {
            arr[i] = QM::ButtonNormal;
            break;
        }
        case QM::ButtonPressed: {
            arr[i] = arr[QM::ButtonHover];
            break;
        }
        case QM::ButtonNormalChecked: {
            arr[i] = arr[QM::ButtonNormal];
            break;
        }
        case QM::ButtonHoverChecked:
        case QM::ButtonDisabledChecked: {
            arr[i] = arr[QM::ButtonNormalChecked];
            break;
        }
        case QM::ButtonPressedChecked: {
            arr[i] = arr[QM::ButtonHoverChecked];
            break;
        }
        default:
            break;
    }
}

static void UpdateClickStateIndexes(int arr[]) {
    for (int i = 0; i < 8; ++i) {
        if (arr[i] == i)
            continue;
        UpdateClickStateIndex(i, arr);
    }
}

QMButtonStates::QMButtonStates() {
    InitClickStateIndexes(m_arr.data());
}

void QMButtonStates::syncState(QM::ButtonState state) {
    m_arr[state] = state;
    syncInternal();
}

void QMButtonStates::syncStates(const QList<QM::ButtonState> &states) {
    int sz = qMin(states.size(), 8);
    for (int i = 0; i < sz; ++i) {
        m_arr[i] = i;
    }
    syncInternal();
}

QM::ButtonState QMButtonStates::state(QM::ButtonState state) const {
    return static_cast<QM::ButtonState>(m_arr[state]);
}

QM::ButtonState QMButtonStates::operator[](QM::ButtonState state) const {
    return static_cast<QM::ButtonState>(m_arr[state]);
}

void QMButtonStates::setState(QM::ButtonState state) {
    m_arr[state] = state;
}

void QMButtonStates::syncInternal() {
    UpdateClickStateIndexes(m_arr.data());
}

QDataStream &operator>>(QDataStream &in, QMButtonStates &bs) {
    for (int i = 0; i < 8; ++i) {
        in >> bs.m_arr[i];
        if (in.status() != QDataStream::Ok) {
            bs = {};
            break;
        }
    }
    return in;
}

QDataStream &operator<<(QDataStream &out, const QMButtonStates &bs) {
    for (int i = 0; i < 8; ++i) {
        out << bs.m_arr[i];
    }
    return out;
}

QM::ButtonState QMButtonStates::buttonState(const QAbstractButton *button) {
    if (button->isChecked()) {
        if (!button->isEnabled()) {
            return QM::ButtonDisabledChecked;
        }
        if (button->isDown()) {
            return QM::ButtonPressedChecked;
        }
        if (button->underMouse()) {
            return QM::ButtonHoverChecked;
        }
        return QM::ButtonNormalChecked;
    }

    if (!button->isEnabled()) {
        return QM::ButtonDisabled;
    }
    if (button->isDown()) {
        return QM::ButtonPressed;
    }
    if (button->underMouse()) {
        return QM::ButtonHover;
    }
    return QM::ButtonNormal;
}