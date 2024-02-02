#include "QFontInfoEx.h"

#include <QColor>
#include <QDebug>
#include <QGuiApplication>

#include <QMCore/QMBatch.h>

#include "qmbuttonstate_p.h"
#include "qmcss_p.h"

class QFontInfoExData : public QSharedData {
public:
    QFontInfoExData() {
        weight = -1;
        italic = -1;
        point = -1;
        pixel = -1;
    }

    int weight;
    int italic;
    double point;
    int pixel;
    QStringList families;
    QMButtonAttributes<QColor> colors;

    void copyAttributes(QFont &font) const {
        if (weight >= 0) {
            font.setWeight(weight);
        }

        if (italic >= 0) {
            font.setItalic(italic);
        }

        if (point > 0) {
            font.setPointSizeF(point);
        } else if (pixel > 0) {
            font.setPixelSize(pixel);
        }

        if (!families.isEmpty()) {
            if (families.size() == 1)
                font.setFamily(families.front());
            else
                font.setFamilies(families);
        }
    }
};

/*!
    \class QFontInfoEx
    \brief QFontInfoEx is a wrapper of QFont.
*/

/*!
    Constructs with the application's default font.
*/
QFontInfoEx::QFontInfoEx() : d(new QFontInfoExData()) {
}

/*!
    Constructs with the specified family, pointSize, weight and italic settings.
*/
QFontInfoEx::QFontInfoEx(const QString &family, int pixelSize, int weight, bool italic)
    : QFontInfoEx() {
    d->families = QStringList{family};
    d->pixel = pixelSize;
    d->weight = weight;
    d->italic = italic;
}

/*!
    Destructor.
*/
QFontInfoEx::~QFontInfoEx() {
}

QFontInfoEx::QFontInfoEx(const QFontInfoEx &other) : d(other.d) {
}

QFontInfoEx::QFontInfoEx(QFontInfoEx &&other) noexcept : d(other.d) {
    other.d = nullptr;
}

/*!
    Copy the explicitly specified attributes to the base font and return the new font.

    If the given QObject has a property named \c font , use it as the base font, otherwise, use the
    application's default font.
*/
QFont QFontInfoEx::toFont(const QObject *obj) const {
    QFont font = QGuiApplication::font();
    if (obj) {
        auto var = obj->property("font");
        if (var.type() == QVariant::Font) {
            font = var.value<QFont>();
        }
    }

    d->copyAttributes(font);

    return font;
}

QFontInfoEx &QFontInfoEx::operator=(const QFontInfoEx &other) {
    if (this == &other)
        return *this;
    d = other.d;
    return *this;
}

QFontInfoEx &QFontInfoEx::operator=(QFontInfoEx &&other) noexcept {
    qSwap(d, other.d);
    return *this;
}

/*!
    Sets the font weight.
*/
int QFontInfoEx::weight() const {
    return d->weight;
}

/*!
    Returns the font weight
*/
void QFontInfoEx::setWeight(int weight) {
    d->weight = weight;
}

/*!
    Returns the font italic property.
*/
bool QFontInfoEx::italic() const {
    return d->italic < 0 ? false : bool(d->italic);
}

/*!
    Sets the font italic property.
*/
void QFontInfoEx::setItalic(bool italic) {
    d->italic = italic;
}

/*!
    Returns the font point size.
*/
double QFontInfoEx::pointSize() const {
    return d->point;
}

/*!
    Sets the font point size.
*/
void QFontInfoEx::setPointSize(double pointSize) {
    d->point = pointSize;
    d->pixel = -1;
}

/*!
    Returns the font pixel size.
*/
int QFontInfoEx::pixelSize() const {
    return d->pixel;
}

/*!
    Sets the font pixel size.
*/
void QFontInfoEx::setPixelSize(int pixelSize) {
    d->pixel = pixelSize;
    d->point = -1;
}

/*!
    Returns the font families.
*/
QStringList QFontInfoEx::families() const {
    return d->families;
}

/*!
    Sets the font families.
*/
void QFontInfoEx::setFamilies(const QStringList &families) {
    d->families = families;
}

/*!
    Returns the color value of a button state.
*/
QColor QFontInfoEx::color(QM::ButtonState state) const {
    return d->colors.value(state);
}

/*!
    Sets the color value of a button state.
*/
void QFontInfoEx::setColor(const QColor &color, QM::ButtonState state) {
    d->colors.setValue(color, state);
}

/*!
    Sets the color value of multiple button states.
*/
void QFontInfoEx::setColors(const QList<QColor> &colors) {
    d->colors.setValues(colors);
}

static int StringToWeight(const QString &str, int defaultValue) {
    int weight = defaultValue;
    if (!str.compare(QLatin1String("thin"), Qt::CaseInsensitive)) {
        weight = QFont::Thin;
    } else if (!str.compare(QLatin1String("extralight"), Qt::CaseInsensitive)) {
        weight = QFont::ExtraLight;
    } else if (!str.compare(QLatin1String("light"), Qt::CaseInsensitive)) {
        weight = QFont::Light;
    } else if (!str.compare(QLatin1String("normal"), Qt::CaseInsensitive)) {
        weight = QFont::Normal;
    } else if (!str.compare(QLatin1String("medium"), Qt::CaseInsensitive)) {
        weight = QFont::Medium;
    } else if (!str.compare(QLatin1String("demibold"), Qt::CaseInsensitive)) {
        weight = QFont::DemiBold;
    } else if (!str.compare(QLatin1String("bold"), Qt::CaseInsensitive)) {
        weight = QFont::Bold;
    } else if (!str.compare(QLatin1String("extrabold"), Qt::CaseInsensitive)) {
        weight = QFont::ExtraBold;
    } else if (!str.compare(QLatin1String("black"), Qt::CaseInsensitive)) {
        weight = QFont::Black;
    }
    return weight;
}

/*!
    Converts a string list to QFontInfoEx, the string list should be the form as
    <tt>["qfont", "..."]</tt>.

    \sa QMCssType
*/
QFontInfoEx QFontInfoEx::fromStringList(const QStringList &stringList) {
    if (stringList.size() != 2 ||
        stringList.front().compare(metaFunctionName(), Qt::CaseInsensitive) != 0) {
        return {};
    }
    const auto &strData = stringList.at(1);

    auto args = QMCss::parseArgList(strData.trimmed(),
                                    {
                                        "color",
                                        "size",
                                        "weight",
                                        "italic",
                                        "family",
                                    },
                                    {});

    auto it = args.find("color");
    if (it == args.end())
        return {};

    QFontInfoEx res;

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

    it = args.find("size");
    if (it != args.end()) {
        const auto &val = it.value();

        if (val.endsWith("pt")) {
            // pt
            res.setPointSize(val.chopped(2).toDouble());
        } else if (val.endsWith("px")) {
            // px
            res.setPixelSize(val.chopped(2).toInt());
        } else {
            // fallback as px
            res.setPixelSize(val.toInt());
        }
    }

    it = args.find("weight");
    if (it != args.end()) {
        const auto &val = it.value();

        int num;
        bool ok;

        num = val.toInt(&ok);
        if (!ok) {
            num = StringToWeight(val, res.weight());
        }
        res.setWeight(num);
    }

    it = args.find("italic");
    if (it != args.end()) {
        res.setItalic(QMCss::parseBoolean(it.value()));
    }

    it = args.find("family");
    if (it != args.end()) {
        const auto &val = it.value();
        auto list = QMCss::parseStringValueList(QM::strRemoveSideParen(val.mid(1, val.size() - 2)));
        for (auto &item : list)
            item = QM::strRemoveSideQuote(item.trimmed());
        res.setFamilies(list);
    }

    return res;
}

/*!
    QFontInfoEx identifier when converting from a string representing as function call.

    \sa QMCssType::parse()
*/
const char *QFontInfoEx::metaFunctionName() {
    return "qfont";
}

QDebug operator<<(QDebug debug, const QFontInfoEx &info) {
    QDebugStateSaver saver(debug);

    QStringList list;
    list << info.color().name();
    if (info.pointSize() > 0) {
        list << QString::number(info.pointSize()) + "pt";
    } else if (info.pixelSize() > 0) {
        list << QString::number(info.pixelSize()) + "px";
    }
    list << QString::number(info.weight());
    if (info.italic()) {
        list << "italic";
    }
    list << "[" + info.families().join(", ") + "]";

    debug.nospace().noquote() << "QFontInfoEx(" << list.join(", ") << ")";
    return debug;
}

namespace {
    struct initializer {
        initializer() {
            QMetaType::registerConverter<QStringList, QFontInfoEx>(QFontInfoEx::fromStringList);
            QMCssType::registerMetaTypeName(qMetaTypeId<QFontInfoEx>(),
                                            QFontInfoEx::metaFunctionName());
        }
        ~initializer() {
            QMetaType::unregisterConverterFunction(qMetaTypeId<QStringList>(),
                                                   qMetaTypeId<QFontInfoEx>());
            QMCssType::unregisterMetaTypeName(qMetaTypeId<QFontInfoEx>());
        }
    } dummy;
}