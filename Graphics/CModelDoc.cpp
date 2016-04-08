#include "StdAfx.h"

BOOL g_bRun = FALSE;

//=============================================================================
// Class Initializers
//=============================================================================

CModelDoc::CModelDoc()
{
  m_pBlockCurr  = NULL;
  m_pLinkCurr   = NULL;
  m_pParent     = NULL;
  m_nHScroll    = 0;
  m_nVScroll    = 0;
  m_bFixdTime   = FALSE;
  m_nSlepTime   = 0;
  m_nCPULimit   = 1;
  m_fDuraTime   = 10;
  m_fStepTime   = 0.001;
  m_fCurrTime   = 0;
  m_fCostTime   = 0;

  m_rRect = Rect(CW_USEDEFAULT, CW_USEDEFAULT, 600, 400);
}

CModelDoc::CModelDoc(CBlockUnit *pParent)
{
  m_pBlockCurr  = NULL;
  m_pLinkCurr   = NULL;
  m_pParent     = pParent;
  m_nHScroll    = 0;
  m_nVScroll    = 0;
  m_bFixdTime   = FALSE;
  m_nSlepTime   = 0;
  m_nCPULimit   = 1;
  m_fDuraTime   = 10;
  m_fStepTime   = 0.001;
  m_fCurrTime   = 0;
  m_fCostTime   = 0;

  m_rRect = Rect(CW_USEDEFAULT, CW_USEDEFAULT, 600, 400);
}

CModelDoc::~CModelDoc()
{
  list<CLinkUnit*>::iterator iLnk;
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    delete *iLnk;
  }

  list<CBlockUnit*>::iterator iBlk;
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    if (m_pParent)
    {
      if ((*iBlk)->m_nType == CBlockUnit::T_INPUT)
      {
        m_pParent->DelInput((*iBlk)->m_vInput[0]);
      }
      else if ((*iBlk)->m_nType == CBlockUnit::T_OUTPUT)
      {
        m_pParent->DelOutput((*iBlk)->m_vOutput[0]);
      }
    }
    delete *iBlk;
  }
}

//=============================================================================
// Operation Interfaces
//=============================================================================

void CModelDoc::DrawDocument(Graphics *pGraph)
{
  // Draw links on the back
  list<CLinkUnit*>::iterator iLnk;
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    (*iLnk)->DrawLink(pGraph);
  }

  // Draw blocks on the front
  list<CBlockUnit*>::iterator iBlk;
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    (*iBlk)->DrawBlock(pGraph);
  }
}

BOOL CModelDoc::Selected(UINT x, UINT y, BOOL bAdd)
{
  list<CBlockUnit*>::iterator iBlock;
  list<CLinkUnit*>::iterator  iLink;

  // Clear current selection
  if (m_pBlockCurr)
  {
    m_pBlockCurr = NULL;
  }
  if (m_pLinkCurr)
  {
    m_pLinkCurr  = NULL;
  }

  // Select blocks as priority
  list<CBlockUnit*>::iterator iBlk;
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    if ((*iBlk)->SelectBlock(x, y))
    {
      iBlock = iBlk;
      m_pBlockCurr = *iBlk;
    }
  }

  // If found, swap to front and return
  if (m_pBlockCurr)
  {
    // Clean all on new selection
    if (!bAdd)
    {
      if (!m_pBlockCurr->m_bSelected)
      {
        CleanSelection();
        m_pBlockCurr->m_bSelected = TRUE;
      }
    }
    else
    {
      m_pBlockCurr->m_bSelected = !m_pBlockCurr->m_bSelected;
    }
    iter_swap(iBlock, m_lBlockList.rbegin());
    return TRUE;
  }

  // Then select links
  list<CLinkUnit*>::iterator iLnk;
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    if ((*iLnk)->SelectLink(x, y))
    {
      iLink = iLnk;
      m_pLinkCurr = *iLnk;
    }
  }

  // If found, return
  if (m_pLinkCurr)
  {
    // Clean all on new selection
    if (!bAdd)
    {
      if (!m_pLinkCurr->m_bSelected)
      {
        CleanSelection();
        m_pLinkCurr->m_bSelected = TRUE;
      }
    }
    else
    {
      m_pLinkCurr->m_bSelected = !m_pLinkCurr->m_bSelected;
    }
    return TRUE;
  }

  // If no selection, clean all
  CleanSelection();

  return FALSE;
}

void CModelDoc::BoxSelection(Rect rect)
{
  // Clear current selection
  if (m_pBlockCurr)
  {
    m_pBlockCurr->m_bSelected = FALSE;
    m_pBlockCurr = NULL;
  }
  if (m_pLinkCurr)
  {
    m_pLinkCurr->m_bSelected = FALSE;
    m_pLinkCurr  = NULL;
  }

  // Select blocks as priority
  list<CBlockUnit*>::iterator iBlk;
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    (*iBlk)->BoxSelection(rect);
  }

  // Then select links
  list<CLinkUnit*>::iterator iLnk;
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    (*iLnk)->BoxSelection(rect);
  }
}

void CModelDoc::MoveSelection(UINT dx, UINT dy)
{
  // Select blocks as priority
  list<CBlockUnit*>::iterator iBlk;
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    if ((*iBlk)->m_bSelected)
    {
      (*iBlk)->MoveBlock(dx, dy);
    }
  }

  // Then select links
  list<CLinkUnit*>::iterator iLnk;
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    if ((*iLnk)->m_bSelected)
    {
      (*iLnk)->MoveLink(dx, dy);
    }
  }
}

void CModelDoc::RoundSelection()
{
  // Select blocks as priority
  list<CBlockUnit*>::iterator iBlk;
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    if ((*iBlk)->m_bSelected)
    {
      (*iBlk)->RoundBlock();
    }
  }

  // Then select links
  list<CLinkUnit*>::iterator iLnk;
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    if ((*iLnk)->m_bSelected)
    {
      (*iLnk)->RoundLink();
    }
  }
}

void CModelDoc::CleanSelection()
{
  // Select blocks as priority
  list<CBlockUnit*>::iterator iBlk;
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    (*iBlk)->m_bSelected = FALSE;
  }

  // Then select links
  list<CLinkUnit*>::iterator iLnk;
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    (*iLnk)->m_bSelected = FALSE;
  }
}

void CModelDoc::DelSelection()
{
  // Select blocks as priority
  list<CBlockUnit*>::iterator iBlk;
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end();)
  {
    if ((*iBlk)->m_bSelected)
    {
      // Remove parent I/O if I/O block
      if (m_pParent)
      {
        if ((*iBlk)->m_nType == CBlockUnit::T_INPUT)
        {
          m_pParent->DelInput((*iBlk)->m_vInput[0]);
        }
        else if ((*iBlk)->m_nType == CBlockUnit::T_OUTPUT)
        {
          m_pParent->DelOutput((*iBlk)->m_vOutput[0]);
        }
      }

      // Remove block
      delete *iBlk;
      iBlk = m_lBlockList.erase(iBlk);
    }
    else
    {
      iBlk++;
    }
  }

  // Then select links
  list<CLinkUnit*>::iterator iLnk;
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end();)
  {
    if((*iLnk)->m_bSelected)
    {
      delete *iLnk;
      iLnk = m_lLinkList.erase(iLnk);
    }
    else
    {
      iLnk++;
    }
  }
}

Point CModelDoc::GetBound()
{
  Point ptBound(0, 0);

  // Look in blocks for right bottom cornor
  list<CBlockUnit*>::iterator iBlk;
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    // Check for horizontal maximum point
    if ((*iBlk)->m_rRect.GetRight() > ptBound.X)
    {
      ptBound.X = (*iBlk)->m_rRect.GetRight();
    }

    // Check for vertical maximum point
    if ((*iBlk)->m_rRect.GetBottom() > ptBound.Y)
    {
      ptBound.Y = (*iBlk)->m_rRect.GetBottom();
    }
  }

  return ptBound;
}

void CModelDoc::AddBlock(CBlockUnit *pBlock)
{
  // Add I/O ports to parent block
  if (m_pParent) switch (pBlock->m_nType)
  {
    case CBlockUnit::T_INPUT:
      {
        m_pParent->AddInput(pBlock->m_vInput[0]);
      }
      break;
    case CBlockUnit::T_OUTPUT:
      {
        m_pParent->AddOutput(pBlock->m_vOutput[0]);
      }
      break;
  }

  // Add to list
  m_lBlockList.push_back(pBlock);

  // Clear selection
  if (m_pBlockCurr)
  {
    m_pBlockCurr->m_bSelected = FALSE;
  }

  // Set selection to current
  m_pBlockCurr = pBlock;
  m_pBlockCurr->m_bSelected = TRUE;
}

void CModelDoc::AddBlock(UINT nType, UINT x, UINT y)
{
  // Create the block
  CBlockUnit *pBlock = new CBlockUnit(nType, x, y);

  // Setup for each type of blocks
  switch (nType)
  {
    case CBlockUnit::T_FUNCTION:
      {
        pBlock->m_sName = "Function";
        pBlock->m_sSource = "module [Output] = Function(Input)\r\n{\r\n\r\n}";
        pBlock->Compile();
      }
      break;
    case CBlockUnit::T_SUBSYSTEM:
      {
        pBlock->m_sName = "Subsystem";
        pBlock->m_pDocument = new CModelDoc(pBlock);
      }
      break;
    case CBlockUnit::T_TEXTBOX:
      {
        pBlock->m_sName = "Text";
      }
      break;
    case CBlockUnit::T_INPUT:
      {
        pBlock->m_sName = "Input";
        pBlock->AddInput(new CVariable("Input"));
        pBlock->AddOutput(new CVariable("Input"));
      }
      break;
    case CBlockUnit::T_OUTPUT:
      {
        pBlock->m_sName = "Output";
        pBlock->AddInput(new CVariable("Output"));
        pBlock->AddOutput(new CVariable("Output"));
      }
      break;
    default:
      {
        ASSERT(TRUE);
      }
  }

  // Add I/O ports to parent block
  if (m_pParent) switch (nType)
  {
    case CBlockUnit::T_INPUT:
      {
        m_pParent->AddInput(pBlock->m_vInput[0]);
      }
      break;
    case CBlockUnit::T_OUTPUT:
      {
        m_pParent->AddOutput(pBlock->m_vOutput[0]);
      }
      break;
  }

  // Add to list
  m_lBlockList.push_back(pBlock);

  // Clear selection
  if (m_pBlockCurr)
  {
    m_pBlockCurr->m_bSelected = FALSE;
  }

  // Set selection to current
  m_pBlockCurr = pBlock;
  m_pBlockCurr->m_bSelected = TRUE;
}

void CModelDoc::SizeCurrBlock(UINT x, UINT y)
{
  ASSERT(m_pBlockCurr == NULL);

  m_pBlockCurr->SizeBlock(x, y);
}

BOOL CModelDoc::OverCurrBlockSizeBox(UINT x, UINT y)
{
  ASSERT(m_pBlockCurr == NULL);

  return m_pBlockCurr->OverSizeBox(x, y);
}

BOOL CModelDoc::OverCurrBlockTitle(UINT x, UINT y)
{
  ASSERT(m_pBlockCurr == NULL);

  return m_pBlockCurr->OverTitle(x, y);
}

void CModelDoc::DelCurrBlock()
{
  ASSERT(m_pBlockCurr == NULL);

  // Remove parent I/O if I/O block
  if (m_pParent)
  {
    if (m_pBlockCurr->m_nType == CBlockUnit::T_INPUT)
    {
      m_pParent->DelInput(m_pBlockCurr->m_vInput[0]);
    }
    else if (m_pBlockCurr->m_nType == CBlockUnit::T_OUTPUT)
    {
      m_pParent->DelOutput(m_pBlockCurr->m_vOutput[0]);
    }
  }

  m_lBlockList.remove(m_pBlockCurr);

  m_pBlockCurr = NULL;

  Check();
}

void CModelDoc::AddLink(UINT x, UINT y)
{
  // Create new link
  CLinkUnit *pLink = new CLinkUnit(x, y);

  // Add to list
  m_lLinkList.push_back(pLink);

  // Clear selection
  if (m_pLinkCurr)
  {
    m_pLinkCurr->m_bSelected = FALSE;
  }

  // Set selection to current
  m_pLinkCurr = pLink;
  m_pLinkCurr->m_bSelected = TRUE;
}

void CModelDoc::AddCurrLinkPoint(UINT x, UINT y)
{
  ASSERT(m_pLinkCurr == NULL);

  m_pLinkCurr->AddPoint(x, y);
}

Point CModelDoc::GetCurrLinkLastPoint()
{
  ASSERT(m_pLinkCurr == NULL);

  return m_pLinkCurr->GetLastPoint();
}

void CModelDoc::MoveCurrLinkPoint(UINT x, UINT y)
{
  ASSERT(m_pLinkCurr == NULL);

  m_pLinkCurr->MoveCurrPoint(x, y);
}

void CModelDoc::DelCurrLinkPoint()
{
  ASSERT(m_pLinkCurr == NULL);

  m_pLinkCurr->DelCurrPoint();

  Check();
}

void CModelDoc::CheckCurrLink()
{
  ASSERT(m_pLinkCurr == NULL);

  // Delete invalid line
  if (m_pLinkCurr->GetPointCount() == 1)
  {
    m_lLinkList.remove(m_pLinkCurr);
    delete m_pLinkCurr;
    m_pLinkCurr = NULL;
  }
}

CVariable *CModelDoc::GetIOPort(UINT x, UINT y)
{
  list<CBlockUnit*>::iterator iBlk;

  // Search in each block
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    if (CVariable *pVar = (*iBlk)->GetIOPort(x, y))
    {
      return pVar;
    }
  }

  return NULL;
}

CLinkUnit *CModelDoc::GetLink(UINT x, UINT y, CLinkUnit *pExcept)
{
  list<CLinkUnit*>::iterator iLnk;

  // Search in each link
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    if ((*iLnk)->SelectLink(x, y) && (*iLnk != pExcept))
    {
      return *iLnk;
    }
  }

  return NULL;
}

void CModelDoc::Update()
{
  BOOL bSuccess = TRUE;
  list<CLinkUnit*>::iterator iLnk;
  list<CBlockUnit*>::iterator iBlk;

  // Initialize block I/O positions
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    CBlockUnit *pBlk = *iBlk;

    pBlk->InitBlock();
  }

  // Rewire all links and blocks
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    CLinkUnit *pLnk = *iLnk;

    // For each link, check connectivity of both side
    Point pt1 = *pLnk->m_vPointList.begin();
    Point pt2 = *pLnk->m_vPointList.rbegin();

    // Acquire new connectivity
    CVariable *pVar1 = GetIOPort(pt1.X, pt1.Y);
    CVariable *pVar2 = GetIOPort(pt2.X, pt2.Y);
    CLinkUnit *pLnk1 = GetLink(pt1.X, pt1.Y, pLnk);
    CLinkUnit *pLnk2 = GetLink(pt2.X, pt2.Y, pLnk);

    // Unlink previous links, donot modify link connections
    if (!pLnk->m_pLnkBegin && pLnk->m_pVarBegin)
    {
      pLnk->m_pVarBegin->m_pLink = NULL;
    }
    if (!pLnk->m_pLnkEnd && pLnk->m_pVarEnd)
    {
      pLnk->m_pVarEnd->m_pLink = NULL;
    }

    // Reconnect current layout
    pLnk->m_pLnkBegin = pLnk1;
    pLnk->m_pVarBegin = pVar1;
    pLnk->m_pLnkEnd = pLnk2;
    pLnk->m_pVarEnd = pVar2;

    // Assign vars from uplink
    if (pLnk1)
    {
      // Only connect to source variables
      if (pLnk1->m_pVarBegin &&
          pLnk1->m_pVarBegin->m_nType == CVariable::T_OUTPUT)
      {
        pLnk->m_pVarBegin = pLnk1->m_pVarBegin;
      }
      else if (pLnk1->m_pVarEnd &&
          pLnk1->m_pVarEnd->m_nType == CVariable::T_OUTPUT)
      {
        pLnk->m_pVarBegin = pLnk1->m_pVarEnd;
      }
    }
    if (pLnk2)
    {
      // Only connect to source variables
      if (pLnk2->m_pVarBegin &&
          pLnk2->m_pVarBegin->m_nType == CVariable::T_OUTPUT)
      {
        pLnk->m_pVarEnd = pLnk2->m_pVarBegin;
      }
      else if (pLnk2->m_pVarEnd &&
          pLnk2->m_pVarEnd->m_nType == CVariable::T_OUTPUT)
      {
        pLnk->m_pVarEnd = pLnk2->m_pVarEnd;
      }
    }

    // Assign this to links
    if (pVar1)
    {
      pVar1->m_pLink = pLnk;
    }
    if (pVar2)
    {
      pVar2->m_pLink = pLnk;
    }
  }
}

BOOL CModelDoc::Check()
{
  BOOL bSuccess = TRUE;
  list<CBlockUnit*>::iterator iBlk;
  list<CLinkUnit*>::iterator iLnk;

  // Check block errors
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    CBlockUnit *pBlk = *iBlk;

    // Check blocks
    if (pBlk->Check() == FALSE)
    {
      bSuccess = FALSE;
    }
  }

  // Check link errors
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    CLinkUnit *pLnk = *iLnk;

    // Check links
    if (pLnk->Check() == FALSE)
    {
      bSuccess = FALSE;
    }
  }

  return bSuccess;
}

BOOL CModelDoc::Compile()
{
  BOOL bSuccess = TRUE;
  list<CBlockUnit*>::iterator iBlk;

  // Compile all blocks
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    CBlockUnit *pBlk = *iBlk;

    if (!pBlk->Compile())
    {
      bSuccess = FALSE;
    }
  }

  // Return TRUE if all set
  return bSuccess;
}

VOID CModelDoc::PreExecute()
{
  list<CBlockUnit*>::iterator iBlk;

  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    CBlockUnit *pBlk = *iBlk;

    pBlk->PreExecute();
  }
}

BOOL CModelDoc::Execute(double fTime, double fDTime)
{
  list<CLinkUnit*>::iterator iLnk;
  list<CBlockUnit*>::iterator iBlk;

  // Update each link
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    CLinkUnit *pLnk = *iLnk;

    // Calculate current cycle
    if (pLnk->Execute() == FALSE)
    {
      g_bRun = FALSE;

      return FALSE;
    }
  }

  // Calculate each block
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    CBlockUnit *pBlk = *iBlk;

    // Setup time variable
    if (pBlk->m_nType == CBlockUnit::T_FUNCTION)
    {
      CVariable *pVar;
      pVar = pBlk->GetVariable("TIME", FALSE);  ASSERT(pVar == NULL);
      pVar->m_fValue = fTime;
      pVar = pBlk->GetVariable("DTIME", FALSE); ASSERT(pVar == NULL);
      pVar->m_fValue = fDTime;
    }

    // Calculate current cycle
    if (pBlk->Execute(fTime, fDTime) == FALSE)
    {
      g_bRun = FALSE;

      return FALSE;
    }
  }

  return TRUE;
}

// TODO: Create thread for simulation use
BOOL CModelDoc::Start()
{
  if (m_pParent)
  {
    return FALSE;
  }
  else
  {
    g_bRun = TRUE;

    // Prepare for simulation
    PreExecute();

    // Create simulation thread
    CreateThread(NULL, 0, SimuThread, (LPVOID)this, 0, NULL);
  }

  return TRUE;
}

void CModelDoc::Stop()
{
  g_bRun = FALSE;
}

BOOL CModelDoc::Save(fstream *pFile)
{
  ASSERT(pFile == NULL);

  int nVersion = 100, nValue;
  list<CLinkUnit*>::iterator  iLnk;
  list<CBlockUnit*>::iterator iBlk;

  // Save file title
  pFile->write("FxSim", 6);

  // Save file version
  pFile->write((char*)&nVersion, 4);

  // Save scrolling data
  pFile->write((char*)&m_nHScroll, 4);
  pFile->write((char*)&m_nVScroll, 4);
  pFile->write((char*)&m_bFixdTime, 4);
  pFile->write((char*)&m_nSlepTime, 4);
  pFile->write((char*)&m_nCPULimit, 4);
  pFile->write((char*)&m_fDuraTime, 8);
  pFile->write((char*)&m_fStepTime, 8);

  // Save window rectangle data
  pFile->write((char*)&m_rRect, sizeof(Rect));

  // Save each link data
  for (iLnk = m_lLinkList.begin(); iLnk != m_lLinkList.end(); iLnk++)
  {
    CLinkUnit *pLnk = *iLnk;

    // Signal for new link
    nValue = T_LNKBEGIN; pFile->write((char*)&nValue, 4);

    for (int i = 0; i < pLnk->m_vPointList.size(); i++)
    {
      // Signal for link points
      nValue = T_LNKITEM; pFile->write((char*)&nValue, 4);

      // Save link point data
      pFile->write((char*)&pLnk->m_vPointList[i], sizeof(Point));
    }
  }

  // Save each block data
  for (iBlk = m_lBlockList.begin(); iBlk != m_lBlockList.end(); iBlk++)
  {
    CBlockUnit *pBlk = *iBlk;

    // Signal for new block
    nValue = T_BLKITEM; pFile->write((char*)&nValue, 4);

    // Save block data
    pFile->write((char*)&pBlk->m_nType, 4);
    nValue = pBlk->m_sName.size() + 1;
    pFile->write((char*)&nValue, 4);
    pFile->write((char*)pBlk->m_sName.c_str(), nValue);
    nValue = pBlk->m_sSource.size() + 1;
    pFile->write((char*)&nValue, 4);
    pFile->write(pBlk->m_sSource.c_str(), nValue);
    pFile->write((char*)&pBlk->m_rRect, sizeof(Rect));
    pFile->write((char*)&pBlk->m_rEditor, sizeof(Rect));
    pFile->write((char*)&pBlk->m_rScope, sizeof(Rect));
    if (pBlk->m_nType == CBlockUnit::T_SUBSYSTEM)
    {
      // Signal for subsystem
      nValue = T_SUBSYS; pFile->write((char*)&nValue, 4);

      // Save subsystem document
      pBlk->m_pDocument->Save(pFile);
    }
  }

  nValue = T_FINISH; pFile->write((char*)&nValue, 4);
  return TRUE;
}

BOOL CModelDoc::Load(fstream *pFile)
{
  ASSERT(pFile == NULL);

  char pBuffer[6];
  int  nVersion, nValue = 0;

  // Load file title
  pFile->read(pBuffer, 6);
  if (strcmp(pBuffer, "FxSim"))
  {
    MessageBox(NULL, "Wrong file format", "Error", MB_ICONERROR);
    return FALSE;
  }

  // Load file version
  pFile->read((char*)&nVersion, 4);
  if (nVersion != 100)
  {
    MessageBox(NULL, "File version not match !", "Error", MB_ICONERROR);
    return FALSE;
  }

  // Load scrolling data
  pFile->read((char*)&m_nHScroll, 4);
  pFile->read((char*)&m_nVScroll, 4);
  pFile->read((char*)&m_bFixdTime, 4);
  pFile->read((char*)&m_nSlepTime, 4);
  pFile->read((char*)&m_nCPULimit, 4);
  pFile->read((char*)&m_fDuraTime, 8);
  pFile->read((char*)&m_fStepTime, 8);

  // Load window rectangle data
  pFile->read((char*)&m_rRect, sizeof(Rect));

  while (nValue != T_FINISH)
  {
    pFile->read((char*)&nValue, 4);
    switch (nValue)
    {
      case T_LNKBEGIN:
        {
          // Create new link
          m_lLinkList.push_back(new CLinkUnit);
        }
        break;
      case T_LNKITEM:
        {
          // Load link points
          Point point;
          pFile->read((char*)&point, sizeof(Point));
          (*m_lLinkList.rbegin())->m_vPointList.push_back(point);
        }
        break;
      case T_BLKITEM:
        {
          char *buf;

          // Restore block data
          CBlockUnit *pBlock = new CBlockUnit;
          pBlock->m_pParent = this;
          pFile->read((char*)&pBlock->m_nType, 4);
          pFile->read((char*)&nValue, 4);
          buf = (char*)malloc(nValue);
          ASSERT(buf == 0);
          pFile->read(buf, nValue);
          pBlock->m_sName = buf;
          free(buf);
          pFile->read((char*)&nValue, 4);
          buf = (char*)malloc(nValue);
          ASSERT(buf == 0);
          pFile->read(buf, nValue);
          pBlock->m_sSource = buf;
          free(buf);
          pFile->read((char*)&pBlock->m_rRect, sizeof(Rect));
          pFile->read((char*)&pBlock->m_rEditor, sizeof(Rect));
          pFile->read((char*)&pBlock->m_rScope, sizeof(Rect));

          // Restore I/O if I/O blocks
          if (pBlock->m_nType == CBlockUnit::T_INPUT)
          {
            pBlock->AddInput(new CVariable(pBlock->m_sName));
            pBlock->AddOutput(new CVariable(pBlock->m_sName));
            if (m_pParent)
            {
              m_pParent->AddInput(pBlock->m_vInput[0]);
            }
          }
          else if (pBlock->m_nType == CBlockUnit::T_OUTPUT)
          {
            pBlock->AddInput(new CVariable(pBlock->m_sName));
            pBlock->AddOutput(new CVariable(pBlock->m_sName));
            if (m_pParent)
            {
              m_pParent->AddOutput(pBlock->m_vOutput[0]);
            }
          }

          // Add to block list
          m_lBlockList.push_back(pBlock);
        }
        break;
      case T_SUBSYS:
        {
          // Load subsystem document
          CBlockUnit *pBlock = *m_lBlockList.rbegin();
          pBlock->m_pDocument = new CModelDoc(pBlock);
          pBlock->m_pDocument->Load(pFile);
        }
        break;
      case T_FINISH:
        break;
      default:
        {
          return FALSE;
        }
    }
  }

  return TRUE;
}

BOOL CModelDoc::Generate()
{
  return TRUE;
}

//=============================================================================
// Simulation Thread
//=============================================================================

DWORD WINAPI SimuThread(LPVOID lpParam)
{
  CModelDoc *pDoc = (CModelDoc*)lpParam;
  double fClckTime, fDiffTime, fAftrTime, fPrevTime = 0, fSimuTime = 0;
  LARGE_INTEGER liFrequency, liCounter;

  // Initialize timer parameters
  QueryPerformanceFrequency(&liFrequency);
  pDoc->m_fCurrTime = 0;

  while (g_bRun)
  {
    // Get current time
    QueryPerformanceCounter(&liCounter);
    fClckTime = (double)liCounter.QuadPart/(double)liFrequency.QuadPart;

    if (pDoc->m_bFixdTime)
    {
      // Update current time
      if (fPrevTime == 0)
      {
        fPrevTime = fSimuTime;
      }

      // Update current time
      pDoc->m_fCurrTime += fDiffTime = fSimuTime - fPrevTime;

      // Execute calculations
      pDoc->Execute(pDoc->m_fCurrTime, fDiffTime);

      // Save previous time
      fPrevTime = fSimuTime;

      // Condition for ending simulation
      if (pDoc->m_fCurrTime > pDoc->m_fDuraTime)
      {
        g_bRun = FALSE;
      }
      else
      {
        fSimuTime += pDoc->m_fStepTime;
      }
    }
    else
    {
      // Update current time
      if (fPrevTime == 0)
      {
        fPrevTime = fClckTime;
      }

      // Update current time
      pDoc->m_fCurrTime += fDiffTime = fClckTime - fPrevTime;

      // Execute calculations
      pDoc->Execute(pDoc->m_fCurrTime, fDiffTime);

      // Save previous time
      fPrevTime = fClckTime;
    }

    // Calculate cost time
    QueryPerformanceCounter(&liCounter);
    fAftrTime = (double)liCounter.QuadPart/(double)liFrequency.QuadPart;
    pDoc->m_fCostTime = (pDoc->m_fCostTime + (fAftrTime - fClckTime)) / 2.0f;

    // Reduce cost of CPU using sleep
    if (!pDoc->m_bFixdTime)
    {
      if (pDoc->m_nSlepTime > 0)
      {
        Sleep(pDoc->m_nSlepTime);
      }
      else switch (pDoc->m_nCPULimit)
      {
        case 0:
          // 100 % - no control
          break;
        case 1:
          {
            // 75 % control
            if (pDoc->m_fCostTime < 0.001)
            {
              Sleep(1);
            }
            else
            {
              Sleep((int)(pDoc->m_fCostTime * 1000 / 0.75f));
            }
          }
          break;
        case 2:
          {
            // 50 % control
            if (pDoc->m_fCostTime < 0.001)
            {
              Sleep(2);
            }
            else
            {
              Sleep((int)(pDoc->m_fCostTime * 1000 / 0.50f));
            }
          }
          break;
        case 3:
          {
            // 25 % control
            if (pDoc->m_fCostTime < 0.001)
            {
              Sleep(4);
            }
            else
            {
              Sleep((int)(pDoc->m_fCostTime * 1000 / 0.25f));
            }
          }
          break;
      }
    }
  }

  ExitThread(0);
}

