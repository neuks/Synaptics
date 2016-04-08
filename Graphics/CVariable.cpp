#include "StdAfx.h"

//=============================================================================
// Initializer/Deinitializers
//=============================================================================

CVariable::CVariable()
{
  // Initialize internal data
  m_nType  = T_COMMON;
  m_nColor = 0xff0000ff;
  m_pLink  = NULL;
  m_lName  = 0;
  m_sName  = "";
  m_fValue = 0;
}

CVariable::CVariable(string sName)
{
  // Initialize internal data
  m_nType = T_COMMON;
  m_nColor = 0xff0000ff;
  m_pLink  = NULL;
  m_lName  = 0;
  m_sName  = sName;
  m_fValue = 0;
}

CVariable::~CVariable()
{
  // Unlink variables when deinitializing
  if (m_pLink)
  {
    if (m_pLink->m_pVarBegin == this)
    {
      m_pLink->m_pVarBegin = NULL;
    }
    if (m_pLink->m_pVarEnd == this)
    {
      m_pLink->m_pVarEnd = NULL;
    }
  }
}

//=============================================================================
// Class Manipulators
//=============================================================================

void CVariable::InitIOPort(INT x, INT y)
{
  if (m_nType == T_INPUT)
  {
    // Save connection point
    m_ptPort.X = x - 10;
    m_ptPort.Y = y;
  }
  else if (m_nType == T_OUTPUT)
  {
    // Save connection point
    m_ptPort.X = x + 10;
    m_ptPort.Y = y;
  }
}

void CVariable::DrawIOPort(Graphics *pGraph, INT x, INT y)
{
  Pen pen(Color(0, 0, 0));
  SolidBrush brush(Color(0, 0, 0));
  Font font(L"Tahoma", 10);
  StringFormat format;

  if (m_nType == T_INPUT)
  {
    // Save connection point
    m_ptPort.X = x - 10;
    m_ptPort.Y = y;

    // Show connectivity
    if (m_pLink)
    {
      pen.SetWidth(2);
    }

    // Draw connection pin
    pGraph->DrawLine(&pen, x, y, m_ptPort.X, m_ptPort.Y);

    // Draw blue text if in link
    if (m_pLink)
    {
      brush.SetColor(Color(0, 0, 200));
    }

    // Draw port text
    RectF rect(x, y-10, 0, 20), rectSize;
    format.SetAlignment(StringAlignmentNear);
    format.SetLineAlignment(StringAlignmentCenter);
    wstring ws(m_sName.begin(), m_sName.end());
    pGraph->DrawString(ws.c_str(), -1, &font, rect, &format, &brush);

    // Measure port text width
    pGraph->MeasureString(ws.c_str(), -1, &font, rect, &format, &rectSize);
    m_lName = (INT)rectSize.Width;
  }
  else if (m_nType == T_OUTPUT)
  {
    // Save connection point
    m_ptPort.X = x + 10;
    m_ptPort.Y = y;

    // Show connectivity
    if (m_pLink)
    {
      pen.SetWidth(2);
    }

    // Draw connection pin
    pGraph->DrawLine(&pen, x, y, m_ptPort.X, m_ptPort.Y);

    // Draw blue text if in link
    if (m_pLink)
    {
      brush.SetColor(Color(0, 0, 200));
    }

    // Draw port text
    RectF rect(x, y-10, 0, 20), rectSize;
    format.SetAlignment(StringAlignmentFar);
    format.SetLineAlignment(StringAlignmentCenter);
    wstring ws(m_sName.begin(), m_sName.end());
    pGraph->DrawString(ws.c_str(), -1, &font, rect, &format, &brush);

    // Measure port text width
    pGraph->MeasureString(ws.c_str(), -1, &font, rect, &format, &rectSize);
    m_lName = (INT)rectSize.Width;
  }
}

