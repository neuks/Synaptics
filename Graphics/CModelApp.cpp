#include "StdAfx.h"

void CModelApp::OnInit()
{
  // Initialize GDI+
  GdiplusStartupInput Input;
  GdiplusStartup(&m_nGdiToken, &Input, NULL);

  // Initialize common controls
  INITCOMMONCONTROLSEX icc;
  InitCommonControlsEx(&icc);

  // Initialize richedit control
  LoadLibrary("Riched32.dll");

  // Create main window
  m_pMainFrame = new CMainFrame();
  m_pMainFrame->Create("Synaptic Designer R2005");
}

void CModelApp::OnExit()
{
  // Deinitialize GDI+
  GdiplusShutdown(m_nGdiToken);
}

IMPLEMENT_APP(CModelApp);

