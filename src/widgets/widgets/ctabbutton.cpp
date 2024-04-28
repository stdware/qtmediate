#include "ctabbutton.h"
#include "ctabbutton_p.h"

#include <QDateTime>
#include <QDebug>
#include <QStyleOptionButton>
#include <QStylePainter>

#include "qmview.h"

CTabButtonPrivate::CTabButtonPrivate() {
    spaceRatio = 0;
}

CTabButtonPrivate::~CTabButtonPrivate() {
}

void CTabButtonPrivate::init() {
}

/*!
    \class CTabButton
    \brief Push button with configurable spacing between icon and text.
*/

/*!
    Constructor.
*/
CTabButton::CTabButton(QWidget *parent) : CTabButton(*new CTabButtonPrivate(), parent) {
}

/*!
    Constructs with the specified text.
*/
CTabButton::CTabButton(const QString &text, QWidget *parent) : CTabButton(parent) {
    setText(text);
}

/*!
    Constructs with the specified icon and text.
*/
CTabButton::CTabButton(const QIcon &icon, const QString &text, QWidget *parent)
    : CTabButton(parent) {
    setIcon(icon);
    setText(text);
}

/*!
    Destructor.
*/
CTabButton::~CTabButton() {
}

/*!
    Returns the ratio of the spacing to the icon width.
*/
double CTabButton::spaceRatio() const {
    Q_D(const CTabButton);
    return d->spaceRatio;
}

/*!
    Sets the ratio of the spacing to the icon width.
*/
void CTabButton::setSpaceRatio(double ratio) {
    Q_D(CTabButton);
    d->spaceRatio = ratio;
    Q_EMIT spaceChanged();
}

QSize CTabButton::sizeHint() const {
    Q_D(const CTabButton);

    QSize sz = QPushButton::sizeHint();
    QSize iconSz = iconSize();
    if (icon().isNull()) {
        iconSz = QSize(0, 0);
    }
    int offset = iconSz.width() * d->spaceRatio;
    return {sz.width() + offset, sz.height()};
}

QSize CTabButton::minimumSizeHint() const {
    return sizeHint();
}

void CTabButton::initStyleOptionEx(QStyleOptionButton *opt) {
    CPushButton::initStyleOptionEx(opt);

    Q_D(CTabButton);
    auto &option = *opt;
    if (option.icon.isNull() || option.iconSize.isEmpty())
        return;

    QSize size = iconSize();
    QPixmap originalPixmap = option.icon.pixmap(size); // Get the pixmap to apply with right size
    if (originalPixmap.isNull())
        return;

    size.rwidth() *= 1 + d->spaceRatio;                // Multiply width

    QPixmap expendedPixmap = QMView::createPixmap(size, window()->windowHandle()); // Expended
    expendedPixmap.fill(Qt::transparent);

    QPainter painter(&expendedPixmap);
    painter.drawPixmap(QRect(QPoint(), iconSize()), originalPixmap);

    option.icon = QIcon(expendedPixmap);                            // Change to real icon
    option.iconSize = originalPixmap.isNull() ? QSize(0, 0) : size; // Change to real size
}

CTabButton::CTabButton(CTabButtonPrivate &d, QWidget *parent) : CPushButton(d, parent) {
    d.init();
}
