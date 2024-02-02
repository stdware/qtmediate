#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMWidgets/qcssvaluemap.h>

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_PROPERTY(QCssValueMap styleValues READ styleValues WRITE setStyleValues)
public:
    explicit MainWindow();
    ~MainWindow();

    QCssValueMap styleValues() const;
    void setStyleValues(const QCssValueMap &styleValues);

protected:
    QCssValueMap m_styleValues;
};

#endif // MAINWINDOW_H
