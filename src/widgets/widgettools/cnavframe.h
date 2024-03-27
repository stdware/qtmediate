#ifndef CNAVFRAME_H
#define CNAVFRAME_H

#include <QAbstractButton>
#include <QFrame>
#include <QSplitter>

#include <QMWidgets/qmwidgetsglobal.h>

class CNavFramePrivate;

class QM_WIDGETS_EXPORT CNavFrame : public QFrame {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CNavFrame)
public:
    explicit CNavFrame(QWidget *parent = nullptr);
    ~CNavFrame();

    QWidget *topWidget() const;
    void setTopWidget(QWidget *w);
    QWidget *takeTopWidget();

    QWidget *bottomWidget() const;
    void setBottomWidget(QWidget *w);
    QWidget *takeBottomWidget();

    QSplitter *splitter() const;

    inline QAbstractButton *addWidget(QWidget *w);
    QAbstractButton *insertWidget(int index, QWidget *w);
    void removeWidget(QWidget *w);
    void removeAt(int index);

    QWidget *widget(int index);
    QWidget *currentWidget() const;
    int currentIndex() const;
    int count() const;
    int indexOf(QWidget *w) const;

    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *w);

protected:
    QScopedPointer<CNavFramePrivate> d_ptr;

    friend class CNavFramePrivate;
};

inline QAbstractButton *CNavFrame::addWidget(QWidget *w) {
    return insertWidget(count(), w);
}

#endif // CNAVFRAME_H
