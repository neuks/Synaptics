#include "StdAfx.h"

//=============================================================================
// Class Initializers
//=============================================================================

CMessageWnd::CMessageWnd(CMainFrame *pParent)
{
  ASSERT(pParent == NULL);

  m_pParent   = pParent;
  m_hListView = NULL;
}

CMessageWnd::~CMessageWnd()
{
}

//=============================================================================
// Class Operators
//=============================================================================

HWND CMessageWnd::Create()
{
  // Create the window
  return CreateWindowEx(WS_EX_TOOLWINDOW, "WINDOW", "Message",
      WS_VISIBLE | WS_CHILD | WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS,
      CW_USEDEFAULT, CW_USEDEFAULT, 100, 200,
      m_pParent->m_hWnd, NULL, NULL, NULL);
}

VOID CMessageWnd::AddMessage(char *pName, UINT line, UINT column, char *pMsg)
{
  CHAR pLine[10], pColumn[10];
  itoa(line, pLine, 10);
  itoa(column, pColumn, 10);
 
  LVITEM lvi;
  lvi.mask = LVIF_TEXT;
  lvi.iItem = m_vMessages.size();
  lvi.iSubItem = 0;
  lvi.pszText = pName;

  ListView_InsertItem(m_hListView, &lvi);
  ListView_SetItemText(m_hListView, m_vMessages.size(), 1, pLine);
  ListView_SetItemText(m_hListView, m_vMessages.size(), 2, pColumn);
  ListView_SetItemText(m_hListView, m_vMessages.size(), 3, pMsg);

  struct Message msg;

  msg.m_sBlkName = pName;
  msg.m_nLine    = line;
  msg.m_nColumn  = column;
  msg.m_sMessage = pMsg;

  m_vMessages.push_back(msg);
}

VOID CMessageWnd::ClrMessage()
{
  m_vMessages.clear();
  ListView_DeleteAllItems(m_hListView);
}

//=============================================================================
// Message Process Center
//=============================================================================

int CMessageWnd::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_CREATE:
      OnCreate(wParam, lParam);
      break;
    case WM_DESTROY:
      OnDestroy(wParam, lParam);
    case WM_SIZE:
      OnSize(wParam, lParam);
    case WM_NCHITTEST:
      return OnNCHitTest(wParam, lParam);;
    case WM_ERASEBKGND:
      return 0;
  }

  return CWnd::MsgProc(uMsg, wParam, lParam);
}

//=============================================================================
// Message Handlers
//=============================================================================

void CMessageWnd::OnCreate(WPARAM wParam, LPARAM lParam)
{
  m_hListView = CreateWindow(WC_LISTVIEW, "", WS_VISIBLE | WS_CHILD |
      LVS_REPORT | LVS_EDITLABELS | LVS_SINGLESEL, 0, 0, 0, 0, m_hWnd,
      0, NULL, NULL);

  ListView_SetExtendedListViewStyle(m_hListView, LVS_EX_FULLROWSELECT |
      LVS_EX_GRIDLINES);

  LVCOLUMN lvc;
  lvc.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
  lvc.iSubItem = 0;
  lvc.pszText  = "Block Name";
  lvc.cx       = 100;
  lvc.fmt      = LVCFMT_LEFT;
  ListView_InsertColumn(m_hListView, 0, &lvc);
  lvc.iSubItem = 1;
  lvc.pszText  = "Line";
  lvc.cx       = 40;
  lvc.fmt      = LVCFMT_LEFT;
  ListView_InsertColumn(m_hListView, 1, &lvc);
  lvc.iSubItem = 2;
  lvc.pszText  = "Column";
  lvc.cx       = 60;
  lvc.fmt      = LVCFMT_LEFT;
  ListView_InsertColumn(m_hListView, 2, &lvc);
  lvc.iSubItem = 3;
  lvc.pszText  = "Message";
  lvc.cx       = 1000;
  lvc.fmt      = LVCFMT_LEFT;
  ListView_InsertColumn(m_hListView, 3, &lvc);
}

void CMessageWnd::OnDestroy(WPARAM wParam, LPARAM lParam)
{
  // Remove existence
  m_pParent->m_pMessage = NULL;

  // Clear all messages
  ClrMessage();

  // Destroy list view
  DestroyWindow(m_hListView);

  m_pParent->OnSize(0, 0);

  delete this;
}

void CMessageWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
  // Update window position
  m_pParent->OnSize(0, 0);

  RECT rect;
  GetClientRect(m_hWnd, &rect);

  MoveWindow(m_hListView, 0, 0, rect.right, rect.bottom, TRUE);

  LVCOLUMN lvc;
  lvc.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
  lvc.iSubItem = 3;
  lvc.pszText  = "Message";
  lvc.cx       = rect.right - 200;
  lvc.fmt      = LVCFMT_LEFT;
  ListView_SetColumn(m_hListView, 3, &lvc);
}

int CMessageWnd::OnNCHitTest(WPARAM wParam, LPARAM lParam)
{
  UINT nFlags = CWnd::MsgProc(WM_NCHITTEST, wParam, lParam);

  if (nFlags == HTCAPTION || nFlags == HTSYSMENU || nFlags == HTLEFT ||
      nFlags == HTRIGHT || nFlags == HTBOTTOM || nFlags == HTTOPLEFT ||
      nFlags == HTTOPRIGHT || nFlags == HTBOTTOMLEFT || nFlags == HTBOTTOMRIGHT)
  {
    return FALSE;
  }

  return nFlags;
}

