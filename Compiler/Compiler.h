#ifndef __Compiler_h__
#define __Compiler_h_

#ifdef __cplusplus
#include "../Graphics/StdAfx.h"
extern "C"
{
#include "Syntax.h"
#include "Lexical.h"
}
#endif
#include <stdio.h>
#include <stdlib.h>
#include "Expression.h"
#include "Statement.h"

#ifdef __cplusplus
int CompileBlock(CBlockUnit *pBlock);
int ExecuteBlock(CBlockUnit *pBlock);
CVariable *GetVariable(char *pName, BOOL bStack);
#endif

#ifdef __cplusplus
extern CBlockUnit *g_pBlock;
extern "C" {
#endif
// For compilation
void Message(int nLine, int nColumn, char *pMsg);
void SetFunction(char *pName, struct stmt *pStmt);
struct stmt *GetFunction(char *pName);
int  SetModule(char *pName, struct stmt *pStmt);
void AddModuleInput(char *pName);
void AddModuleOutput(char *pName);
int  SetModuleIOColor(char *pName, unsigned nColor);

// For execution/generation
void PshStack();
void PopStack();
void PshHeap();
void PopHeap();
void AddGlobal(char *pName, double fValue);
void AddStack(char *pName, double fValue);
int  ChkVariable(char *pName, int bCheck);
#ifdef __cplusplus
}
#endif

#endif
