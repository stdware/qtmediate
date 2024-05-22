#include "ctoolbutton.h"

#include <QDateTime>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>

#include "qmbuttonstate_p.h"
#include "qmcss_p.h"
#include "qmsvgx.h"
#include "qmrasterpaintaccessor.h"

/*!
    \class CToolButton
    \brief CToolButton complements the QtMediate features on the basis of QToolButton.
*/

/*!
    Constructor.
*/
CToolButton::CToolButton(QWidget *parent) : QToolButton(parent) {
}

/*!
    Constructs with the specified text.
*/
CToolButton::CToolButton(const QString &text, QWidget *parent) : QToolButton(parent) {
    setText(text);
}

/*!
    Constructs with the specified icon and text.
*/
CToolButton::CToolButton(const QIcon &icon, const QString &text, QWidget *parent)
    : QToolButton(parent) {
    setIcon(icon);
    setText(text);
}

/*!
    Destructor.
*/
CToolButton::~CToolButton() {
}

void CToolButton::paintEvent(QPaintEvent *event) {
    QStylePainter p(this);
    QStyleOptionToolButton option;
    initStyleOptionEx(&option);
    p.drawComplexControl(QStyle::CC_ToolButton, option);
}

void CToolButton::leaveEvent(QEvent *event) {
    update();
    QAbstractButton::leaveEvent(event);
}

/*!
    Complement \c option after the initializagtion of QToolButton::initStyleOption.
*/
void CToolButton::initStyleOptionEx(QStyleOptionToolButton *opt) {
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
                    style()->drawComplexControl(QStyle::CC_ToolButton, &option, painter, this); //
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
