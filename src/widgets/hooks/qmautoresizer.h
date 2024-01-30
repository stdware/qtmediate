#ifndef QMAUTORESIZER_H
#define QMAUTORESIZER_H

#include <QWidget>

#include <QMWidgets/QMWidgetsGlobal.h>

class QM_WIDGETS_EXPORT QMAutoResizer : public QObject {
    Q_OBJECT
public:
    enum SizeOption {
        SizeHint,
        MinimumSizeHint,
    };
    Q_ENUM(SizeOption)

    enum MeasureOption {
        Width = 1,
        Height = 2,
        WidthAndHeight = Width | Height,
    };
    Q_ENUM(MeasureOption)

    explicit QMAutoResizer(QWidget *parent = nullptr);
    QMAutoResizer(SizeOption so, QWidget *parent = nullptr);
    QMAutoResizer(SizeOption so, bool fixed, QWidget *parent = nullptr);
    QMAutoResizer(SizeOption so, bool fixed, MeasureOption mo, QWidget *parent = nullptr);
    ~QMAutoResizer();

    inline QWidget *widget() const;
    inline SizeOption sizeOption() const;
    inline bool fixed() const;
    inline MeasureOption measureOption() const;

protected:
    QWidget *w;

    SizeOption so;    // Default: sizeHint
    bool fix;         // Default: false
    MeasureOption mo; // Default: WidthAndHeight

    bool eventFilter(QObject *obj, QEvent *event) override;
};

inline QWidget *QMAutoResizer::widget() const {
    return w;
}

inline QMAutoResizer::SizeOption QMAutoResizer::sizeOption() const {
    return so;
}

inline bool QMAutoResizer::fixed() const {
    return fix;
}

inline QMAutoResizer::MeasureOption QMAutoResizer::measureOption() const {
    return mo;
}

#endif // QMAUTORESIZER_H
