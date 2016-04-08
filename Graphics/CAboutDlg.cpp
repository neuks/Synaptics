#include "StdAfx.h"

//=============================================================================
// Class Initializers
//=============================================================================

CAboutDlg::CAboutDlg()
{
}

CAboutDlg::~CAboutDlg()
{
}

//=============================================================================
// Class Operators
//=============================================================================

//=============================================================================
// Message Process Center
//=============================================================================

int CAboutDlg::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_COMMAND:
      OnCommand(wParam, lParam);
      break;
    case WM_INITDIALOG:
      OnInitDialog(wParam, lParam);
      break;
    case WM_CLOSE:
      OnClose(wParam, lParam);
      break;
  }

  return 0;
}

//=============================================================================
// Command Process Center
//=============================================================================

void CAboutDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
  UINT id = LOWORD(wParam);

  switch (id)
  {
    case IDOK:
      {
        EndDialog(m_hWnd, 0);

        delete this;
      }
      break;
    case IDC_HOMEPAGE:
      {
        ShellExecute(NULL, "open", "http://synaptics.wicp.net:88", NULL, NULL,
            SW_SHOW);
      }
      break;
    case IDC_QQTALK:
      {
        ShellExecute(NULL, "open", "http://shang.qq.com/wpa/qunwpa?idkey=8811ee0426243a9ee211bd0465e80982d2c85fb6239a151dd1f873adc854412c", NULL, NULL,
            SW_SHOW);
      }
      break;
  }
}

//=============================================================================
// Message Handlers
//=============================================================================

void CAboutDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
}

void CAboutDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
  EndDialog(m_hWnd, 0);

  delete this;
}

