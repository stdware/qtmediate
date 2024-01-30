#include "qmautoresizer.h"

#include <QApplication>
#include <QEvent>
#include <QWidget>

/*!
    \class QMAutoResizer

    A QWidget which isn't in a layout is not able to update its geometry. But a widget in a scroll
    area is usually independent, this class is used to force the widget to adjust its size when
    a layout request event comes.
*/

/*!
    \enum QMAutoResizer::SizeOption
    \brief

    \val QMAutoResizer::SizeHint
    \brief

    \val QMAutoResizer::MinimumSizeHint
    \brief
*/

/*!
    \enum QMAutoResizer::Width
    \brief

    \val QMAutoResizer::Height
    \brief

    \val QMAutoResizer::WidthAndHeight
    \brief
*/

/*!
    Constructor.
*/
QMAutoResizer::QMAutoResizer(QWidget *parent) : QMAutoResizer(SizeHint, parent) {
}

/*!
    Constructs with the given size option.
*/
QMAutoResizer::QMAutoResizer(QMAutoResizer::SizeOption so, QWidget *parent)
    : QMAutoResizer(so, false, parent) {
}

/*!
    Constructs with the given size option and fixed option.
*/
QMAutoResizer::QMAutoResizer(QMAutoResizer::SizeOption so, bool fixed, QWidget *parent)
    : QMAutoResizer(so, fixed, WidthAndHeight, parent) {
}

/*!
    Constructs with the given size option, fixed option and measure option.
*/
QMAutoResizer::QMAutoResizer(QMAutoResizer::SizeOption so, bool fixed,
                             QMAutoResizer::MeasureOption mo, QWidget *parent)
    : QObject(parent), w(parent), so(so), fix(fixed), mo(mo) {
    w->installEventFilter(this);
}

/*!
    Destructor.
*/
QMAutoResizer::~QMAutoResizer() {
}

bool QMAutoResizer::eventFilter(QObject *obj, QEvent *event) {
    if (obj == w) {
        switch (event->type()) {
            case QEvent::LayoutRequest: {
                auto size = (so == SizeHint) ? w->sizeHint() : w->minimumSizeHint();
                switch (mo) {
                    case Width:
                        fix ? w->setFixedWidth(size.width()) : w->resize(size.width(), w->height());
                        break;
                    case Height:
                        fix ? w->setFixedHeight(size.height())
                            : w->resize(w->width(), size.height());
                        break;
                    default:
                        fix ? w->setFixedSize(size) : w->resize(size);
                        break;
                }
                QApplication::sendEvent(this, event); // ?
                break;
            }
            default:
                break;
        }
    }
    return QObject::eventFilter(obj, event);
}

/*!
    \fn QWidget *QMAutoResizer::widget() const
*/

/*!
    \fn QMAutoResizer::SizeOption QMAutoResizer::sizeOption() const
*/

/*!
    \fn bool QMAutoResizer::fixed() const
*/

/*!
    \fn QMAutoResizer::MeasureOption QMAutoResizer::measureOption() const
*/