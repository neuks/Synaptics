#ifndef __CAboutDlg_h__
#define __CAboutDlg_h__

struct CAboutDlg : CWnd
{
  CAboutDlg();
  ~CAboutDlg();

  int  MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnCommand(WPARAM wParam, LPARAM lParam);
  void OnInitDialog(WPARAM wParam, LPARAM lParam);
  void OnClose(WPARAM wParam, LPARAM lParam);
};

#endif
