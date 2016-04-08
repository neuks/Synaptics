#ifndef __CParameterDlg_h__
#define __CParameterDlg_h__

struct CParameterDlg : CWnd
{
  CParameterDlg(CModelDoc *pDoc);
  ~CParameterDlg();

  CModelDoc *m_pDocument;

  int  MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnCommand(WPARAM wParam, LPARAM lParam);
  void OnInitDialog(WPARAM wParam, LPARAM lParam);
  void OnClose(WPARAM wParam, LPARAM lParam);
};

#endif
