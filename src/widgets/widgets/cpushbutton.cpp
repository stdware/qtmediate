#include "cpushbutton.h"
#include "cpushbutton_p.h"

#include <QDateTime>
#include <QDebug>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>

#include "qmbuttonstate_p.h"
#include "qmcss_p.h"
#include "qmsvgx.h"
#include "qmrasterpaintaccessor.h"

CPushButtonPrivate::CPushButtonPrivate() {
}

CPushButtonPrivate::~CPushButtonPrivate() {
}

void CPushButtonPrivate::init() {
}

/*!
    \class CPushButton
    \brief CPushButton complements the QtMediate features on the basis of QPushButton.
*/

/*!
    Constructor.
*/
CPushButton::CPushButton(QWidget *parent) : QPushButton(parent), d_ptr(new CPushButtonPrivate()) {
}

/*!
    Constructs with the specified text.
*/
CPushButton::CPushButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent), d_ptr(new CPushButtonPrivate()) {
}

/*!
    Constructs with the specified icon and text.
*/
CPushButton::CPushButton(const QIcon &icon, const QString &text, QWidget *parent)
    : QPushButton(icon, text, parent), d_ptr(new CPushButtonPrivate()) {
}

/*!
    Destructor.
*/
CPushButton::~CPushButton() {
}

void CPushButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOptionEx(&option);
    p.drawControl(QStyle::CE_PushButton, option);
}

/*!
    Complement \c option after the initializagtion of CPushButton::initStyleOption.
*/
void CPushButton::initStyleOptionEx(QStyleOptionButton *opt) {
    initStyleOption(opt);

    auto &option = *opt;
    if (option.icon.isNull() || option.iconSize.isEmpty())
        return;

    // Try to correct icon color
    QMSvgx::Icon svgx(&option.icon);
    if (svgx.isValid()) {
        QIcon icon = option.icon;
        QM::ButtonState state = QMButtonStates::buttonState(this);
        svgx.setCurrentState(state);
        if (svgx.color(state) == "auto") {
            QString text = option.text;
            QSize iconSize = option.iconSize;

            // Change options
            option.icon = {};
            option.iconSize = {};
            option.text = QChar(0x25A0);

            // Query for text color
            QMRasterPaintAccessor acc(QImage(option.rect.size(), QImage::Format_ARGB32));
            QPen pen = acc.queryPen(
                [this, &option](QPainter *painter) {
                    style()->drawControl(QStyle::CE_PushButton, &option, painter, this); //
                },
                QMPaintAccessor::PI_Text);
            svgx.setColorHint(QMCss::colorName(pen.color()));

            // Restore options
            option.icon = icon;
            option.iconSize = iconSize;
            option.text = text;
        }
    }
}

/*!
    \internal
*/
CPushButton::CPushButton(CPushButtonPrivate &d, QWidget *parent) : QPushButton(parent), d_ptr(&d) {
    d.q_ptr = this;
    d.init();
}
