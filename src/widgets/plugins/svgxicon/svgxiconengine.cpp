#include "svgxiconengine.h"
#include "svgxiconengine_p.h"

#include <QAtomicInt>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QPixmapCache>
#include <QFileInfo>
#include <QDebug>

#include <private/qguiapplication_p.h>

#include "qmsvgx_p.h"

QAtomicInt SvgxIconEnginePrivate::lastSerialNum;

QString SvgxIconEnginePrivate::pmcKey(const QSize &size, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode)
    Q_UNUSED(state)

    syncData();

    // Cache key arguments: serial num, size, current state, color
    return QLatin1String("$qtm_svgxicon_") +
           QString::number(serialNum, 16).append(QLatin1Char('_')) +
           QString::number((((qint64(size.width()) << 11) | size.height()) << 11) | currentState,
                           16)
               .append(QLatin1Char('_')) +
           colorHint;
}

QIcon::Mode SvgxIconEnginePrivate::loadDataForModeAndState(QSvgRenderer *renderer, QIcon::Mode mode,
                                                           QIcon::State state) {
    Q_UNUSED(mode)
    Q_UNUSED(state)

    const auto &script = svgScripts[currentState];
    if (!script.data.isEmpty()) {
        QString data = script.data;
        if (script.hasCurrentColor && !colorHint.isEmpty()) {
            data.replace("currentColor", colorHint);
        }
        renderer->load(data);
    }
    return QIcon::Normal;
}

void SvgxIconEnginePrivate::setup(const QHash<QM::ButtonState, QString> &fileMap,
                                  const QHash<QM::ButtonState, QString> &colorMap) {
    // Update hash key
    stepSerialNum();

    svgScripts = {};
    svgColors = {};

    for (auto it = fileMap.begin(); it != fileMap.end(); ++it) {
        if (it->isEmpty())
            continue;
        svgScripts.setValue({it.value()}, it.key());
    }
    for (auto it = colorMap.begin(); it != colorMap.end(); ++it) {
        if (it->isEmpty())
            continue;
        svgColors.setValue(it.value(), it.key());
    }
}

void SvgxIconEnginePrivate::syncData() {
    auto &item = svgScripts[currentState];
    if (item.fileName.isEmpty())
        return;

    // Read file (Lazy)
    if (item.data.isEmpty()) {
        QFile file(item.fileName);
        if (file.open(QIODevice::ReadOnly)) {
            item.data = file.readAll();
            item.hasCurrentColor = item.data.contains("currentColor");
        }
    }

    // Update color
    const auto &color = svgColors[currentState];
    if (colorHint.isEmpty()) {
        colorHint = color.isEmpty() ? "black" : color;
    }
}

SvgxIconEngine::SvgxIconEngine() : d(new SvgxIconEnginePrivate()) {
}

SvgxIconEngine::SvgxIconEngine(const SvgxIconEngine &other)
    : QIconEngine(other), d(new SvgxIconEnginePrivate()) {
    d->stepSerialNum();
    d->svgScripts = other.d->svgScripts;
    d->svgColors = other.d->svgColors;
    d->currentState = other.d->currentState;
    d->colorHint = other.d->colorHint;
}

SvgxIconEngine::~SvgxIconEngine() {
}

QSize SvgxIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) {
    QPixmap pm = pixmap(size, mode, state);
    if (pm.isNull())
        return QSize();
    return pm.size();
}

QPixmap SvgxIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) {
    QPixmap pm;
    QString pmckey(d->pmcKey(size, mode, state));
    if (QPixmapCache::find(pmckey, &pm))
        return pm;

    QSvgRenderer renderer;
    const QIcon::Mode loadmode = d->loadDataForModeAndState(&renderer, mode, state);
    if (!renderer.isValid())
        return pm;

    QSize actualSize = renderer.defaultSize();
    if (!actualSize.isNull())
        actualSize.scale(size, Qt::KeepAspectRatio);

    if (actualSize.isEmpty())
        return QPixmap();

    QImage img(actualSize, QImage::Format_ARGB32_Premultiplied);
    img.fill(0x00000000);
    QPainter p(&img);
    renderer.render(&p);
    p.end();
    pm = QPixmap::fromImage(img);
    if (qobject_cast<QGuiApplication *>(QCoreApplication::instance())) {
        if (loadmode != mode && mode != QIcon::Normal) {
            const QPixmap generated =
                QGuiApplicationPrivate::instance()->applyQIconStyleHelper(mode, pm);
            if (!generated.isNull())
                pm = generated;
        }
    }

    if (!pm.isNull())
        QPixmapCache::insert(pmckey, pm);

    return pm;
}

void SvgxIconEngine::addPixmap(const QPixmap &pixmap, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(pixmap)
    Q_UNUSED(mode)
    Q_UNUSED(state)
}

void SvgxIconEngine::addFile(const QString &fileName, const QSize &size, QIcon::Mode mode,
                             QIcon::State state) {
    Q_UNUSED(size)
    Q_UNUSED(mode)
    Q_UNUSED(state)

    // Deserialize file name string
    QHash<QM::ButtonState, QString> fileMap;
    QHash<QM::ButtonState, QString> colorMap;
    if (!QMPrivate::deserializeSvgxArgs(fileName, &fileMap, &colorMap)) {
        return;
    }

    // Initialize values
    d->setup(fileMap, colorMap);
}

void SvgxIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode,
                           QIcon::State state) {
    QSize pixmapSize = rect.size();
    if (painter->device())
        pixmapSize *= painter->device()->devicePixelRatioF();
    painter->drawPixmap(rect, pixmap(pixmapSize, mode, state));
}

QString SvgxIconEngine::key() const {
    return QLatin1String("svgx");
}

QIconEngine *SvgxIconEngine::clone() const {
    return new SvgxIconEngine(*this);
}

namespace {

    QDataStream &operator>>(QDataStream &in, SvgxIconEnginePrivate::SvgScript &s) {
        in >> s.fileName;
        in >> s.data;
        in >> s.hasCurrentColor;
        return in;
    }

    QDataStream &operator<<(QDataStream &out, const SvgxIconEnginePrivate::SvgScript &s) {
        out << s.fileName;
        out << s.data;
        out << s.hasCurrentColor;
        return out;
    }

}

bool SvgxIconEngine::read(QDataStream &in) {
    d = new SvgxIconEnginePrivate();

    in >> d->svgScripts;
    if (in.status() != QDataStream::Ok) {
        return false;
    }

    in >> d->svgColors;
    if (in.status() != QDataStream::Ok) {
        return false;
    }

    return true;
}

bool SvgxIconEngine::write(QDataStream &out) const {
    out << d->svgScripts;
    out << d->svgColors;
    return true;
}

void SvgxIconEngine::virtual_hook(int id, void *data) {
    switch (id) {
        case QIconEngine::IsNullHook: {
            *reinterpret_cast<bool *>(data) = false;
            return;
        }

        case QMPrivate::Create: {
            auto a = reinterpret_cast<void **>(data);
            const auto &fileMap = *reinterpret_cast<QHash<QM::ButtonState, QString> *>(a[0]);
            const auto &colorMap = *reinterpret_cast<QHash<QM::ButtonState, QString> *>(a[1]);
            d->setup(fileMap, colorMap);
            return;
        }

        case QMPrivate::GetState: {
            auto a = reinterpret_cast<void **>(data);
            auto &res = *reinterpret_cast<QM::ButtonState *>(a[0]);
            res = d->currentState;
            return;
        }

        case QMPrivate::SetState: {
            auto a = reinterpret_cast<void **>(data);
            const auto &state = *reinterpret_cast<QM::ButtonState *>(a[0]);
            d->currentState = state;
            d->colorHint.clear();
            return;
        }

        case QMPrivate::GetColor: {
            auto a = reinterpret_cast<void **>(data);
            const auto &state = *reinterpret_cast<QM::ButtonState *>(a[0]);
            auto &res = *reinterpret_cast<QString *>(a[1]);
            res = d->svgColors[state];
            return;
        }

        case QMPrivate::SetColor: {
            auto a = reinterpret_cast<void **>(data);
            const auto &color = *reinterpret_cast<QString *>(a[0]);
            d->colorHint = color;
            return;
        }

        default:
            break;
    }
    QIconEngine::virtual_hook(id, data);
}