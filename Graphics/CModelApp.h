#ifndef __CModelApp_h__
#define __CModelApp_h__

struct CModelApp : CApp
{
  CMainFrame *m_pMainFrame;

  ULONG_PTR m_nGdiToken;

  void OnInit();
  void OnExit();
};

DECLARE_APP(CModelApp);

#endif
