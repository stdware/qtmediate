#ifndef QMWIDGETSGLOBAL_H
#define QMWIDGETSGLOBAL_H

#include <QtGlobal>

#ifndef QM_WIDGETS_EXPORT
#  ifdef QM_WIDGETS_STATIC
#    define QM_WIDGETS_EXPORT
#  else
#    ifdef QM_WIDGETS_LIBRARY
#      define QM_WIDGETS_EXPORT Q_DECL_EXPORT
#    else
#      define QM_WIDGETS_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif

#endif // QMWIDGETSGLOBAL_H
