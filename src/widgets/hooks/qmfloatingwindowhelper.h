#ifndef QMFLOATINGWINDOWHELPER_H
#define QMFLOATINGWINDOWHELPER_H

#include <QMargins>
#include <QObject>

#include <QMWidgets/qmwidgetsglobal.h>

class QMFloatingWindowHelperPrivate;

class QM_WIDGETS_EXPORT QMFloatingWindowHelper : public QObject {
    Q_OBJECT
public:
    explicit QMFloatingWindowHelper(QWidget *w, QObject *parent = nullptr);
    ~QMFloatingWindowHelper();

public:
    bool floating() const;
    void setFloating(bool floating, Qt::WindowFlags flags = Qt::Window);

    QMargins resizeMargins() const;
    void setResizeMargins(const QMargins &resizeMargins);

    static bool isTitleBarWidget(QWidget *w);
    static void setTilteBarWidget(QWidget *w, bool on);

Q_SIGNALS:
    void clicked(Qt::MouseButton button);
    void doubleClicked(Qt::MouseButton button);

private:
    QMFloatingWindowHelperPrivate *d;
};

#endif // QMFLOATINGWINDOWHELPER_H
