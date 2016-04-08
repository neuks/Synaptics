#include "StdAfx.h"

//=============================================================================
// Class Initializers
//=============================================================================

CDataScope::CDataScope(CBlockUnit *pBlock)
{
  ASSERT(pBlock == NULL);

  m_pBlock   = pBlock;
  m_nCursorX = 0;
  m_nCursorY = 0;
  m_fXScale  = 1;
  m_fYScale  = 1;
  m_nHScroll = 0;
  m_nCounter = 0;
  m_bMinimized = FALSE;
}

CDataScope::~CDataScope()
{
}

//=============================================================================
// Class Operators
//=============================================================================

HWND CDataScope::Create(HWND hParent)
{
  BOOL bMaximize;

  // Get the maximized state
  SendMessage(hParent, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximize);

  // Update the window style
  DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN |
    WS_CLIPSIBLINGS;
  if (bMaximize)
  {
    dwStyle |= WS_MAXIMIZE;
  }

  // Create the window
  return CreateMDIWindow("WINDOW", m_pBlock->m_sName.c_str(), dwStyle,
      m_pBlock->m_rScope.X, m_pBlock->m_rScope.Y, m_pBlock->m_rScope.Width,
      m_pBlock->m_rScope.Height, hParent, NULL, 0);
}

VOID CDataScope::Reload()
{
  deque<double> item;

  // Clear all previous data
  m_qData.clear();
  m_nCounter = 0;

  // Load current input signals
  for (int i = 0; i < m_pBlock->m_vInput.size(); i++)
  {
    m_qData.push_back(item);
  }

  // Load current input signals
  for (int i = 0; i < m_pBlock->m_vOutput.size(); i++)
  {
    m_qData.push_back(item);
  }
}

VOID CDataScope::Update(BOOL bAdd)
{
  RECT rect;
  GetClientRect(m_hWnd, &rect);

  // Read data while execution is locked
  for (int i = 0; i < m_qData.size(); i++)
  {
    int nItems = m_qData[i].size();
    int nMaxSz = 10000;

    if (nItems < nMaxSz)
    {
      if (i < m_pBlock->m_vInput.size())
      {
        if (bAdd) m_qData[i].push_back(m_pBlock->m_vInput[i]->m_fValue);
      }
      else
      {
        int j = i - m_pBlock->m_vInput.size();
        if (bAdd) m_qData[i].push_back(m_pBlock->m_vOutput[j]->m_fValue);
      }
    }
    else
    {
      if (i < m_pBlock->m_vInput.size())
      {
        for (int k = 0; k < nItems - nMaxSz; k++)
        {
          m_qData[i].pop_front();
        }
        if (bAdd) m_qData[i].push_back(m_pBlock->m_vInput[i]->m_fValue);
      }
      else
      {
        for (int k = 0; k < nItems - nMaxSz; k++)
        {
          m_qData[i].pop_front();
        }
        int j = i - m_pBlock->m_vInput.size();
        if (bAdd) m_qData[i].push_back(m_pBlock->m_vOutput[j]->m_fValue);
      }
    }
  }

  if (m_nCounter++ % 3 == 0)
  {
    InvalidateRect(m_hWnd, NULL, TRUE);
  }
}

//=============================================================================
// Message Process Center
//=============================================================================

int CDataScope::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
    case WM_MOUSEMOVE:
      OnMouseMove(wParam, lParam);
      break;
    case WM_SIZE:
      OnSize(wParam, lParam);
      break;
    case WM_MOVE:
      OnMove(wParam, lParam);
      break;
    case WM_MOUSEWHEEL:
      OnMouseWheel(wParam, lParam);
      break;
    case WM_KEYDOWN:
      OnKeyDown(wParam, lParam);
      break;
    case WM_ERASEBKGND:
      return 0;
  }

  return DefMDIChildProc(m_hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// Command Process Center
//=============================================================================

void CDataScope::OnCommand(WPARAM wParam, LPARAM lParam)
{
  UINT id = LOWORD(wParam);

  switch (id)
  {
    case IDC_EXIT:
      {
        DestroyWindow(m_hWnd);
      }
      break;
  }
}

//=============================================================================
// Message Handlers
//=============================================================================

void CDataScope::OnCreate(WPARAM wParam, LPARAM lParam)
{
  Reload();

  // Load and set window icon
  m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SCOPE));
  SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
}

void CDataScope::OnDestroy(WPARAM wParam, LPARAM lParam)
{
  // Unregister editor window
  m_pBlock->m_pScope = NULL;

  // Destroy objects
  DestroyIcon(m_hIcon);

  delete this;
}

void CDataScope::OnPaint(WPARAM wParam, LPARAM lParam)
{
  RECT rect;
  PAINTSTRUCT ps;
  INT nValue;
  SolidBrush brush(Color(0, 0, 0));
  Font font(L"Tahoma", 10, FontStyleRegular);

  // Stop rendering if minimized
  if (m_bMinimized)
  {
    return;
  }

  // Begin painting
  GetClientRect(m_hWnd, &rect);
  BeginPaint(m_hWnd, &ps);

  // Create back buffer
  Bitmap bitmap(rect.right, rect.bottom);
  Graphics window(ps.hdc), buffer(&bitmap);
  buffer.SetSmoothingMode(SmoothingModeAntiAlias);
  buffer.Clear(Color(255, 255, 255));

  // Drawing rulers
  Pen ruler(Color(0, 0, 0));
  AdjustableArrowCap arrow(8, 4, TRUE);
  ruler.SetCustomEndCap(&arrow);
  buffer.DrawLine(&ruler, 50, rect.bottom-20, 50, 12);
  buffer.DrawLine(&ruler, 50, rect.bottom-20, rect.right-12, rect.bottom-20);

  // Processing overflow data scrolling
  int nSize = m_qData[0].size();
  int wSize = (int)(((double)rect.right - 70) / m_fXScale);
  int nDiff = 0;
  if (nSize < wSize)
  {
    nDiff = 0;
  }
  else
  {
    nDiff = nSize - wSize;
  }

  // Calculate boundary
  double fMin = 0, fMax = 0;
  if (m_qData.size() > 0)
  {
    fMin = m_qData[0][nDiff];
    fMax = m_qData[0][nDiff];
  }
  for (int i = 0; i < m_qData.size(); i++)
  {
    for (int j = nDiff; j < m_qData[i].size(); j++)
    {
      if (m_qData[i][j] < fMin)
      {
        fMin = m_qData[i][j];
      }
      if (m_qData[i][j] > fMax)
      {
        fMax = m_qData[i][j];
      }
    }
  }
  if (fMax == fMin)
  {
    m_fYScale = 1;
    double nHeight = (rect.bottom - 40) / m_fYScale;
    fMin = fMin - nHeight / 2;
    fMax = fMax + nHeight / 2;
  }
  else
  {
    m_fYScale = (rect.bottom - 40) / (fMax - fMin);
  }

  // Prepare to draw axis labels
  RectF rectf;
  WCHAR buf[256];
  StringFormat format;
  format.SetAlignment(StringAlignmentCenter);
  format.SetLineAlignment(StringAlignmentCenter);

  // Draw Y-axis limit label
  swprintf(buf, L"%f", fMax);
  rectf = RectF(0, 10, 45, 20);
  buffer.DrawString(buf, -1, &font, rectf, &format, &brush);
  swprintf(buf, L"%f", fMin);
  rectf = RectF(0, rect.bottom - 40, 45, 20);
  buffer.DrawString(buf, -1, &font, rectf, &format, &brush);

  // Draw X-axis limit label
  swprintf(buf, L"%f", (double)nDiff / 100);
  rectf = RectF(50, rect.bottom - 20, 45, 20);
  buffer.DrawString(buf, -1, &font, rectf, &format, &brush);
  swprintf(buf, L"%f", (double)(nDiff + wSize) / 100);
  rectf = RectF(rect.right - 50, rect.bottom - 20, 45, 20);
  buffer.DrawString(buf, -1, &font, rectf, &format, &brush);

  // Draw cursor
  Pen cursor(Color(100, 100, 100));
  if ((m_nCursorX >= 50) && (m_nCursorX <= rect.right - 20) &&
      (m_nCursorY >= 20) && (m_nCursorY <= rect.bottom - 20))
  {
    buffer.DrawLine(&cursor, m_nCursorX, rect.bottom-20, m_nCursorX, 12);
    buffer.DrawLine(&cursor, 50, m_nCursorY, rect.right-20, m_nCursorY);

    // Draw Y cursor label
    swprintf(buf, L"%f",
        (double)(rect.bottom - m_nCursorY - 20) / (rect.bottom - 40) *
        (fMax - fMin) + fMin);
    rectf = RectF(0, m_nCursorY - 10, 45, 20);
    buffer.DrawString(buf, -1, &font, rectf, &format, &brush);

    // Draw X cursor label
    swprintf(buf, L"%f", ((m_nCursorX - 50) / m_fXScale + nDiff) / 100);
    rectf = RectF(m_nCursorX - 25, rect.bottom - 20, 50, 20);
    buffer.DrawString(buf, -1, &font, rectf, &format, &brush);
  }

  // Draw item
  Pen line(Color(0, 0, 255));
  for (int i = 0; i < m_qData.size(); i++)
  {
    if (m_qData[i].size() > 0)
    {
      UINT nInputs = m_pBlock->m_vInput.size();
      if (i < nInputs)
      {
        line.SetColor(Color(m_pBlock->m_vInput[i]->m_nColor));
      }
      else
      {
        line.SetColor(Color(m_pBlock->m_vOutput[i - nInputs]->m_nColor));
      }
      // Draw the signals
      for (int j = nDiff; j < m_qData[i].size() - 1; j++)
      {
        buffer.DrawLine(&line,
            (REAL)(50 + m_fXScale * (j - nDiff)),
            (REAL)(rect.bottom - 20 - m_fYScale * (m_qData[i][j] - fMin)),
            (REAL)(50 + m_fXScale * (j - nDiff + 1)),
            (REAL)(rect.bottom - 20 - m_fYScale * (m_qData[i][j + 1] - fMin)));
      }
    }
  }

  // Draw labels
  format.SetAlignment(StringAlignmentFar);
  for (int i = 0; i < m_qData.size(); i++)
  {
    wstring ws;
    rectf = RectF(rect.right - 200, 25 + 20 * i, 180, 20);

    // Setup color and convert strings
    if (m_qData[i].size() > 0)
    {
      UINT nInputs = m_pBlock->m_vInput.size();
      if (i < nInputs)
      {
        brush.SetColor(Color(m_pBlock->m_vInput[i]->m_nColor));
        ws = wstring(m_pBlock->m_vInput[i]->m_sName.begin(),
            m_pBlock->m_vInput[i]->m_sName.end());
      }
      else
      {
        brush.SetColor(Color(m_pBlock->m_vOutput[i - nInputs]->m_nColor));
        ws = wstring(m_pBlock->m_vOutput[i - nInputs]->m_sName.begin(),
            m_pBlock->m_vOutput[i - nInputs]->m_sName.end());
      }

      // Draw the signal labels
      buffer.DrawString(ws.c_str(), -1, &font, rectf, &format, &brush);
    }
  }

  // Draw trademark
  brush.SetColor(Color(0, 0, 0));
  rectf = RectF(rect.right - 200, 0, 190, 20);
  buffer.DrawString(L"(C) Synaptics INC.", -1, &font, rectf, &format, &brush);

  // Finish painting
  window.DrawImage(&bitmap, 0, 0);
  EndPaint(m_hWnd, &ps);
}

void CDataScope::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
  m_nCursorX = GET_X_LPARAM(lParam);
  m_nCursorY = GET_Y_LPARAM(lParam);

  // Set mouse cursor
  SetCursor(LoadCursor(NULL, IDC_CROSS));

  InvalidateRect(m_hWnd, NULL, TRUE);
}

void CDataScope::OnSize(WPARAM wParam, LPARAM lParam)
{
  if (wParam == SIZE_MINIMIZED)
  {
    m_bMinimized = TRUE;
  }
  else if ((wParam == SIZE_RESTORED) || (wParam == SIZE_MAXIMIZED))
  {
    m_bMinimized = FALSE;
  }

  // Get current window rect
  RECT rect, rectParent;
  GetWindowRect(m_hWnd, &rect);
  GetWindowRect(GetParent(m_hWnd), &rectParent);

  // Save current window rect
  m_pBlock->m_rScope = Rect(rect.left - rectParent.left,
      rect.top - rectParent.top, rect.right - rect.left,
      rect.bottom - rect.top);

  // Update current window
  Update(FALSE);
}

void CDataScope::OnMove(WPARAM wParam, LPARAM lParam)
{
  // Get current window rect
  RECT rect, rectParent;
  GetWindowRect(m_hWnd, &rect);
  GetWindowRect(GetParent(m_hWnd), &rectParent);

  // Save current window rect
  m_pBlock->m_rScope = Rect(rect.left - rectParent.left,
      rect.top - rectParent.top, rect.right - rect.left,
      rect.bottom - rect.top);
}

void CDataScope::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
  INT nValue = GET_WHEEL_DELTA_WPARAM(wParam);

  // Update scroll info
  if (nValue > 0)
  {
    m_fXScale *= 0.9;
  }
  else
  {
    m_fXScale /= 0.9;
  }

  InvalidateRect(m_hWnd, NULL, TRUE);
}

void CDataScope::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
  switch (wParam)
  {
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

