#ifndef __CDataScope_h__
#define __CDataScope_h__

struct CDataScope : CWnd
{
  CBlockUnit *m_pBlock;

  INT m_nCounter;
  INT m_nCursorX, m_nCursorY;
  HICON m_hIcon;

  UINT m_nHScroll;
  BOOL m_bMinimized;
  double m_fXScale, m_fYScale;
  vector<deque<double> > m_qData;

  CDataScope(CBlockUnit *pBlock);
  ~CDataScope();

  HWND Create(HWND hParent);
  VOID Reload();
  VOID Update(BOOL bAdd);

  int  MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnCommand(WPARAM wParam, LPARAM lParam);
  void OnCreate(WPARAM wParam, LPARAM lParam);
  void OnDestroy(WPARAM wParam, LPARAM lParam);
  void OnPaint(WPARAM wParam, LPARAM lParam);
  void OnMouseMove(WPARAM wParam, LPARAM lParam);
  void OnSize(WPARAM wParam, LPARAM lParam);
  void OnMove(WPARAM wParam, LPARAM lParam);
  void OnMouseWheel(WPARAM wParam, LPARAM lParam);
  void OnKeyDown(WPARAM wParam, LPARAM lParam);
};

#endif
