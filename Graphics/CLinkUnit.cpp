#include "StdAfx.h"

//=============================================================================
// Initializer / Deinitializer
//=============================================================================

CLinkUnit::CLinkUnit()
{
  // Setup initial values
  m_bSelected = FALSE;
  m_pVarBegin = NULL;
  m_pVarEnd   = NULL;
  m_pLnkBegin = NULL;
  m_pLnkEnd   = NULL;
  m_bError    = FALSE;
}

CLinkUnit::CLinkUnit(UINT x, UINT y)
{
  // Setup initial values
  m_bSelected = FALSE;
  m_pVarBegin = NULL;
  m_pVarEnd   = NULL;
  m_pLnkBegin = NULL;
  m_pLnkEnd   = NULL;
  m_bError    = FALSE;

  // Add initial point
  AddPoint(x, y);
}

CLinkUnit::~CLinkUnit()
{
  if (!m_pLnkBegin && m_pVarBegin)
  {
    m_pVarBegin->m_pLink = NULL;
  }
  if (!m_pLnkEnd && m_pVarEnd)
  {
    m_pVarEnd->m_pLink = NULL;
  }
}

//=============================================================================
// Object Operators
//=============================================================================

void CLinkUnit::DrawLink(Graphics *pGraph)
{
  Pen pen(Color(0, 0, 0));
  SolidBrush brush(Color(0, 0, 0));

  // Show selection
  if (m_bSelected)
  {
    pen.SetWidth(2);
  }

  // Outline error connections
  if (m_bError)
  {
    pen.SetColor(Color(255, 100, 100));
    pen.SetWidth(2);
  }
  else if ((m_pVarBegin != NULL) || (m_pVarEnd != NULL))
  {
    pen.SetColor(Color(100, 100, 255));
    pen.SetWidth(2);
  }

  // Draw line body
  for (int i=0; i<m_vPointList.size()-1; i++)
  {
    pGraph->DrawLine(&pen, (INT)m_vPointList[i].X, m_vPointList[i].Y,
        m_vPointList[i + 1].X, m_vPointList[i + 1].Y);
  }

  // Draw points
  if (m_bSelected) for (int i=0; i<m_vPointList.size(); i++)
  {
    // Mark if point is current
    if (m_nPointCurr == i+1)
    {
      brush.SetColor(Color(0, 0, 0));
    }
    else
    {
      brush.SetColor(Color(150, 150, 150));
    }

    // Draw selection box
    pGraph->FillRectangle(&brush, (INT)m_vPointList[i].X - 3,
       m_vPointList[i].Y - 3, 6, 6);
  }
  else
  {
    // Draw link bridge points
    if (m_pLnkBegin && m_pVarBegin)
    {
      Point pt = *m_vPointList.begin();
      Rect rect(pt.X-3, pt.Y-3, 6, 6);
      pGraph->FillEllipse(&brush, rect);
    }
    if (m_pLnkEnd && m_pVarEnd)
    {
      Point pt = *m_vPointList.rbegin();
      Rect rect(pt.X-3, pt.Y-3, 6, 6);
      pGraph->FillEllipse(&brush, rect);
    }
  }
}

bool CLinkUnit::SelectLink(UINT x, UINT y)
{
  BOOL bResult = FALSE;

  m_nPointCurr = 0;

  for (int i=0; i<m_vPointList.size()-1; i++)
  {
    // position of each point
    float xa = m_vPointList[i].X;
    float ya = m_vPointList[i].Y;
    float xb = m_vPointList[i + 1].X;
    float yb = m_vPointList[i + 1].Y;
    float xc = x;
    float yc = y;

    // length of each border
    float lenA = sqrt((xb - xc) * (xb - xc) + (yb - yc) * (yb - yc));
    float lenB = sqrt((xa - xc) * (xa - xc) + (ya - yc) * (ya - yc));
    float lenC = sqrt((xa - xb) * (xa - xb) + (ya - yb) * (ya - yb));

    // calculate distance from point to line
    float p = (lenA + lenB + lenC) / 2;
    float S = sqrt(p * (p - lenA) * (p - lenB) * (p - lenC));
    float d = 2 * S / lenC;

    // if point is out of line section
    if (lenC * lenC + lenB * lenB < (lenA + 3) * (lenA + 3))
    {
      if (lenB < 3)
      {
        m_nPointCurr = i + 1;
        bResult = TRUE;
      }
    }
    else if (lenC * lenC + lenA * lenA < (lenB + 3) * (lenB + 3))
    {
      if (lenA < 3)
      {
        m_nPointCurr = i + 2;
        bResult = TRUE;
      }
    }
    else if (d < 3)
    {
      m_nPointCurr = 0;
      bResult = TRUE;
    }
  }

  return bResult;
}

void CLinkUnit::BoxSelection(Rect rect)
{
  for (int i=0; i<m_vPointList.size(); i++)
  {
    // position of each point
    INT x = m_vPointList[i].X;
    INT y = m_vPointList[i].Y;

    if (rect.Contains(x, y))
    {
      // mark the selection
      m_bSelected = TRUE;

      return;
    }
  }

  m_bSelected = FALSE;
}

void CLinkUnit::MoveLink(UINT dx, UINT dy)
{
  for (int i=0; i<m_vPointList.size(); i++)
  {
    m_vPointList[i].X += dx;
    m_vPointList[i].Y += dy;
  }
}

void CLinkUnit::RoundLink()
{
  for (int i=0; i<m_vPointList.size(); i++)
  {
    m_vPointList[i].X = ROUND(m_vPointList[i].X);
    m_vPointList[i].Y = ROUND(m_vPointList[i].Y);
  }
}

bool CLinkUnit::PointSelected()
{
  return m_nPointCurr;
}

void CLinkUnit::AddPoint(UINT x, UINT y)
{
  Point pt;

  pt.X = x;
  pt.Y = y;

  m_vPointList.push_back(pt);
  m_nPointCurr = m_vPointList.size();
}

void CLinkUnit::DelCurrPoint()
{
  if (m_vPointList.size() < 3)
  {
    // Abort delete if nodes < 3
    return;
  }
  else
  {
    m_vPointList.erase(m_vPointList.begin() + m_nPointCurr - 1);
    m_nPointCurr = 0;
  }
}

void CLinkUnit::MoveCurrPoint(UINT x, UINT y)
{
  if (m_nPointCurr)
  {
    m_vPointList[m_nPointCurr - 1].X = x;
    m_vPointList[m_nPointCurr - 1].Y = y;
  }
}

UINT CLinkUnit::GetPointCount()
{
  return m_vPointList.size();
}

Point CLinkUnit::GetLastPoint()
{
  return *m_vPointList.rbegin();
}

void CLinkUnit::OpenEditor()
{
}

BOOL CLinkUnit::Check()
{
  m_bError = FALSE;

  // Check for invalid connections
  if ((m_pVarBegin != NULL) && (m_pVarEnd == NULL))
  {
    // <---X
    if (m_pLnkBegin)
    {
      m_bError = m_pLnkBegin->m_bError;
    }
    if (m_pVarBegin->m_nType != CVariable::T_OUTPUT)
    {
      m_bError = TRUE;
    }
  }
  else if ((m_pVarBegin == NULL) && (m_pVarEnd != NULL))
  {
    // X--->
    if (m_pLnkEnd)
    {
      m_bError = m_pLnkEnd->m_bError;
    }
    if (m_pVarEnd->m_nType != CVariable::T_OUTPUT)
    {
      m_bError = TRUE;
    }
  }
  else if ((m_pVarBegin != NULL) && (m_pVarEnd != NULL))
  {
    // <--->
    if (m_pLnkBegin)
    {
      m_bError = m_pLnkBegin->m_bError;
    }
    if (m_pLnkEnd)
    {
      m_bError = m_pLnkEnd->m_bError;
    }
    if ((m_pVarBegin->m_nType != CVariable::T_OUTPUT) &&
        (m_pVarEnd->m_nType   != CVariable::T_OUTPUT))
    {
      m_bError = TRUE;
    }
    if (m_pVarBegin->m_nType == m_pVarEnd->m_nType)
    {
      m_bError = TRUE;
    }
  }

  return !m_bError;
}

BOOL CLinkUnit::Execute()
{
  if (m_pVarBegin && m_pVarEnd)
  {
    if (m_pVarBegin->m_nType == CVariable::T_OUTPUT)
    {
      m_pVarEnd->m_fValue = m_pVarBegin->m_fValue;
    }
    else if (m_pVarEnd->m_nType == CVariable::T_OUTPUT)
    {
      m_pVarBegin->m_fValue = m_pVarEnd->m_fValue;
    }
    else
    {
      return FALSE;
    }
  }
  
  return TRUE;
}

