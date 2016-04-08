#include "Compiler.h"

extern "C" void expr_free(struct expr *expr)
{
  ASSERT(expr == NULL);

  // Free left hand tree
  if (expr->left)
  {
    expr_free(expr->left);
  }

  // Free right hand tree
  if (expr->right)
  {
    expr_free(expr->right);
  }

  // Release name
  if (expr->name)
  {
    free(expr->name);
  }

  // Release exp
  free(expr);
}

extern "C" void expr_dump(struct expr *expr)
{
  ASSERT(expr == NULL);
}

extern "C" int expr_exec(struct expr *expr)
{
  ASSERT(expr == NULL);

  // Execute branches first
  if (expr->right)
  {
    if (expr_exec(expr->right) == FALSE)
    {
      return FALSE;
    }
  }
  if (expr->left)
  {
    if (expr_exec(expr->left) == FALSE)
    {
      return FALSE;
    }
  }

  // Execute current operation
  if (expr->type == ET_NUM)
  {
  }
  else if (expr->type == ET_VAR)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->name);

    if (expr->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    expr->value = pVar->m_fValue;
  }
  else if (expr->type == ET_FNC)
  {
    if (!strcmp(expr->name, "sin"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function sin takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = sin(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "asin"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function asin takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = asin(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "cos"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function cos takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = cos(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "acos"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function acos takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = acos(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "tan"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function tan takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = tan(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "atan"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function atan takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = atan(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "exp"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function exp takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = exp(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "log"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function log takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = log(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "log10"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function log10 takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = log10(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "sqrt"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function sqrt takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = sqrt(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "ceil"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function ceil takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = ceil(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "floor"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function floor takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = floor(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "abs"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function abs takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        expr->value = fabs(expr->left->value);
      }
    }
    else if (!strcmp(expr->name, "rand"))
    {
      expr->value = (double)rand()/(double)RAND_MAX;
    }
    else if (!strcmp(expr->name, "warn"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function error takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        char buf[256];
        sprintf(buf, "Warning: Condition '%d' triggered.\n", expr->left->value);
        Message(0, 0, buf);
      }
    }
    else if (!strcmp(expr->name, "error"))
    {
      if (expr->left == NULL)
      {
        Message(0, 0, "Error: Function error takes 1 parameter.\n");
        return FALSE;
      }
      else
      {
        char buf[256];
        sprintf(buf, "Error: Condition '%d' triggered.\n", expr->left->value);
        Message(0, 0, buf);

        return FALSE;
      }
    }
    else
    {
      struct stmt *stmt = GetFunction(expr->name);

      if (stmt == NULL)
      {
        char buf[256];
        sprintf(buf, "Error: Function '%s' not implemented.\n", expr->name);
        Message(0, 0, buf);

        return FALSE;
      }
      else
      {
        struct expr *inp = expr->left;
        struct expr *out = stmt->expr[0];

        // Advance to inputs
        out = out->left;

        // Create new heap
        PshHeap();

        // Calculate inputs
        while(TRUE)
        {
          // Check for parameter validity
          if ((inp == NULL) || (out == NULL))
          {
            if ((inp != NULL) || (out != NULL))
            {
              char buf[256];
              sprintf(buf, "Error: Function '%s' parameter mismatch.\n",
                  expr->name);
              Message(0, 0, buf);

              return FALSE;
            }
            break;
          }

          // Assign current value
          out->value = inp->value;

          // Add Inputs to stack
          AddStack(out->name, out->value);

          // Switch to next value
          inp = inp->left;
          out = out->left;
        }

        // Execute statemnt
        if (stmt_exec(stmt) == FALSE)
        {
          return FALSE;
        }

        // Assign output
        CVariable *pVar = GetVariable(stmt->expr[0]->name, FALSE);
        if (pVar == NULL)
        {
          char buf[256];
          sprintf(buf, "Error: Variable '%s' not defined.\n",
              stmt->expr[0]->name);
          Message(0, 0, buf);

          return FALSE;
        }
        expr->value = pVar->m_fValue;

        // Restore previous heap
        PopHeap();
      }
    }
  }
  else if (expr->type == ET_PAR)
  {
    expr->value = expr->right->value;
  }
  else if (expr->type == ET_INC)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->name);

    if (expr->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    pVar->m_fValue = pVar->m_fValue + 1;
  }
  else if (expr->type == ET_RINC)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->name);

    if (expr->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    expr->value = pVar->m_fValue;
    pVar->m_fValue = pVar->m_fValue + 1;
  }
  else if (expr->type == ET_DEC)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->name);

    if (expr->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    pVar->m_fValue = pVar->m_fValue - 1;
  }
  else if (expr->type == ET_RDEC)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->name);

    if (expr->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    expr->value = pVar->m_fValue;
    pVar->m_fValue = pVar->m_fValue - 1;
  }
  else if (expr->type == ET_EQU)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->left->name);

    if (expr->left->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->left->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    expr->value = pVar->m_fValue = expr->left->value = expr->right->value;
  }
  else if (expr->type == ET_ADD)
  {
    expr->value = expr->left->value + expr->right->value;
  }
  else if (expr->type == ET_SUB)
  {
    expr->value = expr->left->value - expr->right->value;
  }
  else if (expr->type == ET_MUL)
  {
    expr->value = expr->left->value * expr->right->value;
  }
  else if (expr->type == ET_DIV)
  {
    expr->value = expr->left->value / expr->right->value;
  }
  else if (expr->type == ET_EXP)
  {
    expr->value = pow(expr->left->value, expr->right->value);
  }
  else if (expr->type == ET_LTT)
  {
    expr->value = expr->left->value < expr->right->value;
  }
  else if (expr->type == ET_GTT)
  {
    expr->value = expr->left->value > expr->right->value;
  }
  else if (expr->type == ET_EQQ)
  {
    expr->value = expr->left->value == expr->right->value;
  }
  else if (expr->type == ET_LEQ)
  {
    expr->value = expr->left->value <= expr->right->value;
  }
  else if (expr->type == ET_GEQ)
  {
    expr->value = expr->left->value >= expr->right->value;
  }
  else if (expr->type == ET_PEQ)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->left->name);

    if (expr->left->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->left->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    expr->value = pVar->m_fValue = expr->left->value += expr->right->value;
  }
  else if (expr->type == ET_SEQ)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->left->name);

    if (expr->left->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->left->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    expr->value = pVar->m_fValue = expr->left->value -= expr->right->value;
  }
  else if (expr->type == ET_MEQ)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->left->name);

    if (expr->left->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->left->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    expr->value = pVar->m_fValue = expr->left->value *= expr->right->value;
  }
  else if (expr->type == ET_DEQ)
  {
    CVariable *pVar;
    char name[256], buff[10];

    name[0] = 0;
    strcat(name, expr->left->name);

    if (expr->left->right)
    {
      strcat(name, "-");
      strcat(name, itoa((int)expr->left->right->value, buff, 10));
      pVar = GetVariable(name, FALSE);
    }

    if ((pVar = GetVariable(name, FALSE)) == NULL)
    {
      char buf[256];
      sprintf(buf, "Error: Variable '%s' not defined.\n", name);
      Message(0, 0, buf);

      return FALSE;
    }

    expr->value = pVar->m_fValue = expr->left->value /= expr->right->value;
  }
  else if (expr->type == ET_AND)
  {
    expr->value = expr->left->value && expr->right->value;
  }
  else if (expr->type == ET_ORR)
  {
    expr->value = expr->left->value || expr->right->value;
  }
  else if (expr->type == ET_NOT)
  {
    expr->value = ! expr->left->value;
  }
  else if (expr->type == ET_INV)
  {
    expr->value = - expr->left->value;
  }
  else
  {
    ASSERT(TRUE);
  }

  return TRUE;
}

