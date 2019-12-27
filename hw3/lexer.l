id [a-zA-Z][a-zA-Z0-9]*
unsigned_integer [0-9]+
nl \n

%{
#include "main.tab.h"
extern int lineno;
extern FILE *fptr;
%}

%%
[\r] {}
[\t ] {ECHO/* ignore write space*/;}
AND {return AND;}
BOOLEAN {return BOOLEAN;}
CALL {return CALL;}
DIMENSION {ECHO;return DIMENSION;}
ELSE {return ELSE;}
ENP {return ENP;}
ENS {return ENS;}
EQ {return EQ;}
GE {return GE;}
GT {return GT;}
GTO {return GTO;}
IF {return IF;}
INPUT {return INPUT;}
INTEGER {ECHO;return INTEGER;}
LABEL {return LABEL;}
LE {return LE;}
LT {return LT;}
NE {return NE;}
OR {return OR;}
OUTPUT {return OUTPUT;}
PROGRAM {ECHO;return PROGRAM; }
REAL {return REAL;}
SUBROUTINE {return SUBROUTINE;}
THEN {return THEN;}
VARIABLE {return VARIABLE;}
{id} {ECHO;return IDENTIFIER;}
{unsigned_integer} {ECHO;return UNSIGNED_INTEGER;}
{nl} {ECHO;lineno++;}
. {ECHO;return yytext[0];}
%%
