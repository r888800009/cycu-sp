%{
#include <stdio.h>
%}


%token BOOLEAN CALL DIMENSION ELSE PROGRAM ENP ENS GTO
%token IF INPUT INTEGER LABEL OUTPUT REAL
%token SUBROUTINE THEN VARIABLE

%left OR
%left AND
%left EQ NE GE GT LE LT

%token UNSIGNED_INTEGER IDENTIFIER
%left '+' '-'
%left '*' '/'
%nonassoc ';'

%start program

%%
program:
       main_program subroutine_deck;

main_program:
            program_heading block ENP ';';

program_heading:
               PROGRAM identifier ';' {};

identifier: IDENTIFIER;

block: block_sub | block block_sub;

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
                          DIMENSION array_declaration ';';

array_declaration:
                 type ':' subscripted_variable_list;

subscripted_variable_list:
                         subscripted_variable |
                         subscripted_variable_list ',' subscripted_variable;

subscripted_variable:
                    identifier '(' unsigned_integer_list ')';

unsigned_integer_list:
                     unsigned_integer |
                     unsigned_integer_list ',' unsigned_integer;

unsigned_integer:
                UNSIGNED_INTEGER ;

type:
    INTEGER|REAL|BOOLEAN;

variable_declaration_part:
                           variable_declaration_part_sub_list ;

variable_declaration_part_sub_list:
                                  variable_declaration_part_sub |
                                  variable_declaration_part_sub_list variable_declaration_part_sub;
variable_declaration_part_sub:
                             VARIABLE variable_declaration ';';

variable_declaration: type ':' identifier_list;
identifier_list: identifier | identifier_list ',' identifier;

label_declaration_part:
                      LABEL label_list ';';

label_list: label | label_list ',' label;

label: identifier;
statement_part: statement_list;
statement_list:
              statement |
              statement_list statement;

statement: unlabelled_statement ';' |
         label unlabelled_statement';';

unlabelled_statement:
                    statement_I |if_statement;

statement_I : empty_statement |
            assign_statement |
            call_statement |
            IO_statement|
            go_to_statement;

empty_statement: %empty ;
assign_statement: variable '=' expression;


variable: identifier
        | identifier '(' variable_sub_list ')';

variable_sub_list: variable_sub | variable_sub_list variable_sub;

variable_sub: unsigned_integer | identifier;

expression: simple_expression |
          simple_expression relational_operator simple_expression;

relational_operator: EQ|NE|GT|GE|LT|LE;

simple_expression: term |
                 sign term|
                simple_expression adding_operator term;

adding_operator: '+'|'-'|OR;

term: factor |
    term multiplying_operator factor;
multiplying_operator:'*'|'/'|AND|'↑';
factor: variable | unsigned_constant |'(' expression ')';

unsigned_constant: unsigned_number | constant_identifier;
unsigned_number: unsigned_integer | unsigned_real;
unsigned_real:
             unsigned_integer '.'|
             unsigned_integer '.' unsigned_integer|
             '.' unsigned_integer;

sign: '+'|'-';

constant_identifier: identifier;

call_statement: CALL subroutine_identifier '(' argument_list ')';

argument_list:
             argument |
             argument_list ',' argument;

subroutine_identifier:
                     identifier;
argument:
        identifier|constant;

constant:
        unsigned_constant|sign unsigned_constant;
IO_statement: INPUT variable| OUTPUT variable;
number_size: unsigned_integer;
go_to_statement: GTO label;

if_statement: IF condition THEN statement_I
            | IF condition THEN statement_I ELSE statement_I;

condition:condition_variable relations condition_variable;

condition_variable:variable|constant;

relations:relational_operator|OR|AND;

subroutine_deck: %empty|subroutine_declaration_list;

subroutine_declaration_list:
                           subroutine_declaration|
                           subroutine_declaration_list subroutine_declaration;

subroutine_declaration: subroutine_heading block ENS;

subroutine_heading: SUBROUTINE identifier '(' parameter_group_list ')' ';';

parameter_group_list:
                    parameter_group |
                    parameter_group_list ',' parameter_group;

parameter_group: type ':' parameter_list;
parameter_list:
             parameter|
             parameter_list ',' parameter;

parameter: identifier | array;
array: array_identifier'(' ')';
array_identifier: identifier;
%%

int lineno = 1;
FILE *fptr;
extern FILE *yyin;

int main ()
{
  fptr = fopen("testfile/e1.txt" ,"r");
  if (!fptr) {
    printf("file read error\n");
    return -1;
  }

  yyin = fptr;

	do {
		yyparse();
	} while (!feof(yyin));

  return 0;
}

int yyerror(FILE* fp, char *s) {
  //printf("syntax error line %d: '%s'\n", lineno, s);
  printf("syntax error line %d\n", lineno);
}
