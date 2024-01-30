#include "qmpaintaccessor.h"
#include "qmpaintaccessor_p.h"

QMPaintAccessorPrivate::QMPaintAccessorPrivate() {
}

QMPaintAccessorPrivate::~QMPaintAccessorPrivate() {
}

void QMPaintAccessorPrivate::init() {
}

/*!
    \class QMPaintAccessor
    \brief QMPaintAccessor is a hacking utility to query for an attribute used in a painting
           procedure.
*/

/*!
    Constructor.
*/
QMPaintAccessor::QMPaintAccessor() : QMPaintAccessor(*new QMPaintAccessorPrivate()) {
}

/*!
    Destructor.
*/
QMPaintAccessor::~QMPaintAccessor() {
}

/*!
    \enum QMPaintAccessor::PaintItem

    \var QMPaintAccessor::PaintItem::PI_Rect
    \brief Rectangle

    \var QMPaintAccessor::PaintItem::PI_Line
    \brief Line

    \var QMPaintAccessor::PaintItem::PI_Ellipse
    \brief Ellipse

    \var QMPaintAccessor::PaintItem::PI_Path
    \brief Painter path

    \var QMPaintAccessor::PaintItem::PI_Point
    \brief Point

    \var QMPaintAccessor::PaintItem::PI_Polygon
    \brief Polygon

    \var QMPaintAccessor::PaintItem::PI_Text
    \brief Text item

*/

/*!
    Queries for a pen attribute.
*/
QPen QMPaintAccessor::queryPen(const PaintProc &proc, PaintItems items) {
    return {};
}

/*!
    Queries for a brush attribute.
*/
QBrush QMPaintAccessor::queryBrush(const PaintProc &proc, PaintItems items) {
    return {};
}

/*!
    \internal
*/
QMPaintAccessor::QMPaintAccessor(QMPaintAccessorPrivate &d) : d_ptr(&d) {
    d.q_ptr = this;

    d.init();
}
