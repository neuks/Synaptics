#ifndef __CLinkUnit_h__
#define __CLinkUnit_h__

#define ROUND(v) (((v + 5) / 10) * 10)

struct CVariable;

struct CLinkUnit
{
  // Static data
  vector<Point> m_vPointList;

  // Dynamic data
  UINT m_nPointCurr;
  BOOL m_bSelected, m_bError;
  CVariable *m_pVarBegin, *m_pVarEnd;
  CLinkUnit *m_pLnkBegin, *m_pLnkEnd;

  CLinkUnit();
  CLinkUnit(UINT x, UINT y);
  ~CLinkUnit();

  void DrawLink(Graphics *pGraph);
  bool SelectLink(UINT x, UINT y);
  void BoxSelection(Rect rect);
  void MoveLink(UINT dx, UINT dy);
  void RoundLink();
  bool PointSelected();
  void AddPoint(UINT x, UINT y);
  void DelCurrPoint();
  void MoveCurrPoint(UINT x, UINT y);
  UINT GetPointCount();
  Point GetLastPoint();

  void OpenEditor();
  BOOL Check();
  BOOL Execute();
};

#endif
