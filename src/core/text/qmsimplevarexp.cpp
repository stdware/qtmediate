#include "qmsimplevarexp.h"

#include "QMSystem.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

static const char Slash = '/';

static const char DefaultPattern[] = R"(\$\{(\w+)\})";

/*!
    \class QMSimpleVarExp
    \brief Variable expression parsing class.
*/

/*!
    Constructs an instance with the default escaping pattern <tt>${XXX}</tt>.
*/
QMSimpleVarExp::QMSimpleVarExp() : QMSimpleVarExp(DefaultPattern) {
}

/*!
    Constructs an instance with the given escaping pattern, should be a regular expression.
*/
QMSimpleVarExp::QMSimpleVarExp(const QString &pattern) : Pattern(pattern) {
}

/*!
    Destructor.
*/
QMSimpleVarExp::~QMSimpleVarExp() {
}

/*!
    Adds a json object to the searching map, only string values will be included.
*/
void QMSimpleVarExp::addJsonObject(const QJsonObject &obj) {
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        if (it->isString()) {
            Variables.insert(it.key(), it->toString());
        }
    }
}

/*!
    Adds a variant map to the searching map, only string values will be included.
*/
void QMSimpleVarExp::addVariantMap(const QVariantMap &map) {
    for (auto it = map.begin(); it != map.end(); ++it) {
        if (it->type() == QVariant::String) {
            Variables.insert(it.key(), it->toString());
        }
    }
}

/*!
    Adds a variant hash to the searching map, only string values will be included.
*/
void QMSimpleVarExp::addVariantHash(const QVariantHash &hash) {
    for (auto it = hash.begin(); it != hash.end(); ++it) {
        if (it->type() == QVariant::String) {
            Variables.insert(it.key(), it->toString());
        }
    }
}

/*!
    Adds a string map to the searching map.
*/
void QMSimpleVarExp::addMap(const QMap<QString, QString> &map) {
    for (auto it = map.begin(); it != map.end(); ++it) {
        Variables.insert(it.key(), it.value());
    }
}

/*!
    Adds a string hash to the searching map.
*/
void QMSimpleVarExp::addHash(const QHash<QString, QString> &hash) {
    for (auto it = hash.begin(); it != hash.end(); ++it) {
        Variables.insert(it.key(), it.value());
    }
}

/*!
    Adds a key-value pair to the searching map.
*/
void QMSimpleVarExp::add(const QString &key, const QString &value) {
    Variables.insert(key, value);
}

/*!
    Clears the searching map.
*/
void QMSimpleVarExp::clear() {
    Variables.clear();
}

/*!
    \internal
*/
static QString dfs(QString s, const QRegularExpression &reg, const QHash<QString, QString> &vars,
                   bool recursive = true) {
    QRegularExpressionMatch match;
    int index = 0;
    bool hasMatch = false;
    while ((index = s.indexOf(reg, index, &match)) != -1) {
        hasMatch = true;

        const auto &name = match.captured(1);
        QString val;
        auto it = vars.find(name);
        if (it == vars.end()) {
            val = name;
        } else {
            val = it.value();
        }

        s.replace(index, match.captured(0).size(), val);
    }
    if (!hasMatch) {
        return s;
    }
    return dfs(s, reg, vars);
}

/*!
    Parses the given expression.
*/
QString QMSimpleVarExp::parse(const QString &exp) const {
    return dfs(exp, QRegularExpression(Pattern), Variables);
}

/*!
    Returns a string hash containing common system environment variables.

    \li DESKTOP: Desktop location
    \li DOCUMENTS: Documents location
    \li APPLICATIONS: Applications location
    \li HOME: Home location
    \li APPDATA: Application data location
    \li TEMP: Temporary location
    \li ROOT: Filesystem root
    \li APPPATH: Application directory path
    \li APPNAME: Application name
*/
QHash<QString, QString> QMSimpleVarExp::systemValues() {
    return {
        {"DESKTOP",      QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)     },
        {"DOCUMENTS",    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)   },
        {"APPLICATIONS", QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)},
        {"HOME",         QStandardPaths::writableLocation(QStandardPaths::HomeLocation)        },
        {"APPDATA",      QMFs::appDataPath()                                                   },
        {"TEMP",         QDir::tempPath()                                                      },
        {"ROOT",         QDir::rootPath()                                                      },
        {"APPPATH",      qApp->applicationDirPath()                                            },
        {"APPNAME",      qApp->applicationName()                                               },
    };
}

/*!
    Parses a variable expression using the given variable \c dict .
*/
QString QMSimpleVarExp::evaluate(const QString &s, const QHash<QString, QString> &dict,
                                 const QString &pattern) {
    return dfs(s, QRegularExpression(pattern.isEmpty() ? DefaultPattern : pattern), dict);
}
