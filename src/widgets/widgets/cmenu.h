#ifndef CMENU_H
#define CMENU_H

#include <QMenu>

#include <QMWidgets/qmwidgetsglobal.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using CMenu = QMenu;
#else

class CMenuPrivate;

class QM_WIDGETS_EXPORT CMenu : public QMenu {
    Q_OBJECT
public:
    explicit CMenu(QWidget *parent = nullptr);
    explicit CMenu(const QString &title, QWidget *parent = nullptr);
    ~CMenu();

protected:
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;

private:
    CMenuPrivate *d;
};

#endif

#endif // CMENU_H
