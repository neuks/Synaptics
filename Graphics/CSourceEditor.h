#ifndef __CSourceEditor_h__
#define __CSourceEditor_h__

struct CSourceEditor : CWnd
{
  HMENU m_hMenu;
  HWND  m_hEditor;
  HFONT m_hFont;
  HICON m_hIcon;

  vector<string> m_vKeywords, m_vFunctions;

  CBlockUnit *m_pBlock;

  CSourceEditor(CBlockUnit *pBlock);
  ~CSourceEditor();

  HWND Create(HWND hParent);
  VOID Render(INT nBegin, INT nEnd);
  VOID RenderCurrLine();
  VOID Save();

  int  MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnCommand(WPARAM wParam, LPARAM lParam);
  void OnCreate(WPARAM wParam, LPARAM lParam);
  void OnDestroy(WPARAM wParam, LPARAM lParam);
  void OnSetFocus(WPARAM wParam, LPARAM lParam);
  void OnSize(WPARAM wParam, LPARAM lParam);
  void OnMove(WPARAM wParam, LPARAM lParam);
  int  OnNotify(WPARAM wParam, LPARAM lParam);
};

#endif
