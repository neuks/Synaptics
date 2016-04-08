#include "StdAfx.h"

CBlockUnit *g_pTmpBlk = NULL;

//=============================================================================
// Class Initializers
//=============================================================================

CModelView::CModelView(CMainFrame *pParent, CModelDoc *pDoc)
{
  ASSERT(pDoc == NULL);

  m_pDocument = pDoc;
  m_pParent   = pParent;
  m_hIOEditor = NULL;
  m_hCursor   = LoadCursor(NULL, IDC_ARROW);
}

CModelView::~CModelView()
{
}

//=============================================================================
// Class Operators
//=============================================================================

HWND CModelView::Create(string sTitle, HWND hParent)
{
  BOOL bMaximize;

  // Get the maximized state
  SendMessage(hParent, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximize);

  // Update the window style
  DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS |
    WS_CLIPCHILDREN;
  if (bMaximize)
  {
    dwStyle |= WS_MAXIMIZE;
  }

  // Create the window
  return CreateMDIWindow("WINDOW", sTitle.c_str(), dwStyle,
      m_pDocument->m_rRect.X, m_pDocument->m_rRect.Y,
      m_pDocument->m_rRect.Width, m_pDocument->m_rRect.Height,
      hParent, NULL, 0);
}

//=============================================================================
// Message Process Center
//=============================================================================

int CModelView::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
    case WM_PAINT:
      OnPaint(wParam, lParam);
      break;
    case WM_LBUTTONDOWN:
      OnLButtonDown(wParam, lParam);
      break;
    case WM_LBUTTONDBLCLK:
      OnLButtonDblClk(wParam, lParam);
      break;
    case WM_LBUTTONUP:
      OnLButtonUp(wParam, lParam);
      break;
    case WM_RBUTTONDOWN:
      OnRButtonDown(wParam, lParam);
      break;
    case WM_MOUSEMOVE:
      OnMouseMove(wParam, lParam);
      break;
    case WM_HSCROLL:
      OnHScroll(wParam, lParam);
      break;
    case WM_VSCROLL:
      OnVScroll(wParam, lParam);
      break;
    case WM_SIZE:
      OnSize(wParam, lParam);
      break;
    case WM_MOVE:
      OnMove(wParam, lParam);
      break;
    case WM_KEYDOWN:
      OnKeyDown(wParam, lParam);
      break;
    case WM_MOUSEWHEEL:
      OnMouseWheel(wParam, lParam);
      break;
    case WM_ERASEBKGND:
      return 0;
  }

  return DefMDIChildProc(m_hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// Command Process Center
//=============================================================================

#include <assert.h>

void CModelView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  UINT id = LOWORD(wParam);

  switch (id)
  {
    // Subsystem special load/save function
    case IDC_LOADBLK:
      {
        ASSERT(m_pDocument->m_pBlockCurr == NULL);

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
          // Close possibly opened editors
          if (m_pDocument->m_pBlockCurr->m_pScope)
          {
            m_pDocument->m_pBlockCurr->m_pScope->OnDestroy(0, 0);
          }
          if (m_pDocument->m_pBlockCurr->m_pView)
          {
            m_pDocument->m_pBlockCurr->m_pView->OnDestroy(0, 0);
          }

          // Clear previous document
          delete m_pDocument->m_pBlockCurr->m_pDocument;
          m_pDocument->m_pBlockCurr->m_pDocument = new CModelDoc(
              m_pDocument->m_pBlockCurr);

          // Open target file
          fstream file;
          file.open(ofn.lpstrFile, ios::in|ios::binary);

          // Load data from file
          if (m_pDocument->m_pBlockCurr->m_pDocument->Load(&file) == FALSE)
          {
            delete m_pDocument->m_pBlockCurr->m_pDocument;
            m_pDocument->m_pBlockCurr->m_pDocument = new CModelDoc(
                m_pDocument->m_pBlockCurr);
          }

          // End of file operating
          file.close();

          // Compile blocks
          m_pDocument->m_pBlockCurr->m_pDocument->Compile();

          // Update document
          m_pDocument->m_pBlockCurr->m_pDocument->Update();
          m_pDocument->m_pBlockCurr->m_pDocument->Check();

          // Update window
          InvalidateRect(m_hWnd, NULL, TRUE);
        }
      }
      break;
    case IDC_SAVEBLK:
      {
        ASSERT(m_pDocument->m_pBlockCurr == NULL);

        // Prepare for common dialog box
        OPENFILENAME ofn;
        CHAR pBuffer[MAX_PATH];
        
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
        if (GetSaveFileName(&ofn) == TRUE)
        {
          // Save target file
          fstream file;
          file.open(ofn.lpstrFile, ios::out|ios::binary|ios::trunc);

          // Save data to file
          m_pDocument->m_pBlockCurr->m_pDocument->Save(&file);

          // End of file operating
          file.close();
        }
      }
      break;
    case IDC_CUT:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Clean previous temp pointers
        if (g_pTmpBlk)
        {
          delete g_pTmpBlk;
          g_pTmpBlk = NULL;
        }

        // Save temp block
        g_pTmpBlk = m_pDocument->m_pBlockCurr;

        // Remove from display list
        m_pDocument->DelCurrBlock();

        InvalidateRect(m_hWnd, NULL, TRUE);
      }
      break;
    case IDC_COPY:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Clean previous temp pointers
        if (g_pTmpBlk)
        {
          delete g_pTmpBlk;
          g_pTmpBlk = NULL;
        }

        // Copy to temp block
        g_pTmpBlk = new CBlockUnit;
        g_pTmpBlk->m_nType   = m_pDocument->m_pBlockCurr->m_nType;
        g_pTmpBlk->m_sName   = m_pDocument->m_pBlockCurr->m_sName;
        g_pTmpBlk->m_sSource = m_pDocument->m_pBlockCurr->m_sSource;
        g_pTmpBlk->m_rRect   = m_pDocument->m_pBlockCurr->m_rRect;
        g_pTmpBlk->m_rEditor = m_pDocument->m_pBlockCurr->m_rEditor;
        g_pTmpBlk->m_rScope  = m_pDocument->m_pBlockCurr->m_rScope;
        g_pTmpBlk->m_sName   = "Copy of " + g_pTmpBlk->m_sName;

        // Setup for each type of blocks
        switch (g_pTmpBlk->m_nType)
        {
          case CBlockUnit::T_SUBSYSTEM:
            {
              g_pTmpBlk->m_pDocument = new CModelDoc(g_pTmpBlk);
            }
            break;
          case CBlockUnit::T_INPUT:
            {
              g_pTmpBlk->AddInput(new CVariable(g_pTmpBlk->m_sName));
              g_pTmpBlk->AddOutput(new CVariable(g_pTmpBlk->m_sName));
            }
            break;
          case CBlockUnit::T_OUTPUT:
            {
              g_pTmpBlk->AddInput(new CVariable(g_pTmpBlk->m_sName));
              g_pTmpBlk->AddOutput(new CVariable(g_pTmpBlk->m_sName));
            }
            break;
        }

        InvalidateRect(m_hWnd, NULL, TRUE);
      }
      break;
    case IDC_PASTE:
      {
        // Disable when in simulation
        if (g_bRun) break;

        if (g_pTmpBlk)
        {
          POINT pt;

          CBlockUnit *pBlk = m_pDocument->m_pParent;
          while(pBlk != NULL)
          {
            if (g_pTmpBlk == pBlk)
            {
              MessageBox(m_hWnd, "Cannot paste on itself.", "Error", MB_ICONERROR);
              return;
            }

            pBlk = pBlk->m_pParent->m_pParent;
          }

          GetCursorPos(&pt);
          ScreenToClient(m_hWnd, &pt);

          // Add to display list
          m_pDocument->AddBlock(g_pTmpBlk);

          // Empty temp pointer
          g_pTmpBlk = NULL;

          // Enter block moving mode
          if (g_bRun) break;
          m_nState = S_PSTMOVE;
        }
      }
      break;
    case IDC_DELETE:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Delete current selection
        m_pDocument->DelSelection();

        InvalidateRect(m_hWnd, NULL, TRUE);
      }
      break;
    case IDC_MAKEFUNC:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Update mouse cursor
        m_hCursor= LoadCursor(NULL, IDC_CROSS);

        // Switch to make function block mode
        m_nState = S_MAKEFUNC;
      }
      break;
    case IDC_MAKESUBSYS:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_CROSS);

        // Switch to make subsystem block mode
        m_nState = S_MAKESUBSYS;
      }
      break;
    case IDC_MAKETEXT:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_CROSS);

        // Switch to make plugin block mode
        m_nState = S_MAKETEXT;
      }
      break;
    case IDC_MAKEINPUT:
      {
        // Disable when in simulation
        if (g_bRun)
        {
          break;
        }

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_CROSS);

        // Switch to make input port mode
        m_nState = S_MAKEINPUT;
      }
      break;
    case IDC_MAKEOUTPUT:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_CROSS);

        // Switch to make output port mode
        m_nState = S_MAKEOUTPUT;
      }
      break;
    case IDC_MAKELINK:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_CROSS);

        // Switch to make output port mode
        m_nState = S_MAKELINK1;
      }
      break;
    case IDC_LNKEXTEND:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_CROSS);

        // Switch to make output port mode
        m_nState = S_MAKELINK2;
      }
      break;
    case IDC_NODDELETE:
      {
        // Disable when in simulation
        if (g_bRun) break;

        // Delete current selected block
        m_pDocument->DelCurrLinkPoint();

        // Update document
        m_pDocument->Update();
        m_pDocument->Check();
      }
      break;
    case IDC_BLKEDIT:
      {
        m_pDocument->m_pBlockCurr->OpenEditor(GetParent(m_hWnd));
        m_pDocument->m_pBlockCurr->m_pParent = m_pDocument;
      }
      break;
    case IDC_BLKEXPLORER:
      {
        m_pDocument->m_pBlockCurr->OpenScope(GetParent(m_hWnd));
        m_pDocument->m_pBlockCurr->m_pParent = m_pDocument;
      }
      break;
    case IDC_COMPILE:
      {
        // Disable when in simulation
        if (g_bRun) break;

        if (m_pParent)
        {
          m_pParent->ClrMessage();
        }

        // Compile blocks
        m_pDocument->Compile();

        // Update document
        m_pDocument->Update();
        m_pDocument->Check();

        // Update window
        InvalidateRect(m_hWnd, NULL, TRUE);
      }
      break;
    case IDC_GENERATE:
      {
        // Disable when in simulation
        if (g_bRun) break;

        m_pDocument->Generate();
      }
      break;
    case IDC_STARTSIM:
      {
        BOOL bReady = TRUE;

        // Disable when in simulation
        if (g_bRun) break;

        if (m_pParent)
        {
          m_pParent->ClrMessage();
        }

        // Compile blocks
        if (m_pDocument->Compile() == FALSE)
        {
          bReady = FALSE;
        }

        // Update document
        m_pDocument->Update();

        if (m_pDocument->Check() == FALSE)
        {
          bReady = FALSE;
        }

        // Update window
        InvalidateRect(m_hWnd, NULL, TRUE);

        // Show error if any
        if (!bReady)
        {
          break;
        }

        // Start simulation
        m_pDocument->Start();

        // Refresh window
        InvalidateRect(m_hWnd, NULL, TRUE);
      }
      break;
    case IDC_STOPSIM:
      {
        m_pDocument->Stop();
      }
      break;
  }
}

//=============================================================================
// Message Handlers
//=============================================================================

void CModelView::OnCreate(WPARAM wParam, LPARAM lParam)
{
  // Load menu resources
  m_hPopupMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_POPUPMENU));

  // TODO: Load and set window icon
  m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_VIEW));
  SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);

  // Setup initial mode
  m_nState = S_VIEWMODE;
}

void CModelView::OnDestroy(WPARAM wParam, LPARAM lParam)
{
  if (m_pDocument->m_pParent)
  {
    // If not root document, just release current window
    m_pDocument->m_pParent->m_pView = NULL;

    // De-select elements to avoid floating
    if (m_pDocument->m_pBlockCurr)
    {
      m_pDocument->m_pBlockCurr->m_bSelected = FALSE;
      m_pDocument->m_pBlockCurr = NULL;
    }
    m_pDocument->m_pLinkCurr = NULL;
  }
  else
  {
    g_bRun = FALSE;

    m_pParent->m_pMainWnd = NULL;

    delete m_pDocument;
    m_pDocument = NULL;
  }

  // Destroy objects
  DestroyMenu(m_hPopupMenu);
  DestroyIcon(m_hIcon);

  // Delete window object
  delete this;
}

void CModelView::OnPaint(WPARAM wParam, LPARAM lParam)
{
  RECT rect;
  PAINTSTRUCT ps;

  // Stop draw when document empty
  if (!m_pDocument)
  {
    return;
  }

  // Initializations
  GetClientRect(m_hWnd, &rect);
  BeginPaint(m_hWnd, &ps);

  // Create back buffer
  Bitmap bitmap(rect.right, rect.bottom);
  Graphics window(ps.hdc), buffer(&bitmap);
  buffer.SetSmoothingMode(SmoothingModeAntiAlias);
  buffer.Clear(Color(255, 255, 255));
  buffer.TranslateTransform(-m_pDocument->m_nHScroll,
      -m_pDocument->m_nVScroll);

  // Draw the current frame
  m_pDocument->DrawDocument(&buffer);

  // Update current frame
  window.DrawImage(&bitmap, 0, 0);
  EndPaint(m_hWnd, &ps);

  // Setup initial state of scroll bars
  SCROLLINFO si;
  si.cbSize = sizeof(SCROLLINFO);
  si.fMask  = SIF_ALL;
  GetScrollInfo(m_hWnd, SB_HORZ, &si);
  si.nMin  = 0;
  si.nMax  = m_pDocument->GetBound().X + rect.right;
  si.nPage = rect.right;
  SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
  GetScrollInfo(m_hWnd, SB_VERT, &si);
  si.nMin  = 0;
  si.nMax  = m_pDocument->GetBound().Y + rect.bottom;
  si.nPage = rect.bottom;
  SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
}

void CModelView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
  UINT x = GET_X_LPARAM(lParam) + m_pDocument->m_nHScroll;
  UINT y = GET_Y_LPARAM(lParam) + m_pDocument->m_nVScroll;

  switch (m_nState)
  {
    case S_VIEWMODE:
      {
        if (m_hIOEditor)
        {
          // Replace I/O name by name in the box
          int nLength = GetWindowTextLength(m_hIOEditor) + 1;
          CHAR *pString = new CHAR [nLength];
          GetWindowText(m_hIOEditor, pString, nLength);
          if (m_pDocument->m_pBlockCurr->m_nType == CBlockUnit::T_SUBSYSTEM)
          {
            m_pDocument->m_pBlockCurr->m_sName = pString;
          }
          else if (m_pDocument->m_pBlockCurr->m_nType == CBlockUnit::T_TEXTBOX)
          {
            m_pDocument->m_pBlockCurr->m_sName = pString;
          }
          else
          {
            m_pDocument->m_pBlockCurr->m_sName = pString;
            m_pDocument->m_pBlockCurr->m_vInput[0]->m_sName = pString;
            m_pDocument->m_pBlockCurr->m_vOutput[0]->m_sName = pString;
          }
          DestroyWindow(m_hIOEditor);
          m_hIOEditor = NULL;
          delete [] pString;
        }

        // Do selection
        if (wParam & MK_CONTROL)
        {
          m_pDocument->Selected(x, y, TRUE);
        }
        else
        {
          m_pDocument->Selected(x, y, FALSE);
        }

        if (CVariable *pVar = m_pDocument->GetIOPort(ROUND(x), ROUND(y)))
        {
          if (pVar->m_pLink)
          {
            MessageBox(m_hWnd, "Target is already linked", "Error",
                MB_OK|MB_ICONSTOP);
            break;
          }

          // Add to link list
          m_pDocument->AddLink(ROUND(x), ROUND(y));

          // Set cursor to drawing mode
          m_hCursor = LoadCursor(NULL, IDC_CROSS);

          // Switch to link drawing mode
          m_nState = S_MAKELINK2;
        }
      }
      break;
    case S_MAKEFUNC:
      {
        // Create new block data and enqueue
        m_pDocument->AddBlock(CBlockUnit::T_FUNCTION, ROUND(x), ROUND(y));

        // Switch to stage 2
        m_nState = S_MAKEBLOCK;
      }
      break;
    case S_MAKESUBSYS:
      {
        // Create new block data and enqueue
        m_pDocument->AddBlock(CBlockUnit::T_SUBSYSTEM, ROUND(x), ROUND(y));

        // Switch to stage 2
        m_nState = S_MAKEBLOCK;
      }
      break;
    case S_MAKETEXT:
      {
        // Create new block data and enqueue
        m_pDocument->AddBlock(CBlockUnit::T_TEXTBOX, ROUND(x), ROUND(y));

        // Switch to stage 2
        m_nState = S_MAKEBLOCK;
      }
      break;
    case S_MAKEINPUT:
      {
        // Create new block data and enqueue
        m_pDocument->AddBlock(CBlockUnit::T_INPUT, ROUND(x), ROUND(y));

        // Switch to stage 2
        m_nState = S_MAKEBLOCK;
      }
      break;
    case S_MAKEOUTPUT:
      {
        // Create new block data and enqueue
        m_pDocument->AddBlock(CBlockUnit::T_OUTPUT, ROUND(x), ROUND(y));

        // Switch to stage 2
        m_nState = S_MAKEBLOCK;
      }
      break;
    case S_MAKELINK1:
      {
        if (CVariable *pVar = m_pDocument->GetIOPort(ROUND(x), ROUND(y)))
        {
          // Reset if point on connected ports
          if (pVar->m_pLink)
          {
            MessageBox(m_hWnd, "Target is already linked", "Error",
                MB_OK|MB_ICONSTOP);
            break;
          }
        }

        // Add to link list
        m_pDocument->AddLink(ROUND(x), ROUND(y));

        // Switch to link drawing mode
        m_nState = S_MAKELINK2;

        // Set cursor to drawing mode
        m_hCursor = LoadCursor(NULL, IDC_CROSS);
      }
      break;
    case S_MAKELINK2:
      {
        Point pt = m_pDocument->GetCurrLinkLastPoint(), np(x, y);

        // Only can draw rectangle lines
        if (abs(np.X - pt.X) > abs(np.Y - pt.Y))
        {
          np.Y = pt.Y;
        }
        else
        {
          np.X = pt.X;
        }

        CVariable *pVar = m_pDocument->GetIOPort(ROUND(np.X), ROUND(np.Y));
        CLinkUnit *pLnk = m_pDocument->GetLink(ROUND(np.X), ROUND(np.Y), NULL);

        // Reset if point on connected ports
        if (pVar && pVar->m_pLink)
        {
          MessageBox(m_hWnd, "Target is already linked", "Error",
              MB_OK|MB_ICONSTOP);
          break;
        }

        // Add new point
        m_pDocument->AddCurrLinkPoint(ROUND(np.X), ROUND(np.Y));

        if (pVar || pLnk)
        {
          // End of drawing on target
          m_nState = CModelView::S_VIEWMODE;
        }

        // Update document
        m_pDocument->Update();
        m_pDocument->Check();
      }
      break;
  }

  // Update current frame
  InvalidateRect(m_hWnd, NULL, TRUE);
}

void CModelView::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
  UINT x = GET_X_LPARAM(lParam) + m_pDocument->m_nHScroll;
  UINT y = GET_Y_LPARAM(lParam) + m_pDocument->m_nVScroll;

  switch (m_nState)
  {
    case S_VIEWMODE:
      {
        if (m_pDocument->m_pBlockCurr)
        {
          if (g_bRun)
          {
            if (m_pDocument->m_pBlockCurr->m_nType !=
                CBlockUnit::T_TEXTBOX)
            {
              m_pDocument->m_pBlockCurr->OpenScope(GetParent(m_hWnd));
              m_pDocument->m_pBlockCurr->m_pParent = m_pDocument;
            }
          }
          else
          {
            if ((m_pDocument->m_pBlockCurr->m_nType == CBlockUnit::T_INPUT) ||
                (m_pDocument->m_pBlockCurr->m_nType == CBlockUnit::T_OUTPUT))
            {
              m_hIOEditor = CreateWindow("EDIT",
                  m_pDocument->m_pBlockCurr->m_sName.c_str(),
                  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                  m_pDocument->m_pBlockCurr->m_rRect.X -
                  m_pDocument->m_nHScroll,
                  m_pDocument->m_pBlockCurr->m_rRect.Y -
                  m_pDocument->m_nVScroll,
                  m_pDocument->m_pBlockCurr->m_rRect.Width,
                  m_pDocument->m_pBlockCurr->m_rRect.Height,
                  m_hWnd, NULL, NULL, NULL);
              break;
            }
            else if (m_pDocument->m_pBlockCurr->m_nType ==
                CBlockUnit::T_TEXTBOX)
            {
              m_hIOEditor = CreateWindow("EDIT",
                  m_pDocument->m_pBlockCurr->m_sName.c_str(),
                  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL |
                  ES_MULTILINE | ES_AUTOVSCROLL,
                  m_pDocument->m_pBlockCurr->m_rRect.X -
                  m_pDocument->m_nHScroll,
                  m_pDocument->m_pBlockCurr->m_rRect.Y -
                  m_pDocument->m_nVScroll,
                  m_pDocument->m_pBlockCurr->m_rRect.Width,
                  m_pDocument->m_pBlockCurr->m_rRect.Height,
                  m_hWnd, NULL, NULL, NULL);
              break;
            }
            else if (m_pDocument->m_pBlockCurr->m_nType ==
                CBlockUnit::T_SUBSYSTEM)
            {
              if (!g_bRun && m_pDocument->OverCurrBlockTitle(x, y))
              {
                m_hIOEditor = CreateWindow("EDIT",
                    m_pDocument->m_pBlockCurr->m_sName.c_str(),
                    WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                    m_pDocument->m_pBlockCurr->m_rRect.X -
                    m_pDocument->m_nHScroll,
                    m_pDocument->m_pBlockCurr->m_rRect.Y -
                    m_pDocument->m_nVScroll,
                    m_pDocument->m_pBlockCurr->m_rRect.Width,
                    20, m_hWnd, NULL, NULL, NULL);
                break;
              }
              else
              {
                m_pDocument->m_pBlockCurr->OpenEditor(GetParent(m_hWnd));
                m_pDocument->m_pBlockCurr->m_pParent = m_pDocument;
              }
            }
            else
            {
              m_pDocument->m_pBlockCurr->OpenEditor(GetParent(m_hWnd));
              m_pDocument->m_pBlockCurr->m_pParent = m_pDocument;
            }
          }
        }
      }
      break;
  }
}

void CModelView::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
  UINT x = GET_X_LPARAM(lParam) + m_pDocument->m_nHScroll;
  UINT y = GET_Y_LPARAM(lParam) + m_pDocument->m_nVScroll;

  switch (m_nState)
  {
    case S_BLKMOVE:
    case S_LNKMOVE:
    case S_PSTMOVE:
    case S_LPTMOVE:
      {
        // Round the move result
        m_pDocument->RoundSelection();

        // Switch back to view mode
        m_nState = S_VIEWMODE;

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_ARROW);

        // Update document
        m_pDocument->Update();
        m_pDocument->Check();
      }
      break;
    case S_BLKSIZE:
    case S_MAKEBLOCK:
      {
        // Update right bottom connor of block
        m_pDocument->SizeCurrBlock(ROUND(x), ROUND(y));

        // Switch back to view mode
        m_nState = S_VIEWMODE;

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_ARROW);

        // Update document
        m_pDocument->Update();
        m_pDocument->Check();
      }
      break;
    case S_BOXSELECT:
      {
        // Switch back to view mode
        m_nState = S_VIEWMODE;

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_ARROW);
      }
      break;
  }

  // Update current frame
  InvalidateRect(m_hWnd, NULL, TRUE);
}

void CModelView::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
  UINT x = GET_X_LPARAM(lParam) + m_pDocument->m_nHScroll;
  UINT y = GET_Y_LPARAM(lParam) + m_pDocument->m_nVScroll;

  switch (m_nState)
  {
    case S_VIEWMODE:
      {
        POINT pt;
        HMENU hMenu;

        // Get current screen position
        GetCursorPos(&pt);

        // Show menu according to selection type
        if (m_pDocument->Selected(x, y, FALSE))
        {
          if (m_pDocument->m_pBlockCurr)
          {
            if (m_pDocument->m_pBlockCurr->m_nType == CBlockUnit::T_SUBSYSTEM)
            {
              // Display OnSysBlock menu
              hMenu = GetSubMenu(m_hPopupMenu, 2);
            }
            else if (m_pDocument->m_pBlockCurr->m_nType ==
                CBlockUnit::T_TEXTBOX)
            {
              // Display none
              break;
            }
            else
            {
              // Display OnBlock menu
              hMenu = GetSubMenu(m_hPopupMenu, 1);
            }
          }
          else if (m_pDocument->m_pLinkCurr)
          {
            // Disable in simulation
            if (g_bRun) break;

            if (m_pDocument->m_pLinkCurr->PointSelected())
            {
              // Display OnPoint menu
              hMenu = GetSubMenu(m_hPopupMenu, 4);
            }
            else
            {
              // Display OnLink menu
              hMenu = GetSubMenu(m_hPopupMenu, 3);
            }
          }
        }
        else
        {
          // Disable in simulation
          if (g_bRun) break;

          // Display OnBoard menu
          hMenu = GetSubMenu(m_hPopupMenu, 0);
        }

        // Update current frame
        InvalidateRect(m_hWnd, NULL, TRUE);
        UpdateWindow(m_hWnd);

        // Show the menu
        TrackPopupMenu(hMenu, 0, pt.x, pt.y, 0, m_hWnd, NULL);
      }
      break;
    case S_MAKELINK2:
      {
        // Check link validity
        m_pDocument->CheckCurrLink();
      }
    case S_MAKEFUNC:
    case S_MAKESUBSYS:
    case S_MAKETEXT:
    case S_MAKEINPUT:
    case S_MAKEOUTPUT:
    case S_MAKELINK1:
      {
        // Switch to menu mode
        m_nState = S_VIEWMODE;

        // Update mouse cursor
        m_hCursor = LoadCursor(NULL, IDC_ARROW);
      }
      break;
  }

  // Update current frame
  InvalidateRect(m_hWnd, NULL, TRUE);
}

void CModelView::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
  static RECT rSelection;
  static UINT px = 0, py = 0;
  UINT x = GET_X_LPARAM(lParam) + m_pDocument->m_nHScroll;
  UINT y = GET_Y_LPARAM(lParam) + m_pDocument->m_nVScroll;

  SetCursor(m_hCursor);

  switch (m_nState)
  {
    case S_VIEWMODE:
      {
        if (wParam & MK_LBUTTON)
        {
          if (m_pDocument->m_pBlockCurr)
          {
            if (m_pDocument->OverCurrBlockSizeBox(x, y))
            {
              // Enter block sizing mode
              if (g_bRun) break;
              m_nState = S_BLKSIZE;
            }
            else
            {
              // Switch to move mode
              if (g_bRun) break;
              m_nState = S_BLKMOVE;

              // Update mouse cursor
              m_hCursor = LoadCursor(NULL, IDC_SIZEALL);
            }
          }
          else if (m_pDocument->m_pLinkCurr)
          {
            if (m_pDocument->m_pLinkCurr->PointSelected())
            {
              // Switch to move mode
              if (g_bRun) break;
              m_nState = S_LPTMOVE;

              // Update mouse cursor
              m_hCursor = LoadCursor(NULL, IDC_SIZEALL);
            }
            else
            {
              // Switch to move mode
              if (g_bRun) break;
              m_nState = S_LNKMOVE;

              // Update mouse cursor
              m_hCursor = LoadCursor(NULL, IDC_SIZEALL);
            }
          }
          else
          {
            // Setup selection box top-left cornor
            rSelection.left = x - m_pDocument->m_nHScroll;
            rSelection.top  = y - m_pDocument->m_nVScroll;

            // Switch to selection mode
            if (g_bRun) break;
            m_nState = S_BOXSELECT;
          }
        }
        else
        {
          // Show cursor over IO pin
          if (m_pDocument->GetIOPort(ROUND(x), ROUND(y)))
          {
            // Show drawing cursor
            m_hCursor = LoadCursor(NULL, IDC_CROSS);
          }
          else if (m_pDocument->m_pBlockCurr)
          {
            // Show cursor over size box
            if (m_pDocument->OverCurrBlockSizeBox(x, y))
            {
              // Update mouse cursor
              m_hCursor = LoadCursor(NULL, IDC_SIZENWSE);
            }
            else
            {
              // Update mouse cursor
              m_hCursor = LoadCursor(NULL, IDC_ARROW);
            }
          }
          else
          {
            // Update mouse cursor
            m_hCursor = LoadCursor(NULL, IDC_ARROW);
          }
        }
      }
      break;
    case S_PSTMOVE:
      {
        // Paste using pointer position
        m_pDocument->m_pBlockCurr->m_rRect.X = x;
        m_pDocument->m_pBlockCurr->m_rRect.Y = y;

        // Update current frame
        InvalidateRect(m_hWnd, NULL, TRUE);
      }
      break;
    case S_BLKMOVE:
    case S_LNKMOVE:
      {
        // Move current link
        m_pDocument->MoveSelection(x-px, y-py);

        // Update current frame
        InvalidateRect(m_hWnd, NULL, TRUE);
      }
      break;
    case S_LPTMOVE:
      {
        // Move current link
        m_pDocument->MoveCurrLinkPoint(ROUND(x), ROUND(y));

        // Update current frame
        InvalidateRect(m_hWnd, NULL, TRUE);
      }
      break;
    case S_BLKSIZE:
    case S_MAKEBLOCK:
      {
        // Update right bottom connor of block
        m_pDocument->SizeCurrBlock(ROUND(x), ROUND(y));

        // Update current frame
        InvalidateRect(m_hWnd, NULL, TRUE);
      }
      break;
    case S_MAKELINK2:
      {
        Point pt = m_pDocument->GetCurrLinkLastPoint(), np(x, y);

        // Only can draw rectangle lines
        if (abs(np.X - pt.X) > abs(np.Y - pt.Y))
        {
          np.Y = pt.Y;
        }
        else
        {
          np.X = pt.X;
        }

        // Update current frame
        InvalidateRect(m_hWnd, NULL, TRUE);
        UpdateWindow(m_hWnd);

        // Draw link drawing guide
        HDC hDC = GetDC(m_hWnd);
        Graphics graph(hDC);
        Pen pen(Color(0, 0, 0), 2);
        graph.TranslateTransform(-m_pDocument->m_nHScroll,
            -m_pDocument->m_nVScroll);
        graph.DrawLine(&pen, pt.X, pt.Y, ROUND(np.X), ROUND(np.Y));
        ReleaseDC(m_hWnd, hDC);
      }
      break;
    case S_BOXSELECT:
      {
        // Setup selection box bottom-right cornor
        rSelection.right  = x - m_pDocument->m_nHScroll;
        rSelection.bottom = y - m_pDocument->m_nVScroll;

        // Update current frame
        InvalidateRect(m_hWnd, NULL, TRUE);
        UpdateWindow(m_hWnd);

        // Draw selection box
        HDC hDC = GetDC(m_hWnd);
        DrawFocusRect(hDC, &rSelection);
        ReleaseDC(m_hWnd, hDC);

        // Select items in document
        Rect rect(rSelection.left, rSelection.top,
            rSelection.right - rSelection.left, 
            rSelection.bottom - rSelection.top);
        m_pDocument->BoxSelection(rect);
      }
      break;
  }

  // Preserve previous position
  px = x;
  py = y;
}

void CModelView::OnHScroll(WPARAM wParam, LPARAM lParam)
{
  UINT nRequest = LOWORD(wParam);

  SCROLLINFO si;
  si.cbSize = sizeof(SCROLLINFO);
  si.fMask  = SIF_ALL;

  GetScrollInfo(m_hWnd, SB_HORZ, &si);

  // If dragging
  if (nRequest & SB_THUMBTRACK)
  {
    m_pDocument->m_nHScroll = si.nTrackPos;
    ScrollWindow(m_hWnd, si.nPos-si.nTrackPos, 0, NULL, NULL);
    UpdateWindow(m_hWnd);
  }

  // If dragging finished
  if (nRequest & SB_THUMBPOSITION)
  {
    si.nPos = si.nTrackPos;
  }

  SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
}

void CModelView::OnVScroll(WPARAM wParam, LPARAM lParam)
{
  UINT nRequest = LOWORD(wParam);

  SCROLLINFO si;
  si.cbSize = sizeof(SCROLLINFO);
  si.fMask  = SIF_ALL;

  GetScrollInfo(m_hWnd, SB_VERT, &si);

  // If dragging
  if (nRequest & SB_THUMBTRACK)
  {
    m_pDocument->m_nVScroll = si.nTrackPos;
    ScrollWindow(m_hWnd, 0, si.nPos-si.nTrackPos, NULL, NULL);
    UpdateWindow(m_hWnd);
  }

  // If dragging finished
  if (nRequest & SB_THUMBPOSITION)
  {
    si.nPos = si.nTrackPos;
  }

  SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
}

void CModelView::OnSize(WPARAM wParam, LPARAM lParam)
{
  // Setup initial state of scroll bars
  SCROLLINFO si;
  si.cbSize = sizeof(SCROLLINFO);
  si.fMask  = SIF_ALL;
  GetScrollInfo(m_hWnd, SB_HORZ, &si);
  si.nMin  = 0;
  si.nMax  = m_pDocument->GetBound().X + LOWORD(lParam);
  si.nPos  = m_pDocument->m_nHScroll;
  si.nPage = LOWORD(lParam);
  SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
  GetScrollInfo(m_hWnd, SB_VERT, &si);
  si.nMin  = 0;
  si.nMax  = m_pDocument->GetBound().Y + HIWORD(lParam);
  si.nPos  = m_pDocument->m_nVScroll;
  si.nPage = HIWORD(lParam);
  SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);

  // Save window rectangle
  RECT rect, rectParent;
  GetWindowRect(m_hWnd, &rect);
  GetWindowRect(GetParent(m_hWnd), &rectParent);

  m_pDocument->m_rRect = Rect(rect.left - rectParent.left,
      rect.top - rectParent.top, rect.right - rect.left,
      rect.bottom - rect.top);
}

void CModelView::OnMove(WPARAM wParam, LPARAM lParam)
{
  // Save window rectangle
  RECT rect, rectParent;
  GetWindowRect(m_hWnd, &rect);
  GetWindowRect(GetParent(m_hWnd), &rectParent);

  m_pDocument->m_rRect = Rect(rect.left - rectParent.left,
      rect.top - rectParent.top, rect.right - rect.left,
      rect.bottom - rect.top);
}

void CModelView::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
  switch (wParam)
  {
    case VK_DELETE:
      SendMessage(m_hWnd, WM_COMMAND, IDC_DELETE, 0);
      break;
    case 'X':
      if (GetKeyState(VK_CONTROL) & 0x8000)
      {
        SendMessage(m_hWnd, WM_COMMAND, IDC_CUT, 0);
      }
      break;
    case 'C':
      if (GetKeyState(VK_CONTROL) & 0x8000)
      {
        SendMessage(m_hWnd, WM_COMMAND, IDC_COPY, 0);
      }
      break;
    case 'V':
      if (GetKeyState(VK_CONTROL) & 0x8000)
      {
        SendMessage(m_hWnd, WM_COMMAND, IDC_PASTE, 0);
      }
      break;
    case 'S':
      if (GetKeyState(VK_CONTROL) & 0x8000)
      {
        SendMessage(theApp.m_pMainFrame->m_hWnd, WM_COMMAND, IDC_SAVE, 0);
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
}

void CModelView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
  INT nValue = GET_WHEEL_DELTA_WPARAM(wParam);

  SCROLLINFO si;
  si.cbSize = sizeof(SCROLLINFO);
  si.fMask  = SIF_POS;

  // Update scroll info
  GetScrollInfo(m_hWnd, SB_VERT, &si);
  if (nValue > 0)
  {
    si.nPos -= 15;
  }
  else
  {
    si.nPos += 15;
  }
  SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
  OnVScroll(MAKEWPARAM(SB_THUMBTRACK|SB_THUMBPOSITION, 0), 0);

  InvalidateRect(m_hWnd, NULL, TRUE);
}

