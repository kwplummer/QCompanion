#ifndef MOCK_GLOBAL_H
#define MOCK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MOCK_LIBRARY)
#  define MOCKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MOCKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MOCK_GLOBAL_H
