#ifndef SVGXICONENGINEPLUGIN_H
#define SVGXICONENGINEPLUGIN_H

#define QT_STATICPLUGIN

#include <QIconEnginePlugin>

class SvgxIconEnginePlugin : public QIconEnginePlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QIconEngineFactoryInterface_iid FILE "plugin.json")
public:
    QIconEngine *create(const QString &fileName) override;
};

#endif // SVGXICONENGINEPLUGIN_H
