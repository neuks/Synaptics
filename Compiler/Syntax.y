
%{
#include "Compiler.h"
%}

%union {
  unsigned nColor;
  char    *pString;
  double   fNumber;
  struct expr *pExpr;
  struct stmt *pStmt;
}

%token EQQ LEQ GEQ PEQ SEQ MEQ DEQ AND ORR INC DEC
%token MODULE FUNCTION VAR IF ELSE FOR
%token <fNumber> NUM
%token <nColor> COLOR
%token <pString> ID
%type  <pStmt> stmt gpar spar
%type  <pExpr> expr fpar rpar cpar

%right '=' '<' '>' EQQ LEQ GEQ PEQ SEQ MEQ DEQ AND ORR
%left '+' '-'
%left '*' '/'
%left '^' '!'

%locations
%pure-parser

%%

prog:
    | prog FUNCTION rpar '=' ID '(' cpar ')' '{' stmt '}'
    {
      struct stmt *stmt = malloc(sizeof(struct stmt));
      stmt->type    = ST_DEF;
      stmt->expr[0] = $3;
      stmt->expr[0]->left = $7;
      stmt->expr[1] = NULL;
      stmt->expr[2] = NULL;
      stmt->expr[3] = NULL;
      stmt->stmt[0] = NULL;
      stmt->stmt[1] = NULL;
      stmt->next    = $10;

      // Add function to library
      if (GetFunction($5))
      {
        char buf[256];
        sprintf(buf, "Error: Function '%s' already defined.\n", $5);
        Message(@1.first_line, @1.first_column, buf);

        stmt_free(stmt);
        SetFunction($5, NULL);
        free($5);
        return yyerror(NULL);
      }
      else
      {
        SetFunction($5, stmt);
        free($5);
      }
    }
    | prog MODULE '[' opar ']' '=' ID '(' ipar ')' '{' stmt '}'
    {
      if (SetModule($7, $12) == 0)
      {
        char buf[256];
        sprintf(buf, "Error: Module '%s' already defined.\n", $7);
        Message(@1.first_line, @1.first_column, buf);

        stmt_free($12);
        return yyerror(NULL);
      }
      free($7);
    }
    | prog ID ':' COLOR ';'
    {
      if (SetModuleIOColor($2, $4) == 0)
      {
        char buf[256];
        sprintf(buf, "Error: Variable '%s' undefined.\n", $2);
        Message(@1.first_line, @1.first_column, buf);

        return yyerror(NULL);
      }
    }
    | prog VAR gpar ';'
    ;

rpar: ID
    {
      // create input/output definition statement
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_VAR;
      $$->value = 0;
      $$->name  = $1;
      $$->left  = NULL;
      $$->right = NULL;
    }
    ;

cpar:
    {
      $$ = NULL;
    }
    | ID
    {
      // create input/output definition statement
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_VAR;
      $$->value = 0;
      $$->name  = $1;
      $$->left  = NULL;
      $$->right = NULL;
    }
    | ID ',' cpar
    {
      // create input/output definition statement
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_VAR;
      $$->value = 0;
      $$->name  = $1;
      $$->left  = $3;
      $$->right = NULL;
    }

ipar:
    {
    }
    | ID
    {
      if (ChkVariable($1, 0))
      {
        char buf[256];
        sprintf(buf, "Error: input variable '%s' exists.\n", $1);
        Message(@1.first_line, @1.first_column, buf);

        free($1);
        return yyerror(NULL);
      }
      else
      {
        AddModuleInput($1);
        free($1);
      }
    }
    | ID ':' COLOR
    {
      if (ChkVariable($1, 0))
      {
        char buf[256];
        sprintf(buf, "Error: input variable '%s' exists.\n", $1);
        Message(@1.first_line, @1.first_column, buf);

        free($1);
        return yyerror(NULL);
      }
      else
      {
        AddModuleInput($1);
        SetModuleIOColor($1, $3);
        free($1);
      }
    }
    | ipar ',' ID
    {
      if (ChkVariable($3, 1))
      {
        char buf[256];
        sprintf(buf, "Error: Input variable '%s' exists.\n", $3);
        Message(@1.first_line, @1.first_column, buf);

        free($3);
        return yyerror(NULL);
      }
      else
      {
        AddModuleInput($3);
        free($3);
      }
    }
    | ipar ',' ID ':' COLOR
    {
      if (ChkVariable($3, 1))
      {
        char buf[256];
        sprintf(buf, "Error: Input variable '%s' exists.\n", $3);
        Message(@1.first_line, @1.first_column, buf);

        free($3);
        return yyerror(NULL);
      }
      else
      {
        AddModuleInput($3);
        SetModuleIOColor($3, $5);
        free($3);
      }
    }
    ;

opar:
    {
    }
    | ID
    {
      if (ChkVariable($1, 0))
      {
        char buf[256];
        sprintf(buf, "Error: Output variable '%s' exists.\n", $1);
        Message(@1.first_line, @1.first_column, buf);

        free($1);
        return yyerror(NULL);
      }
      else
      {
        AddModuleOutput($1);
        free($1);
      }
    }
    | ID ':' COLOR
    {
      if (ChkVariable($1, 0))
      {
        char buf[256];
        sprintf(buf, "Error: Output variable '%s' exists.\n", $1);
        Message(@1.first_line, @1.first_column, buf);

        free($1);
        return yyerror(NULL);
      }
      else
      {
        AddModuleOutput($1);
        SetModuleIOColor($1, $3);
        free($1);
      }
    }
    | opar ',' ID
    {
      if (ChkVariable($3, 0))
      {
        char buf[256];
        sprintf(buf, "Error: Output variable '%s' exists.\n", $3);
        Message(@1.first_line, @1.first_column, buf);
        free($3);

        return yyerror(NULL);
      }
      else
      {
        AddModuleOutput($3);
        free($3);
      }
    }
    | opar ',' ID ':' COLOR
    {
      if (ChkVariable($3, 0))
      {
        char buf[256];
        sprintf(buf, "Error: Output variable '%s' exists.\n", $3);
        Message(@1.first_line, @1.first_column, buf);
        free($3);

        return yyerror(NULL);
      }
      else
      {
        AddModuleOutput($3);
        SetModuleIOColor($3, $5);
        free($3);
      }
    }
    ;

gpar:
    {
    }
    | ID
    {
      if (ChkVariable($1, 0))
      {
        char buf[256];
        sprintf("Error: Global variable '%s' exists.\n", $1);
        Message(@1.first_line, @1.first_column, buf);

        free($1);
        return yyerror(NULL);
      }
      else
      {
        AddGlobal($1, 0);
        free($1);
      }
    }
    | ID '[' NUM ']'
    {
      char name[256], buff[256];

      name[0] = 0;
      strcat(name, $1);
      strcat(name, "-0");
      if (ChkVariable(name, 0))
      {
        char buf[256];
        sprintf("Error: Global variable '%s' exists.\n", name);
        Message(@1.first_line, @1.first_column, buf);

        free($1);
        return yyerror(NULL);
      }
      else
      {
        int i, max = (int)$3;
        for (i = 0; i < max; i++)
        {
          name[0] = 0;
          strcat(name, $1);
          strcat(name, "-");
          strcat(name, itoa(i, buff, 10));
          AddGlobal(name, 0);
        }
        free($1);
      }
    }
    | ID '=' NUM
    {
      if (ChkVariable($1, 0))
      {
        char buf[256];
        sprintf("Error: Global variable '%s' exists.\n", $1);
        Message(@1.first_line, @1.first_column, buf);

        free($1);
        return yyerror(NULL);
      }
      else
      {
        AddGlobal($1, $3);
        free($1);
      }
    }
    | gpar ',' ID
    {
      if (ChkVariable($3, 0))
      {
        char buf[256];
        sprintf("Error: Global variable '%s' exists.\n", $3);
        Message(@1.first_line, @1.first_column, buf);

        free($3);
        return yyerror(NULL);
      }
      else
      {
        AddGlobal($3, 0);
        free($3);
      }
    }
    | gpar ',' ID '[' NUM ']'
    {
      char name[256], buff[256];

      // Check for names
      name[0] = 0;
      strcat(name, $3);
      strcat(name, "-0");
      if (ChkVariable(name, 0))
      {
        char buf[256];
        sprintf("Error: Global variable '%s' exists.\n", name);
        Message(@1.first_line, @1.first_column, buf);

        free($3);
        return yyerror(NULL);
      }
      else
      {
        int i, max = (int)$5;
        for (i = 0; i < max; i++)
        {
          name[0] = 0;
          strcat(name, $3);
          strcat(name, "-");
          strcat(name, itoa(i, buff, 10));
          AddGlobal(name, 0);
        }
        free($3);
      }
    }
    | gpar ',' ID '=' NUM
    {
      if (ChkVariable($3, 0))
      {
        char buf[256];
        sprintf("Error: Global variable '%s' exists.\n", $3);
        Message(@1.first_line, @1.first_column, buf);

        free($3);
        return yyerror(NULL);
      }
      else
      {
        AddGlobal($3, $5);
        free($3);
      }
    }
    ;

stmt:
    {
      $$ = NULL;
    }
    | expr ';' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_EXP;
      $$->expr[0] = $1;
      $$->expr[1] = NULL;
      $$->expr[2] = NULL;
      $$->expr[3] = NULL;
      $$->stmt[0] = NULL;
      $$->stmt[1] = NULL;
      $$->next    = $3;

      // Syntax error if not assignment or function call
      if (($1->type != ET_EQU) && ($1->type != ET_PEQ) &&
          ($1->type != ET_SEQ) && ($1->type != ET_MEQ) &&
          ($1->type != ET_DEQ) && ($1->type != ET_FNC) &&
          ($1->type != ET_INC) && ($1->type != ET_DEC))
      {
        Message(@1.first_line, @1.first_column,
            "Error: Statement not assignment or invoke.\n");

        stmt_free($$);
        $$ = NULL;
        return yyerror(NULL);
      }
    }
    | VAR spar ';' stmt
    {
      if ($2 == NULL)
      {
        // If expressions has errors, free all allocated spaces
        if ($4)
        {
          stmt_free($4);
        }

        return yyerror(NULL);
      }
      else
      {
        // Allocate statement descriptor
        $$ = $2;
        $$->next = $4;
      }
    }
    | IF '(' expr ')' '{' stmt '}' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_IF1;
      $$->expr[0] = $3;
      $$->expr[1] = NULL;
      $$->expr[2] = NULL;
      $$->expr[3] = NULL;
      $$->stmt[0] = $6;
      $$->stmt[1] = NULL;
      $$->next    = $8;
    }
    | IF '(' expr ')' '{' stmt '}' ELSE '{' stmt '}' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_IF2;
      $$->expr[0] = $3;
      $$->expr[1] = NULL;
      $$->expr[2] = NULL;
      $$->expr[3] = NULL;
      $$->stmt[0] = $6;
      $$->stmt[1] = $10;
      $$->next    = $12;
    }
    | IF '(' expr ')' '{' stmt '}' ELSE expr ';' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_IF3;
      $$->expr[0] = $3;
      $$->expr[1] = $9;
      $$->expr[2] = NULL;
      $$->expr[3] = NULL;
      $$->stmt[0] = $6;
      $$->stmt[1] = NULL;
      $$->next    = $11;
    }
    | IF '(' expr ')' expr ';' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_IF4;
      $$->expr[0] = $3;
      $$->expr[1] = $5;
      $$->expr[2] = NULL;
      $$->expr[3] = NULL;
      $$->stmt[0] = NULL;
      $$->stmt[1] = NULL;
      $$->next    = $7;
    }
    | IF '(' expr ')' expr ';' ELSE '{' stmt '}' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_IF5;
      $$->expr[0] = $3;
      $$->expr[1] = $5;
      $$->expr[2] = NULL;
      $$->expr[3] = NULL;
      $$->stmt[0] = $9;
      $$->stmt[1] = NULL;
      $$->next    = $11;
    }
    | IF '(' expr ')' expr ';' ELSE expr ';' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_IF6;
      $$->expr[0] = $3;
      $$->expr[1] = $5;
      $$->expr[2] = $8;
      $$->expr[3] = NULL;
      $$->stmt[0] = NULL;
      $$->stmt[1] = NULL;
      $$->next    = $10;
    }
    | FOR '(' expr ';' expr ';' expr ')' '{' stmt '}' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_FOR1;
      $$->expr[0] = $3;
      $$->expr[1] = $5;
      $$->expr[2] = $7;
      $$->expr[3] = NULL;
      $$->stmt[0] = $10;
      $$->stmt[1] = NULL;
      $$->next    = $12;
    }
    | FOR '(' expr ';' expr ';' expr ')' expr ';' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_FOR2;
      $$->expr[0] = $3;
      $$->expr[1] = $5;
      $$->expr[2] = $7;
      $$->expr[3] = $9;
      $$->stmt[0] = NULL;
      $$->stmt[1] = NULL;
      $$->next    = $11;
    }
    | '{' stmt '}' stmt
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_STACK;
      $$->expr[0] = NULL;
      $$->expr[1] = NULL;
      $$->expr[2] = NULL;
      $$->expr[3] = NULL;
      $$->stmt[0] = $2;
      $$->stmt[1] = NULL;
      $$->next    = $4;
    }
    ;

spar: expr
    {
      // Allocate statement descriptor
      $$ = malloc(sizeof(struct stmt));
      $$->type    = ST_DEF;
      $$->expr[0] = $1;
      $$->expr[1] = NULL;
      $$->expr[2] = NULL;
      $$->expr[3] = NULL;
      $$->stmt[0] = NULL;
      $$->stmt[1] = NULL;
      $$->next    = NULL;

      // Syntax error if not assignment
      if (($1->type != ET_EQU) && ($1->type != ET_VAR))
      {
        Message(@1.first_line, @1.first_column,
            "Error: Invalid variable definition.");

        stmt_free($$);

        // Indicate the error
        $$ = NULL;
      }
    }
    | expr ',' spar
    {
      if ($3 == NULL)
      {
        // Stop execution on error
        $$ = NULL;
      }
      else
      {
        // Allocate statement descriptor
        $$ = malloc(sizeof(struct stmt));
        $$->type    = ST_DEF;
        $$->expr[0] = $1;
        $$->expr[1] = NULL;
        $$->expr[2] = NULL;
        $$->expr[3] = NULL;
        $$->stmt[0] = $3;
        $$->stmt[1] = NULL;
        $$->next    = NULL;

        // Syntax error if not assignment
        if (($1->type != ET_EQU) && ($1->type != ET_VAR))
        {
          Message(@1.first_line, @1.first_column,
              "Error: Invalid variable definition.");
  
          stmt_free($$);

          // Indicate the error
          $$ = NULL;
        }
      }
    }
    ;

expr: ID
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_VAR;
      $$->name  = $1;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = NULL;
    }
    | NUM
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_NUM;
      $$->name  = NULL;
      $$->value = $1;
      $$->left  = NULL;
      $$->right = NULL;
    }
    | ID '[' expr ']'
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_VAR;
      $$->name  = $1;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = $3;
    }
    | ID '(' fpar ')'
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_FNC;
      $$->name  = $1;
      $$->value = 0;
      $$->left  = $3;
      $$->right = NULL;
    }
    | ID INC
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_INC;
      $$->name  = $1;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = NULL;
    }
    | ID '[' expr ']' INC
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_INC;
      $$->name  = $1;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = $3;
    }
    | INC ID
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_RINC;
      $$->name  = $2;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = NULL;
    }
    | INC ID '[' expr ']'
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_RINC;
      $$->name  = $2;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = $4;
    }
    | ID DEC
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_DEC;
      $$->name  = $1;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = NULL;
    }
    | ID '[' expr ']' DEC
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_DEC;
      $$->name  = $1;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = $3;

    }
    | DEC ID
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_RDEC;
      $$->name  = $2;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = NULL;
    }
    | DEC ID '[' expr ']'
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_RDEC;
      $$->name  = $2;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = $4;

    }
    | expr '=' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_EQU;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Error if receiver is not variable
      if ($1->type != ET_VAR)
      {
        Message(@1.first_line, @1.first_column,
            "Error: Assignment to non-variable.");

        // Release allocated memory
        expr_free($$);

        return yyerror(NULL);
      }
    }
    | expr '+' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_ADD;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value + $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr '-' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_SUB;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value - $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr '*' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_MUL;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value * $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr '/' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_DIV;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value / $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr '^' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_EXP;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = pow($1->value, $3->value);
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr '<' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_LTT;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value < $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr '>' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_GTT;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value > $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr EQQ expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_EQQ;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value < $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr LEQ expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_LEQ;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value < $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr GEQ expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_GEQ;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value > $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr PEQ expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_PEQ;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Error if receiver is not variable
      if ($1->type != ET_VAR)
      {
        Message(@1.first_line, @1.first_column,
            "Error: Assignment to non-variable.");

        // Release allocated memory
        expr_free($$);

        return yyerror(NULL);
      }
    }
    | expr SEQ expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_SEQ;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Error if receiver is not variable
      if ($1->type != ET_VAR)
      {
        Message(@1.first_line, @1.first_column,
            "Error: Assignment to non-variable.");

        // Release allocated memory
        expr_free($$);

        return yyerror(NULL);
      }
    }
    | expr MEQ expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_MEQ;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Error if receiver is not variable
      if ($1->type != ET_VAR)
      {
        Message(@1.first_line, @1.first_column,
            "Error: Assignment to non-variable.");

        // Release allocated memory
        expr_free($$);

        return yyerror(NULL);
      }
    }
    | expr DEQ expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_DEQ;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Error if receiver is not variable
      if ($1->type != ET_VAR)
      {
        Message(@1.first_line, @1.first_column,
            "Error: Assignment to non-variable.");

        // Release allocated memory
        expr_free($$);

        return yyerror(NULL);
      }
    }
    | expr AND expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_AND;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value && $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | expr ORR expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_ORR;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $1;
      $$->right = $3;

      // Do simplification
      if ($1->type == ET_NUM && $3->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = $1->value || $3->value;
        $$->left  = NULL;
        $$->right = NULL;

        // Release allocated memory
        expr_free($1);
        expr_free($3);
      }
    }
    | '!' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_NOT;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $2;
      $$->right = NULL;

      // Do simplification
      if ($2->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = ! $2->value;
        $$->left  = NULL;

        // Release allocated memory
        expr_free($2);
      }
    }
    | '-' expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_INV;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $2;
      $$->right = NULL;

      // Do simplification
      if ($2->type == ET_NUM)
      {
        $$->type  = ET_NUM;
        $$->value = - $2->value;
        $$->left  = NULL;

        // Release allocated memory
        expr_free($2);
      }
    }
    | '(' expr ')'
    {
      $$ = $2;
    }
    ;

fpar:
    {
      $$ = NULL;
    }
    | expr
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_PAR;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = NULL;
      $$->right = $1;
    }
    | expr ',' fpar
    {
      // Allocate expression descriptor
      $$ = malloc(sizeof(struct expr));
      $$->type  = ET_PAR;
      $$->name  = NULL;
      $$->value = 0;
      $$->left  = $3;
      $$->right = $1;
    }
    ;

%%

