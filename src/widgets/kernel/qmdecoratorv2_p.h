#ifndef QMDECORATORV2_P_H
#define QMDECORATORV2_P_H

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
#include <QPointer>
#include <QSet>
#include <QTranslator>
#include <QWidget>

#include <QMCore/private/qmcoredecoratorv2_p.h>
#include <QMWidgets/qmdecoratorv2.h>

class QMDecoratorThemeGuardV2;

class QMDecoratorV2Private : public QMCoreDecoratorV2Private {
    Q_DECLARE_PUBLIC(QMDecoratorV2)
public:
    QMDecoratorV2Private();
    virtual ~QMDecoratorV2Private();

    void init();

    void scanForThemes() const;

    QSet<QString> themePaths;
    QHash<QWidget *, QMDecoratorThemeGuardV2 *> themeSubscribers;
    QString currentTheme;
    double fontRatio;
    double zoomRatio;

    mutable bool themeFilesDirty;
    mutable QMap<QString, QMap<QString, QString>>
        stylesheetCaches;                                      // themeKey - [ namespace - content ]
    mutable QHash<QString, QStringList> nsMappings;            // widgetKey - namespaces
    mutable QHash<QString, QHash<QString, QString>> variables; // themeKey - [ varKey - var ]

    // static int globalImageCacheSerialNum;

    static QString replaceFontSizes(const QString &stylesheet, double ratio, bool rounding);
    static QString replaceSizes(const QString &stylesheet, double ratio, bool rounding);
    static QString replaceCustomKeyWithQProperty(const QString &stylesheet);
    static QString replaceCssGrammars(const QString &stylesheet);
    static QString removeAllComments(const QString &stylesheet);

    bool hasPendingRefreshTask;

private:
    void _q_themeSubscriberDestroyed();
};

#endif // QMDECORATORV2_P_H
