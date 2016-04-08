#include "Compiler.h"

//=============================================================================
// Interface functions from C++ to C
//=============================================================================

CBlockUnit *g_pBlock;

int CompileBlock(CBlockUnit *pBlock)
{
  ASSERT (g_pBlock != NULL);

  BOOL bRet;

  g_pBlock = pBlock;

  // Release previous program
  if (g_pBlock->m_pProgram)
  {
    stmt_free(g_pBlock->m_pProgram);
    g_pBlock->m_pProgram = NULL;
  }

  // Setup for current code
  yy_scan_string(pBlock->m_sSource.c_str());

  // Compile & check for result
  if (!yyparse())
  {
    bRet = TRUE;
  }
  else
  {
    bRet = FALSE;
  }

  g_pBlock = NULL;

  return bRet;
}

int ExecuteBlock(CBlockUnit *pBlock)
{
  ASSERT (g_pBlock != NULL);

  BOOL iRet;

  g_pBlock = pBlock;

  // Execute current program
  iRet = stmt_exec(pBlock->m_pProgram);

  g_pBlock = NULL;

  return iRet;
}

CVariable *GetVariable(char *pName, BOOL bCheck)
{
  ASSERT(pName == NULL);

  return g_pBlock->GetVariable(pName, bCheck);
}

//=============================================================================
// Interface functions from C to C++
//=============================================================================

extern "C" void Message(int nLine, int nColumn, char *pMsg)
{
  ASSERT(g_pBlock == NULL);

  theApp.m_pMainFrame->AddMessage((char*)g_pBlock->m_sName.c_str(),
      nLine, nColumn, pMsg);
}

extern "C" void SetFunction(char *pName, struct stmt *pStmt)
{
  ASSERT(g_pBlock == NULL);

  g_pBlock->AddLibrary(pName, pStmt);
}

extern "C" struct stmt *GetFunction(char *pName)
{
  ASSERT(g_pBlock == NULL);

  return g_pBlock->GetLibrary(pName);
}

extern "C" int SetModule(char *pName, struct stmt *pStmt)
{
  ASSERT(g_pBlock == NULL);

  if (g_pBlock->m_pProgram == NULL)
  {
    g_pBlock->m_sName = pName;
    g_pBlock->m_pProgram = pStmt;

    return 1;
  }
  else
  {
    return 0;
  }
}

extern "C" void AddModuleInput(char *pName)
{
  ASSERT(g_pBlock == NULL);

  g_pBlock->AddInput(new CVariable(pName));
}

extern "C" void AddModuleOutput(char *pName)
{
  ASSERT(g_pBlock == NULL);

  g_pBlock->AddOutput(new CVariable(pName));
}

extern "C" int SetModuleIOColor(char *pName, unsigned nColor)
{
  ASSERT(g_pBlock == NULL);

  CVariable *pVar = g_pBlock->GetVariable(pName, FALSE);

  if (pVar != NULL)
  {
    pVar->m_nColor = 0xff000000 | nColor;

    return 1;
  }
  else
  {
    return 0;
  }
}

extern "C" void PshStack()
{
  ASSERT(g_pBlock == NULL);

  g_pBlock->PshStack();
}

extern "C" void PopStack()
{
  ASSERT(g_pBlock == NULL);

  g_pBlock->PopStack();
}

extern "C" void PshHeap()
{
  ASSERT(g_pBlock == NULL);

  g_pBlock->PshHeap();
}

extern "C" void PopHeap()
{
  ASSERT(g_pBlock == NULL);

  g_pBlock->PopHeap();
}

extern "C" void AddGlobal(char *pName, double fValue)
{
  ASSERT(g_pBlock == NULL);

  g_pBlock->AddGlobal(new CVariable(pName), fValue);
}

extern "C" void AddStack(char *pName, double fValue)
{
  ASSERT(g_pBlock == NULL);

  g_pBlock->AddStack(new CVariable(pName), fValue);
}

extern "C" int ChkVariable(char *pName, int bCheck)
{
  ASSERT(pName == NULL);

  if (g_pBlock->GetVariable(pName, bCheck))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

