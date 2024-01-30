#ifndef QMCOREDECORATORV2_P_H
#define QMCOREDECORATORV2_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QtMediate API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include <QHash>
#include <QMap>
#include <QSet>
#include <QTranslator>

#include <QMCore/qmcoredecoratorv2.h>

class QM_CORE_EXPORT QMCoreDecoratorV2Private : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(QMCoreDecoratorV2)
public:
    QMCoreDecoratorV2Private();
    virtual ~QMCoreDecoratorV2Private();

    void init();

    void scanTranslations() const;

    void insertTranslationFiles_helper(const QMap<QString, QStringList> &map) const;

    QMCoreDecoratorV2 *q_ptr;

    QSet<QString> translationPaths;
    QList<QTranslator *> translators;
    QString currentLocale;
    QHash<QObject *, QList<std::function<void()>>> localeSubscribers;

    mutable bool qmFilesDirty;
    mutable QMap<QString, QStringList> qmFiles;

private:
    void _q_localeSubscriberDestroyed();
};

#endif // QMCOREDECORATORV2_P_H
