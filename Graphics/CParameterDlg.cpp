#include "StdAfx.h"

//=============================================================================
// Class Initializers
//=============================================================================

CParameterDlg::CParameterDlg(CModelDoc *pDoc)
{
  ASSERT(pDoc == NULL);

  m_pDocument = pDoc;
}

CParameterDlg::~CParameterDlg()
{
}

//=============================================================================
// Message Process Center
//=============================================================================

int CParameterDlg::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

void CParameterDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
  UINT id = LOWORD(wParam);

  switch (id)
  {
    case IDOK:
      {
        char pBuffer[256];

        // Save selection boxes
        HWND hAlgo = GetDlgItem(m_hWnd, IDB_ALGO);
        m_pDocument->m_bFixdTime = SendMessage(hAlgo, CB_GETCURSEL, 0, 0);
        HWND hLimit = GetDlgItem(m_hWnd, IDB_LIMIT);
        m_pDocument->m_nCPULimit = SendMessage(hLimit, CB_GETCURSEL, 0, 0);

        // Save text box values
        m_pDocument->m_nSlepTime = GetDlgItemInt(m_hWnd, IDB_SLEEP, NULL,
            FALSE);
        m_pDocument->m_fDuraTime = GetDlgItemInt(m_hWnd, IDB_DURATION, NULL,
            FALSE);
        m_pDocument->m_fStepTime = GetDlgItemInt(m_hWnd, IDB_STEP, NULL,
            FALSE) / 1000.0f;

        // Do error checks
        if (m_pDocument->m_bFixdTime)
        {
          if ((m_pDocument->m_fDuraTime == 0) ||
              (m_pDocument->m_fStepTime == 0))
          {
            MessageBox(m_hWnd, "Fixed time parameters must not be zero.",
                "Error", MB_ICONERROR);
            break;
          }
        }

        OnClose(0, 0);
      }
      break;
    case IDCANCEL:
      {
        OnClose(0, 0);
      }
      break;
  }
}

//=============================================================================
// Message Handlers
//=============================================================================

void CParameterDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
  char pBuffer[256];

  // Insert items to selection boxes
  HWND hAlgo = GetDlgItem(m_hWnd, IDB_ALGO);
  SendMessage(hAlgo, CB_ADDSTRING, 0, (LPARAM)"Realtime Simulation");
  SendMessage(hAlgo, CB_ADDSTRING, 0, (LPARAM)"Fixed-Time Simulation");
  SendMessage(hAlgo, CB_SETCURSEL, m_pDocument->m_bFixdTime, 0);

  HWND hLimit = GetDlgItem(m_hWnd, IDB_LIMIT);
  SendMessage(hLimit, CB_ADDSTRING, 0, (LPARAM)"100 %");
  SendMessage(hLimit, CB_ADDSTRING, 0, (LPARAM)"75 %");
  SendMessage(hLimit, CB_ADDSTRING, 0, (LPARAM)"50 %");
  SendMessage(hLimit, CB_ADDSTRING, 0, (LPARAM)"25 %");
  SendMessage(hLimit, CB_SETCURSEL, m_pDocument->m_nCPULimit, 0);

  // Setup read-only text boxes
  sprintf(pBuffer, "%f", m_pDocument->m_fCostTime * 1000);
  SetDlgItemText(m_hWnd, IDB_CYCLE, pBuffer);
  sprintf(pBuffer, "%f", m_pDocument->m_fCurrTime);
  SetDlgItemText(m_hWnd, IDB_CURRENT, pBuffer);
  sprintf(pBuffer, "%d", m_pDocument->m_nSlepTime);
  SetDlgItemText(m_hWnd, IDB_SLEEP, pBuffer);
  sprintf(pBuffer, "%.0f", m_pDocument->m_fDuraTime);
  SetDlgItemText(m_hWnd, IDB_DURATION, pBuffer);
  sprintf(pBuffer, "%.0f", m_pDocument->m_fStepTime * 1000);
  SetDlgItemText(m_hWnd, IDB_STEP, pBuffer);
}

void CParameterDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
  EndDialog(m_hWnd, 0);

  delete this;
}

