#include "svgxiconengineplugin.h"

#include <QStaticPlugin>

#include "svgxiconengine.h"

QIconEngine *SvgxIconEnginePlugin::create(const QString &fileName) {
    auto engine = new SvgxIconEngine();

    // Create empty engine
    if (fileName.compare(".svgx", Qt::CaseInsensitive) == 0) {
        return engine;
    }
    engine->addFile(fileName, {}, {}, {});
    return engine;
}

Q_IMPORT_PLUGIN(SvgxIconEnginePlugin)