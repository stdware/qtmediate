#include "cmenubar.h"
#include "cmenu.h"

#include <QDebug>
#include <QToolButton>
#include <QApplication>

static const char QT_MENU_EXTENSION_NAME[] = "qt_menubar_ext_button";

// Since the QMenu class doesn't inherit font from QApplication
// This class is designed to change default menu bar's extension menu font

class CMenuBarPrivate {
public:
    QToolButton *m_extension;
};

/*!
    \class CMenuBar
    \brief CMenuBar is extended from QMenuBar which does more initialization work.
*/

/*!
    Constructor.

    The menubar font will be set as the application font, which QMenuBar doesn't.
*/
CMenuBar::CMenuBar(QWidget *parent) : QMenuBar(parent), d(new CMenuBarPrivate()) {
    setFont(qApp->font());

    d->m_extension = findChild<QToolButton *>(QLatin1String(QT_MENU_EXTENSION_NAME));
    Q_ASSERT(d->m_extension);
    d->m_extension->setMenu(new CMenu(this));
}

/*!
    Destructor.
*/
CMenuBar::~CMenuBar() {
    delete d;
}

/*!
    Returns the icon of the menubar extension button.
*/
QIcon CMenuBar::extensionIcon() const {
    return d->m_extension->icon();
}

/*!
    Sets the icon of the menubar extension button.
*/
void CMenuBar::setExtensionIcon(const QIcon &extensionIcon) {
    d->m_extension->setIcon(extensionIcon);
}

/*!
    Returns the context menu of the menubar extension button.
*/
QMenu *CMenuBar::extensionMenu() const {
    return d->m_extension->menu();
}
