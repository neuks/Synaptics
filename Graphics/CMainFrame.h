#ifndef __CMainFrame_h__
#define __CMainFrame_h__

#define IDR_CLIENT 50000
#define IDR_STATUS 50001
#define IDR_REBAR  50002

struct CMainFrame : CWnd
{
  HICON m_hIcon;
  HMENU m_hMainMenu;
  HWND m_hClient;
  HWND m_hStatus;
  HWND m_hToolbar;
  HWND m_hRebar;

  string m_sFileName;

  CModelView *m_pMainWnd;
  CMessageWnd *m_pMessage;

  CMainFrame();
  ~CMainFrame();

  HWND Create(string sTitle);
  VOID AddMessage(char *pName, UINT nLine, UINT nColumn, char *pMsg);
  VOID ClrMessage();

  int  MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnCommand(WPARAM wParam, LPARAM lParam);
  void OnCreate(WPARAM wParam, LPARAM lParam);
  void OnDestroy(WPARAM wParam, LPARAM lParam);
  void OnSize(WPARAM wParam, LPARAM lParam);
  void OnTimer(WPARAM wParam, LPARAM lParam);
};

#endif
