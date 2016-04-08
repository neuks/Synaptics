#include "StdAfx.h"

//=============================================================================
// Class Initializers
//=============================================================================

CMainFrame::CMainFrame()
{
  m_pMessage = NULL;
  m_pMainWnd = NULL;
}

CMainFrame::~CMainFrame()
{
}

//=============================================================================
// Class Operators
//=============================================================================

HWND CMainFrame::Create(string sTitle)
{
  m_hMainMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MAINFRAME));

  return CWnd::Create(sTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE |
      WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT, NULL, m_hMainMenu);
}

VOID CMainFrame::AddMessage(char *pName, UINT nLine, UINT nColumn, char *pMsg)
{
  if (!m_pMessage)
  {
    SendMessage(m_hWnd, WM_COMMAND, IDC_MESSAGE, 0);
    m_pMessage->AddMessage(pName, nLine, nColumn, pMsg);
  }
  else
  {
    m_pMessage->AddMessage(pName, nLine, nColumn, pMsg);
  }
}

VOID CMainFrame::ClrMessage()
{
  if (m_pMessage)
  {
    m_pMessage->ClrMessage();
  }
}

//=============================================================================
// Message Process Center
//=============================================================================

int CMainFrame::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
    case WM_SIZE:
      OnSize(wParam, lParam);
      return 1;
    case WM_TIMER:
      OnTimer(wParam, lParam);
      return 1;
    case WM_ERASEBKGND:
      return 0;
  }

  return DefFrameProc(m_hWnd, m_hClient, uMsg, wParam, lParam);
}

//=============================================================================
// Command Process Center
//=============================================================================

void CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
  UINT id = LOWORD(wParam);

  switch (id)
  {
    case IDC_NEW:
      {
        // Ask of to discard previous document
        if (m_pMainWnd && MessageBox(m_hWnd, "Discard current document ?",
              "New", MB_OKCANCEL) == IDCANCEL)
        {
          return;
        }

        // Delete current document and create new
        if (m_pMainWnd)
        {
          m_pMainWnd->OnDestroy(0, 0);
        }
        m_pMainWnd = new CModelView(this, new CModelDoc);
        m_pMainWnd->Create("Root Document", m_hClient);
        m_sFileName.clear();

        // Update window
        InvalidateRect(m_pMainWnd->m_hWnd, NULL, TRUE);
      }
      break;
    case IDC_OPEN:
      {
        // Prepare for common dialog box
        OPENFILENAME ofn;
        CHAR pBuffer[260];

        pBuffer[0] = 0;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize     = sizeof(OPENFILENAME);
        ofn.hwndOwner       = m_hWnd;
        ofn.lpstrFile       = pBuffer;
        ofn.nMaxFile        = sizeof(pBuffer);
        ofn.lpstrDefExt     = ".syn";
        ofn.lpstrFilter     = "Network File(*.syn)\0*.syn\0";
        ofn.nFilterIndex    = 1;
        ofn.lpstrFileTitle  = NULL;
        ofn.nMaxFileTitle   = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Open selecte file dialog
        if (GetOpenFileName(&ofn) == TRUE)
        {
          // Clear previous document
          if (m_pMainWnd)
          {
            m_pMainWnd->OnDestroy(0, 0);
          }
          m_pMainWnd = new CModelView(this, new CModelDoc);

          // Assign file name
          m_sFileName = ofn.lpstrFile;

          // Open target file
          fstream file;
          file.open(m_sFileName.c_str(), ios::in|ios::binary);

          // Load data from file
          if (m_pMainWnd->m_pDocument->Load(&file) == FALSE)
          {
            // If error, clean file name
            m_sFileName.clear();

            // Create the window
            m_pMainWnd->Create("Root Document", m_hClient);

            // Release error data
            m_pMainWnd->OnDestroy(0, 0);
            m_pMainWnd = NULL;
          }
          else
          {
            // Create the model window
            m_pMainWnd->Create("Root Document", m_hClient);

            // Compile blocks
            m_pMainWnd->m_pDocument->Compile();

            // Update document
            m_pMainWnd->m_pDocument->Update();
            m_pMainWnd->m_pDocument->Check();

            // Update window
            InvalidateRect(m_pMainWnd->m_hWnd, NULL, TRUE);
          }

          // End of file operating
          file.close();
        }
      }
      break;
    case IDC_SAVE:
      {
        // Prepare for common dialog box
        OPENFILENAME ofn;
        CHAR pBuffer[MAX_PATH];

        if (!m_pMainWnd)
        {
          break;
        }

        // Save documents if command is processed
        HWND hCurrWnd = (HWND)SendMessage(m_hClient, WM_MDIGETACTIVE, 0, 0);
        SendMessage(hCurrWnd, WM_COMMAND, wParam, lParam);

        pBuffer[0] = 0;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize     = sizeof(OPENFILENAME);
        ofn.hwndOwner       = m_hWnd;
        ofn.lpstrFile       = pBuffer;
        ofn.nMaxFile        = MAX_PATH;
        ofn.lpstrDefExt     = ".syn";
        ofn.lpstrFilter     = "Network File(*.syn)\0*.syn\0";
        ofn.nFilterIndex    = 1;
        ofn.lpstrFileTitle  = NULL;
        ofn.nMaxFileTitle   = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Open selecte file dialog
        if (m_sFileName.size() > 0)
        {
          // Open target file
          fstream file;
          file.open(m_sFileName.c_str(), ios::out|ios::binary|ios::trunc);

          // Save data to file
          m_pMainWnd->m_pDocument->Save(&file);

          // End of file operating
          file.close();
        }
        else if (GetSaveFileName(&ofn) == TRUE)
        {
          // Assign file name
          m_sFileName = ofn.lpstrFile;

          // Save target file
          fstream file;
          file.open(m_sFileName.c_str(), ios::out|ios::binary|ios::trunc);

          // Save data to file
          m_pMainWnd->m_pDocument->Save(&file);

          // End of file operating
          file.close();
        }
      }
      break;
    case IDC_SAVEAS:
      {
        // Prepare for common dialog box
        OPENFILENAME ofn;
        CHAR pBuffer[260];

        if (!m_pMainWnd)
        {
          break;
        }

        pBuffer[0] = 0;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize     = sizeof(OPENFILENAME);
        ofn.hwndOwner       = m_hWnd;
        ofn.lpstrFile       = pBuffer;
        ofn.nMaxFile        = sizeof(pBuffer);
        ofn.lpstrDefExt     = ".syn";
        ofn.lpstrFilter     = "Network File(*.syn)\0*.syn\0";
        ofn.nFilterIndex    = 1;
        ofn.lpstrFileTitle  = NULL;
        ofn.nMaxFileTitle   = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Open selecte file dialog
        if (GetSaveFileName(&ofn) == TRUE)
        {
          // Assign file name
          m_sFileName = ofn.lpstrFile;

          // Save target file
          fstream file;
          file.open(m_sFileName.c_str(), ios::out|ios::binary|ios::trunc);

          // Save data to file
          m_pMainWnd->m_pDocument->Save(&file);

          // End of file operating
          file.close();
        }
      }
      break;
    case IDC_EXIT:
      {
        // Exit current window
        DestroyWindow(m_hWnd);
      }
      break;
    case IDC_CASCADE:
      {
        SendMessage(m_hClient, WM_MDICASCADE, 0, 0);
      }
      break;
    case IDC_HTILE:
      {
        SendMessage(m_hClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
      }
      break;
    case IDC_VTILE:
      {
        SendMessage(m_hClient, WM_MDITILE, MDITILE_VERTICAL, 0);
      }
      break;
    case IDC_ARRANGE:
      {
        SendMessage(m_hClient, WM_MDIICONARRANGE, 0, 0);
      }
      break;
    case IDC_STARTSIM:
      {
        if (m_pMainWnd)
        {
          SendMessage(m_pMainWnd->m_hWnd, WM_COMMAND, wParam, lParam);
          SetTimer(m_hWnd, 0, 200, NULL);
        }
      }
      break;
    case IDC_STOPSIM:
      {
        if (m_pMainWnd)
        {
          SendMessage(m_pMainWnd->m_hWnd, WM_COMMAND, wParam, lParam);
        }
      }
      break;
    case IDC_COMPILE:
      {
        if (m_pMainWnd)
        {
          SendMessage(m_hStatus, SB_SETTEXT, 0, (WPARAM)"Compiling...");
          SendMessage(m_pMainWnd->m_hWnd, WM_COMMAND, wParam, lParam);
          SendMessage(m_hStatus, SB_SETTEXT, 0, (WPARAM)"Stopped");
        }
      }
      break;
    case IDC_CUT:
    case IDC_COPY:
    case IDC_PASTE:
    case IDC_DELETE:
    case IDC_MAKEINPUT:
    case IDC_MAKEOUTPUT:
    case IDC_MAKEFUNC:
    case IDC_MAKESUBSYS:
    case IDC_MAKETEXT:
      {
        HWND hCurrWnd = (HWND)SendMessage(m_hClient, WM_MDIGETACTIVE, 0, 0);
        SendMessage(hCurrWnd, WM_COMMAND, wParam, lParam);
      }
      break;
    case IDC_MESSAGE:
      {
        // Create message window
        if (!m_pMessage)
        {
          m_pMessage = new CMessageWnd(this);
          m_pMessage->Create();
          OnSize(0, 0);
        }
      }
      break;
    case IDC_ABOUT:
      {
        CAboutDlg dlg;
        DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUT), m_hWnd, (DLGPROC)_WndProc);
      }
      break;
    case IDC_PARAMETER:
      {
        if (m_pMainWnd)
        {
          CParameterDlg dlg(m_pMainWnd->m_pDocument);
          DialogBox(NULL, MAKEINTRESOURCE(IDD_PARAMETER), m_hWnd,
              (DLGPROC)_WndProc);
        }
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

void CMainFrame::OnCreate(WPARAM wParam, LPARAM lParam)
{
  // Create MDI client window
  CLIENTCREATESTRUCT ccs;
  ccs.hWindowMenu = GetSubMenu(GetMenu(m_hWnd), 5);
  ccs.idFirstChild = 3;

  m_hClient = CreateWindow("MDICLIENT", NULL, WS_CHILD | WS_VISIBLE |
      WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL | WS_HSCROLL,
      0, 0, 0, 0, m_hWnd, (HMENU)IDR_CLIENT, NULL, &ccs); 

  // Create status bar
  m_hStatus = CreateStatusWindow(WS_VISIBLE | WS_CHILD | SBARS_SIZEGRIP,
      "Stopped", m_hWnd, IDR_STATUS);

  // Create rebar
  m_hRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL,
      WS_CHILD | WS_VISIBLE |WS_BORDER| WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
      RBS_VARHEIGHT | CCS_NODIVIDER | RBS_AUTOSIZE | RBS_VARHEIGHT |
      RBS_BANDBORDERS, 0, 0, 0, 0, m_hWnd, (HMENU)IDR_REBAR, NULL,
      NULL);

  // Initialize rebar
  REBARINFO rbi;
  rbi.cbSize = sizeof(REBARINFO);
  ZeroMemory(&rbi, sizeof(REBARINFO));
  SendMessage(m_hRebar, RB_SETBARINFO, 0, (LPARAM)&rbi);

  // Load toolbar from resource
  m_hToolbar = LoadToolbar(NULL, TBSTYLE_FLAT | TBSTYLE_WRAPABLE |
      CCS_NODIVIDER |CCS_NOPARENTALIGN | CCS_NORESIZE , IDR_TOOLBAR, m_hRebar);
  
  // Insert toolbar into rebar
  SIZE size;
  REBARBANDINFO rbbi;
  SendMessage(m_hToolbar, TB_GETMAXSIZE, 0, (LPARAM)&size);
  ZeroMemory(&rbbi, sizeof(REBARBANDINFO));
  rbbi.cbSize     = sizeof(REBARBANDINFO);
  rbbi.fMask      = RBBIM_CHILD| RBBIM_CHILDSIZE | RBBIM_STYLE;
  rbbi.fStyle     = RBBS_GRIPPERALWAYS; 
  rbbi.hwndChild  = m_hToolbar;
  rbbi.cxMinChild = size.cx;
  rbbi.cyMinChild = size.cy;
  SendMessage(m_hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbbi);

  // TODO: Load and set window icon
  m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APP));
  SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
  SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);

  // Open command argument file if exists and valid
  if (__argc == 2)
  {
    // Create default model window
    m_pMainWnd = new CModelView(this, new CModelDoc);

    // Assign file name
    m_sFileName = __argv[1];

    // Open target file
    fstream file;
    file.open(__argv[1], ios::in|ios::binary);

    // Load data from file
    if (m_pMainWnd->m_pDocument->Load(&file) == FALSE)
    {
      // If error, clean file name
      m_sFileName.clear();

      // Create the window
      m_pMainWnd->Create("Root Document", m_hClient);

      // Release error data
      m_pMainWnd->OnDestroy(0, 0);
      m_pMainWnd = NULL;
    }
    else
    {
      // Create the window
      m_pMainWnd->Create("Root Document", m_hClient);

      // Compile blocks
      m_pMainWnd->m_pDocument->Compile();

      // Update document
      m_pMainWnd->m_pDocument->Update();
      m_pMainWnd->m_pDocument->Check();

      // Update window
      InvalidateRect(m_pMainWnd->m_hWnd, NULL, TRUE);
    }

    // End of file operating
    file.close();
  }
}

void CMainFrame::OnDestroy(WPARAM wParam, LPARAM lParam)
{
  if (m_pMainWnd)
  {
    m_pMainWnd->OnDestroy(0, 0);
  }

  DestroyIcon(m_hIcon);
  PostQuitMessage(0);
}

void CMainFrame::OnSize(WPARAM wParam, LPARAM lParam)
{
  // Update toolbar & status bar position
  SendMessage(m_hRebar,  WM_SIZE, 0, 0);
  SendMessage(m_hStatus, WM_SIZE, 0, 0);

  // update MDI client area
  RECT rReBar, rStatus, rClient, rMessage;

  GetWindowRect(m_hRebar,   &rReBar);
  GetWindowRect(m_hStatus,  &rStatus);
  GetClientRect(m_hWnd,     &rClient);

  int x = 0;
  int y = rReBar.bottom    - rReBar.top;
  int w = rClient.right    - rClient.left;
  int h = (rClient.bottom  - rClient.top ) -
    (rReBar.bottom   - rReBar.top  ) -
    (rStatus.bottom  - rStatus.top );

  if (m_pMessage)
  {
    GetWindowRect(m_pMessage->m_hWnd, &rMessage);
    h -= rMessage.bottom - rMessage.top;

    MoveWindow(m_pMessage->m_hWnd, 0, h + rReBar.bottom - rReBar.top, w,
        rMessage.bottom - rMessage.top, TRUE);
  }

  MoveWindow(m_hClient, x, y, w, h, TRUE);
}

void CMainFrame::OnTimer(WPARAM wParam, LPARAM lParam)
{
  if (g_bRun)
  {
    SendMessage(m_hStatus, SB_SETTEXT, 0, (WPARAM)"Running ...");
  }
  else
  {
    KillTimer(m_hWnd, 0);
    SendMessage(m_hStatus, SB_SETTEXT, 0, (WPARAM)"Stopped");
  }
}

