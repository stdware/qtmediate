#include "qmrasterpaintaccessor.h"

#include <private/qicon_p.h>
#include <private/qimage_p.h>
#include <private/qpaintengine_p.h>
#include <private/qpaintengine_raster_p.h>
#include <private/qpixmap_raster_p.h>

#include "qmpaintaccessor_p.h"

namespace {

#define _IMPL(FLAG)                                                                                \
    override {                                                                                     \
        if (this->items & FLAG) {                                                                  \
            this->pen = painter()->pen();                                                          \
            this->brush = painter()->brush();                                                      \
        }                                                                                          \
    }

    class HackPaintEngine : public QRasterPaintEngine {
    public:
        HackPaintEngine(QPaintDevice *device, QMRasterPaintAccessor::PaintItems items)
            : QRasterPaintEngine(device) {
        }
        ~HackPaintEngine() = default;

        void fillRect(const QRectF &rect, const QBrush &brush) _IMPL(QMPaintAccessor::PI_Rect);
        void fillRect(const QRectF &rect, const QColor &color) _IMPL(QMPaintAccessor::PI_Rect);

        void drawRects(const QRect *rects, int rectCount) _IMPL(QMPaintAccessor::PI_Rect);
        void drawRects(const QRectF *rects, int rectCount) _IMPL(QMPaintAccessor::PI_Rect);

        void drawLines(const QLine *lines, int lineCount) _IMPL(QMPaintAccessor::PI_Line);
        void drawLines(const QLineF *lines, int lineCount) _IMPL(QMPaintAccessor::PI_Line);

        void drawEllipse(const QRectF &r) _IMPL(QMPaintAccessor::PI_Ellipse);
        void drawEllipse(const QRect &r) _IMPL(QMPaintAccessor::PI_Ellipse);

        void drawPath(const QPainterPath &path) _IMPL(QMPaintAccessor::PI_Path);

        void drawPoints(const QPointF *points, int pointCount) _IMPL(QMPaintAccessor::PI_Point);
        void drawPoints(const QPoint *points, int pointCount) _IMPL(QMPaintAccessor::PI_Point);

        void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
            _IMPL(QMPaintAccessor::PI_Polygon);
        void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode)
            _IMPL(QMPaintAccessor::PI_Polygon);

        void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr) _IMPL(0);
        void drawTextItem(const QPointF &p, const QTextItem &textItem)
            _IMPL(QMPaintAccessor::PI_Text);
        void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s) _IMPL(0);
        void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                       Qt::ImageConversionFlags flags) _IMPL(0);

        QMPaintAccessor::PaintItems items;
        QPen pen;
        QBrush brush;
    };

#undef _IMPL

}

class QMRasterPaintAccessorPrivate : public QMPaintAccessorPrivate {
public:
    QMRasterPaintAccessorPrivate(const QImage &image) : image(image) {
    }
    ~QMRasterPaintAccessorPrivate() {
    }

    QImage image;
};

/*!
    \class QMRasterPaintAccessor
    \brief QMRasterPaintAccessor is to query for an attribute when painting an image.
*/

/*!
    Constructor.

    You should specify an image of a specific size and color mode, the query function will use this
    image as a container.
*/
QMRasterPaintAccessor::QMRasterPaintAccessor(const QImage &image)
    : QMPaintAccessor(*new QMRasterPaintAccessorPrivate(image)) {
}

/*!
    Destructor.
*/
QMRasterPaintAccessor::~QMRasterPaintAccessor() {
}

/*!
    Queries for a pen attribute during a painting procedure on a raster device.

    This function is especially useful to get the text color of a widget.
*/
QPen QMRasterPaintAccessor::queryPen(const PaintProc &proc, PaintItems items) {
    Q_D(QMRasterPaintAccessor);

    auto image = d->image; // make a copy

    auto engine = new HackPaintEngine(&image, items);
    image.data_ptr()->paintEngine = engine;

    QPainter p2(&image);
    proc(&p2);

    return engine->pen;
}

/*!
    Queries for a brush attribute during a painting procedure on a raster device.

    This function is especially useful to get the text color of a widget.
*/
QBrush QMRasterPaintAccessor::queryBrush(const PaintProc &proc, PaintItems items) {
    Q_D(QMRasterPaintAccessor);

    auto image = d->image; // make a copy

    auto engine = new HackPaintEngine(&image, items);
    image.data_ptr()->paintEngine = engine;

    QPainter p2(&image);
    proc(&p2);

    return engine->brush;
}
