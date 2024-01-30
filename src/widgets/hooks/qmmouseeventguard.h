#ifndef QMMOUSEEVENTGUARD_H
#define QMMOUSEEVENTGUARD_H

#include <QMouseEvent>

#include <QMWidgets/QMWidgetsGlobal.h>

class QMMouseEventGuardPrivate;

class QM_WIDGETS_EXPORT QMMouseEventGuard : public QObject {
    Q_OBJECT
public:
    explicit QMMouseEventGuard(QWidget *parent = nullptr);
    ~QMMouseEventGuard();

public:
    void addButton(Qt::MouseButton button, const char *slot);
    void addButton(Qt::MouseButton button, const std::function<void(QMouseEvent *)> &slot);
    void removeButton(Qt::MouseButton button);

private:
    QMMouseEventGuardPrivate *d;
};

#endif // QMMOUSEEVENTGUARD_H
