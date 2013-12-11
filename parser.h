/*******************************************************************************************
File Name: parser.h
Compiler: MS Visual Studio 2010
Author: Kyle Hinskens (040747198), Kwok Hong Kelvin Chan (040742238)
Course: CST 8152 - Compilers, Lab Section: 401
Assignment: 4
Date: 6 December 2013
Purpose: Include all the necessary components and function declarations necessary for the 
		parser implementation.
*********************************************************************************************/

#ifndef PARSER_H_
#define PARSER_H_

/* Define keyword indices from kw_table */
#define ELSE 0
#define IF 1
#define INPUT 2
#define OUTPUT 3
#define PLATYPUS 4
#define REPEAT 5
#define THEN 6
#define USING 7
#define NO_ATTR 8


/* Define two static global variables: lookahead_token of type Token, and sc_buf of type pointer to Buffer. */
static Token lookahead_token;
static Buffer *sc_buf;
int synerrno; /* Additionally define a global variable synerrno of type int. */

extern int line; /* source code line number - defined in scanner.c */
extern STD sym_table; /* symbol table variable - defined in stable.c*/
extern Buffer *str_LTBL; /* string literal buffer - in platy.c */
extern Token mlwpar_next_token(Buffer *); /* get next token - defined in scanner.c */
extern char *kw_table[]; /* keyword table - from scanner.c */


/* Function prototypes */
void parser(Buffer *);
void match(int, int);
void program(void);
void gen_incode(char *);
void syn_eh(int);
void syn_printe(void);
void opt_statements(void);
void statements(void);
void statements_p(void);
void statement(void);
void assignment_statement(void);
void assignment_expression(void);
void selection_statement(void);
void iteration_statement(void);
void input_statement(void);
void variable_identifier(void);
void variable_list(void);
void variable_list_p(void);
void output_statement(void);
void output_list(void);
void arithmetic_expression(void);
void unary__arithmetic_expression(void);
void additive_arithmetic_expression(void);
void additive_arithmetic_expression_p(void);
void multiplicative_arithmetic_expression(void);
void multiplicative_arithmetic_expression_p(void);
void primary_arithmetic_expression(void);
void string_expression(void);
void string_expression_p(void);
void primary_string_expression(void);
void conditional_expression(void);
void logical_OR_expression(void);
void logical_OR_expression_p(void);
void logical_AND_expression(void);
void logical_AND_expression_p(void);
void relational_expression(void);
void relational_operator(void);
void p_operator(void);
void primary_a_relational_expression(void);
void primary_s_relational_expression(void);

#endif
