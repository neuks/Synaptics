#ifndef __CMessageWnd_h__
#define __CMessageWnd_h_

struct Message
{
  string m_sBlkName;
  UINT m_nLine, m_nColumn;
  string m_sMessage;
};

struct CMessageWnd : CWnd
{
  HWND m_hListView;
  CMainFrame *m_pParent;
  vector<Message> m_vMessages;

  CMessageWnd(CMainFrame *pParent);
  ~CMessageWnd();

  HWND Create();
  VOID AddMessage(char *pName, UINT line, UINT column, char *pMsg);
  VOID ClrMessage();

  int  MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnCreate(WPARAM wParam, LPARAM lParam);
  void OnDestroy(WPARAM wParam, LPARAM lParam);
  void OnSize(WPARAM wParam, LPARAM lParam);
  int  OnNCHitTest(WPARAM wParam, LPARAM lParam);
  void OnNotify(WPARAM wParam, LPARAM lParam);
};

#endif
