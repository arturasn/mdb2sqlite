#ifndef _WX_WIDGETS_LINK_H_
#define _WX_WIDGETS_LINK_H_

#define WXUSINGDLL
#define __WXMSW__

#ifdef _DEBUG
	#define _WX_BASE_LIB "wxbase31ud.lib"
    #define _WX_CORE_LIB "wxmsw31ud_core.lib"
    #define _WX_ADVN_LIB "wxmsw31ud_adv.lib"
#else
    #define _WX_CORE_LIB "wxmsw31u_core.lib"
    #define _WX_ADVN_LIB "wxmsw31u_adv.lib"
	#define _WX_BASE_LIB "wxbase31u.lib"
#endif

#pragma comment(lib, _WX_BASE_LIB)
#pragma comment(lib, _WX_CORE_LIB)
#pragma comment(lib, _WX_ADVN_LIB)

#ifndef _WX_WX_H_
	#include "wx/wx.h"
#endif

#pragma comment(lib, "Comdlg32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Advapi32.lib")

#pragma warning(default:4097)
#pragma warning(default:4244)
#pragma warning(default:4511)
#pragma warning(default:4127)
#pragma warning(default:4702)
#pragma warning(default:4003)
#pragma warning(default:4267)

#endif