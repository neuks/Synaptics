#include "StdAfx.h"

//=============================================================================
// Class Initializers
//=============================================================================

CSourceEditor::CSourceEditor(CBlockUnit *pBlock)
{
  ASSERT(pBlock == NULL);

  m_pBlock = pBlock;

  // Setup keywords
  m_vKeywords.push_back("include");
  m_vKeywords.push_back("module");
  m_vKeywords.push_back("function");
  m_vKeywords.push_back("var");
  m_vKeywords.push_back("if");
  m_vKeywords.push_back("else");
  m_vKeywords.push_back("for");

  // Setup special functions
  m_vFunctions.push_back("TIME");
  m_vFunctions.push_back("DTIME");
  m_vFunctions.push_back("sin");
  m_vFunctions.push_back("asin");
  m_vFunctions.push_back("cos");
  m_vFunctions.push_back("acos");
  m_vFunctions.push_back("tan");
  m_vFunctions.push_back("atan");
  m_vFunctions.push_back("exp");
  m_vFunctions.push_back("log");
  m_vFunctions.push_back("log10");
  m_vFunctions.push_back("sqrt");
  m_vFunctions.push_back("ceil");
  m_vFunctions.push_back("floor");
  m_vFunctions.push_back("srand");
  m_vFunctions.push_back("rand");
  m_vFunctions.push_back("warn");
  m_vFunctions.push_back("error");
}

CSourceEditor::~CSourceEditor()
{
}

//=============================================================================
// Class Operators
//=============================================================================

HWND CSourceEditor::Create(HWND hParent)
{
  BOOL bMaximize;

  // Get the maximized state
  SendMessage(hParent, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximize);

  // Update the window style
  DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
  if (bMaximize)
  {
    dwStyle |= WS_MAXIMIZE;
  }

  // Create the window
  return CreateMDIWindow("WINDOW", m_pBlock->m_sName.c_str(), dwStyle,
      m_pBlock->m_rEditor.X, m_pBlock->m_rEditor.Y, m_pBlock->m_rEditor.Width, 
      m_pBlock->m_rEditor.Height, hParent, NULL, 0);
}

VOID CSourceEditor::Render(INT nBegin, INT nEnd)
{
  CHARRANGE cr;
  FINDTEXTEX ft;

  UINT nCond = FR_DOWN | FR_MATCHCASE | FR_WHOLEWORD;

  // Preserve currsor position
  SendMessage(m_hEditor, EM_EXGETSEL, 0, (LPARAM)&cr);

  // Setup rendering format and render all to black
  CHARFORMAT cf;
  cf.cbSize      = sizeof(CHARFORMAT);
  cf.dwMask      = CFM_COLOR;
  cf.dwEffects   = 0;
  cf.crTextColor = RGB(0, 0, 0);

  // Clear previous highlighting
  if (nEnd == -1)
  {
    // Full clean
    SendMessage(m_hEditor, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
  }
  else
  {
    // Line clean
    CHARRANGE cr;
    cr.cpMin = nBegin;
    cr.cpMax = nEnd;
    SendMessage(m_hEditor, EM_EXSETSEL, 0, (LPARAM)&cr);
    SendMessage(m_hEditor, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
  }

  // Render keywords
  cf.crTextColor = RGB(0, 0, 255);
  for (int i = 0; i < m_vKeywords.size(); i++)
  {
    // Adjust to beginning
    ft.chrg.cpMin = nBegin;
    ft.chrg.cpMax = nEnd;

    // Assign word
    ft.lpstrText = (char*)m_vKeywords[i].c_str();

    // Search & render
    while (SendMessage(m_hEditor, EM_FINDTEXTEX, nCond, (LPARAM)&ft) != -1)
    {
      SendMessage(m_hEditor, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
      SendMessage(m_hEditor, EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD,
          (LPARAM)&cf);
      ft.chrg.cpMin = ft.chrgText.cpMax + 1;
    }
  }
  
  // Render special functions
  cf.crTextColor = RGB(255, 0, 0);
  for (int i = 0; i < m_vFunctions.size(); i++)
  {
    // Adjust to beginning
    ft.chrg.cpMin = nBegin;
    ft.chrg.cpMax = nEnd;

    // Assign word
    ft.lpstrText = (char*)m_vFunctions[i].c_str();

    // Search & render
    while (SendMessage(m_hEditor, EM_FINDTEXTEX, nCond, (LPARAM)&ft) != -1)
    {
      SendMessage(m_hEditor, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
      SendMessage(m_hEditor, EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD,
          (LPARAM)&cf);
      ft.chrg.cpMin = ft.chrgText.cpMax + 1;
    }
  }

  // Restore currsor position
  SendMessage(m_hEditor, EM_EXSETSEL, 0, (LPARAM)&cr);
}

VOID CSourceEditor::RenderCurrLine()
{
  UINT nLine  = SendMessage(m_hEditor, EM_LINEFROMCHAR, (WPARAM)-1, 0);
  UINT nBegin = SendMessage(m_hEditor, EM_LINEINDEX, nLine, 0);
  UINT nEnd   = SendMessage(m_hEditor, EM_LINELENGTH, nBegin, 0) + nBegin;

  // Do rendering operation
  Render(nBegin, nEnd);
}

VOID CSourceEditor::Save()
{
  int nLength = GetWindowTextLength(m_hEditor) + 1;
  char *pBuffer = (char*)malloc(nLength);

  // Save current text
  GetWindowText(m_hEditor, pBuffer, nLength);
  m_pBlock->m_sSource = pBuffer;

  free(pBuffer);
}

//=============================================================================
// Message Process Center
//=============================================================================

int CSourceEditor::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_COMMAND:
      OnCommand(wParam, lParam);
      break;
    case WM_CREATE:
      OnCreate(wParam, lParam);
      break;
    case WM_DESTROY:
      OnDestroy(wParam, lParam);
      break;
    case WM_SETFOCUS:
      OnSetFocus(wParam, lParam);
      break;
    case WM_SIZE:
      OnSize(wParam, lParam);
      break;
    case WM_MOVE:
      OnMove(wParam, lParam);
      break;
    case WM_NOTIFY:
      return OnNotify(wParam, lParam);
  }

  return DefMDIChildProc(m_hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// Command Process Center
//=============================================================================

void CSourceEditor::OnCommand(WPARAM wParam, LPARAM lParam)
{
  UINT id = LOWORD(wParam);
  UINT msg = HIWORD(wParam);

  switch (id)
  {
    case IDC_CUT:
      SendMessage(m_hEditor, WM_CUT, 0, 0);
      break;
    case IDC_COPY:
      SendMessage(m_hEditor, WM_COPY, 0, 0);
      break;
    case IDC_PASTE:
      SendMessage(m_hEditor, WM_PASTE, 0, 0);
      break;
    case IDC_DELETE:
      SendMessage(m_hEditor, WM_CLEAR, 0, 0);
      break;
    case IDC_SAVE:
      Save();
      break;
  }

  switch (msg)
  {
    case EN_CHANGE:
      RenderCurrLine();
      break;
  }
}

//=============================================================================
// Message Handlers
//=============================================================================

void CSourceEditor::OnCreate(WPARAM wParam, LPARAM lParam)
{
  // Create editor window
  m_hEditor = CreateWindow(RICHEDIT_CLASS10A , m_pBlock->m_sSource.c_str(),
      WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL |
      ES_AUTOVSCROLL | ES_MULTILINE, 0, 0, 10, 10, m_hWnd, NULL, NULL, NULL);

  // Load popup menu
  m_hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_EDITOR));

  // Update editor font
  m_hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
      GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
      DEFAULT_QUALITY, DEFAULT_PITCH, "FixedSys");
  SendMessage(m_hEditor, WM_SETFONT, (WPARAM)m_hFont, TRUE);
  
  // Load and set window icon
  m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_EDITOR));
  SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);

  // Set editor tab size
  UINT nSize = 8;
  SendMessage(m_hEditor, EM_SETTABSTOPS, 1, (LPARAM)&nSize);

  // Set editor margins
  SendMessage(m_hEditor, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
      MAKELPARAM(3, 3));

  // Initial render of text colors
  Render(0, -1);

  // Setup notification messages
  SendMessage(m_hEditor, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_MOUSEEVENTS |
      ENM_KEYEVENTS);
}

void CSourceEditor::OnDestroy(WPARAM wParam, LPARAM lParam)
{
  // Unregister editor window
  m_pBlock->m_pEditor = NULL;

  // Clear allocated objects
  DestroyIcon(m_hIcon);
  DestroyMenu(m_hMenu);
  DeleteObject(m_hFont);
  DestroyWindow(m_hEditor);

  delete this;
}

void CSourceEditor::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
  SetFocus(m_hEditor);
}

void CSourceEditor::OnSize(WPARAM wParam, LPARAM lParam)
{
  // Adjust editor window
  MoveWindow(m_hEditor, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);

  // Get current window rect
  RECT rect, rectParent;
  GetWindowRect(m_hWnd, &rect);
  GetWindowRect(GetParent(m_hWnd), &rectParent);

  // Save current window rect
  m_pBlock->m_rEditor = Rect(rect.left - rectParent.left,
      rect.top - rectParent.top, rect.right - rect.left,
      rect.bottom - rect.top);
}

void CSourceEditor::OnMove(WPARAM wParam, LPARAM lParam)
{
  // Get current window rect
  RECT rect, rectParent;
  GetWindowRect(m_hWnd, &rect);
  GetWindowRect(GetParent(m_hWnd), &rectParent);

  // Save current window rect
  m_pBlock->m_rEditor = Rect(rect.left - rectParent.left,
      rect.top - rectParent.top, rect.right - rect.left,
      rect.bottom - rect.top);
}

int CSourceEditor::OnNotify(WPARAM wParam, LPARAM lParam)
{
  MSGFILTER *pFilter = (MSGFILTER*)lParam;

  switch (pFilter->msg)
  {
    case WM_RBUTTONDOWN:
      {
        POINT pt;
        HMENU hMenu;

        // Load the menu
        hMenu = GetSubMenu(m_hMenu, 0);

        // Get current screen position
        GetCursorPos(&pt);

        // Display the menu
        TrackPopupMenu(hMenu, 0, pt.x, pt.y, 0, m_hWnd, NULL);
      }
      break;
    case WM_KEYDOWN:
      switch (pFilter->wParam)
      {
        case 'S':
          if (GetKeyState(VK_CONTROL) & 0x8000)
          {
            SendMessage(theApp.m_pMainFrame->m_hWnd, WM_COMMAND, IDC_SAVE, 0);
          }
          break;
        case 'Z':
          if (GetKeyState(VK_CONTROL) & 0x8000)
          {
            return TRUE;
          }
          break;
        case VK_F5:
          SendMessage(theApp.m_pMainFrame->m_hWnd, WM_COMMAND, IDC_COMPILE, 0);
          break;
        case VK_F6:
          SendMessage(theApp.m_pMainFrame->m_hWnd, WM_COMMAND, IDC_STARTSIM, 0);
          break;
        case VK_F7:
          SendMessage(theApp.m_pMainFrame->m_hWnd, WM_COMMAND, IDC_STOPSIM, 0);
          break;
        case VK_F8:
          SendMessage(theApp.m_pMainFrame->m_hWnd, WM_COMMAND, IDC_GENERATE, 0);
          break;
      }
      break;
  }

  return 0;
}

