#ifndef QMGLOBAL_H
#define QMGLOBAL_H

#include <QtGlobal>

#ifndef QM_CORE_EXPORT
#  ifdef QMCORE_STATIC
#    define QM_CORE_EXPORT
#  else
#    ifdef QMCORE_LIBRARY
#      define QM_CORE_EXPORT Q_DECL_EXPORT
#    else
#      define QM_CORE_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif

#endif // QMGLOBAL_H
