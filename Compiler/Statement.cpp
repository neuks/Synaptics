#include "Compiler.h"

extern "C" void stmt_free(struct stmt *stmt)
{
  ASSERT(stmt == NULL);

  // Do reverse release
  if (stmt->next)
  {
    stmt_free(stmt->next);
  }

  // Free expressions
  if (stmt->expr[0])
  {
    expr_free(stmt->expr[0]);
  }
  if (stmt->expr[1])
  {
    expr_free(stmt->expr[1]);
  }
  if (stmt->expr[2])
  {
    expr_free(stmt->expr[2]);
  }

  // Free statements
  if (stmt->stmt[0])
  {
    stmt_free(stmt->stmt[0]);
  }
  if (stmt->stmt[1])
  {
    stmt_free(stmt->stmt[1]);
  }

  // Release current object
  free(stmt);
}

extern "C" void stmt_dump(struct stmt *stmt)
{
}

extern "C" int  stmt_exec(struct stmt *stmt)
{
  if (stmt == NULL)
  {
    return TRUE;
  }

  // Execute current statement
  if (stmt->type == ST_EXP)
  {
    // Execute expression
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }
  }
  else if (stmt->type == ST_DEF)
  {
    // Define variable
    if (stmt->expr[0]->type == ET_EQU)
    {
      if (!ChkVariable(stmt->expr[0]->left->name, TRUE))
      {
        AddStack(stmt->expr[0]->left->name, 0);
      }
      else
      {
        char buf[256];
        sprintf(buf, "error: redefinition of variable '%s'.\n",
            stmt->expr[0]->left->name);
        Message(0, 0, buf);
        return FALSE;
      }
    }
    else if (stmt->expr[0]->type == ET_VAR)
    {
      if (!ChkVariable(stmt->expr[0]->name, TRUE))
      {
        AddStack(stmt->expr[0]->name, 0);
      }
      else
      {
        char buf[256];
        sprintf(buf, "error: redefinition of variable '%s'.\n",
            stmt->expr[0]->name);
        Message(0, 0, buf);
        return FALSE;
      }
    }
    else
    {
      ASSERT(TRUE);
    }

    // Execute expression
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }

    // Execute next definition
    if (stmt_exec(stmt->stmt[0]) == FALSE)
    {
      return FALSE;
    }
  }
  else if (stmt->type == ST_IF1)
  {
    // Execute judgement expression
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }

    // Execute stmt[0] if TRUE
    if (stmt->expr[0]->value != 0)
    {
      // Push stack for {
      PshStack();

      if (stmt_exec(stmt->stmt[0]) == FALSE)
      {
        return FALSE;
      }

      // Pop stack for }
      PopStack();
    }
  }
  else if (stmt->type == ST_IF2)
  {
    // Execute judgement expression
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }

    // Judge from expression result
    if (stmt->expr[0]->value != 0)
    {
      // Push stack for {
      PshStack();

      // Execute stmt[0] if TRUE
      if (stmt_exec(stmt->stmt[0]) == FALSE)
      {
        return FALSE;
      }

      // Pop stack for }
      PopStack();
    }
    else
    {
      // Push stack for {
      PshStack();

      // Execute stmt[1] if FALSE
      if (stmt_exec(stmt->stmt[1]) == FALSE)
      {
        return FALSE;
      }

      // Pop stack for }
      PopStack();
    }
  }
  else if (stmt->type == ST_IF3)
  {
    // Execute judgement expression
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }

    // Judge from expression result
    if (stmt->expr[0]->value != 0)
    {
      // Push stack for {
      PshStack();

      // Execute stmt[0] if TRUE
      if (stmt_exec(stmt->stmt[0]) == FALSE)
      {
        return FALSE;
      }

      // Pop stack for }
      PopStack();
    }
    else
    {
      // Execute expr[1] if FALSE
      if (expr_exec(stmt->expr[1]) == FALSE)
      {
        return FALSE;
      }
    }
  }
  else if (stmt->type == ST_IF4)
  {
    // Execute judgement expression
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }

    // Execute expr[1] if TRUE
    if (stmt->expr[0]->value != 0) if (expr_exec(stmt->expr[1]) == FALSE)
    {
      return FALSE;
    }
  }
  else if (stmt->type == ST_IF5)
  {
    // Execute judgement expression
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }

    // Judge from expression result
    if (stmt->expr[0]->value != 0)
    {
      // Execute expr[1] if TRUE
      if (expr_exec(stmt->expr[1]) == FALSE)
      {
        return FALSE;
      }
    }
    else
    {
      // Push stack for {
      PshStack();

      // Execute stmt[0] if FALSE
      if (stmt_exec(stmt->stmt[0]) == FALSE)
      {
        return FALSE;
      }

      // Pop stack for }
      PopStack();
    }
  }
  else if (stmt->type == ST_IF6)
  {
    // Execute judgement expression
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }

    // Judge from expression result
    if (stmt->expr[0]->value != 0)
    {
      // Execute expr[1] if TRUE
      if (expr_exec(stmt->expr[1]) == FALSE)
      {
        return FALSE;
      }
    }
    else
    {
      // Execute expr[2] if FALSE
      if (expr_exec(stmt->expr[2]) == FALSE)
      {
        return FALSE;
      }
    }
  }
  else if (stmt->type == ST_FOR1)
  {
    // Execute expr 1
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }

    // Execute expr 2
    if (expr_exec(stmt->expr[1]) == FALSE)
    {
      return FALSE;
    }

    // Do loop until condition not match
    while (stmt->expr[1]->value != 0)
    {
      // Push stack for {
      PshStack();

      // Evaluate statement
      if (stmt_exec(stmt->stmt[0]) == FALSE)
      {
        return FALSE;
      }

      // Execute expr 3
      if (expr_exec(stmt->expr[2]) == FALSE)
      {
        return FALSE;
      }

      // Execute expr 2
      if (expr_exec(stmt->expr[1]) == FALSE)
      {
        return FALSE;
      }

      // Pop stack for }
      PopStack();
    }

  }
  else if (stmt->type == ST_FOR2)
  {
    // Execute expr 1
    if (expr_exec(stmt->expr[0]) == FALSE)
    {
      return FALSE;
    }

    // Execute expr 2
    if (expr_exec(stmt->expr[1]) == FALSE)
    {
      return FALSE;
    }

    // Do loop until condition not match
    while (stmt->expr[1]->value != 0)
    {
      // Push stack for {
      PshStack();

      // Evaluate statement
      if (expr_exec(stmt->expr[3]) == FALSE)
      {
        return FALSE;
      }

      // Execute expr 3
      if (expr_exec(stmt->expr[2]) == FALSE)
      {
        return FALSE;
      }

      // Execute expr 2
      if (expr_exec(stmt->expr[1]) == FALSE)
      {
        return FALSE;
      }

      // Pop stack for }
      PopStack();
    }
  }
  else if (stmt->type == ST_STACK)
  {
    // Push stack for {
    PshStack();

    // Execute statement
    if (stmt_exec(stmt->stmt[0]) == FALSE)
    {
      return FALSE;
    }

    // Pop stack for }
    PopStack();
  }
  else
  {
    ASSERT(stmt == NULL);
  }

  // Execute next statement
  return stmt_exec(stmt->next);
}

