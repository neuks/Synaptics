#ifndef __CModelDoc_h__
#define __CModelDoc_h__

struct CModelDoc
{
  enum
  {
    T_LNKBEGIN,
    T_LNKITEM,
    T_BLKITEM,
    T_SUBSYS,
    T_FINISH,
  };

  Rect m_rRect;
  list<CBlockUnit*> m_lBlockList;
  list<CLinkUnit*>  m_lLinkList;
  INT m_nHScroll, m_nVScroll;

  BOOL m_bFixdTime;
  UINT m_nSlepTime, m_nCPULimit;
  double m_fCurrTime, m_fCostTime, m_fDuraTime, m_fStepTime;
  CBlockUnit *m_pBlockCurr, *m_pParent;
  CLinkUnit  *m_pLinkCurr;

  CModelDoc();
  CModelDoc(CBlockUnit *pParent);
  ~CModelDoc();

  void DrawDocument(Graphics *pGraph);
  BOOL Selected(UINT x, UINT y, BOOL bAdd);
  void BoxSelection(Rect rect);
  void MoveSelection(UINT dx, UINT dy);
  void RoundSelection();
  void CleanSelection();
  void DelSelection();
  Point GetBound();

  void AddBlock(CBlockUnit *pBlock);
  void AddBlock(UINT nType, UINT x, UINT y);
  void SizeCurrBlock(UINT x, UINT y);
  BOOL OverCurrBlockSizeBox(UINT x, UINT y);
  BOOL OverCurrBlockTitle(UINT x, UINT y);
  void DelCurrBlock();

  void AddLink(UINT x, UINT y);
  void AddCurrLinkPoint(UINT x, UINT y);
  Point GetCurrLinkLastPoint();
  void MoveCurrLinkPoint(UINT x, UINT y);
  void DelCurrLinkPoint();
  void CheckCurrLink();

  CVariable *GetIOPort(UINT x, UINT y);
  CLinkUnit *GetLink(UINT x, UINT y, CLinkUnit *pExcept);

  void Update();
  BOOL Check();
  BOOL Compile();
  void PreExecute();
  BOOL Execute(double fTime, double fDTime);
  BOOL Start();
  void Stop();

  BOOL Save(fstream *pFile);
  BOOL Load(fstream *pFile);
  BOOL Generate();
};

extern BOOL g_bRun;
DWORD WINAPI SimuThread(LPVOID lpParam);

#endif
