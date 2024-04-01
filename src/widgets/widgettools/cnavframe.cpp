#include "cnavframe.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QSplitter>
#include <QStackedLayout>

#include "ctabbutton.h"

class CNavFramePrivate : public QObject {
    Q_DECLARE_PUBLIC(CNavFrame)
public:
    CNavFrame *q_ptr;

    QHBoxLayout *layout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *buttonLayout;

    QWidget *topWidget;
    QWidget *bottomWidget;

    QStackedLayout *stack;

    QFrame *leftFrame;
    QFrame *rightFrame;

    QSplitter *splitter;

    QButtonGroup *btnGroup;

private:
    void _q_currentChanged(int index);
    void _q_buttonClicked(int id);
};

void CNavFramePrivate::_q_currentChanged(int index) {
    qobject_cast<QAbstractButton *>(buttonLayout->itemAt(index)->widget())->setChecked(true);
}

void CNavFramePrivate::_q_buttonClicked(int id) {
    Q_Q(CNavFrame);

    Q_UNUSED(id);
    q->setCurrentIndex(buttonLayout->indexOf(btnGroup->checkedButton()));
}

CNavFrame::CNavFrame(QWidget *parent) : QFrame(parent), d_ptr(new CNavFramePrivate()) {
    Q_D(CNavFrame);

    d->q_ptr = this;

    // Left
    d->buttonLayout = new QVBoxLayout();
    d->buttonLayout->setMargin(0);
    d->buttonLayout->setSpacing(0);

    d->leftLayout = new QVBoxLayout();
    d->leftLayout->setMargin(0);
    d->leftLayout->setSpacing(0);

    d->leftLayout->addLayout(d->buttonLayout);
    d->leftLayout->addStretch();

    d->leftFrame = new QFrame();
    d->leftFrame->setObjectName("left-frame");
    d->leftFrame->setLayout(d->leftLayout);

    // Right
    d->stack = new QStackedLayout();
    d->stack->setMargin(0);

    d->rightFrame = new QFrame();
    d->rightFrame->setObjectName("right-frame");
    d->rightFrame->setLayout(d->stack);

    // Main
    d->splitter = new QSplitter();
    d->splitter->setObjectName("splitter");
    d->splitter->setChildrenCollapsible(false);
    d->splitter->addWidget(d->leftFrame);
    d->splitter->addWidget(d->rightFrame);

    d->splitter->setStretchFactor(0, 0);
    d->splitter->setStretchFactor(1, 1);

    d->layout = new QHBoxLayout();
    d->layout->setMargin(0);
    d->layout->setSpacing(0);

    d->layout->addWidget(d->splitter);
    setLayout(d->layout);

    d->btnGroup = new QButtonGroup(this);
    d->btnGroup->setExclusive(true);

    d->topWidget = nullptr;
    d->bottomWidget = nullptr;

    connect(d->stack, &QStackedLayout::currentChanged, d, &CNavFramePrivate::_q_currentChanged);
    connect(d->btnGroup, &QButtonGroup::idClicked, d, &CNavFramePrivate::_q_buttonClicked);
}

CNavFrame::~CNavFrame() {
}

QWidget *CNavFrame::topWidget() const {
    Q_D(const CNavFrame);
    return d->topWidget;
}

void CNavFrame::setTopWidget(QWidget *w) {
    Q_D(CNavFrame);

    takeTopWidget();
    d->leftLayout->insertWidget(0, w);
    d->topWidget = w;
}

QWidget *CNavFrame::takeTopWidget() {
    Q_D(CNavFrame);

    QWidget *w = nullptr;
    if (d->topWidget) {
        w = d->topWidget;
        d_ptr->leftLayout->removeWidget(w);
        d->topWidget = nullptr;
    }
    return w;
}

QWidget *CNavFrame::bottomWidget() const {
    Q_D(const CNavFrame);
    return d->bottomWidget;
}

void CNavFrame::setBottomWidget(QWidget *w) {
    Q_D(CNavFrame);

    takeBottomWidget();
    d->leftLayout->addWidget(w);
    d->bottomWidget = w;
}

QWidget *CNavFrame::takeBottomWidget() {
    Q_D(CNavFrame);

    QWidget *w = nullptr;
    if (d->bottomWidget) {
        w = d->bottomWidget;
        d_ptr->leftLayout->removeWidget(w);
        d->bottomWidget = nullptr;
    }
    return w;
}

QSplitter *CNavFrame::splitter() const {
    Q_D(const CNavFrame);
    return d->splitter;
}

QAbstractButton *CNavFrame::insertWidget(int index, QWidget *w) {
    Q_D(CNavFrame);

    index = qMin(index, count());

    // New button
    auto btn = new CTabButton();
    btn->setCheckable(true);
    d->btnGroup->addButton(btn);

    // Insert to layouts
    d->buttonLayout->insertWidget(index, btn);
    d->stack->insertWidget(index, w);

    return btn;
}

void CNavFrame::removeWidget(QWidget *w) {
    Q_D(CNavFrame);
    removeAt(d->stack->indexOf(w));
}

void CNavFrame::removeAt(int index) {
    if (index >= count() || index < 0) {
        return;
    }
    Q_D(CNavFrame);

    // Find button
    auto btn = qobject_cast<QAbstractButton *>(d->buttonLayout->itemAt(index)->widget());
    d->btnGroup->removeButton(btn);

    // Remove from layouts
    d->buttonLayout->removeWidget(btn);
    d->stack->removeWidget(d->stack->widget(index));

    // Delete button
    btn->deleteLater();
}

QWidget *CNavFrame::widget(int index) {
    Q_D(const CNavFrame);
    return d->stack->widget(index);
}

QWidget *CNavFrame::currentWidget() const {
    Q_D(const CNavFrame);
    return d->stack->currentWidget();
}

int CNavFrame::currentIndex() const {
    Q_D(const CNavFrame);
    return d->stack->currentIndex();
}

int CNavFrame::count() const {
    Q_D(const CNavFrame);
    return d->stack->count();
}

int CNavFrame::indexOf(QWidget *w) const {
    Q_D(const CNavFrame);
    return d->stack->indexOf(w);
}

void CNavFrame::setCurrentIndex(int index) {
    Q_D(CNavFrame);
    d->stack->setCurrentIndex(index);
}

void CNavFrame::setCurrentWidget(QWidget *w) {
    Q_D(const CNavFrame);
    d->stack->setCurrentWidget(w);
}