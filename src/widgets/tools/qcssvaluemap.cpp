#include "qcssvaluemap.h"

#include <QDebug>

#include <QMCore/qmbatch.h>

#include "qmcss_p.h"

/*!
    \class QCssValueMap
    \brief QCssValueMap is a wrapper of QVariantMap.
*/

/*!
    \fn QCssValueMap::QCssValueMap()

    Default constructor.
*/

/*!
    \fn QCssValueMap::QCssValueMap(const QVariantMap &map)

    Constructs from a QVariantMap.
*/

/*!
    \fn QCssValueMap::QCssValueMap(std::initializer_list<std::pair<QString, QVariant>> list)

    Constructs from an intializer list.
*/

/*!
    \fn QCssValueMap::~QCssValueMap()

    Destructor.
*/

/*!
    \internal
*/
static QByteArray metaFunctionName() {
    return QByteArrayLiteral("qmap");
}

/*!
    Converts a string list to QCssValueMap, the string list should be the form as
    <tt>["qmap", "..."]</tt>.

    \sa QMCssType
*/
QCssValueMap QCssValueMap::fromStringList(const QStringList &stringList) {
    if (stringList.size() != 2 ||
        stringList.front().compare(metaFunctionName(), Qt::CaseInsensitive) != 0) {
        return {};
    }
    const auto &strData = stringList.at(1);

    QCssValueMap res;
    QStringList valueList = QMCss::parseStringValueList(strData);

    for (auto item : qAsConst(valueList)) {
        item = item.trimmed();
        auto eq = QMCss::indexOfEqSign(item);
        if (eq < 0)
            continue;
        res.get().insert(QM::strRemoveSideQuote(item.left(eq).trimmed(), true),
                         QMCssType::parse(item.mid(eq + 1).trimmed()));
    }
    return res;
}

QDebug operator<<(QDebug debug, const QCssValueMap &map) {
    debug << map.get();
    return debug;
}

namespace {
    struct initializer {
        initializer() {
            QMetaType::registerConverter<QStringList, QCssValueMap>(QCssValueMap::fromStringList);
            QMCssType::registerMetaTypeName(qMetaTypeId<QCssValueMap>(), metaFunctionName());
        }
        ~initializer() {
            QMCssType::unregisterConverterFunction<QStringList, QCssValueMap>();
            QMCssType::unregisterMetaTypeName(qMetaTypeId<QCssValueMap>());
        }
    } dummy;
}