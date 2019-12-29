%{
#include <cstdbool>
#include <iostream>
#include "compiler.h"
#include "quadruple.h"
#include "identifier_table.h"
#include <vector>
#include <sstream>

extern "C"
{
        int yyparse(void);
        int yylex(void);
        int yywrap()
        {
                return 1;
        }

        void yyerror(char const  *s);
        void yyrestart(FILE *pt);
}

enum Check {check_disable, check_semicolon, check_brackets} check;
extern FILE *yyin, *yyout;

vector<int> argRegister;
vector<int> infoTab;
int arrayBegin, arrayIndex;
int arraySize;

int lineno = 1;

int dataType; // compiler.h TYPE_*

void defineVar(const string &id);
void defineReserved(TokenData token);
void defineArray(const string &id, int pointer);
void assignVar(TokenData source, TokenData dest);
void assignArray(TokenData source, TokenData dest, TokenData destIndex);
void getArray(TokenData source, TokenData sourceIndex, TokenData dest);
void eqaltion(TokenData op1, TokenData op2, TokenData dest);
bool isArray(ArrayToken token);
void defineLabel(const string &id);

#define must_defined(str) {\
  if (!isIDDefined(str, get_scope())) {\
    yyerror("undefined identifier"); \
    fprintf(yyout,"undefined: '%s'\n", str);\
    YYABORT;\
  }\
}

#define must_undefined(str) {\
  if (isIDDefined(str, get_scope())) {\
  yyerror("duplicate identifier"); \
  fprintf(yyout,"duplicate: '%s'\n", str);\
  YYABORT;\
  }\
}

%}

%code requires {
#include "token.h"
#include "stdbool.h"
}

%union {
  char *intStr;
  char *floatStr;
  char *idStr;
  char lastChar;
  int dataType;
  TokenData token;
  ArrayToken arrayToken;
}

%token BOOLEAN CALL DIMENSION ELSE PROGRAM ENP ENS GTO
%token IF INPUT INTEGER LABEL OUTPUT REAL
%token SUBROUTINE THEN VARIABLE

%left OR
%left AND
%left EQ NE GE GT LE LT

%token <intStr> UNSIGNED_INTEGER
%token <floatStr> UNSIGNED_REAL
%token <idStr> IDENTIFIER

%type <intStr> unsigned_integer
%type <idStr> identifier label parameter array array_identifier
%type <token> relational_operator adding_operator multiplying_operator relations variable_sub_list variable_sub
%type <arrayToken> variable
%type <token> expression simple_expression term sign factor unsigned_constant unsigned_number unsigned_real
%type <token> subroutine_identifier argument_list argument constant constant_identifier

%right '='
%left '+' '-'
%left '*' '/'
%right POW
%nonassoc ';'

%start program

%%
program:
       main_program subroutine_deck;

main_program:
            program_heading block ENP {
              defineReserved({RESERVED_WORD_TABLE, 6});
              pop_scope();
            } semicolon;
program_heading:
               PROGRAM identifier {must_undefined($2);}
               semicolon { push_scope(defineID($2, get_scope(), -1, -1).value);};

identifier: IDENTIFIER { $$ = $1; /*add_identifier($1);*/};

block: %empty | block_sub | block block_sub;

block_sub:
     array_declaration_part|
     variable_declaration_part|
     label_declaration_part|
     statement_part;

array_declaration_part: array_declaration_part_sub_list;

array_declaration_part_sub_list:
                      array_declaration_part_sub
                      |array_declaration_part_sub_list array_declaration_part_sub;

array_declaration_part_sub:
                          DIMENSION array_declaration semicolon;

array_declaration:
                 type ':' subscripted_variable_list;

subscripted_variable_list:
                         subscripted_variable |
                         subscripted_variable_list ',' subscripted_variable;

subscripted_variable:
                    identifier {must_undefined($1);} bracket_left {argRegister.clear();}
                    unsigned_integer_list
                    bracket_right {

                      defineArray($1, add_arrayInfo(dataType, argRegister));
                    };

unsigned_integer_list:
                     unsigned_integer {argRegister.push_back(atoi($1));}|
                     unsigned_integer_list ',' unsigned_integer{argRegister.push_back(atoi($3));};
unsigned_integer:
                UNSIGNED_INTEGER{ $$ = $1; add_integer($1);}  ;

type:INTEGER {dataType = TYPE_INTEGER;}
    |REAL {dataType = TYPE_REAL;}
    |BOOLEAN {dataType = TYPE_BOOLEAN;};

variable_declaration_part:
                           variable_declaration_part_sub_list ;

variable_declaration_part_sub_list:
                                  variable_declaration_part_sub |
                                  variable_declaration_part_sub_list variable_declaration_part_sub;
variable_declaration_part_sub:
                             VARIABLE variable_declaration semicolon;

variable_declaration: type ':' identifier_list;
identifier_list: identifier {
                 must_undefined($1);
                 defineVar($1);
               }
               | identifier_list ',' identifier {
                 must_undefined($3);
                 defineVar($3);
               };

label_declaration_part:
                      LABEL label_list semicolon;

label_list: label {
            must_undefined($1);
            defineLabel($1);
          } 
          | label_list ',' label {
            must_undefined($3);
            defineLabel($3);
          };

label: identifier {$$ = $1;};
statement_part: statement_list;
statement_list:
              statement |
              statement_list statement;

statement: unlabelled_statement semicolon |
         label unlabelled_statement semicolon;

unlabelled_statement:
                    statement_I |if_statement;

statement_I : empty_statement |
            assign_statement |
            call_statement |
            IO_statement|
            go_to_statement;

empty_statement: %empty ;
assign_statement: variable '=' expression {
                  if (isArray($1))
                    assignArray($3, $1.token, $1.index);
                  else if (!isArray($1))
                    assignVar($3, $1.token);
                };

variable: identifier {
          must_defined($1);
          $$ = {getID($1, get_scope()), {-1,-1}};
        }
        | identifier
        {
          infoTab = getInfoTable();
          arrayBegin = getPointer($1, get_scope());
          arrayBegin++; // type to dim
          arraySize = infoTab.at(arrayBegin);
          arrayBegin++; // dim to item1
          arrayIndex = 0;
        }
        bracket_left
        variable_sub_list
        bracket_right {
          must_defined($1);
          $$ = {getID($1, get_scope()), $4};
        };

variable_sub_list: variable_sub {
                    $$ = $1;
                    arrayIndex++;
                 }
                 | variable_sub_list ',' variable_sub
                 {
                    // variable_sub - 1
                    TokenData sub1 = getTemper();
                    addQForm({{DELIMITER_TABLE, 6}, $3 ,{INTEGER_TABLE, add_integer("1")}, sub1 });

                    // (variable_sub - 1) * variable_sub_list.size
                    TokenData mul1 = getTemper();

                    if (arrayIndex >= arraySize) {
                      stringstream ss;
                      ss << "array dim error size is " << arraySize;
                      yyerror(ss.str().c_str());
                      YYABORT;
                    }

                    string sizeStr = to_string(infoTab.at(arrayBegin + arrayIndex - 1));
                    arrayIndex++;

                    addQForm({{DELIMITER_TABLE, 7}, sub1 ,{INTEGER_TABLE, add_integer(sizeStr)}, mul1 });

                    // (variable_sub - 1) * variable_sub_list.size + variable_sub_list
                    $$ = getTemper();
                    addQForm({{DELIMITER_TABLE, 5}, $1, mul1, $$});
                 };

variable_sub: unsigned_integer {
              $$ = {INTEGER_TABLE, add_integer($1)};
            }
            | identifier {
              must_defined($1);
              $$ = getID($1, get_scope());
            };

expression: simple_expression {$$ = $1;}|
          simple_expression relational_operator simple_expression {
            $$ = getTemper();
            addQForm({$2, $1, $3, $$});
          };

relational_operator:EQ {$$ = {RESERVED_WORD_TABLE, 8};}
                   |NE {$$ = {RESERVED_WORD_TABLE, 18};}
                   |GT {$$ = {RESERVED_WORD_TABLE, 10};}
                   |GE {$$ = {RESERVED_WORD_TABLE, 9};}
                   |LT {$$ = {RESERVED_WORD_TABLE, 17};}
                   |LE {$$ = {RESERVED_WORD_TABLE, 16};};

simple_expression: term {$$ = $1;} |
                 sign term {
                  $$ = getTemper();
                  addQForm({$1, {INTEGER_TABLE, add_integer("0")}, $2, $$ });}
                  |simple_expression adding_operator term {
                  $$ = getTemper();
                  addQForm({$2, $1, $3, $$});
                 };

adding_operator:'+' {$$ = {DELIMITER_TABLE, 5};}
               |'-' {$$ = {DELIMITER_TABLE, 6};}
               |OR {$$ = {RESERVED_WORD_TABLE, 19};};

term: factor {$$ = $1;}
    | term multiplying_operator factor {
                  $$ = getTemper();
                  addQForm({$2, $1, $3, $$});
                 }
    | factor POW term {
                  $$ = getTemper();
                  addQForm({{DELIMITER_TABLE, 9}, $1, $3, $$});
                 }

multiplying_operator:'*' {$$ = {DELIMITER_TABLE, 7};}
                    |'/' {$$ = {DELIMITER_TABLE, 8};}
                    |AND {$$ = {RESERVED_WORD_TABLE, 1};}

factor: variable {
                  if (isArray($1)) {
                   $$ = getTemper();
                   getArray($1.token, $1.index, $$);
                  } else if (!isArray($1))
                    $$ = $1.token;
      }
      | unsigned_constant {$$ = $1;}
      | bracket_left expression bracket_right {$$ = $2;};

unsigned_constant: unsigned_number {$$ = $1;}
                 | constant_identifier {$$ = $1;};

unsigned_number: unsigned_integer {$$ = {INTEGER_TABLE, add_integer($1)};}
               | unsigned_real {$$ = $1;};

unsigned_real: UNSIGNED_REAL {$$ = {REAL_NUMBER_TABLE, add_real($1)};};

sign: '+' {$$ = {DELIMITER_TABLE, 5};}
    |'-' {$$ = {DELIMITER_TABLE, 6};};

constant_identifier: identifier {
                     must_defined($1);
                     $$ = getID($1, get_scope());
                   };

call_statement: CALL subroutine_identifier bracket_left {argRegister.clear();}
              argument_list bracket_right {
                  addQForm({{RESERVED_WORD_TABLE, 3}, $2, NULL_TOKEN, {INFORMATION_TABLE, add_callInfo(argRegister)}});
              };

argument_list:
             argument {
               argRegister.push_back($1.type);
               argRegister.push_back($1.value);
             }
             |argument_list ',' argument {
               argRegister.push_back($3.type);
               argRegister.push_back($3.value);
             };

subroutine_identifier:
                      identifier {
                        if (!isSubroutine($1, -1, lineno)) {
                          yyerror("undefine subroutine");
                          YYABORT;
                        }

                        $$ = getID($1, -1);
                      };
argument:
        identifier {
          must_defined($1);
          printf("check bug identifer?\n");
          $$ = getID($1, get_scope());
        } | constant { $$ = $1;};

constant:
        unsigned_constant {$$ = $1;}
        | sign unsigned_constant {
                  $$ = getTemper();
                  addQForm({$1, {INTEGER_TABLE, add_integer("0")}, $2, $$ });
        };


IO_statement: INPUT variable| OUTPUT variable;
number_size: unsigned_integer;
go_to_statement: GTO label;

if_statement: IF condition THEN statement_I
            | IF condition THEN statement_I ELSE statement_I;

condition:condition_variable relations condition_variable;

condition_variable:variable|constant;

relations:relational_operator {$$ = $1;}
         |OR {$$ = {RESERVED_WORD_TABLE, 19};}
         |AND {$$ = {RESERVED_WORD_TABLE, 1};};

subroutine_deck: %empty
               |subroutine_declaration_list;

subroutine_declaration_list:
                           subroutine_declaration|
                           subroutine_declaration_list subroutine_declaration;

subroutine_declaration: subroutine_heading block ENS {
                        defineReserved({RESERVED_WORD_TABLE, 7});
                        pop_scope();
                      } semicolon;

subroutine_heading: SUBROUTINE identifier {
                      if (!isSubroutine($2, -1, lineno)) {
                        yyerror("duplicate identifier or subroutine");
                        YYABORT;
                      } else
                        push_scope(defineSubroutine($2, get_scope(), getQFormSize()).value);
                    }
                  bracket_left parameter_group_list bracket_right
                  semicolon ;

parameter_group_list:
                    parameter_group |
                    parameter_group_list ',' parameter_group;

parameter_group: type ':' parameter_list;
parameter_list:
             parameter {
                 must_undefined($1);
                 defineVar($1);
               }|
             parameter_list ',' parameter{
                 must_undefined($3);
                 defineVar($3);
               };


semicolon: {check = check_semicolon;} ';' {check = check_disable;};

parameter: identifier {$$ = $1;}| array {$$ = $1;};

array: array_identifier bracket_left bracket_right {$$ = $1;} ;

array_identifier: identifier {must_undefined($1); $$ = $1;} ;

bracket_left: '(' {check = check_brackets;};
bracket_right: ')' {check = check_disable;};
%%

using namespace std;

FILE *fptr;

void defineVar(const string &id) {
  addQForm({
    defineID(id, get_scope(), dataType, -1),
    NULL_TOKEN,
    NULL_TOKEN,
    NULL_TOKEN
   });
}

void defineReserved(TokenData token) {
  addQForm({
    token,
    NULL_TOKEN,
    NULL_TOKEN,
    NULL_TOKEN
   });
}

void defineArray(const string &id, int pointer) {
  addQForm({
    defineID(id, get_scope(), TYPE_ARRAY, pointer),
    NULL_TOKEN,
    NULL_TOKEN,
    NULL_TOKEN
   });
}

void defineLabel(const string &id) {
  addQForm({
    defineID(id, get_scope(), TYPE_LABEL, -1),
    NULL_TOKEN,
    NULL_TOKEN,
    NULL_TOKEN
   });
}

void assignVar(TokenData source, TokenData dest) {
  addQForm({
    {DELIMITER_TABLE, 4},
    source,
    NULL_TOKEN,
    dest 
   });
}

void assignArray(TokenData source, TokenData dest, TokenData destIndex) {
  addQForm({
    {DELIMITER_TABLE, 4},
    source,
    dest,
    destIndex
   });
}

void getArray(TokenData source, TokenData sourceIndex, TokenData dest) {
  addQForm({
    {DELIMITER_TABLE, 4},
    source,
    sourceIndex,
    dest
   });
}

bool isArray(ArrayToken token) {
  if (isTokenEqual(token.index, {-1, -1}))
    return false;
  else
    return true;
}

int init()
{
  check = check_disable;
  lineno = 1;
  yyrestart(fptr);
  resetCompiler();
}

int main ()
{
  string filename;

  while (cin >> filename) {
    cout << "read file: \"" << filename <<
    "\"" << endl;

    fptr = fopen(filename.c_str() ,"r");
    //fptr = fopen("testfile/e1.txt" ,"r");
    //fptr = fopen("e2.txt" ,"r");
    //fptr = fopen("testfile/input.txt" ,"r");
    //fptr = fopen("tet.txt" ,"r");
    if (!fptr) {
      printf("file read error\n");
      continue;
    }

    init();
    yyin = fptr;

    if(yyparse() == 0) {
      printQForm(yyout);
      int undefine_line = hasSubRoutineUndefine();
      if (undefine_line != -1)
        fprintf(yyout, "Subroutine undefine: line %d\n", undefine_line);
    }

    fclose(yyin);
  }

  return 0;
}

void yyerror(char const *s) {
  //printf("syntax error line %d: '%s'\n", lineno, s);
  printQForm(yyout);
  switch (check) {
    case check_semicolon:
      fprintf(yyout, "%s line %d: ';' not found?\n",s ,lineno - 1);
      fprintf(yyout, "(or line %d has some problem)\n", lineno);
      break;
    case check_brackets:
      fprintf(yyout, "%s line %d: ')' not found?\n",s ,lineno);
      break;
    default:
      fprintf(yyout, "%s line %d\n", s, lineno);
  }
}
