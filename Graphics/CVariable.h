#ifndef __CVariable_h__
#define __CVariable_h__

struct CVariable
{
  enum
  {
    T_COMMON,
    T_INPUT,
    T_OUTPUT,
    T_SYSTEM,
    T_STACK,
    T_HEAP,
  };

  UINT   m_nType;
  UINT   m_nColor;
  Point  m_ptPort;
  string m_sName;
  INT    m_lName;
  double m_fValue;

  CLinkUnit *m_pLink;

  CVariable();
  CVariable(string sName);
  ~CVariable();

  void InitIOPort(INT x, INT y);
  void DrawIOPort(Graphics *pGraph, INT x, INT y);
};

#endif
