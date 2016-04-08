#ifndef __Expression_h__
#define __Expression_h__

#ifdef __cplusplus
extern "C" {
#endif

enum expr_type
{
  // Data
  ET_NUM,
  ET_VAR,
  ET_FNC,
  ET_PAR,

  // Operation
  ET_INC,
  ET_RINC,
  ET_DEC,
  ET_RDEC,
  ET_EQU,
  ET_ADD,
  ET_SUB,
  ET_MUL,
  ET_DIV,
  ET_EXP,
  ET_LTT,
  ET_GTT,
  ET_EQQ,
  ET_LEQ,
  ET_GEQ,
  ET_PEQ,
  ET_SEQ,
  ET_MEQ,
  ET_DEQ,
  ET_AND,
  ET_ORR,
  ET_NOT,
  ET_INV,
};

struct expr
{
  int type;
  double value;
  char *name;

  struct expr *left, *right;
};

void expr_free(struct expr *expr);
void expr_dump(struct expr *expr);
int  expr_exec(struct expr *expr);

#ifdef __cplusplus
}
#endif

#endif
