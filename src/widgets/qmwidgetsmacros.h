#ifndef QMWIDGETSMACROS_H
#define QMWIDGETSMACROS_H

#include <QMGui/QPixelSize.h>

//=====================================================================================================
// Q_PROPERTY

#define Q_PROPERTY_DECLARE(TYPE, prop, Prop)                                                                           \
private:                                                                                                               \
    Q_PROPERTY(TYPE prop READ prop WRITE set##Prop NOTIFY prop##Changed)                                               \
    TYPE m_##prop;                                                                                                     \
                                                                                                                       \
public:                                                                                                                \
    inline TYPE prop() const {                                                                                         \
        return m_##prop;                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    inline void set##Prop(const TYPE &prop) {                                                                          \
        m_##prop = prop;                                                                                               \
        update();                                                                                                      \
        emit prop##Changed();                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
Q_SIGNALS:                                                                                                             \
    void prop##Changed();                                                                                              \
                                                                                                                       \
private:

// Definition
#define Q_D_PROPERTY(TYPE, prop, Prop)                                                                                 \
private:                                                                                                               \
    Q_PROPERTY(TYPE prop READ prop WRITE set##Prop NOTIFY prop##Changed)                                               \
                                                                                                                       \
public:                                                                                                                \
    TYPE prop() const;                                                                                                 \
    void set##Prop(const TYPE &prop);                                                                                  \
                                                                                                                       \
Q_SIGNALS:                                                                                                             \
    void prop##Changed();                                                                                              \
                                                                                                                       \
private:

// Declaration
#define Q_D_PROPERTY_DECLARE(TYPE, prop, Prop, Class)                                                                  \
    TYPE Class::prop() const {                                                                                         \
        Q_D(const Class);                                                                                              \
        return d->prop;                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    void Class::set##Prop(const TYPE &prop) {                                                                          \
        Q_D(Class);                                                                                                    \
        d->prop = prop;                                                                                                \
        emit prop##Changed();                                                                                          \
    }

// Q_PROPERTY
//=====================================================================================================

//=====================================================================================================
// Q_LAYOUT_PROPERTY
#define Q_LAYOUT_PROPERTY_DELCARE                                                                                      \
private:                                                                                                               \
    Q_PROPERTY(QMargins layoutMargins READ layoutMargins WRITE setLayoutMargins NOTIFY layoutChanged)                  \
    Q_PROPERTY(QPixelSize layoutSpacing READ layoutSpacing WRITE setLayoutSpacing NOTIFY layoutChanged)                \
public:                                                                                                                \
    inline QMargins layoutMargins() const {                                                                            \
        auto m = QWidget::layout();                                                                                    \
        return m ? m->contentsMargins() : QMargins();                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    inline void setLayoutMargins(const QMargins &margins) {                                                            \
        auto m = QWidget::layout();                                                                                    \
        m ? m->setContentsMargins(margins) : void(0);                                                                  \
        emit layoutChanged();                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
    inline QPixelSize layoutSpacing() const {                                                                          \
        auto m = QWidget::layout();                                                                                    \
        return QPixelSize(m ? m->spacing() : 0);                                                                       \
    }                                                                                                                  \
                                                                                                                       \
    inline void setLayoutSpacing(const QPixelSize &spacing) {                                                          \
        auto m = QWidget::layout();                                                                                    \
        m ? m->setSpacing(spacing.value()) : void(0);                                                                  \
        emit layoutChanged();                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
Q_SIGNALS:                                                                                                             \
    void layoutChanged();                                                                                              \
                                                                                                                       \
private:

#define Q_PRIVATE_LAYOUT_PROPERTY_DELCARE(LL, CC, m)                                                                   \
private:                                                                                                               \
    Q_PROPERTY(                                                                                                        \
        QMargins LL##LayoutMargins READ LL##LayoutMargins WRITE set##CC##LayoutMargins NOTIFY LL##LayoutChanged)       \
    Q_PROPERTY(                                                                                                        \
        QPixelSize LL##LayoutSpacing READ LL##LayoutSpacing WRITE set##CC##LayoutSpacing NOTIFY LL##LayoutChanged)     \
public:                                                                                                                \
    inline QMargins LL##LayoutMargins() const {                                                                        \
        return m->contentsMargins();                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    inline void set##CC##LayoutMargins(const QMargins &margins) {                                                      \
        m->setContentsMargins(margins);                                                                                \
        emit LL##LayoutChanged();                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    inline QPixelSize LL##LayoutSpacing() const {                                                                      \
        return QPixelSize(m->spacing());                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    inline void set##CC##LayoutSpacing(const QPixelSize &spacing) {                                                    \
        m->setSpacing(spacing.value());                                                                                \
        emit LL##LayoutChanged();                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
Q_SIGNALS:                                                                                                             \
    void LL##LayoutChanged();                                                                                          \
                                                                                                                       \
private:

// Q_LAYOUT_PROPERTY
//=====================================================================================================

//=====================================================================================================
// Q_D_LAYOUT_PROPERTY
// Definition
#define Q_D_LAYOUT_PROPERTY(LL, CC)                                                                                    \
private:                                                                                                               \
    Q_PROPERTY(                                                                                                        \
        QMargins LL##LayoutMargins READ LL##LayoutMargins WRITE set##CC##LayoutMargins NOTIFY LL##LayoutChanged)       \
    Q_PROPERTY(                                                                                                        \
        QPixelSize LL##LayoutSpacing READ LL##LayoutSpacing WRITE set##CC##LayoutSpacing NOTIFY LL##LayoutChanged)     \
public:                                                                                                                \
    QMargins LL##LayoutMargins() const;                                                                                \
    void set##CC##LayoutMargins(const QMargins &margins);                                                              \
                                                                                                                       \
    QPixelSize LL##LayoutSpacing() const;                                                                              \
    void set##CC##LayoutSpacing(const QPixelSize &spacing);                                                            \
                                                                                                                       \
Q_SIGNALS:                                                                                                             \
    void LL##LayoutChanged();                                                                                          \
                                                                                                                       \
private:

// Declaration
#define Q_D_LAYOUT_PROPERTY_DECLARE(LL, CC, NAME, Class)                                                               \
    QMargins Class::LL##LayoutMargins() const {                                                                        \
        Q_D(const Class);                                                                                              \
        return d->NAME->contentsMargins();                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    void Class::set##CC##LayoutMargins(const QMargins &margins) {                                                      \
        Q_D(Class);                                                                                                    \
        d->NAME->setContentsMargins(margins);                                                                          \
        emit LL##LayoutChanged();                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    QPixelSize Class::LL##LayoutSpacing() const {                                                                      \
        Q_D(const Class);                                                                                              \
        return QPixelSize(d->NAME->spacing());                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    void Class::set##CC##LayoutSpacing(const QPixelSize &spacing) {                                                    \
        Q_D(Class);                                                                                                    \
        d->NAME->setSpacing(spacing.value());                                                                          \
        emit LL##LayoutChanged();                                                                                      \
    }


// Q_D_LAYOUT_PROPERTY
//=====================================================================================================

#endif // QMWIDGETSMACROS_H
