#ifndef __StdAfx_h__
#define __StdAfx_h__

#include <list>
#include <stack>
#include <vector>
#include <string>
#include <fstream>
#include <typeinfo>
#include <afx.h>
#include <tchar.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <richedit.h>

#ifdef ASSERT
#undef ASSERT
#define ASSERT(cond) \
  if (cond) \
  { \
    char buf[256]; \
    sprintf(buf, \
        "Error: You have encountered a assertion error, please\n" \
        "report to our developer if this error continues.\n\n" \
        "The Content Is:\n\n" \
        "%s:%d: ASSERT: '%s'\n\n" \
        "This program will now exit !\n", \
        __FILE__, __LINE__, #cond); \
    printf("%s:%d:ASSERT:%s\n", __FILE__, __LINE__, #cond); \
    MessageBox(NULL, buf, "ASSERT", MB_ICONERROR); \
    exit(-1); \
  }
#endif

using namespace std;
using namespace Gdiplus;

#include "CLinkUnit.h"
#include "CVariable.h"
#include "CBlockUnit.h"
#include "CDataScope.h"
#include "CSourceEditor.h"
#include "CModelDoc.h"
#include "CModelView.h"
#include "CMessageWnd.h"
#include "CParameterDlg.h"
#include "CAboutDlg.h"
#include "CMainFrame.h"
#include "CModelApp.h"
#include "../Compiler/Compiler.h"
#include "../Resources/Resource.h"

#endif
