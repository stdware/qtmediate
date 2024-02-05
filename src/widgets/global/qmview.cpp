#include "qmview.h"

#include <QWidget>
#include <QScreen>
#include <QWindow>
#include <QGuiApplication>
#include <QPainter>

/*!
    \namespace QMView
    \brief Namespace of Qt graphics utilities.
*/

namespace QMView {

    /*!
       Creates a pixmap whose resolution and pixel ratio is suitable for rendering to screen.

       \param logicalPixelSize Desired size of pixmap, in OS logical pixels
       \param window Pointer to QWindow whose DPR is taken. If it's nullptr, use qApp's DPR instead

       \return pixmap created
     */
    QPixmap createPixmap(const QSize &logicalPixelSize, QWindow *window) {
#ifndef Q_OS_MACOS
        qreal targetDPR;
        if (window)
            targetDPR = window->devicePixelRatio();
        else
            targetDPR = qApp->devicePixelRatio();
        QPixmap ret(logicalPixelSize * targetDPR);
        ret.setDevicePixelRatio(targetDPR);
        return ret;
#else
        Q_UNUSED(window);
        return QPixmap(logicalPixelSize);
#endif
    }

    /*!
        Makes a window show in the center of the screen.
    */
    void centralizeWindow(QWidget *w, QSizeF ratio) {
        QSize desktopSize;
        if (w->parentWidget()) {
            desktopSize = w->parentWidget()->size();
        } else {
            desktopSize = w->screen()->size();
        }

        int dw = desktopSize.width();
        int dh = desktopSize.height();

        double rw = ratio.width();
        double rh = ratio.height();

        QSize size = w->size();
        if (rw > 0 && rw <= 1) {
            size.setWidth(dw * rw);
        }
        if (rh > 0 && rh <= 1) {
            size.setHeight(dh * rh);
        }

        w->setGeometry((dw - size.width()) / 2, (dh - size.height()) / 2, size.width(),
                       size.height());
    }

}