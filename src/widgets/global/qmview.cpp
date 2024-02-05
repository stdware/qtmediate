#include "qmview.h"

#include <QWidget>
#include <QScreen>
#include <QWindow>
#include <QGuiApplication>
#include <QPainter>

#ifdef Q_OS_WINDOWS
#  include <windows.h>
#endif

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

    /*!
        Bring the window to the toppest.
    */
    void raiseWindow(QWidget *w) {
        // Make sure the window isn't minimized
        // TODO: this always puts it in the "normal" state but it might have been maximized
        // before minimized...so either a flag needs stored or find a Qt call to do it appropriately
        if (w->isMinimized())
            w->showNormal();

#ifdef Q_OS_WIN
        // TODO: there doesn't seem to be a cross platform way to force the window
        // to the foreground. So this will need moved to a platform specific file

        HWND hWnd = reinterpret_cast<HWND>(w->effectiveWinId());
        if (hWnd) {
            // I have no idea what this does but it works mostly
            // https://www.codeproject.com/Articles/1724/Some-handy-dialog-box-tricks-tips-and-workarounds

            ::AttachThreadInput(::GetWindowThreadProcessId(::GetForegroundWindow(), nullptr),
                                ::GetCurrentThreadId(), TRUE);

            ::SetForegroundWindow(hWnd);
            ::SetFocus(hWnd);

            ::AttachThreadInput(GetWindowThreadProcessId(GetForegroundWindow(), nullptr),
                                GetCurrentThreadId(), FALSE);
        }
#endif
    }

}