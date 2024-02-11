#include "cmenu.h"

#include <QDebug>
#include <QFontMetrics>

#include <private/qaction_p.h>
#include <private/qkeysequence_p.h>
#include <private/qmenu_p.h>
#include <qiconengine.h>

#ifdef Q_OS_WINDOWS
#  include <dwmapi.h>
#endif

#include "qmsvgx.h"
#include "qmcss_p.h"
#include "qmrasterpaintaccessor.h"

// ======================================================================================
// CMenuImpl

#include <private/qaction_p.h>
#include <private/qeffects_p.h>
#include <private/qguiapplication_p.h>
#include <private/qmenu_p.h>
#include <qpa/qplatformtheme.h>

#include <QTimer>
#include <QScreen>
#include <QActionGroup>

static bool appUseFullScreenForPopup() {
    auto theme = QGuiApplicationPrivate::platformTheme();
    return theme && theme->themeHint(QPlatformTheme::UseFullScreenForPopupMenu).toBool();
}

int QMenuPrivate::scrollerHeight() const {
    Q_Q(const QMenu);
    return q->style()->pixelMetric(QStyle::PM_MenuScrollerHeight, nullptr, q);
}

// Windows and KDE allow menus to cover the taskbar, while GNOME and macOS
// don't. Torn-off menus are again different
inline bool QMenuPrivate::useFullScreenForPopup() const {
    return !tornoff && appUseFullScreenForPopup();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QRect QMenuPrivate::popupGeometry(QScreen *screen) const {
    Q_Q(const QMenu);
    if (useFullScreenForPopup())
        return screen ? screen->geometry() : QWidgetPrivate::screenGeometry(q);
    return screen ? screen->availableGeometry() : QWidgetPrivate::availableScreenGeometry(q);
}
#else
QRect QMenuPrivate::popupGeometry() const {
    Q_Q(const QMenu);
    return useFullScreenForPopup() ? QApplication::desktop()->screenGeometry(q)
                                   : QApplication::desktop()->availableGeometry(q);
}

QRect QMenuPrivate::popupGeometry(int screen) const {
    return useFullScreenForPopup() ? QApplication::desktop()->screenGeometry(screen)
                                   : QApplication::desktop()->availableGeometry(screen);
}
#endif

bool QMenuPrivate::isContextMenu() const {
    return qobject_cast<const QMenuBar *>(topCausedWidget()) == nullptr;
}

void QMenuPrivate::updateActionRects() const {
    updateActionRects(popupGeometry());
}

void QMenuPrivate::updateActionRects(const QRect &screen) const {
    Q_Q(const QMenu);
    if (!itemsDirty)
        return;

    q->ensurePolished();

    // let's reinitialize the buffer
    actionRects.resize(actions.count());
    actionRects.fill(QRect());

    int lastVisibleAction = getLastVisibleAction();

    QStyle *style = q->style();
    QStyleOption opt;
    opt.initFrom(q);
    const int hmargin = style->pixelMetric(QStyle::PM_MenuHMargin, &opt, q),
              vmargin = style->pixelMetric(QStyle::PM_MenuVMargin, &opt, q),
              icone = style->pixelMetric(QStyle::PM_SmallIconSize, &opt, q);
    const int fw = style->pixelMetric(QStyle::PM_MenuPanelWidth, &opt, q);
    const int deskFw = style->pixelMetric(QStyle::PM_MenuDesktopFrameWidth, &opt, q);
    const int tearoffHeight =
        tearoff ? style->pixelMetric(QStyle::PM_MenuTearoffHeight, &opt, q) : 0;
    const int base_y =
        vmargin + fw + topmargin + (scroll ? scroll->scrollOffset : 0) + tearoffHeight;
    const int column_max_y = screen.height() - 2 * deskFw - (vmargin + bottommargin + fw);
    int max_column_width = 0;
    int y = base_y;

    // for compatibility now - will have to refactor this away
    tabWidth = 0;
    maxIconWidth = 0;
    hasCheckableItems = false;
    ncols = 1;

    for (int i = 0; i < actions.count(); ++i) {
        QAction *action = actions.at(i);
        if (action->isSeparator() || !action->isVisible() || widgetItems.contains(action))
            continue;
        //..and some members
        hasCheckableItems |= action->isCheckable();
        QIcon is = action->icon();
        if (!is.isNull()) {
            maxIconWidth = qMax<uint>(maxIconWidth, icone + 4);
        }
    }

    // calculate size
    QFontMetrics qfm = q->fontMetrics();
    bool previousWasSeparator = true; // this is true to allow removing the leading separators
    const bool contextMenu = isContextMenu();
    for (int i = 0; i <= lastVisibleAction; i++) {
        QAction *action = actions.at(i);
        const bool isSection =
            action->isSeparator() && (!action->text().isEmpty() || !action->icon().isNull());
        const bool isPlainSeparator =
            (isSection && !q->style()->styleHint(QStyle::SH_Menu_SupportsSections)) ||
            (action->isSeparator() && !isSection);

        if (!action->isVisible() ||
            (collapsibleSeparators && previousWasSeparator && isPlainSeparator))
            continue; // we continue, this action will get an empty QRect

        previousWasSeparator = isPlainSeparator;

        // let the style modify the above size..
        QStyleOptionMenuItem opt;
        q->initStyleOption(&opt, action);
        const QFontMetrics &fm = opt.fontMetrics;

        QSize sz;
        if (QWidget *w = widgetItems.value(action)) {
            sz = w->sizeHint()
                     .expandedTo(w->minimumSize())
                     .expandedTo(w->minimumSizeHint())
                     .boundedTo(w->maximumSize());
        } else {
            // calc what I think the size is..
            if (action->isSeparator()) {
                sz = QSize(2, 2);
            } else {
                QString s = action->text();
                int t = s.indexOf(QLatin1Char('\t'));
                if (t != -1) {
                    tabWidth = qMax(int(tabWidth), qfm.horizontalAdvance(s.mid(t + 1)));
                    s = s.left(t);
#ifndef QT_NO_SHORTCUT
                } else if (action->isShortcutVisibleInContextMenu() || !contextMenu) {
                    QKeySequence seq = action->shortcut();
                    if (!seq.isEmpty())
                        tabWidth =
                            qMax(int(tabWidth),
                                 qfm.horizontalAdvance(seq.toString(QKeySequence::NativeText)));
#endif
                }
                sz.setWidth(
                    fm.boundingRect(QRect(), Qt::TextSingleLine | Qt::TextShowMnemonic, s).width());
                sz.setHeight(qMax(fm.height(), qfm.height()));

                QIcon is = action->icon();
                if (!is.isNull()) {
                    QSize is_sz = QSize(icone, icone);
                    if (is_sz.height() > sz.height())
                        sz.setHeight(is_sz.height());
                }
            }
            sz = style->sizeFromContents(QStyle::CT_MenuItem, &opt, sz, q);
        }


        if (!sz.isEmpty()) {
            max_column_width = qMax(max_column_width, sz.width());
            // wrapping
            if (!scroll && y + sz.height() > column_max_y) {
                ncols++;
                y = base_y;
            } else {
                y += sz.height();
            }
            // update the item
            actionRects[i] = QRect(0, 0, sz.width(), sz.height());
        }
    }

    max_column_width += tabWidth; // finally add in the tab width
    if (!tornoff ||
        (tornoff &&
         scroll)) { // exclude non-scrollable tear-off menu since the tear-off menu has a fixed size
        const int sfcMargin =
            style->sizeFromContents(QStyle::CT_Menu, &opt, QSize(0, 0), q).width();
        const int min_column_width =
            q->minimumWidth() - (sfcMargin + leftmargin + rightmargin + 2 * (fw + hmargin));
        max_column_width = qMax(min_column_width, max_column_width);
    }

    // calculate position
    int x = hmargin + fw + leftmargin;
    y = base_y;

    for (int i = 0; i < actions.count(); i++) {
        QRect &rect = actionRects[i];
        if (rect.isNull())
            continue;
        if (!scroll && y + rect.height() > column_max_y) {
            x += max_column_width + hmargin;
            y = base_y;
        }
        rect.translate(x, y);            // move
        rect.setWidth(max_column_width); // uniform width

        // we need to update the widgets geometry
        if (QWidget *widget = widgetItems.value(actions.at(i))) {
            widget->setGeometry(rect);
            widget->setVisible(actions.at(i)->isVisible());
        }

        y += rect.height();
    }
    itemsDirty = 0;
}

int QMenuPrivate::getLastVisibleAction() const {
    // let's try to get the last visible action
    int lastVisibleAction = actions.count() - 1;
    for (; lastVisibleAction >= 0; --lastVisibleAction) {
        const QAction *action = actions.at(lastVisibleAction);
        if (action->isVisible()) {
            // removing trailing separators
            if (action->isSeparator() && collapsibleSeparators)
                continue;
            break;
        }
    }
    return lastVisibleAction;
}


QRect QMenuPrivate::actionRect(QAction *act) const {
    int index = actions.indexOf(act);
    if (index == -1)
        return QRect();

    updateActionRects();

    // we found the action
    return actionRects.at(index);
}

void QMenuPrivate::hideMenu(QMenu *menu) {
    if (!menu)
        return;

    // See two execs below. They may trigger an akward situation
    // when 'menu' (also known as 'q' or 'this' in the many functions
    // around) to become a dangling pointer if the loop manages
    // to execute 'deferred delete' ... posted while executing
    // this same loop. Not good!
    struct Reposter : QObject {
        Reposter(QMenu *menu) : q(menu) {
            Q_ASSERT(q);
            q->installEventFilter(this);
        }
        ~Reposter() {
            if (deleteLater)
                q->deleteLater();
        }
        bool eventFilter(QObject *obj, QEvent *event) override {
            if (obj == q && event->type() == QEvent::DeferredDelete)
                return deleteLater = true;

            return QObject::eventFilter(obj, event);
        }
        QMenu *q = nullptr;
        bool deleteLater = false;
    };

#if QT_CONFIG(effects)
    QSignalBlocker blocker(menu);
    aboutToHide = true;
    // Flash item which is about to trigger (if any).
    if (menu->style()->styleHint(QStyle::SH_Menu_FlashTriggeredItem) && currentAction &&
        currentAction == actionAboutToTrigger && menu->actions().contains(currentAction)) {
        QEventLoop eventLoop;
        QAction *activeAction = currentAction;

        menu->setActiveAction(nullptr);
        const Reposter deleteDeleteLate(menu);
        QTimer::singleShot(60, &eventLoop, SLOT(quit()));
        eventLoop.exec();

        // Select and wait 20 ms.
        menu->setActiveAction(activeAction);
        QTimer::singleShot(20, &eventLoop, SLOT(quit()));
        eventLoop.exec();
    }

    aboutToHide = false;
    blocker.unblock();
#endif // QT_CONFIG(effects)
    if (activeMenu == menu)
        activeMenu = nullptr;
    menu->d_func()->causedPopup.action = nullptr;
    menu->close();
    menu->d_func()->causedPopup.widget = nullptr;
}

// return the top causedPopup.widget that is not a QMenu
QWidget *QMenuPrivate::topCausedWidget() const {
    QWidget *top = causedPopup.widget;
    while (QMenu *m = qobject_cast<QMenu *>(top))
        top = m->d_func()->causedPopup.widget;
    return top;
}

QAction *QMenuPrivate::actionAt(QPoint p) const {
    if (!rect().contains(p)) // sanity check
        return nullptr;

    for (int i = 0; i < actionRects.count(); i++) {
        if (actionRects.at(i).contains(p))
            return actions.at(i);
    }
    return nullptr;
}

void QMenuPrivate::setOverrideMenuAction(QAction *a) {
    Q_Q(QMenu);
    QObject::disconnect(menuAction, SIGNAL(destroyed()), q, SLOT(_q_overrideMenuActionDestroyed()));
    if (a) {
        menuAction = a;
        QObject::connect(a, SIGNAL(destroyed()), q, SLOT(_q_overrideMenuActionDestroyed()));
    } else { // we revert back to the default action created by the QMenu itself
        menuAction = defaultMenuAction;
    }
}

void QMenuPrivate::_q_overrideMenuActionDestroyed() {
    menuAction = defaultMenuAction;
}

void QMenuPrivate::updateLayoutDirection() {
    Q_Q(QMenu);
    // we need to mimic the cause of the popup's layout direction
    // to allow setting it on a mainwindow for example
    // we call setLayoutDirection_helper to not overwrite a user-defined value
    if (!q->testAttribute(Qt::WA_SetLayoutDirection)) {
        if (QWidget *w = causedPopup.widget)
            setLayoutDirection_helper(w->layoutDirection());
        else if (QWidget *w = q->parentWidget())
            setLayoutDirection_helper(w->layoutDirection());
        else
            setLayoutDirection_helper(QGuiApplication::layoutDirection());
    }
}

void QMenuPrivate::drawScroller(QPainter *painter, QMenuPrivate::ScrollerTearOffItem::Type type,
                                const QRect &rect) {
    if (!painter || rect.isEmpty())
        return;

    if (!scroll || !(scroll->scrollFlags & (QMenuPrivate::QMenuScroller::ScrollUp |
                                            QMenuPrivate::QMenuScroller::ScrollDown)))
        return;

    Q_Q(QMenu);
    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(q);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    menuOpt.reservedShortcutWidth = 0;
#else
    menuOpt.tabWidth = 0;
#endif
    menuOpt.rect = rect;
    menuOpt.menuItemType = QStyleOptionMenuItem::Scroller;
    menuOpt.state |= QStyle::State_Enabled;
    if (type == QMenuPrivate::ScrollerTearOffItem::ScrollDown)
        menuOpt.state |= QStyle::State_DownArrow;

    painter->setClipRect(menuOpt.rect);
    q->style()->drawControl(QStyle::CE_MenuScroller, &menuOpt, painter, q);
}

void QMenuPrivate::drawTearOff(QPainter *painter, const QRect &rect) {
    if (!painter || rect.isEmpty())
        return;

    if (!tearoff)
        return;

    Q_Q(QMenu);
    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(q);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    menuOpt.reservedShortcutWidth = 0;
#else
    menuOpt.tabWidth = 0;
#endif
    menuOpt.rect = rect;
    menuOpt.menuItemType = QStyleOptionMenuItem::TearOff;
    if (tearoffHighlighted)
        menuOpt.state |= QStyle::State_Selected;

    painter->setClipRect(menuOpt.rect);
    q->style()->drawControl(QStyle::CE_MenuTearoff, &menuOpt, painter, q);
}

QRect QMenuPrivate::rect() const {
    Q_Q(const QMenu);
    QStyle *style = q->style();
    QStyleOption opt(0);
    opt.initFrom(q);
    const int hmargin = style->pixelMetric(QStyle::PM_MenuHMargin, &opt, q);
    const int vmargin = style->pixelMetric(QStyle::PM_MenuVMargin, &opt, q);
    const int fw = style->pixelMetric(QStyle::PM_MenuPanelWidth, &opt, q);
    return (q->rect().adjusted(hmargin + fw + leftmargin, vmargin + fw + topmargin,
                               -(hmargin + fw + rightmargin), -(vmargin + fw + bottommargin)));
}

QMenuPrivate::ScrollerTearOffItem::ScrollerTearOffItem(QMenuPrivate::ScrollerTearOffItem::Type type,
                                                       QMenuPrivate *mPrivate, QWidget *parent,
                                                       Qt::WindowFlags f)
    : QWidget(parent, f), menuPrivate(mPrivate), scrollType(type) {
    if (parent)
        setMouseTracking(
            parent->style()->styleHint(QStyle::SH_Menu_MouseTracking, nullptr, parent));
}

void QMenuPrivate::ScrollerTearOffItem::paintEvent(QPaintEvent *e) {
    if (!e->rect().intersects(rect()))
        return;

    QPainter p(this);
    QWidget *parent = parentWidget();

    // paint scroll up / down arrows
    menuPrivate->drawScroller(&p, scrollType, QRect(0, 0, width(), menuPrivate->scrollerHeight()));
    // paint the tear off
    if (scrollType == QMenuPrivate::ScrollerTearOffItem::ScrollUp) {
        QRect rect(0, 0, width(),
                   parent->style()->pixelMetric(QStyle::PM_MenuTearoffHeight, nullptr, parent));
        if (menuPrivate->scroll &&
            menuPrivate->scroll->scrollFlags & QMenuPrivate::QMenuScroller::ScrollUp)
            rect.translate(0, menuPrivate->scrollerHeight());
        menuPrivate->drawTearOff(&p, rect);
    }
}

void QMenuPrivate::ScrollerTearOffItem::updateScrollerRects(const QRect &rect) {
    if (rect.isEmpty())
        setVisible(false);
    else {
        setGeometry(rect);
        raise();
        setVisible(true);
    }
}

// CMenuImpl
// ======================================================================================

class CMenuPrivate {
public:
    CMenu *q;

    CMenuPrivate(CMenu *q) : q(q) {
        // Initialize Font
        q->setFont(qApp->font());

#ifdef Q_OS_WINDOWS
        // Call Windows enhancement (if applicable)
        initWindowsEnhancement();
#endif
    }

    void updateActionStats() {
        for (const auto &action : q->actions()) {
            if (!action->icon().isNull()) {
                q->setProperty("stats", "icon");
                goto out;
            }
        }

        for (const auto &action : q->actions()) {
            if (action->isCheckable()) {
                q->setProperty("stats", "checkable");
                goto out;
            }
        }

        q->setProperty("stats", "trivial");

    out:
        q->style()->polish(q);
    }

private:
#ifdef Q_OS_WINDOWS
    void initWindowsEnhancement() {
        // Disable Qt drop shadow attribute in order to remove CS_DROPSHADOW
        q->setWindowFlag(Qt::NoDropShadowWindowHint, true);
        // Enable DWM shadow for popup
        m_winEnhanceTrigger = QObject::connect(q, &CMenu::aboutToShow, q, [this]() {
            constexpr int mgn = 1;
            // Constants defined to make older Windows SDK happy
            constexpr int DWMWA_USE_IMMERSIVE_DARK_MODE_ = 20;
            constexpr int DWMWA_WINDOW_CORNER_PREFERENCE_ = 33;
            DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
            /*DWM_WINDOW_CORNER_PREFERENCE*/ INT dwcp = /*DWMWCP_ROUNDSMALL*/ 3;
            UINT dark = 1;
            MARGINS margins = {mgn, mgn, mgn, mgn};
            Q_ASSERT(this->q->winId());
            // Let DWM compose non-client area
            DwmSetWindowAttribute(reinterpret_cast<HWND>(this->q->winId()),
                                  DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
            // Allow extending frame into popup, this way CMenu will have better drop shadow
            DwmExtendFrameIntoClientArea(reinterpret_cast<HWND>(this->q->winId()), &margins);
            // This undocumented API call is required, since with shadow composition, Qt cannot keep
            // up drawing the contents 1 frame too late when the popup window was shown. Without
            // this call the 1 frame will be white which causes flicker. Workaround taken from
            // https://github.com/AvaloniaUI/Avalonia/issues/8316#issuecomment-1166417480
            DwmSetWindowAttribute(reinterpret_cast<HWND>(this->q->winId()),
                                  DWMWA_USE_IMMERSIVE_DARK_MODE_, &dark, sizeof(dark));
            // This round corner settings only works for Windows 11, sets round corner to small so
            // it doesn't look too off
            DwmSetWindowAttribute(reinterpret_cast<HWND>(this->q->winId()),
                                  DWMWA_WINDOW_CORNER_PREFERENCE_, &dwcp, sizeof(dwcp));
            // Disconnect this connection, don't run again
            QObject::disconnect(m_winEnhanceTrigger);
        });
    }
    QMetaObject::Connection m_winEnhanceTrigger;
#endif
};

CMenu::CMenu(QWidget *parent) : QMenu(parent), d(new CMenuPrivate(this)) {
}

CMenu::CMenu(const QString &title, QWidget *parent) : CMenu(parent) {
    setTitle(title);
}

CMenu::~CMenu() {
    delete d;
}

bool CMenu::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::ActionAdded:
        case QEvent::ActionChanged:
        case QEvent::ActionRemoved:
            d->updateActionStats();
            break;
        default:
            break;
    }
    return QMenu::event(event);
}

void CMenu::paintEvent(QPaintEvent *event) {
    auto d = reinterpret_cast<QMenuPrivate *>(d_ptr.data());

    d->updateActionRects();
    QPainter p(this);
    QRegion emptyArea = QRegion(rect());

    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(this);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    menuOpt.reservedShortcutWidth = 0;
#else
    menuOpt.tabWidth = 0;
#endif

    style()->drawPrimitive(QStyle::PE_PanelMenu, &menuOpt, &p, this);

    // calculate the scroll up / down rect
    const int fw = style()->pixelMetric(QStyle::PM_MenuPanelWidth, nullptr, this);
    const int hmargin = style()->pixelMetric(QStyle::PM_MenuHMargin, nullptr, this);
    const int vmargin = style()->pixelMetric(QStyle::PM_MenuVMargin, nullptr, this);

    QRect scrollUpRect, scrollDownRect;
    const int leftmargin = fw + hmargin + d->leftmargin;
    const int topmargin = fw + vmargin + d->topmargin;
    const int bottommargin = fw + vmargin + d->bottommargin;
    const int contentWidth = width() - (fw + hmargin) * 2 - d->leftmargin - d->rightmargin;
    if (d->scroll) {
        if (d->scroll->scrollFlags & QMenuPrivate::QMenuScroller::ScrollUp)
            scrollUpRect.setRect(leftmargin, topmargin, contentWidth, d->scrollerHeight());

        if (d->scroll->scrollFlags & QMenuPrivate::QMenuScroller::ScrollDown)
            scrollDownRect.setRect(leftmargin, height() - d->scrollerHeight() - bottommargin,
                                   contentWidth, d->scrollerHeight());
    }

    // calculate the tear off rect
    QRect tearOffRect;
    if (d->tearoff) {
        tearOffRect.setRect(leftmargin, topmargin, contentWidth,
                            style()->pixelMetric(QStyle::PM_MenuTearoffHeight, nullptr, this));
        if (d->scroll && d->scroll->scrollFlags & QMenuPrivate::QMenuScroller::ScrollUp)
            tearOffRect.translate(0, d->scrollerHeight());
    }

    // draw the items that need updating..
    QRect scrollUpTearOffRect = scrollUpRect.united(tearOffRect);
    for (int i = 0; i < d->actions.count(); ++i) {
        QAction *action = d->actions.at(i);
        QRect actionRect = d->actionRects.at(i);
        if (!event->rect().intersects(actionRect) || d->widgetItems.value(action))
            continue;
        // set the clip region to be extra safe (and adjust for the scrollers)
        emptyArea -= QRegion(actionRect);

        QRect adjustedActionRect = actionRect;
        if (!scrollUpTearOffRect.isEmpty() &&
            adjustedActionRect.bottom() <= scrollUpTearOffRect.top())
            continue;

        if (!scrollDownRect.isEmpty() && adjustedActionRect.top() >= scrollDownRect.bottom())
            continue;

        if (adjustedActionRect.intersects(scrollUpTearOffRect)) {
            if (adjustedActionRect.bottom() <= scrollUpTearOffRect.bottom())
                continue;
            else
                adjustedActionRect.setTop(scrollUpTearOffRect.bottom() + 1);
        }

        if (adjustedActionRect.intersects(scrollDownRect)) {
            if (adjustedActionRect.top() >= scrollDownRect.top())
                continue;
            else
                adjustedActionRect.setBottom(scrollDownRect.top() - 1);
        }

        QRegion adjustedActionReg(adjustedActionRect);
        p.setClipRegion(adjustedActionReg);

        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        opt.rect = actionRect;

        QMSvgx::Icon svgx(&opt.icon);
        if (svgx.isValid()) {
            QM::ButtonState state =
                (opt.state & QStyle::State_Enabled)
                    ? ((opt.state & QStyle::State_Selected) ? QM::ButtonHover : QM::ButtonNormal)
                    : QM::ButtonDisabled;
            svgx.setCurrentState(state);
            if (svgx.color(state) == "auto") {
                // Change options
                QString text = opt.text;
                opt.text = QChar(0x25A0);

                // Query for text color
                QMRasterPaintAccessor acc(QImage(opt.rect.size(), QImage::Format_ARGB32));
                QPen pen = acc.queryPen(
                    [this, &opt](QPainter *painter) {
                        style()->drawControl(QStyle::CE_MenuItem, &opt, painter, this); //
                    },
                    QMPaintAccessor::PI_Text);
                svgx.setColorHint(QMCss::colorName(pen.color()));

                // Restore options
                opt.text = text;
            }
        }

        style()->drawControl(QStyle::CE_MenuItem, &opt, &p, this);
    }

    emptyArea -= QRegion(scrollUpTearOffRect);
    emptyArea -= QRegion(scrollDownRect);

    if (d->scrollUpTearOffItem || d->scrollDownItem) {
        if (d->scrollUpTearOffItem)
            d->scrollUpTearOffItem->updateScrollerRects(scrollUpTearOffRect);
        if (d->scrollDownItem)
            d->scrollDownItem->updateScrollerRects(scrollDownRect);
    } else {
        // paint scroll up /down
        d->drawScroller(&p, QMenuPrivate::ScrollerTearOffItem::ScrollUp, scrollUpRect);
        d->drawScroller(&p, QMenuPrivate::ScrollerTearOffItem::ScrollDown, scrollDownRect);
        // paint the tear off..
        d->drawTearOff(&p, tearOffRect);
    }

    // draw border
    if (fw) {
        QRegion borderReg;
        borderReg += QRect(0, 0, fw, height());            // left
        borderReg += QRect(width() - fw, 0, fw, height()); // right
        borderReg += QRect(0, 0, width(), fw);             // top
        borderReg += QRect(0, height() - fw, width(), fw); // bottom
        p.setClipRegion(borderReg);
        emptyArea -= borderReg;
        QStyleOptionFrame frame;
        frame.rect = rect();
        frame.palette = palette();
        frame.state = QStyle::State_None;
        frame.lineWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth, &frame);
        frame.midLineWidth = 0;
        style()->drawPrimitive(QStyle::PE_FrameMenu, &frame, &p, this);
    }

    // finally the rest of the spaces
    p.setClipRegion(emptyArea);
    menuOpt.state = QStyle::State_None;
    menuOpt.menuItemType = QStyleOptionMenuItem::EmptyArea;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.rect = rect();
    menuOpt.menuRect = rect();

    style()->drawControl(QStyle::CE_MenuEmptyArea, &menuOpt, &p, this);
}

void CMenu::initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const {
    if (!option || !action)
        return;

    bool mouseDown;
    // Avoid using QMenuPrivate::mouseDown
    {
        QStyleOptionMenuItem testOption;
        QMenu::initStyleOption(&testOption, action);
        mouseDown = testOption.state & QStyle::State_Sunken;
    }

    auto d = reinterpret_cast<QMenuPrivate *>(d_ptr.data());

    option->initFrom(this);
    option->palette = palette();
    option->state = QStyle::State_None;

    if (window()->isActiveWindow())
        option->state |= QStyle::State_Active;
    if (isEnabled() && action->isEnabled() && (!action->menu() || action->menu()->isEnabled()))
        option->state |= QStyle::State_Enabled;
    else
        option->palette.setCurrentColorGroup(QPalette::Disabled);

    option->font = action->font().resolve(font());
    option->fontMetrics = QFontMetrics(option->font);

    if (d->currentAction && d->currentAction == action && !d->currentAction->isSeparator()) {
        option->state |=
            QStyle::State_Selected | (mouseDown ? QStyle::State_Sunken : QStyle::State_None);
    }

    option->menuHasCheckableItems = d->hasCheckableItems;
    if (!action->isCheckable()) {
        option->checkType = QStyleOptionMenuItem::NotCheckable;
    } else {
        option->checkType = (action->actionGroup() && action->actionGroup()->isExclusive())
                                ? QStyleOptionMenuItem::Exclusive
                                : QStyleOptionMenuItem::NonExclusive;
        option->checked = action->isChecked();
    }
    if (action->menu()) {
        option->menuItemType = QStyleOptionMenuItem::SubMenu;
    } else if (action->isSeparator()) {
        option->menuItemType = QStyleOptionMenuItem::Separator;
    } else if (d->defaultAction == action) {
        option->menuItemType = QStyleOptionMenuItem::DefaultItem;
    } else {
        option->menuItemType = QStyleOptionMenuItem::Normal;
    }
    if (action->isIconVisibleInMenu()) {
        option->icon = action->icon();
    }

    QString textAndAccel = action->text();
    int tabWidth = d->tabWidth;

    // Two patches:
    // 1. Change key sequence separator from "," to " "
    // 2. Change tab width to right align shortcut text

#ifndef QT_NO_SHORTCUT
    if ((action->isShortcutVisibleInContextMenu() || !d->isContextMenu()) &&
        textAndAccel.indexOf(QLatin1Char('\t')) == -1) {
        QKeySequence seq = action->shortcut();
        if (!seq.isEmpty()) {
            QString seqText = seq.toString(QKeySequence::NativeText);
            seqText.replace(", ", " ");
            textAndAccel += QLatin1Char('\t') + seqText;
            tabWidth = QFontMetrics(font()).horizontalAdvance(seqText);
        }
    }
#endif
    option->text = textAndAccel;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    option->reservedShortcutWidth = tabWidth;
#else
    option->tabWidth = tabWidth;
#endif
    option->maxIconWidth = d->maxIconWidth;
    option->menuRect = rect();
}
