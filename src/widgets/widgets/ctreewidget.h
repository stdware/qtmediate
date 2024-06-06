#ifndef CTREEWIDGET_H
#define CTREEWIDGET_H

#include <QTreeWidget>

#include <QMWidgets/qmwidgetsglobal.h>

class QM_WIDGETS_EXPORT CTreeWidget : public QTreeWidget {
    Q_OBJECT
public:
    explicit CTreeWidget(QWidget *parent = nullptr);
    ~CTreeWidget();

public:
    static QTreeWidgetItem *index2item(const QModelIndex &index);

    void setItemExpandedRecursively(QTreeWidgetItem *item, bool expanded);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dropEvent(QDropEvent *event) override;

Q_SIGNALS:
    void itemClickedEx(QTreeWidgetItem *item, int column, Qt::MouseButton button);
    void itemDropped(QTreeWidgetItem *item, int index);
};

#endif // CTREEWIDGET_H
