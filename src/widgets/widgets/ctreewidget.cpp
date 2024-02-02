#include "ctreewidget.h"

#include <private/qtreewidget_p.h>

/*!
    \class CTreeWidget
    \brief CTreeWidget is extended from QTreeWidget, providing a few more APIs.
*/

/*!
    Constructor.

    The widget font will be set as the application font, which QTreeWidget doesn't.
*/
CTreeWidget::CTreeWidget(QWidget *parent) : QTreeWidget(parent) {
    setFont(qApp->font());
}

/*!
    Destructor.
*/
CTreeWidget::~CTreeWidget() {
}

/*!
    Converts a QModelIndex to the item on the tree widget, return \c null if the index is invalid.
*/
QTreeWidgetItem *CTreeWidget::index2item(const QModelIndex &index) {
    if (!index.isValid()) {
        return nullptr;
    }
    return static_cast<QTreeWidgetItem *>(index.internalPointer());
}

/*!
    Makes the item collapse or expend based on the \c expended argument.
*/
void CTreeWidget::setItemExpandedRecursively(QTreeWidgetItem *item, bool expanded) {
    std::list<QTreeWidgetItem *> queue;
    queue.push_back(item);
    while (!queue.empty()) {
        auto curItem = queue.back();
        queue.pop_back();
        curItem->setExpanded(expanded);
        for (int i = 0; i < curItem->childCount(); ++i) {
            auto childItem = curItem->child(i);
            queue.push_back(childItem);
        }
    }
}

void CTreeWidget::mouseReleaseEvent(QMouseEvent *event) {
    QTreeWidget::mouseReleaseEvent(event);

    auto d = static_cast<QTreeWidgetPrivate *>(d_ptr.data());
    QPoint pos = event->pos();
    QPersistentModelIndex index = indexAt(pos);
    bool click = (index == d->pressedIndex && index.isValid());

    if (click) {
        auto item = index2item(index);
        emit itemClickedEx(item, index.column(), event->button());
    }
}

/*!
    \fn void CTreeWidget::itemClickedEx(QTreeWidgetItem *item, int column, Qt::MouseButton button)

    This signal is emitted when the mouse click on an item, which also tells the slot
    which mouse button is clicked.
*/