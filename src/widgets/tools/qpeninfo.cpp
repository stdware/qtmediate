#include "QPenInfo.h"

#include <QDebug>

#include <QMCore/qmbatch.h>

#include "qpixelsize.h"
#include "qmbuttonstate_p.h"
#include "qmcss_p.h"

static const char QCssCustomValue_Pen_Line_None[] = "none";
static const char QCssCustomValue_Pen_Line_Solid[] = "solid";
static const char QCssCustomValue_Pen_Line_Dash[] = "dash";
static const char QCssCustomValue_Pen_Line_Dot[] = "dot";
static const char QCssCustomValue_Pen_Line_DashDot[] = "dashdot";
static const char QCssCustomValue_Pen_Line_DashDotDot[] = "dashdotdot";
static const char QCssCustomValue_Pen_Cap_Flat[] = "flat";
static const char QCssCustomValue_Pen_Cap_Square[] = "square";
static const char QCssCustomValue_Pen_Cap_Round[] = "round";
static const char QCssCustomValue_Pen_Join_Miter[] = "miter";
static const char QCssCustomValue_Pen_Join_Bevel[] = "bevel";
static const char QCssCustomValue_Pen_Join_Round[] = "round";

static Qt::PenStyle StringToLineStyle(const QString &str, Qt::PenStyle defaultValue) {
    Qt::PenStyle style = defaultValue;
    if (!str.compare(QLatin1String(QCssCustomValue_Pen_Line_None), Qt::CaseInsensitive)) {
        style = Qt::NoPen;
    } else if (!str.compare(QLatin1String(QCssCustomValue_Pen_Line_Solid), Qt::CaseInsensitive)) {
        style = Qt::SolidLine;
    } else if (!str.compare(QLatin1String(QCssCustomValue_Pen_Line_Dash), Qt::CaseInsensitive)) {
        style = Qt::DashLine;
    } else if (!str.compare(QLatin1String(QCssCustomValue_Pen_Line_Dot), Qt::CaseInsensitive)) {
        style = Qt::DotLine;
    } else if (!str.compare(QLatin1String(QCssCustomValue_Pen_Line_DashDot), Qt::CaseInsensitive)) {
        style = Qt::DashDotLine;
    } else if (!str.compare(QLatin1String(QCssCustomValue_Pen_Line_DashDotDot),
                            Qt::CaseInsensitive)) {
        style = Qt::DashDotDotLine;
    }
    return style;
}

static Qt::PenCapStyle StringToCapStyle(const QString &str, Qt::PenCapStyle defaultValue) {
    Qt::PenCapStyle style = defaultValue;
    if (!str.compare(QLatin1String(QCssCustomValue_Pen_Cap_Flat), Qt::CaseInsensitive)) {
        style = Qt::FlatCap;
    } else if (!str.compare(QLatin1String(QCssCustomValue_Pen_Cap_Square), Qt::CaseInsensitive)) {
        style = Qt::SquareCap;
    } else if (!str.compare(QLatin1String(QCssCustomValue_Pen_Cap_Round), Qt::CaseInsensitive)) {
        style = Qt::RoundCap;
    }
    return style;
}

static Qt::PenJoinStyle StringToJoinStyle(const QString &str, Qt::PenJoinStyle defaultValue) {
    Qt::PenJoinStyle style = defaultValue;
    if (!str.compare(QLatin1String(QCssCustomValue_Pen_Join_Miter), Qt::CaseInsensitive)) {
        style = Qt::MiterJoin;
    } else if (!str.compare(QLatin1String(QCssCustomValue_Pen_Join_Bevel), Qt::CaseInsensitive)) {
        style = Qt::BevelJoin;
    } else if (!str.compare(QLatin1String(QCssCustomValue_Pen_Join_Round), Qt::CaseInsensitive)) {
        style = Qt::RoundJoin;
    }
    return style;
}

class QPenInfoData : public QSharedData {
public:
    QMButtonAttributes<QBrush> brushes;
};

/*!
    \class QPenInfo
    \brief QPenInfo is a wrapper of QPen.

    \warning This class is derived from a non-polymorphic class, don't use its super class pointer
             to delete the derived instance!
*/

/*!
    Constructs a default black solid line pen with 1 width.
*/
QPenInfo::QPenInfo() : d(new QPenInfoData()) {
}

/*!
    Constructs a black pen with 1 width and the given style
*/
QPenInfo::QPenInfo(Qt::PenStyle style) : QPenInfo() {
    setStyle(style);
}

/*!
    Constructs a solid line pen with 1 width and the given color.
*/
QPenInfo::QPenInfo(const QColor &color) : QPenInfo() {
    setBrush(color);
}

/*!
    Constructs a pen with the specified brush, width, pen style, cap style and join style.
*/
QPenInfo::QPenInfo(const QBrush &brush, qreal width, Qt::PenStyle s, Qt::PenCapStyle c,
                   Qt::PenJoinStyle j)
    : QPenInfo() {
    setBrush(brush);
    setWidthF(width);
    setStyle(s);
    setCapStyle(c);
    setJoinStyle(j);
}

/*!
    Destructor.
*/
QPenInfo::~QPenInfo() {
}

QPenInfo::QPenInfo(const QPenInfo &other) : QPen(other), d(other.d) {
}

QPenInfo::QPenInfo(QPenInfo &&other) noexcept : QPen(other), d(other.d) {
    other.d = nullptr;
}

QPenInfo &QPenInfo::operator=(const QPenInfo &other) {
    if (this == &other)
        return *this;
    QPen::operator=(other);
    d = other.d;
    return *this;
}

QPenInfo &QPenInfo::operator=(QPenInfo &&other) noexcept {
    QPen::operator=(other);
    qSwap(d, other.d);
    return *this;
}

/*!
    Converts to QPen with the brush of a button state.
*/
QPen QPenInfo::toPen(QM::ButtonState state) const {
    QPen pen = *this;
    if (state != QM::ButtonNormal)
        pen.setBrush(d->brushes.value(state));
    return pen;
}

/*!
    Returns the brush value of a button state.
*/
QBrush QPenInfo::brush(QM::ButtonState state) const {
    return d->brushes.value(state);
}

/*!
    Sets the brush value of a button state.
*/
void QPenInfo::setBrush(const QBrush &brush, QM::ButtonState state) {
    d->brushes.setValue(brush, state);
    if (state == QM::ButtonNormal)
        QPen::setBrush(brush);
}

/*!
    Sets the brush value of multiple button states.
*/
void QPenInfo::setBrushes(const QList<QBrush> &brushes) {
    d->brushes.setValues(brushes);
    if (brushes.size() > 0)
        QPen::setBrush(brushes.front());
}

/*!
    Converts a string list to QPenInfo, the string list should be the form as
    <tt>["qpen", "..."]</tt>.

    \sa QMCssType
*/
QPenInfo QPenInfo::fromStringList(const QStringList &stringList) {
    if (stringList.size() != 2 ||
        stringList.front().compare(metaFunctionName(), Qt::CaseInsensitive) != 0) {
        return {};
    }
    const auto &strData = stringList.at(1);

    auto args = QMCss::parseArgList(strData.trimmed(),
                                    {
                                        "color",
                                        "width",
                                        "style",
                                        "cap",
                                        "join",
                                        "dashPattern",
                                        "dashOffset",
                                        "miterLimit",
                                        "cosmetic",
                                    },
                                    {});

    auto it = args.find("color");
    if (it == args.end())
        return {};

    QPenInfo res;

    QString colorStrings[8];
    const auto &colorExpressions = it->trimmed();
    if (colorExpressions.startsWith('(') && colorExpressions.endsWith(')')) {
        QMCss::parseButtonStateList(colorExpressions.mid(1, colorExpressions.size() - 2),
                                    colorStrings, false);

        for (int i = 0; i < 8; ++i) {
            if (colorStrings[i].isEmpty())
                continue;
            res.setBrush(QMCss::parseColor(colorStrings[i]), static_cast<QM::ButtonState>(i));
        }
    } else {
        res.setColor(QMCss::parseColor(colorExpressions));
    }

    it = args.find("width");
    if (it != args.end()) {
        res.setWidthF(QPixelSize::fromString(it.value()));
    }

    it = args.find("style");
    if (it != args.end()) {
        res.setStyle(StringToLineStyle(it.value(), res.style()));
    }

    it = args.find("cap");
    if (it != args.end()) {
        res.setCapStyle(StringToCapStyle(it.value(), res.capStyle()));
    }

    it = args.find("join");
    if (it != args.end()) {
        res.setJoinStyle(StringToJoinStyle(it.value(), res.joinStyle()));
    }

    it = args.find("dashPattern");
    if (it != args.end()) {
        res.setDashPattern(
            QMCss::parseSizeFValueList(QM::strRemoveSideParen(it.value())).toVector());
    }

    it = args.find("dashOffset");
    if (it != args.end()) {
        res.setDashOffset(QPixelSize::fromString(it.value()));
    }

    it = args.find("miterLimit");
    if (it != args.end()) {
        res.setMiterLimit(QPixelSize::fromString(it.value()));
    }

    it = args.find("cosmetic");
    if (it != args.end()) {
        res.setCosmetic(QMCss::parseBoolean(it.value()));
    }

    return res;
}

/*!
    QPenInfo identifier when converting from a string representing as function call.

    \sa QMCssType::parse()
*/
const char *QPenInfo::metaFunctionName() {
    return "qpen";
}

QDebug operator<<(QDebug debug, const QPenInfo &info) {
    debug << info.toPen();
    return debug;
}

namespace {
    struct initializer {
        initializer() {
            QMetaType::registerConverter<QStringList, QPenInfo>(QPenInfo::fromStringList);
            QMCssType::registerMetaTypeName(qMetaTypeId<QPenInfo>(), QPenInfo::metaFunctionName());
        }
        ~initializer() {
            QMetaType::unregisterConverterFunction(qMetaTypeId<QStringList>(),
                                                   qMetaTypeId<QPenInfo>());
            QMCssType::unregisterMetaTypeName(qMetaTypeId<QPenInfo>());
        }
    } dummy;
}