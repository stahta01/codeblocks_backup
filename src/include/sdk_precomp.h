/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

/** Code::Blocks SDK precompiled headers support for internal usage.
  * This is precompiled with -DEXPORT_LIB -DEXPORT_EVENTS
  */

#ifndef SDK_PRECOMP_H
#define SDK_PRECOMP_H


#if defined(__WXMSW__) || defined(__WINDOWS__) || defined(_WIN32)
    #ifdef CB_PRECOMP
        #define EXPORT_LIB
        #define EXPORT_EVENTS
        #define WXUSINGDLL
        #define WXMAKINGDLL_SCI
    #endif

    #include "sdk.h" // Included instead of sdk_common.h to define the header guard for sdk.h
#else
    // for non-windows platforms, one PCH is enough
    #include "sdk_common.h"
#endif

#endif // SDK_PRECOMP_H
