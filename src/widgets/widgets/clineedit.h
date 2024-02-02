#ifndef CLINEEDIT_H
#define CLINEEDIT_H

#include <QLineEdit>

#include <QMWidgets/qmwidgetsglobal.h>

class QM_WIDGETS_EXPORT CLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit CLineEdit(QWidget *parent = nullptr);
    explicit CLineEdit(const QString &text, QWidget *parent = nullptr);
    ~CLineEdit();

signals:
    void specialKeyPressed(int key);

protected:
    bool event(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // CLINEEDIT_H
