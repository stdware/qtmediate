#include "qrectinfo.h"

#include <QColor>
#include <QDebug>

#include <QMCore/qmbatch.h>

#include "qpixelsize.h"
#include "qmbuttonstate_p.h"
#include "qmcss_p.h"

class QRectInfoData : public QSharedData {
public:
    QRectInfoData() {
        for (auto &item : nums)
            item = -1;
        numSize = 0;
        radius = 0;

        colors.setValueAll(Qt::transparent);
    }

    int nums[4];
    int numSize;

    QMButtonAttributes<QColor> colors;

    int radius;
};

/*!
    \class QRectInfo
    \brief QRectInfo is a wrapper of QRect and extended properties.
*/

/*!
    Constructor.
*/
QRectInfo::QRectInfo() : d(new QRectInfoData()) {
}

/*!
    Destructor.
*/
QRectInfo::~QRectInfo() {
}

QRectInfo::QRectInfo(const QRectInfo &other) : d(other.d) {
}

QRectInfo::QRectInfo(QRectInfo &&other) noexcept : d(other.d) {
    other.d = nullptr;
}

QRectInfo &QRectInfo::operator=(const QRectInfo &other) {
    if (this == &other)
        return *this;
    d = other.d;
    return *this;
}

QRectInfo &QRectInfo::operator=(QRectInfo &&other) noexcept {
    qSwap(d, other.d);
    return *this;
}

/*!
    Returns the rect value of the internal data.
*/
QRect QRectInfo::rect() const {
    switch (d->numSize) {
        case 1:
            return {0, 0, d->nums[0], d->nums[0]};
            break;
        case 2:
            return {0, 0, d->nums[0], d->nums[1]};
            break;
        case 3:
            return {d->nums[0], d->nums[1], d->nums[2], d->nums[2]};
            break;
        case 4:
            return {d->nums[0], d->nums[1], d->nums[2], d->nums[3]};
            break;
        default:
            break;
    }
    return {};
}

/*!
    Sets the rect value of the internal data.
*/
void QRectInfo::setRect(const QRect &rect) {
    d->nums[0] = rect.x();
    d->nums[1] = rect.y();
    d->nums[2] = rect.width();
    d->nums[3] = rect.height();
    d->numSize = 4;
}

/*!
    Returns the margins value of the internal data.
*/
QMargins QRectInfo::margins() const {
    switch (d->numSize) {
        case 1:
            return {d->nums[0], d->nums[0], d->nums[0], d->nums[0]};
            break;
        case 2:
        case 3:
            return {d->nums[1], d->nums[0], d->nums[1], d->nums[0]};
            break;
        case 4:
            return {d->nums[0], d->nums[1], d->nums[2], d->nums[3]};
            break;
        default:
            break;
    }
    return {};
}

/*!
    Sets the margin value of the internal data.
*/
void QRectInfo::setMargins(const QMargins &margins) {
    d->nums[0] = margins.left();
    d->nums[1] = margins.top();
    d->nums[2] = margins.right();
    d->nums[3] = margins.bottom();
    d->numSize = 4;
}

/*!
    Returns the color value of a button state.
*/
QColor QRectInfo::color(QM::ButtonState state) const {
    return d->colors.value(state);
}

/*!
    Sets the color value of a button state.
*/
void QRectInfo::setColor(const QColor &color, QM::ButtonState state) {
    d->colors.setValue(color, state);
}

/*!
    Sets the color value of multiple button states.
*/
void QRectInfo::setColors(const QList<QColor> &colors) {
    d->colors.setValues(colors);
}

/*!
    Returns the radius property.
*/
int QRectInfo::radius() const {
    return d->radius;
}

/*!
    Sets the radius property.
*/
void QRectInfo::setRadius(int r) {
    d->radius = r;
}

/*!
    \internal
*/
static QByteArray metaFunctionName() {
    return QByteArrayLiteral("qrect");
}

/*!
    Converts a string list to QRectInfo, the string list should be the form as
    <tt>["qrect", "..."]</tt>.

    \sa QMCssType
*/
QRectInfo QRectInfo::fromStringList(const QStringList &stringList) {
    if (stringList.size() != 2 ||
        stringList.front().compare(metaFunctionName(), Qt::CaseInsensitive) != 0) {
        return {};
    }
    const auto &strData = stringList.at(1);

    auto args = QMCss::parseArgList(strData.trimmed(),
                                    {
                                        "color",
                                        "numbers",
                                        "radius",
                                    },
                                    {});

    auto it = args.find("color");
    if (it == args.end())
        return {};

    QRectInfo res;

    QString colorStrings[8];
    const auto &colorExpressions = it->trimmed();
    if (colorExpressions.startsWith('(') && colorExpressions.endsWith(')')) {
        QMCss::parseButtonStateList(colorExpressions.mid(1, colorExpressions.size() - 2),
                                    colorStrings, false);

        for (int i = 0; i < 8; ++i) {
            if (colorStrings[i].isEmpty())
                continue;
            res.setColor(QMCss::parseColor(colorStrings[i]), static_cast<QM::ButtonState>(i));
        }
    } else {
        res.setColor(QMCss::parseColor(colorExpressions));
    }

    it = args.find("numbers");
    if (it != args.end()) {
        auto list = QMCss::parseSizeValueList(QM::strRemoveSideParen(it.value()));
        auto sz = qMin(4, list.size());
        for (int i = 0; i < sz; ++i) {
            res.d->nums[i] = list.at(i);
        }
        res.d->numSize = sz;
    }

    it = args.find("radius");
    if (it != args.end()) {
        res.setRadius(QPixelSize::fromString(it.value()));
    }

    return res;
}

QDebug operator<<(QDebug debug, const QRectInfo &info) {
    QDebugStateSaver saver(debug);

    debug.nospace().noquote() << "QRectInfo(" << info.color().name() << ", " << info.rect() << ", "
                              << info.radius() << ")";
    return debug;
}

namespace {
    struct initializer {
        initializer() {
            QMetaType::registerConverter<QStringList, QRectInfo>(QRectInfo::fromStringList);
            QMCssType::registerMetaTypeName(qMetaTypeId<QRectInfo>(), metaFunctionName());
        }
        ~initializer() {
            QMCssType::unregisterConverterFunction<QStringList, QRectInfo>();
            QMCssType::unregisterMetaTypeName(qMetaTypeId<QRectInfo>());
        }
    } dummy;
}