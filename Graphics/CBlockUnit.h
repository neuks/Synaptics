#ifndef __CBlockUnit_h__
#define __CBlockUnit_h__

#define ROUND(v) (((v + 5) / 10) * 10)

struct CModelDoc;
struct CModelView;
struct CDataScope;
struct CSourceEditor;

struct Function
{
  string m_sName;
  struct stmt *m_pProgram;
};

struct CBlockUnit
{
  enum
  {
    T_FUNCTION,
    T_SUBSYSTEM,
    T_TEXTBOX,
    T_INPUT,
    T_OUTPUT,
  };

  // Static data
  UINT m_nType;
  string m_sName, m_sSource;
  Rect m_rRect, m_rEditor, m_rScope;

  // Dynamic data
  INT m_lName;
  double m_fPrevTime;
  CModelView *m_pView;
  CDataScope *m_pScope;
  CSourceEditor *m_pEditor;
  struct stmt *m_pProgram;
  BOOL m_bSelected, m_bError;
  CModelDoc *m_pDocument, *m_pParent;
  vector<CVariable*> m_vInput, m_vOutput, m_vGlobal, m_vStack;
  vector<Function> m_vLibrary;

  CBlockUnit();
  CBlockUnit(UINT nType, UINT x, UINT y);
  ~CBlockUnit();

  void InitBlock();
  void DrawBlock(Graphics *pGraph);
  void MoveBlock(UINT dx, UINT dy);
  void SizeBlock(UINT x, UINT y);
  BOOL SelectBlock(UINT x, UINT y);
  void BoxSelection(Rect rect);
  BOOL OverSizeBox(UINT x, UINT y);
  BOOL OverTitle(UINT x, UINT y);
  void RoundBlock();
  UINT GetMinWidth();
  UINT GetMinHeight();

  void AddLibrary(string sName, struct stmt *stmt);
  void ClrLibrary();
  void AddInput(CVariable *pVar);
  void DelInput(CVariable *pVar);
  void ClrInput();
  void AddOutput(CVariable *pVar);
  void DelOutput(CVariable *pVar);
  void ClrOutput();
  void AddGlobal(CVariable *pVar, double fValue);
  void DelGlobal(CVariable *pVar);
  void ClrGlobal();
  void AddStack(CVariable *pVar, double fValue);
  void PshStack();
  void PopStack();
  void PshHeap();
  void PopHeap();
  void ClrStack();

  struct stmt *GetLibrary(string sName);
  CVariable *GetVariable(string sName, BOOL bCheck);
  CVariable *GetIOPort(UINT x, UINT y);

  void OpenEditor(HWND hParent);
  void OpenScope(HWND hParent);
  BOOL Check();
  BOOL Compile();
  void PreExecute();
  BOOL Execute(double fTime, double fDTime);
};

#endif
