#include "StdAfx.h"

//=============================================================================
// Initializer/Deinitializers
//=============================================================================

CBlockUnit::CBlockUnit()
{
  // Setup initial data
  m_bSelected = FALSE;
  m_bError    = FALSE;
  m_pParent   = NULL;
  m_pDocument = NULL;
  m_lName     = 0;
  m_nType     = 0;
  m_pView     = NULL;
  m_pScope    = NULL;
  m_pEditor   = NULL;
  m_pProgram  = NULL;
  m_fPrevTime = 0;
}

CBlockUnit::CBlockUnit(UINT nType, UINT x, UINT y)
{
  // Setup initial data
  m_bSelected = FALSE;
  m_bError    = FALSE;
  m_pParent   = NULL;
  m_pDocument = NULL;
  m_lName     = 0;
  m_nType     = nType;
  m_pView     = NULL;
  m_pScope    = NULL;
  m_pEditor   = NULL;
  m_pProgram  = NULL;
  m_fPrevTime = 0;

  // Setup initial metrics
  m_rRect.X = x;
  m_rRect.Y = y;
  m_rRect.Width  = GetMinWidth();
  m_rRect.Height = GetMinHeight();

  // Setup editor metrics
  m_rEditor.X = CW_USEDEFAULT;
  m_rEditor.Y = CW_USEDEFAULT;
  m_rEditor.Width  = 640;
  m_rEditor.Height = 480;
  
  // Setup explorer metrics
  m_rScope.X = CW_USEDEFAULT;
  m_rScope.Y = CW_USEDEFAULT;
  m_rScope.Width  = 640;
  m_rScope.Height = 480;
}

CBlockUnit::~CBlockUnit()
{
  // Free memory allocated for variables
  if (m_nType != T_SUBSYSTEM)
  {
    for (int i=0; i<m_vInput.size(); i++)
    {
      delete m_vInput[i];
    }
    for (int i=0; i<m_vOutput.size(); i++)
    {
      delete m_vOutput[i];
    }
    for (int i=0; i<m_vGlobal.size(); i++)
    {
      delete m_vGlobal[i];
    }
    for (int i=0; i<m_vStack.size(); i++)
    {
      delete m_vStack[i];
    }
  }

  // Cleanup subsystem documents
  if (m_pDocument)
  {
    delete m_pDocument;
  }

  // Cleanup existing program structures
  if (m_pProgram)
  {
    stmt_free(m_pProgram);
  }

  // Close current editor and scope window if exists
  if (m_pEditor)
  {
    m_pEditor->OnDestroy(0, 0);
  }
  if (m_pScope)
  {
    m_pScope->OnDestroy(0, 0);
  }
  if (m_pView)
  {
    m_pView->OnDestroy(0, 0);
  }
}

//=============================================================================
// Class Manipulators
//=============================================================================

void CBlockUnit::InitBlock()
{
  // Draw input variables
  Point ptInput(m_rRect.X, m_rRect.GetTop() + 10);
  if ((m_nType != T_INPUT) && (m_nType != T_OUTPUT))
  {
    ptInput.Y += 20;
  }
  for (int i=0; i<m_vInput.size(); i++)
  {
    // Skip if input port
    if (m_nType == T_INPUT)
    {
      break;
    }

    // Draw port
    m_vInput[i]->InitIOPort(ptInput.X, ptInput.Y);
    ptInput.Y += 20;

    // Only draw one if I/O ports
    if (m_nType == T_OUTPUT)
    {
      break;
    }
  }

  // Draw output variables
  Point ptOutput(m_rRect.GetRight(), m_rRect.Y + 10);
  if ((m_nType != T_INPUT) && (m_nType != T_OUTPUT))
  {
    ptOutput.Y += 20;
  }
  for (int i=0; i<m_vOutput.size(); i++)
  {
    // Skip if output port
    if (m_nType == T_OUTPUT)
    {
      break;
    }

    // Draw port
    m_vOutput[i]->InitIOPort(ptOutput.X, ptOutput.Y);
    ptOutput.Y += 20;

    // Only draw one if I/O ports
    if (m_nType == T_INPUT)
    {
      break;
    }
  }
}

void CBlockUnit::DrawBlock(Graphics *pGraph)
{
  Pen pen(Color(0, 0, 0));
  Font fontBold(L"Tahoma", 10, FontStyleBold);
  Font fontNormal(L"Tahoma", 10, FontStyleRegular);
  SolidBrush brush(Color(0, 0, 0));

  // Check & update block size
  UINT nMinWidth  = GetMinWidth();
  UINT nMinHeight = GetMinHeight();
  if ((INT)m_rRect.Width < (INT)nMinWidth)
  {
    m_rRect.Width = ROUND(nMinWidth);
  }
  if ((INT)m_rRect.Height < (INT)nMinHeight)
  {
    m_rRect.Height = ROUND(nMinHeight);
  }

  // select color according to type
  switch (m_nType)
  {
    case T_FUNCTION:
      {
        brush.SetColor(Color(220, 220, 255));
      }
      break;
    case T_SUBSYSTEM:
      {
        brush.SetColor(Color(255, 220, 220));
      }
      break;
    case T_TEXTBOX:
      {
        brush.SetColor(Color(255, 255, 230));
      }
      break;
    case T_INPUT:
      {
        brush.SetColor(Color(255, 255, 220));
      }
      break;
    case T_OUTPUT:
      {
        brush.SetColor(Color(255, 255, 220));
      }
      break;
    default:
      {
        ASSERT(TRUE);
      }
  }

  // Mark selected
  if (m_bSelected)
  {
    pen.SetWidth(2);
  }

  // Outline error connections
  if (m_bError)
  {
    pen.SetColor(Color(255, 100, 100));
  }
  else
  {
    pen.SetColor(Color(0, 0, 0));
  }

  // Draw rectangle body
  pGraph->FillRectangle(&brush, m_rRect);
  pGraph->DrawRectangle(&pen, m_rRect);

  if ((m_nType != T_INPUT) && (m_nType != T_OUTPUT))
  {
    pen.SetWidth(1);
    brush.SetColor(Color(0, 0, 0));
    RectF rect, rectSize;
    StringFormat format;

    // Write title string
    if (m_nType == T_TEXTBOX)
    {
      rect = RectF(m_rRect.X, m_rRect.Y, m_rRect.Width, m_rRect.Height);
      wstring ws(m_sName.begin(), m_sName.end());
      pGraph->DrawString(ws.c_str(), -1, &fontNormal, rect, &format, &brush);

      // Measure title text width
      rect.Width = 0;
      pGraph->MeasureString(ws.c_str(), -1, &fontNormal, rect, &format,
          &rectSize);
      m_lName = (INT)rectSize.Width;
    }
    else
    {
      // Draw title line
      pGraph->DrawLine(&pen, m_rRect.X, m_rRect.Y+20, m_rRect.GetRight(),
          m_rRect.Y+20);

      format.SetAlignment(StringAlignmentCenter);
      format.SetLineAlignment(StringAlignmentCenter);
      rect = RectF(m_rRect.X, m_rRect.Y, m_rRect.Width, 20);
      wstring ws(m_sName.begin(), m_sName.end());
      pGraph->DrawString(ws.c_str(), -1, &fontBold, rect, &format, &brush);

      // Measure title text width
      rect.Width = 0;
      pGraph->MeasureString(ws.c_str(), -1, &fontBold, rect, &format,
          &rectSize);
      m_lName = (INT)rectSize.Width;
    }
  }

  // Draw input variables
  Point ptInput(m_rRect.X, m_rRect.GetTop() + 10);
  if ((m_nType != T_INPUT) && (m_nType != T_OUTPUT))
  {
    ptInput.Y += 20;
  }
  for (int i=0; i<m_vInput.size(); i++)
  {
    // Skip if input port
    if (m_nType == T_INPUT)
    {
      break;
    }

    // Draw port
    m_vInput[i]->DrawIOPort(pGraph, ptInput.X, ptInput.Y);
    ptInput.Y += 20;

    // Only draw one if I/O ports
    if (m_nType == T_OUTPUT)
    {
      break;
    }
  }

  // Draw output variables
  Point ptOutput(m_rRect.GetRight(), m_rRect.Y + 10);
  if ((m_nType != T_INPUT) && (m_nType != T_OUTPUT))
  {
    ptOutput.Y += 20;
  }
  for (int i=0; i<m_vOutput.size(); i++)
  {
    // Skip if output port
    if (m_nType == T_OUTPUT)
    {
      break;
    }

    // Draw port
    m_vOutput[i]->DrawIOPort(pGraph, ptOutput.X, ptOutput.Y);
    ptOutput.Y += 20;

    // Only draw one if I/O ports
    if (m_nType == T_INPUT)
    {
      break;
    }
  }
}

void CBlockUnit::MoveBlock(UINT dx, UINT dy)
{
  m_rRect.Offset(dx, dy);
}

void CBlockUnit::SizeBlock(UINT x, UINT y)
{
  UINT nMinWidth  = GetMinWidth();
  UINT nMinHeight = GetMinHeight();

  // Different policy for I/O ports
  if ((m_nType == T_INPUT) || (m_nType == T_OUTPUT))
  {
    // Assign values
    m_rRect.Width  = x - m_rRect.X;
    m_rRect.Height = 20;
  }
  else
  {
    m_rRect.Width  = x - m_rRect.X;
    m_rRect.Height = y - m_rRect.Y;
  }
}

BOOL CBlockUnit::SelectBlock(UINT x, UINT y)
{
  return m_rRect.Contains(x, y);
}

void CBlockUnit::BoxSelection(Rect rect)
{
  if (m_rRect.IntersectsWith(rect))
  {
    m_bSelected = TRUE;
  }
  else
  {
    m_bSelected = FALSE;
  }
}

BOOL CBlockUnit::OverSizeBox(UINT x, UINT y)
{
  Rect rect;

  // Build the size box
  rect.X = m_rRect.GetRight() - 8;
  rect.Y = m_rRect.GetBottom() - 8;
  rect.Width  = 6;
  rect.Height = 6;

  // Return check value
  return rect.Contains(x, y);
}

BOOL CBlockUnit::OverTitle(UINT x, UINT y)
{
  Rect rect;

  // Build the size box
  rect.X = m_rRect.X;
  rect.Y = m_rRect.Y;
  rect.Width  = m_rRect.Width;
  rect.Height = 20;

  // Return check value
  return rect.Contains(x, y);
}

void CBlockUnit::RoundBlock()
{
  m_rRect.X = ROUND(m_rRect.X);
  m_rRect.Y = ROUND(m_rRect.Y);
  m_rRect.Width  = ROUND(m_rRect.Width);
  m_rRect.Height = ROUND(m_rRect.Height);

  if (m_rRect.X < 0)
  {
    m_rRect.X = 0;
  }
  if (m_rRect.Y < 0)
  {
    m_rRect.Y = 0;
  }
}

UINT CBlockUnit::GetMinWidth()
{
  int nInputs  = m_vInput.size();
  int nOutputs = m_vOutput.size();
  int nWidth =   m_lName;

  for (int i = 0; i < max(nInputs, nOutputs); i++)
  {
    // Calculate for each line of IO
    int nInpSz = 0, nOutSz = 0, nTotal;
    if (i < nInputs)
    {
      if (m_nType == T_INPUT)
      {
        nInpSz = 0;
      }
      else
      {
        nInpSz = m_vInput[i]->m_lName;
      }
    }
    if (i < nOutputs)
    {
      if (m_nType == T_OUTPUT)
      {
        nOutSz = 0;
      }
      else
      {
        nOutSz = m_vOutput[i]->m_lName;
      }
    }
    nTotal = nInpSz + nOutSz;

    // Preserve the longgest
    if (nTotal > nWidth)
    {
      nWidth = nTotal;
    }
  }

  return nWidth + 10;
}

UINT CBlockUnit::GetMinHeight()
{
  if ((m_nType == T_INPUT) || (m_nType == T_OUTPUT))
  {
    return 20;
  }

  int nInputs  = m_vInput.size();
  int nOutputs = m_vOutput.size();
  int nHeight  = 20 + 20 * max(nInputs, nOutputs);

  return nHeight;
}

void CBlockUnit::AddLibrary(string sName, struct stmt *stmt)
{
  struct Function func;

  // Setup values
  func.m_sName = sName;
  func.m_pProgram = stmt;

  // Add it to library
  m_vLibrary.push_back(func);
}

void CBlockUnit::ClrLibrary()
{
  for (int i = 0; i < m_vLibrary.size(); i++)
  {
    m_vLibrary[i].m_sName.clear();
    stmt_free(m_vLibrary[i].m_pProgram);
  }

  m_vLibrary.clear();
}

void CBlockUnit::AddInput(CVariable *pVar)
{
  pVar->m_nType = CVariable::T_INPUT;

  m_vInput.push_back(pVar);
}

void CBlockUnit::DelInput(CVariable *pVar)
{
  for (int i=0; i<m_vInput.size(); i++)
  {
    if (m_vInput[i] == pVar)
    {
      m_vInput.erase(m_vInput.begin()+i);
    }
  }
}

void CBlockUnit::ClrInput()
{
  // Free allocated variable space
  for (int i=0; i<m_vInput.size(); i++)
  {
    delete m_vInput[i];
  }

  // Clear vector table
  m_vInput.clear();
}

void CBlockUnit::AddOutput(CVariable *pVar)
{
  pVar->m_nType = CVariable::T_OUTPUT;

  m_vOutput.push_back(pVar);
}

void CBlockUnit::DelOutput(CVariable *pVar)
{
  // Search and erase
  for (int i=0; i<m_vOutput.size(); i++)
  {
    if (m_vOutput[i] == pVar)
    {
      m_vOutput.erase(m_vOutput.begin()+i);
    }
  }
}

void CBlockUnit::ClrOutput()
{
  // Free allocated variable space
  for (int i=0; i<m_vOutput.size(); i++)
  {
    delete m_vOutput[i];
  }

  m_vOutput.clear();
}

void CBlockUnit::AddGlobal(CVariable *pVar, double fValue)
{
  pVar->m_nType = CVariable::T_SYSTEM;
  pVar->m_fValue = fValue;

  m_vGlobal.push_back(pVar);
}

void CBlockUnit::DelGlobal(CVariable *pVar)
{
  // Search and erase
  for (int i=0; i<m_vGlobal.size(); i++)
  {
    if (m_vOutput[i] == pVar)
    {
      m_vGlobal.erase(m_vGlobal.begin()+i);
    }
  }
}

void CBlockUnit::ClrGlobal()
{
  // Free allocated variable space
  for (int i=0; i<m_vGlobal.size(); i++)
  {
    delete m_vGlobal[i];
  }

  m_vGlobal.clear();
}

void CBlockUnit::AddStack(CVariable *pVar, double fValue)
{
  pVar->m_nType = CVariable::T_COMMON;
  pVar->m_fValue = fValue;

  m_vStack.push_back(pVar);
}

void CBlockUnit::PshStack()
{
  // Add a boundary to current frame
  CVariable *pVar = new CVariable;

  pVar->m_nType = CVariable::T_STACK;

  m_vStack.push_back(pVar);
}

void CBlockUnit::PopStack()
{
  // Pop up item if not boundary reached
  vector<CVariable*>::reverse_iterator iVar;
  for (iVar = m_vStack.rbegin(); iVar != m_vStack.rend(); iVar++)
  {
    CVariable *pVar = *iVar;
    if ((m_vStack.size() != 0) && (pVar->m_nType != CVariable::T_STACK))
    {
      m_vStack.pop_back();
      delete pVar;
    }
    else
    {
      m_vStack.pop_back();
      delete pVar;
      break;
    }
  }
}

void CBlockUnit::PshHeap()
{
  // Add a boundary to current frame
  CVariable *pVar = new CVariable;

  pVar->m_nType = CVariable::T_HEAP;

  m_vStack.push_back(pVar);
}

void CBlockUnit::PopHeap()
{
  // Pop up item if not boundary reached
  vector<CVariable*>::reverse_iterator iVar;
  for (iVar = m_vStack.rbegin(); iVar != m_vStack.rend(); iVar++)
  {
    CVariable *pVar = *iVar;
    if ((m_vStack.size() != 0) && (pVar->m_nType != CVariable::T_HEAP))
    {
      m_vStack.pop_back();
      delete pVar;
    }
    else
    {
      m_vStack.pop_back();
      delete pVar;
      break;
    }
  }
}

void CBlockUnit::ClrStack()
{
  // Free allocated variable space
  for (int i=0; i<m_vStack.size(); i++)
  {
    CVariable *pVar = m_vStack[i];

    // Delete item
    delete m_vStack[i];
  }

  m_vStack.clear();
}

struct stmt *CBlockUnit::GetLibrary(string sName)
{
  for (int i = 0; i < m_vLibrary.size(); i++)
  {
    if (m_vLibrary[i].m_sName == sName)
    {
      return m_vLibrary[i].m_pProgram;
    }
  }

  return NULL;
}

CVariable *CBlockUnit::GetVariable(string sName, BOOL bCheck)
{
  // Special search if used for stack
  if (bCheck)
  {
    // Just seek in current stack and I/O
    vector<CVariable*>::reverse_iterator iVar;
    for (iVar = m_vStack.rbegin(); iVar != m_vStack.rend(); iVar++)
    {
      CVariable *pVar = *iVar;
      if (pVar->m_nType == CVariable::T_HEAP)
      {
        return 0;
      }
      else if (pVar->m_nType == CVariable::T_STACK)
      {
        return 0;
      }
      else if (pVar->m_sName == sName)
      {
        return pVar;
      }
    }
  }
  else
  {
    // Seek in all stacks and I/O in current heap
    vector<CVariable*>::reverse_iterator iVar;
    for (iVar = m_vStack.rbegin(); iVar != m_vStack.rend(); iVar++)
    {
      CVariable *pVar = *iVar;
      if (pVar->m_nType == CVariable::T_HEAP)
      {
        break;
      }
      else if (pVar->m_sName == sName)
      {
        return pVar;
      }
    }
  }

  // Find global variable with same name
  for (int i=0; i<m_vGlobal.size(); i++)
  {
    if (m_vGlobal[i]->m_sName == sName)
    {
      return m_vGlobal[i];
    }
  }

  // Find input variable with same name
  for (int i=0; i<m_vInput.size(); i++)
  {
    if (m_vInput[i]->m_sName == sName)
    {
      return m_vInput[i];
    }
  }

  // Find output variable with same name
  for (int i=0; i<m_vOutput.size(); i++)
  {
    if (m_vOutput[i]->m_sName == sName)
    {
      return m_vOutput[i];
    }
  }

  return NULL;
}

CVariable *CBlockUnit::GetIOPort(UINT x, UINT y)
{
  Rect rect;

  if (x == 0 && y == 0)
  {
    return NULL;
  }

  // Check for input ports
  for (int i = 0; i < m_vInput.size(); i++)
  {
    // Assign selection box rect
    rect.X = m_vInput[i]->m_ptPort.X - 5;
    rect.Y = m_vInput[i]->m_ptPort.Y - 5;
    rect.Width  = 10;
    rect.Height = 10;

    // Check if pos in box
    if (rect.Contains(x, y))
    {
      return m_vInput[i];
    }
  }

  // Check for output ports
  for (int i = 0; i < m_vOutput.size(); i++)
  {
    // Assign selection box rect
    rect.X = m_vOutput[i]->m_ptPort.X - 5;
    rect.Y = m_vOutput[i]->m_ptPort.Y - 5;
    rect.Width  = 10;
    rect.Height = 10;

    // Check if pos in box
    if (rect.Contains(x, y))
    {
      return m_vOutput[i];
    }
  }

  return NULL;
}

void CBlockUnit::OpenEditor(HWND hParent)
{
  if (m_nType == T_FUNCTION)
  {
    // Focus on editor if already open
    if (m_pEditor)
    {
      SetFocus(m_pEditor->m_hWnd);
      return;
    }

    // Open editor if not exist
    m_pEditor = new CSourceEditor(this);
    m_pEditor->Create(hParent);
  }
  else if (m_nType == T_SUBSYSTEM)
  {
    // Focus on editor if already open
    if (m_pView)
    {
      SetFocus(m_pView->m_hWnd);
      return;
    }

    // Open editor if not exist
    m_pView = new CModelView(NULL, m_pDocument);
    m_pView->Create(m_sName, hParent);
  }
}

void CBlockUnit::OpenScope(HWND hParent)
{
  // Focus on editor if already open
  if (m_pScope)
  {
    SetFocus(m_pScope->m_hWnd);
    return;
  }

  // Open editor if not exist
  m_pScope = new CDataScope(this);
  m_pScope->Create(hParent);
}

BOOL CBlockUnit::Check()
{
  // Return if compile error
  if (m_bError == TRUE)
  {
    return FALSE;
  }

  // Check sub-system validity
  if (m_nType == T_SUBSYSTEM)
  {
    if (m_pDocument->Check() == FALSE)
    {
      m_bError = TRUE;

      return FALSE;
    }
  }

  // Check validity of each I/O
  if (m_nType != T_INPUT) for (int i=0; i<m_vInput.size(); i++)
  {
    CVariable *pVar = m_vInput[i];

    if (!pVar->m_pLink)
    {
      // Error on no input
      m_bError = TRUE;

      return FALSE;
    }
  }

  return TRUE;
}

BOOL CBlockUnit::Compile()
{
  m_bError = FALSE;

  if (m_pEditor)
  {
    // Save before compiling
    m_pEditor->Save();
  }

  if (m_nType == T_FUNCTION)
  {
    // Cleanup all I/Os
    ClrInput();
    ClrOutput();
    ClrGlobal();
    ClrStack();
    ClrLibrary();

    // Compile the source code
    m_bError = (CompileBlock(this) == FALSE);

    // Add system timer variable
    AddGlobal(new CVariable("TIME"), 0);
    AddGlobal(new CVariable("DTIME"), 0);
  }
  else if (m_nType == T_SUBSYSTEM)
  {
    // Compile subsystem document
   m_bError  = (m_pDocument->Compile() == FALSE);

    // Update document
    m_pDocument->Update();
    m_pDocument->Check();
  }

  return !m_bError;
}

void CBlockUnit::PreExecute()
{
  m_fPrevTime = 0;

  if (m_pScope)
  {
    m_pScope->Reload();
  }

  if (m_nType == T_SUBSYSTEM)
  {
    m_pDocument->PreExecute();
  }
}

BOOL CBlockUnit::Execute(double fTime, double fDTime)
{
  BOOL bRet = TRUE;

  if ((m_nType == T_INPUT) || (m_nType == T_OUTPUT))
  {
    m_vOutput[0]->m_fValue = m_vInput[0]->m_fValue;
  }
  else if (m_nType == T_FUNCTION)
  {
    ClrStack();

    // Execute current block
    m_bError = !(bRet = ExecuteBlock(this));
  }
  else if (m_nType == T_SUBSYSTEM)
  {
    m_bError = !(bRet = m_pDocument->Execute(fTime, fDTime));
  }

  // Update data scope
  if (m_pScope) if (fTime - m_fPrevTime > 0.00999f)
  {
    m_pScope->Update(TRUE);
    m_fPrevTime = fTime;
  }

  return bRet;
}

