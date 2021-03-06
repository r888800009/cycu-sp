%{
#include <cstdbool>
#include <iostream>
#include "compiler.h"
#include "quadruple.h"
#include "identifier_table.h"
#include <vector>
#include <sstream>
#include <queue>
#include <map>

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

enum Check {check_disable, check_semicolon, check_brackets ,check_ens, check_enp} check;
enum CheckBlock {block_ens, block_enp, block_null} block_flag;
extern FILE *yyin, *yyout;

vector<int> argRegister;
vector<int> infoTab;
int arrayBegin, arrayIndex;
int arraySize;

int lineno = 1;
int bracketDeep = 0;

int delayTmpRegIndex;
int delayTmpIndex;
map<int, TokenData> delayTmpMap;

int hashtable_size = 100;

typedef struct DelayQForm {
  QuadrupleForm qform;
  int bracketDeep;
  int index;
  bool operator<(const DelayQForm& dqform) const
  {
    if (bracketDeep == dqform.bracketDeep)
      return index > dqform.index;

    return bracketDeep < dqform.bracketDeep;
  }

  bool operator==(const DelayQForm& dqform) const
  {
    return bracketDeep == dqform.bracketDeep;
  }
} DelayQForm;

priority_queue<DelayQForm> delayQFormQueue;

void push_bracket();
TokenData pop_bracket();
void addDelayQForm(QuadrupleForm qform);
TokenData getDelayReg();
bool isDelayRegToken(TokenData token);


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

bool enableErrorMode = false;
#define GTO_TABLE_VALUE 11
#define must_defined(str) {\
  if (!isIDDefined(str, get_scope())) {\
    yyerror("undefined identifier"); \
    fprintf(yyout,"undefined: '%s'\n", str);\
    YYABORT;\
  }\
}

#define must_undefined(str) {\
  if (enableErrorMode && isIDDefined(str, get_scope())) {\
  yyerror("duplicate identifier"); \
  fprintf(yyout,"duplicate: '%s'\n", str);\
  YYABORT;\
  }\
}

int ifBeginIndex, ifTrueBranchIndex;
void putIfBeginCode(TokenData condition);
void putIfTrueEndCode();
void putIfFalseCode();

int semicolonIndex;


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

%token SUBROUTINE THEN VARIABLE
%token BOOLEAN CALL DIMENSION ELSE PROGRAM ENP ENS GTO
%token IF INPUT INTEGER LABEL OUTPUT REAL


%token <intStr> UNSIGNED_INTEGER
%token <floatStr> UNSIGNED_REAL
%token <idStr> IDENTIFIER

%type <intStr> unsigned_integer
%type <idStr> identifier label parameter array array_identifier
%type <token> relational_operator adding_operator variable_sub_list variable_sub
%type <arrayToken> variable
%type <token> expression simple_expression term sign factor unsigned_constant unsigned_number unsigned_real
%type <token> subroutine_identifier argument_list argument constant constant_identifier condition condition_variable

%nonassoc ';'

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%left OR
%left AND
%left EQ NE GE GT LE LT

%right '='
%left '+' '-'
%left '*' '/'
%right POW
%left '(' ')'

%start program

%%
program:
       main_program subroutine_deck;

main_program:
            program_heading block {
              defineReserved({RESERVED_WORD_TABLE, 6});
              pop_scope();
            };

end_statement: {check = check_enp; block_flag = block_enp;} ENP {
              if (block_flag != block_enp){
                if (block_flag == block_ens)
                  yyerror("Do you mean ENP?");
                else 
                  yyerror("End error!");
                  YYABORT;
              }
              check = check_disable;
             }
             | {check = check_ens; block_flag = block_ens;} ENS {
              if (block_flag != block_ens){
                if (block_flag == block_enp)
                  yyerror("Do you mean ENS?");
                else 
                  yyerror("End error!");
                YYABORT;
              }
              check = check_disable;
              defineReserved({RESERVED_WORD_TABLE, 7});
              pop_scope();
             };

program_heading:
               PROGRAM identifier {must_undefined($2);}
               semicolon { push_scope(defineID($2, -1, -1, -1).value);};

identifier: IDENTIFIER { $$ = $1; /*add_identifier($1);*/};

block: %empty | block_sub | block block_sub;

block_sub:
     array_declaration_part|
     variable_declaration_part|
     label_declaration_part|
     statement;

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

statement: unlabelled_statement semicolon |
         label { 
                if(!defineLabelStatement($1, get_scope(), getQFormNext())) {
                    yyerror("define label statement error");
                    YYABORT;
                  }
            }
         unlabelled_statement semicolon;

unlabelled_statement:
                    statement_I |if_statement | end_statement;

statement_I : empty_statement |
            assign_statement |
            call_statement |
            IO_statement|
            go_to_statement ;

empty_statement: %empty ;
assign_statement: variable '=' {push_bracket();} expression {
                  pop_bracket();

                  if (isArray($1))
                    assignArray($4, $1.token, $1.index);
                  else if (!isArray($1))
                    assignVar($4, $1.token);
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

                    if (arrayIndex >= arraySize) {
                      stringstream ss;
                      ss << "array dim error size is " << arraySize;
                      yyerror(ss.str().c_str());
                      YYABORT;
                    }

                    TokenData mul1;
                    int index = arrayIndex;
                    while (index > 0) {
                      mul1 = getTemper();
                      string sizeStr = to_string(infoTab.at(arrayBegin + index - 1));
                      addQForm({{DELIMITER_TABLE, 7}, sub1 ,{INTEGER_TABLE, add_integer(sizeStr)}, mul1 });
                      sub1 = mul1;
                      index--;
                    }

                    arrayIndex++;

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
            $$ = getDelayReg();
            addDelayQForm({$2, $1, $3, $$});
          };

relational_operator:EQ {$$ = {RESERVED_WORD_TABLE, 8};}
                   |NE {$$ = {RESERVED_WORD_TABLE, 18};}
                   |GT {$$ = {RESERVED_WORD_TABLE, 10};}
                   |GE {$$ = {RESERVED_WORD_TABLE, 9};}
                   |LT {$$ = {RESERVED_WORD_TABLE, 17};}
                   |LE {$$ = {RESERVED_WORD_TABLE, 16};};

simple_expression: term {$$ = $1;} |
                 sign term {
                  $$ = getDelayReg();
                  addDelayQForm({$1, {INTEGER_TABLE, add_integer("0")}, $2, $$ });}
                  |simple_expression adding_operator term {
                  $$ = getDelayReg();
                  addDelayQForm({$2, $1, $3, $$});
                 };

adding_operator:'+' {$$ = {DELIMITER_TABLE, 5};}
               |'-' {$$ = {DELIMITER_TABLE, 6};}
               |OR {$$ = {RESERVED_WORD_TABLE, 19};};

term: factor {$$ = $1;}
    | term '*' term { $$ = getDelayReg(); addDelayQForm({{DELIMITER_TABLE, 7}, $1, $3, $$});}
    | term '/' term { $$ = getDelayReg(); addDelayQForm({{DELIMITER_TABLE, 8}, $1, $3, $$});}
    | term AND term { $$ = getDelayReg(); addDelayQForm({{DELIMITER_TABLE, 1}, $1, $3, $$});}
    | term POW term {
                  $$ = getDelayReg();
                  addDelayQForm({{DELIMITER_TABLE, 9}, $1, $3, $$});
                 }

factor: variable {
                  if (isArray($1)) {
                   $$ = getDelayReg();
                   getArray($1.token, $1.index, $$);
                  } else if (!isArray($1))
                    $$ = $1.token;
      }
      | unsigned_constant {$$ = $1;}
      | bracket_left {push_bracket();} expression bracket_right {$$ = $3; pop_bracket();};

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


IO_statement: INPUT variable {
              TokenData t;
              pop_bracket();
              if (isArray($2)) {
                t = getTemper();
                 getArray($2.token, $2.index, t);
              } else if (!isArray($2))
                t = $2.token;
              pop_bracket();

              addQForm({{RESERVED_WORD_TABLE, 13}, NULL_TOKEN, NULL_TOKEN, t });
            }
            | OUTPUT variable {
              TokenData t;

              pop_bracket();
              if (isArray($2)) {
                t = getTemper();
                 getArray($2.token, $2.index, t);
              } else if (!isArray($2))
                t = $2.token;
              pop_bracket();

              addQForm({{RESERVED_WORD_TABLE, 20}, NULL_TOKEN, NULL_TOKEN, t });
            };

go_to_statement: GTO label {
                 int pointer = addQForm({{RESERVED_WORD_TABLE, GTO_TABLE_VALUE}, NULL_TOKEN, NULL_TOKEN, NULL_TOKEN});
                 if(!referenceLabel($2, get_scope(),  pointer)) {
                   yyerror("reference label error");
                   YYABORT;
                 }

                 cout << "GTO not work!" << endl;
               };

if_statement: IF condition THEN {putIfBeginCode($2);} statement_I if_else;

if_else: ELSE {putIfTrueEndCode();} statement_I {putIfFalseCode();}
       | %empty {putIfTrueEndCode(); putIfFalseCode();};

condition:condition_variable EQ condition_variable {$$ = getTemper(); addQForm({{RESERVED_WORD_TABLE, 8}, $1, $3, $$});};
         |condition_variable NE condition_variable {$$ = getTemper(); addQForm({{RESERVED_WORD_TABLE, 18}, $1, $3, $$});};
         |condition_variable GT condition_variable {$$ = getTemper(); addQForm({{RESERVED_WORD_TABLE, 10}, $1, $3, $$});};
         |condition_variable GE condition_variable {$$ = getTemper(); addQForm({{RESERVED_WORD_TABLE, 9}, $1, $3, $$});};
         |condition_variable LT condition_variable {$$ = getTemper(); addQForm({{RESERVED_WORD_TABLE, 17}, $1, $3, $$});};
         |condition_variable LE condition_variable {$$ = getTemper(); addQForm({{RESERVED_WORD_TABLE, 16}, $1, $3, $$});};
         |condition_variable OR condition_variable {$$ = getTemper(); addQForm({{RESERVED_WORD_TABLE, 19}, $1, $3, $$});};
         |condition_variable AND condition_variable {$$ = getTemper(); addQForm({{RESERVED_WORD_TABLE, 1}, $1, $3, $$});};

condition_variable:variable {
                  if (isArray($1)) {
                   $$ = getTemper();
                   getArray($1.token, $1.index, $$);
                  } else if (!isArray($1))
                    $$ = $1.token;
                  }
                  |constant { $$ = $1;}
                  |condition {$$ = $1;};

subroutine_deck: %empty
               |subroutine_declaration_list;

subroutine_declaration_list:
                           subroutine_declaration|
                           subroutine_declaration_list subroutine_declaration;
subroutine_declaration: subroutine_heading block ;

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


semicolon: {check = check_semicolon; semicolonIndex = lineno;} ';' {check = check_disable;};

parameter: identifier {$$ = $1;}| array {$$ = $1;};

array: array_identifier bracket_left bracket_right {$$ = $1;} ;

array_identifier: identifier {must_undefined($1); $$ = $1;} ;

bracket_left: '(' {check = check_brackets;};
bracket_right: ')' {check = check_disable;};
%%

using namespace std;

FILE *fptr;
QuadrupleForm checkQForm(QuadrupleForm data);
void defineVar(const string &id) {
  addQForm({
    defineID(id, get_scope(), dataType, -1),
    NULL_TOKEN,
    NULL_TOKEN,
    NULL_TOKEN
   });
}

bool hasIfGTO;
void putIfBeginCode(TokenData condition){
  ifBeginIndex = addQForm({
    {RESERVED_WORD_TABLE, 12},
    condition,
    NULL_TOKEN,
    NULL_TOKEN
  });

  modifyQFormOP2(ifBeginIndex, {QUADRUPLE_TABLE, ifBeginIndex + 2}); // start from 0, index + 1, then next = index +2
}

void putIfTrueEndCode(){
  if (!isTopGoto()) {
    ifTrueBranchIndex = addQForm({{RESERVED_WORD_TABLE, GTO_TABLE_VALUE}, NULL_TOKEN, NULL_TOKEN, NULL_TOKEN});
    hasIfGTO = true;
    modifyQFormResult(ifBeginIndex, {QUADRUPLE_TABLE, ifTrueBranchIndex + 2});
  } else {
    ifTrueBranchIndex = getQFormSize();
    hasIfGTO = false;
    modifyQFormResult(ifBeginIndex, {QUADRUPLE_TABLE, ifTrueBranchIndex + 1});
  }
}

void putIfFalseCode(){
  if (hasIfGTO)
    modifyQFormResult(ifTrueBranchIndex, {QUADRUPLE_TABLE, getQFormNext()});
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
  addQForm(checkQForm({
    {DELIMITER_TABLE, 4},
    source,
    NULL_TOKEN,
    dest 
   }));
}

QuadrupleForm checkQForm(QuadrupleForm data) {
  // check op1
  if (isDelayRegToken(data.opd1)) {
    auto it = delayTmpMap.find(data.opd1.value);
    if (it == delayTmpMap.end())
      cout << "Error! opd1 not set"  <<endl;
    else {
      data.opd1 = it->second;
    }
  }

  // check op2
  if (isDelayRegToken(data.opd2)) {
    auto it = delayTmpMap.find(data.opd2.value);
    if (it == delayTmpMap.end())
      cout << "Error! opd2 not set"  <<endl;
    else {
      data.opd2 = it->second;
    }
  }

  // gen result op
  if (isDelayRegToken(data.result)) {
      TokenData reg = getTemper();
      delayTmpMap[data.result.value] = reg;
      data.result = reg;
  }

  return data;
}

void assignArray(TokenData source, TokenData dest, TokenData destIndex) {
  addQForm(checkQForm({
    {DELIMITER_TABLE, 4},
    source,
    dest,
    destIndex
   }));
}

void getArray(TokenData source, TokenData sourceIndex, TokenData dest) {
  addQForm(checkQForm({
    {DELIMITER_TABLE, 4},
    source,
    sourceIndex,
    dest
   }));
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
  bracketDeep = 0;
  delayTmpRegIndex = 0;
  delayTmpIndex = 0;
  yyrestart(fptr);
  resetCompiler();
  delayQFormQueue = priority_queue<DelayQForm> ();
  block_flag = block_null;
}

void addDelayQForm(QuadrupleForm qform) {
  delayQFormQueue.push({qform, bracketDeep, delayTmpIndex++});
}

void push_bracket() {
  bracketDeep++;
  delayTmpIndex = 0;
}

bool isDelayRegToken(TokenData token){
  return token.type == DELAY_TMPER_REGISTER;
}

TokenData pop_bracket() {
  TokenData lastToken;
  QForm data;
  while (!delayQFormQueue.empty() && delayQFormQueue.top().bracketDeep == bracketDeep) {
    data = delayQFormQueue.top().qform;
    data = checkQForm(data);

    // gen code
    lastToken = data.result;
    addQForm(data);
    delayQFormQueue.pop();
  }
  if (bracketDeep > 1)
    bracketDeep--;
  return lastToken;
}

TokenData getDelayReg() {
  return {DELAY_TMPER_REGISTER, delayTmpRegIndex++};
}

int main ()
{
#ifdef TTEESSTT
  qformTest();
#endif
  string filename, writeName;
  cout << "use \"set_mode1\" or \"set_mode2\" use different hash" << endl;
  cout << "use \"error_mode\" check error" << endl;
  cout << "input filename:" ;
  enableErrorMode = false;
  while (cin >> filename) {
    if (filename == "set_mode1") {
      cout << "use mode1" << endl;
      hashtable_size = 100;
      enableNumHash(true);
      continue;
    } else if (filename == "set_mode2") {
      cout << "use mode2" << endl;
      hashtable_size = 64;
      enableNumHash(false);
      continue;
    }

    if (filename == "error_mode") {
      enableErrorMode = true;
    }

    cout << "read file: \"" << filename <<
    "\"" << endl;
    writeName = filename + ".output";
    cout << "write file\"" << writeName <<
    "\"" << endl;

    fptr = fopen(filename.c_str() ,"r");
    FILE* outpt =  fopen(writeName.c_str() ,"w");
    if (!fptr) {
      printf("file read error\n");
      cout << "input filename:" ;
      continue;
    }

    if (!outpt) {
      printf("write file error\n");
      cout << "input filename:" ;
      continue;
    }

    init();
    yyin = fptr;
    yyout = outpt;

    // lex first
    while (yylex())
      ;
    yyrestart(fptr);
    lineno = 1;
    // reopen
    fclose(yyin);
    fptr = fopen(filename.c_str() ,"r");
    yyin = fptr;

    if(yyparse() == 0) {
      printQForm(yyout);
      int undefine_line = hasSubRoutineUndefine();
      if (undefine_line != -1)
        fprintf(yyout, "Subroutine undefine: line %d\n", undefine_line);
    }

    fclose(yyin);
    fclose(outpt);

    // print outfile
    fptr = fopen(writeName.c_str() ,"r");
    char buf[100];
    while (fgets(buf, sizeof(buf), fptr) != NULL) {
        cout << buf;
    }
    //printIntTableMode2() ;

    fclose(fptr);
    cout << "input filename:" ;
  }

  return 0;
}

void yyerror(char const *s) {
  printQForm(yyout);
  //fprintf(yyout, "syntax error line %d: '%s'\n", lineno, yylval.intStr); // debug line
  switch (check) {
    case check_semicolon:
      fprintf(yyout, "%s line %d: ';' not found?\n", s, semicolonIndex);
      fprintf(yyout, "(or line %d has some problem)\n", lineno);
      break;
    case check_brackets:
      fprintf(yyout, "%s line %d: ')' not found?\n",s ,lineno);
      break;
    default:
      fprintf(yyout, "%s line %d\n", s, lineno);
  }
}
