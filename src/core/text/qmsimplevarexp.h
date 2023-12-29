#ifndef QMSIMPLEVAREXP_H
#define QMSIMPLEVAREXP_H

#include <QJsonObject>

#include <QMCore/qmglobal.h>

class QM_CORE_EXPORT QMSimpleVarExp {
public:
    QMSimpleVarExp();
    explicit QMSimpleVarExp(const QString &pattern);
    ~QMSimpleVarExp();

    QString Pattern;
    QHash<QString, QString> Variables;

public:
    void addJsonObject(const QJsonObject &obj);
    void addVariantMap(const QVariantMap &map);
    void addVariantHash(const QVariantHash &hash);
    void addMap(const QMap<QString, QString> &map);
    void addHash(const QHash<QString, QString> &hash);
    void add(const QString &key, const QString &value);
    void clear();

    QString parse(const QString &exp) const;

public:
    static QHash<QString, QString> systemValues();

    static QString evaluate(const QString &s, const QHash<QString, QString> &dict,
                            const QString &pattern = {});
};

#endif // QMSIMPLEVAREXP_H