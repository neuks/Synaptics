
%{
#include <windows.h>
#include <shlwapi.h>
#include "Syntax.h"

#define UPDATE_POS() \
  column += yyleng; \
  yylloc->first_line = line; \
  yylloc->first_column = column; \
  yylloc->last_line = line; \
  yylloc->last_column = column + yyleng;

int line = 1, column = 0;
%}

%x include

%option bison-bridge bison-locations

COLOR "0x"[0-9a-zA-Z]+
DIGIT [0-9]+|[0-9]+"."[0-9]+
IDENT [_a-zA-Z][_a-zA-Z0-9]*

%%

include {
  UPDATE_POS();
  BEGIN(include);
}

<include>[ "\t]* {
  UPDATE_POS();
}

<include>[^" "]+ {
  char path[MAX_PATH];
  GetModuleFileName(NULL, path, MAX_PATH);
  PathRemoveFileSpec(path);
  strcat(path, "\\Library\\");
  strcat(path, yytext);

  // Search for global library first
  if (!(yyin = fopen(path, "r")))
  {
    // Search in current directory
    if (!(yyin = fopen(yytext, "r")))
    { 
      char buf[256];
      printf("Error: Cannot open include file '%s'.\n", yytext);
      Message(yylloc->first_line, yylloc->first_column, buf);
      yyerror(NULL);
      BEGIN(INITIAL);
      yyterminate();
    }
  }

  YY_BUFFER_STATE buf = yy_create_buffer(yyin, YY_BUF_SIZE);
  yypush_buffer_state(buf);

  UPDATE_POS();

  BEGIN(INITIAL);
}

<<EOF>> {
  UPDATE_POS();
  yypop_buffer_state();

  if (!YY_CURRENT_BUFFER)
  {
    yyterminate();
  }
}

"//".*$ {
  UPDATE_POS();
}

{DIGIT} {
  UPDATE_POS();
  yylval->fNumber = atof(yytext);
  return NUM;
}

{COLOR} {
  UPDATE_POS();
  yylval->nColor = strtol(yytext, NULL, 16);
  return COLOR;
}

function {
  UPDATE_POS();
  return FUNCTION;
}

module {
  UPDATE_POS();
  return MODULE;
}

if {
  UPDATE_POS();
  return IF;
}

else {
  UPDATE_POS();
  return ELSE;
}

for {
  UPDATE_POS();
  return FOR;
}

var {
  UPDATE_POS();
  return VAR;
}

"==" {
  UPDATE_POS();
  return EQQ;
}

"<=" {
  UPDATE_POS();
  return LEQ;
}

">=" {
  UPDATE_POS();
  return GEQ;
}

"+=" {
  UPDATE_POS();
  return PEQ;
}

"-=" {
  UPDATE_POS();
  return SEQ;
}

"*=" {
  UPDATE_POS();
  return MEQ;
}

"/=" {
  UPDATE_POS();
  return DEQ;
}

"&&" {
  UPDATE_POS();
  return AND;
}

"||" {
  UPDATE_POS();
  return ORR;
}

"++" {
  UPDATE_POS();
  return INC;
}

"--" {
  UPDATE_POS();
  return DEC;
}

"+"|"-"|"*"|"/"|"^"|"!"|"<"|">"|"["|"]"|"("|")"|"{"|"}"|"="|","|"."|":"|";" {
  UPDATE_POS();
  return yytext[0];
}

{IDENT} {
  UPDATE_POS();
  yylval->pString = malloc(yyleng + 1);
  strcpy(yylval->pString, yytext);
  return ID;
}

[\n] {
  line += 1;
  column = 0;
}

. {
  UPDATE_POS();
}

%%

int yywrap()
{
  line = 1;
  column = 0;

  return 1;
}

int yyerror(char *s)
{
  if (s)
  {
    char buf[256];
    sprintf(buf, "Error: %s near '%s'.\n", s, yytext);
    Message(line, column, buf);
  }

  line = 1;
  column = 0;

  return 1;
}

