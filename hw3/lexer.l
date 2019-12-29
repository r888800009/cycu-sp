id [a-zA-Z][a-zA-Z0-9]*
unsigned_integer [0-9]+
real [0-9]+\.[0-9]*
nl \n
pow ¡ô

%{
#include "main.tab.hpp"
extern int lineno;

%}

%%
[\r] {}
[\t ] {/* ignore write space*/;}
AND {return AND;}
BOOLEAN {return BOOLEAN;}
CALL {return CALL;}
DIMENSION {return DIMENSION;}
ELSE {return ELSE;}
ENP {return ENP;}
ENS {return ENS;}
EQ {return EQ;}
GE {return GE;}
GT {return GT;}
GTO {return GTO;}
IF {return IF;}
INPUT {return INPUT;}
INTEGER {return INTEGER;}
LABEL {return LABEL;}
LE {return LE;}
LT {return LT;}
NE {return NE;}
OR {return OR;}
OUTPUT {return OUTPUT;}
PROGRAM {return PROGRAM; }
REAL {return REAL;}
SUBROUTINE {return SUBROUTINE;}
THEN {return THEN;}
VARIABLE {return VARIABLE;}
{pow} {return POW;}

{real} {
  yylval.floatStr = strdup(yytext);
  return UNSIGNED_REAL;
}

{id} {
  yylval.idStr = strdup(yytext);
  if (strcmp(yytext, "LABLE") == 0)
    warning("do you mean LABEL?");

  return IDENTIFIER;
}

{unsigned_integer} {
  yylval.intStr = strdup(yytext);
  return UNSIGNED_INTEGER;
}

{nl} {lineno++;}
. {
  yylval.lastChar = yytext[0];
  return yytext[0];
}
%%

void warning(const char *str) {
  fprintf(yyout, "Warning line %d: %s\n", lineno, str);
}
