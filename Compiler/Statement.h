#ifndef __Statement_h__
#define __Statement_h__

#include "Expression.h"

#ifdef __cplusplus
extern "C" {
#endif

enum stmt_type
{
  ST_EXP,
  ST_DEF,
  ST_IF1,
  ST_IF2,
  ST_IF3,
  ST_IF4,
  ST_IF5,
  ST_IF6,
  ST_FOR1,
  ST_FOR2,
  ST_STACK,
};

struct stmt
{
  int type;
  struct expr *expr[4];
  struct stmt *stmt[2], *next;
};

void stmt_free(struct stmt *stmt);
void stmt_dump(struct stmt *stmt);
int  stmt_exec(struct stmt *stmt);

#ifdef __cplusplus
}
#endif

#endif
