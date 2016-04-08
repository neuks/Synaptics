#ifndef __CModelView_h__
#define __CModelView_h__

#define ROUND(v) (((v + 5) / 10) * 10)
#define HEIGHT(r) (r.bottom - r.top)
#define WIDTH(r) (r.right - r.left)

//=============================================================================
// Model View Class
//
// Designing Philosophy:
//   This class is responsible for all graphic manipulations, the model data
//   only responsible for maintaining and acquiring data storage.
//=============================================================================

struct CMainFrame;

extern CBlockUnit *g_pTmpBlk;

struct CModelView : CWnd
{
  enum
  {
    S_VIEWMODE,
    S_BLKMOVE,
    S_LNKMOVE,
    S_PSTMOVE,
    S_LPTMOVE,
    S_BLKSIZE,
    S_MAKEFUNC,
    S_MAKESUBSYS,
    S_MAKETEXT,
    S_MAKEINPUT,
    S_MAKEOUTPUT,
    S_MAKEBLOCK,
    S_MAKELINK1,
    S_MAKELINK2,
    S_BOXSELECT
  };

  UINT  m_nState;
  HWND  m_hIOEditor;
  HICON m_hIcon;
  HMENU m_hPopupMenu;
  HCURSOR m_hCursor;
  CMainFrame *m_pParent;
  CModelDoc *m_pDocument;


  CModelView(CMainFrame *pParent, CModelDoc *pDoc);
  ~CModelView();

  HWND Create(string sTitle, HWND hParent);

  int  MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnCommand(WPARAM wParam, LPARAM lParam);
  void OnCreate(WPARAM wParam, LPARAM lParam);
  void OnDestroy(WPARAM wParam, LPARAM lParam);
  void OnPaint(WPARAM wParam, LPARAM lParam);
  void OnLButtonDown(WPARAM wParam, LPARAM lParam);
  void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
  void OnLButtonUp(WPARAM wParam, LPARAM lParam);
  void OnRButtonDown(WPARAM wParam, LPARAM lParam);
  void OnMouseMove(WPARAM wParam, LPARAM lParam);
  void OnHScroll(WPARAM wParam, LPARAM lParam);
  void OnVScroll(WPARAM wParam, LPARAM lParam);
  void OnSize(WPARAM wParam, LPARAM lParam);
  void OnMove(WPARAM wParam, LPARAM lParam);
  void OnKeyDown(WPARAM wParam, LPARAM lParam);
  void OnMouseWheel(WPARAM wParam, LPARAM lParam);
};

#endif
