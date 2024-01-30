#ifndef SVGXICONENGINE_H
#define SVGXICONENGINE_H

#include <QIconEngine>
#include <QSharedData>

#include <QMCore/QMNamespace.h>

class SvgxIconEnginePrivate;

class SvgxIconEngine : public QIconEngine {
public:
    SvgxIconEngine();
    SvgxIconEngine(const SvgxIconEngine &other);
    ~SvgxIconEngine();

public:
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;

    void addPixmap(const QPixmap &pixmap, QIcon::Mode mode, QIcon::State state) override;
    void addFile(const QString &fileName, const QSize &size, QIcon::Mode mode,
                 QIcon::State state) override;

    QString key() const override;
    QIconEngine *clone() const override;
    bool read(QDataStream &in) override;
    bool write(QDataStream &out) const override;

    void virtual_hook(int id, void *data) override;

private:
    QSharedDataPointer<SvgxIconEnginePrivate> d;

    friend class SvgxIconEnginePlugin;
};

#endif // SVGXICONENGINE_H
