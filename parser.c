/*******************************************************************************************
File Name: parser.c
Compiler: MS Visual Studio 2010
Author: Kyle Hinskens (040747198), Kwok Hong Kelvin Chan (040742238)
Course: CST 8152 - Compilers, Lab Section: 401
Assignment: 4
Date: 6 December 2013
Purpose: Implementing a recursive descent predictive parser for the PLATYPUS language.
*********************************************************************************************/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland 5.02 projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>	/* standard input / output */
#include <stdlib.h> /* standard library functions and constants */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "stable.h"
#include "parser.h"


/* Provided by S.Ranev */
void parser(Buffer *in_buf) {
	sc_buf = in_buf;
	lookahead_token = mlwpar_next_token(sc_buf);
	program();
	match(SEOF_T, NO_ATTR);
	gen_incode("Source file parsed");
}


/*
Purpose: Matches two tokens: the current input token (lookahead_token) 
		and the token required by the parser. 
Author: Kyle Hinskens, Kwok Hong Kelvin Chan
Version: 13.12.6
Called functions: syn_eh, mlwpar_next_token
Parameters: 
			int pr_token_code The token code
			int pr_token_attribute The token attribute
Return value: None
*/
void match(int pr_token_code, int pr_token_attribute) {
	int matched = 0;

	/* The attribute code is used only when the token code is one of
		the following codes: KW_T, LOG_OP_T, ART_OP_T, REL_OP_T. In all other cases
		the token code is matched only. */
	if (lookahead_token.code == pr_token_code) {
		switch(pr_token_code) {
		case KW_T:
		case LOG_OP_T:
		case ART_OP_T:
		case REL_OP_T:
			if (lookahead_token.attribute.get_int == pr_token_attribute) matched = 1;
			break;
		default:
			matched = 1;
			break;
		}
	}

	/* If the match is successful and the lookahead_token is SEOF_T, the function returns. */
	if (matched) {
		if (lookahead_token.code == SEOF_T) return;

		/* If the match is successful and the lookahead_token is not SEOF_T, the function
			advances to the next input token */
		lookahead_token = mlwpar_next_token(sc_buf);

		/* If the new lookahead token is ERR_T, the function calls the error printing function */
		if (lookahead_token.code == ERR_T) {
			syn_printe();
			lookahead_token = mlwpar_next_token(sc_buf); /* Advance to the next input token */
			++synerrno;
			return;
		}
	}
	else {
		/* If the match is unsuccessful, the function calls the error handler and returns. */
		syn_eh(pr_token_code);
		return;
	}
}


/*
Purpose: Error handling function implementing simple panic mode.
Author: Kyle Hinskens, Kwok Hong Kelvin Chan
Version: 13.12.6
Called functions: syn_printe, mlwpar_next_token
Parameters: int sync_token_code The token code
Return value: None
*/
void syn_eh(int sync_token_code) {

	/* First, the function calls syn_printe() and increments the error counter. Then the
		function implements a panic mode error recovery: the function advances the input token
		(lookahead_token) until it finds a token code matching the one required by the parser
		(pr_token_code passed to the function as sync_token_code). */
	syn_printe();
	++synerrno;

	/* It is possible, when advancing, that the function can reach the end of the source file
		without finding the matching token. To prevent from overrunning the input buffer, before
		every move the function checks if the end of the file is reached. If the function looks for
		sync_token_code different from SEOF_T and reaches the end of the source file, the
		function calls exit(synerrno). */
	while (sync_token_code != lookahead_token.code) {
		if (sync_token_code != SEOF_T && lookahead_token.code == SEOF_T) {
			exit(synerrno);
		}

		lookahead_token = mlwpar_next_token(sc_buf);
	}

	/* If a matching token is found and the matching token is not SEOF_T, the function
		advances the input token one more time and returns. If a matching token is found and
		the matching token is SEOF_T, the function returns. */
	if (sync_token_code != SEOF_T) {	
		lookahead_token = mlwpar_next_token(sc_buf);
	}
}


/* Provided by S.Ranev */
void syn_printe(void) {
	Token t = lookahead_token;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch(t.code){
		case  ERR_T: /* ERR_T     0   Error token */
			printf("%s\n",t.attribute.err_lex);
		 break;
		case  SEOF_T: /*SEOF_T    1   Source end-of-file token */
			printf("NA\n" );
		 break;
		case  AVID_T: /* AVID_T    2   Arithmetic Variable identifier token */
		case  SVID_T :/* SVID_T    3  String Variable identifier token */
			printf("%s\n", sym_table.pstvr[t.attribute.get_int].plex);
		 break;
		case  FPL_T: /* FPL_T     4  Floating point literal token */
			printf("%5.1f\n",t.attribute.flt_value);
		 break;
		case INL_T: /* INL_T      5   Integer literal token */
				printf("%d\n",t.attribute.get_int);
		 break;
		case STR_T:/* STR_T     6   String literal token */
				printf("%s\n",b_get_chmemloc(str_LTBL,(short)t.attribute.get_int));
		break;
        
			case SCC_OP_T: /* 7   String concatenation operator token */
				printf("NA\n" );
		break;
	
		case  ASS_OP_T:/* ASS_OP_T  8   Assignment operator token */
			printf("NA\n" );
		break;
		case  ART_OP_T:/* ART_OP_T  9   Arithmetic operator token */
			printf("%d\n",t.attribute.get_int);
		break;
		case  REL_OP_T: /*REL_OP_T  10   Relational operator token */ 
			printf("%d\n",t.attribute.get_int);
		break;
		case  LOG_OP_T:/*LOG_OP_T 11  Logical operator token */
			printf("%d\n",t.attribute.get_int);
		break;
	
		case  LPR_T: /*LPR_T    12  Left parenthesis token */
			printf("NA\n" );
		break;
		case  RPR_T: /*RPR_T    13  Right parenthesis token */
				printf("NA\n" );
		break;
		case LBR_T: /*    14   Left brace token */
				printf("NA\n" );
		break;
		case RBR_T: /*    15  Right brace token */
				printf("NA\n" );
		break;
		
		case KW_T: /*     16   Keyword token */
				printf("%s\n",kw_table[t.attribute.get_int]);
		break;
	
		case COM_T: /* 17   Comma token */
				printf("NA\n");
		break;
		case EOS_T: /*    18  End of statement *(semi - colon) */
				printf("NA\n" );
		break; 		
		default:
				printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
		}/*end switch*/
}


/*
Purpose: The function takes a string as an argument and prints it
Author: Kyle Hinskens
Version: 13.12.6
Called functions: printf
Parameters: char *message The string to output
Return value: None
*/
void gen_incode(char* message) {
	printf("PLATY: %s\n", message);
}


/* FIRST(program)={PLATYPUS}
<program> ->
PLATYPUS {<opt_statements>}
Author: S.Ranev */
void program(void) {
	match(KW_T, PLATYPUS);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	gen_incode("Program parsed");
}


/* FIRST set: {AVID_T,SVID_T,KW_T (but not PLATYPUS, ELSE, THEN, REPEAT), AVID_T, SVID_T, and e} 
<opt_statements> -> statements()
Author: Kyle Hinskens */
void opt_statements(void){
	switch(lookahead_token.code){
	   case AVID_T:
	   case SVID_T: statements();break;
	   case KW_T:
	   /* check for PLATYPUS, ELSE, THEN, REPEAT here and in statements_p()*/
	if (lookahead_token.attribute.get_int != PLATYPUS
		&& lookahead_token.attribute.get_int != ELSE
		&& lookahead_token.attribute.get_int != THEN
		&& lookahead_token.attribute.get_int != REPEAT)
	{
	    statements();
		break; 
	}
	   	default: /*empty string – optional statements*/ ;
	    gen_incode("Opt_statements parsed");
	} 
}


/* FIRST(statements)={AVID,SVID,IF,USING,INPUT,OUTPUT}
<statements> -> 
	<statement><statements'>
Author: Kwok Hong Kelvin Chan */
void statements(void){
	switch(lookahead_token.code){
		case AVID_T:
		case SVID_T: statement(); statements_p(); break;
		case KW_T:
			if (lookahead_token.attribute.get_int == IF
				|| lookahead_token.attribute.get_int == USING
				|| lookahead_token.attribute.get_int == INPUT
				|| lookahead_token.attribute.get_int == OUTPUT) 
			{
				statement(); 
				statements_p();
			}	
			break;
		
		default:
			syn_printe();
			break;
	}	
}


/* FIRST(statements’)={AVID,SVID,IF,USING,INPUT,OUTPUT,e}
<statements’> -> 
	<statement><statements’>|e
Author: Kyle Hinskens */
void statements_p(void){
	switch(lookahead_token.code){
		case AVID_T:
		case SVID_T: statement(); statements_p(); break;
		case KW_T:
			if (lookahead_token.attribute.get_int == IF
				|| lookahead_token.attribute.get_int == USING
				|| lookahead_token.attribute.get_int == INPUT
				|| lookahead_token.attribute.get_int == OUTPUT) 
			{
				statement();
				statements_p();
			}	
			break;
 	}
}

/* FIRST(statement)={AVID,SVID,IF,USING,INPUT,OUTPUT}
<statement> ->
	<assignment statement>
	| <selection statement> 
	| <iteration statement> 
	| <input statement>
	| <output statement>
Author: Kwok Hong Kelvin Chan */
void statement(void){
	switch(lookahead_token.code){
		case AVID_T:
		case SVID_T:
			assignment_statement();
			break;
		case KW_T:
			switch(lookahead_token.attribute.get_int) {
			case IF:
				selection_statement();
				break;
			case USING:
				iteration_statement();
				break;
			case INPUT:
				input_statement();
				break;
			case OUTPUT:
				output_statement();
				break;
			default:
				syn_printe();
				break;
			}
			break;
		default:
			syn_printe();
			break;
	}
}


/* FIRST(assignment statement)={AVID ,SVID}
<assignment statement> -> 
	<assignment expression>; 
Author: Kyle Hinskens */
void assignment_statement(void){
	switch(lookahead_token.code){
		case AVID_T:
		case SVID_T:
			assignment_expression(); 
			match(EOS_T, NO_ATTR);
			gen_incode("Assignment statement parsed");
			break;
	}
}


/* FIRST(assignment expression)={AVID ,SVID}
<assignment expression> ->	
	AVID = <arithmetic expression>
	| SVID = <string expression>
Author: Kwok Hong Kelvin Chan */
void assignment_expression(void){
	switch(lookahead_token.code){
	case AVID_T:
		match(AVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		arithmetic_expression();
		gen_incode("Assignment expression (arithmetic) parsed");
		break;
	case SVID_T:
		match(SVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		string_expression();
		gen_incode("Assignment expression (string) parsed");
		break;
	default:
		syn_printe();
		break;
	}
}

/* FIRST(selection statement)={IF}
<selection statement> ->
	IF (<conditional expression>) THEN <opt_statements> 
	ELSE { <opt_statements> } ; 
Author: Kyle Hinskens */
void selection_statement(void){
	if (lookahead_token.attribute.get_int == IF){
		match(KW_T,IF);
		match(LPR_T,NO_ATTR);
		conditional_expression();
		match(RPR_T,NO_ATTR);
		match(KW_T,THEN);
		opt_statements();
		match(KW_T,ELSE);
		match(LBR_T,NO_ATTR);
		opt_statements();
		match(RBR_T,NO_ATTR);
		match(EOS_T,NO_ATTR);
		gen_incode("IF statement parsed");
	}
}

/* FIRST(iteration statement)={USING}
<iteration statement> ->
	USING (<assignment expression>, <conditional expression>, <assignment expression>) 
		REPEAT {
			< opt_statements>
		};
Author: Kwok Hong Kelvin Chan */
void iteration_statement(void) {
	if (lookahead_token.code == KW_T && lookahead_token.attribute.get_int == USING) {
		match(KW_T,USING);
		match(LPR_T,NO_ATTR);
		assignment_expression();
		match(COM_T, NO_ATTR);
		conditional_expression();
		match(COM_T, NO_ATTR);
		assignment_expression();
		match(RPR_T,NO_ATTR);
		match(KW_T,REPEAT);
		match(LBR_T,NO_ATTR);
		opt_statements();
		match(RBR_T,NO_ATTR);
		match(EOS_T,NO_ATTR);
		gen_incode("USING statement parsed");
	}
}


/* FIRST(input statement)={INPUT}
<input statement> ->
	INPUT(<variable list>);
Author: Kyle Hinskens */
void input_statement(void){
	match(KW_T,INPUT);
	match(LPR_T,NO_ATTR);
	variable_list();
	match(RPR_T,NO_ATTR);
	match(EOS_T,NO_ATTR);
	gen_incode("INPUT statement parsed");
}

/* FIRST(variable identifier)={AVID_T, SVID_T}
<variable identifier> -> AVID_T | SVID_T
Author: Kwok Hong Kelvin Chan */
void variable_identifier(void) {
	switch(lookahead_token.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR);
		break;
	case SVID_T:
		match(SVID_T, NO_ATTR);
		break;
	default:
		syn_printe();
		break;
	}
}

/* FIRST(variable list)={variable identifier}
<variable list> -> 
	<variable identifier><variable list’>
Author: Kyle Hinskens */
void variable_list(void) {
	variable_identifier();
	variable_list_p();
	gen_incode("Variable list parsed");
}

/* FIRST(variable list’)={,,e}
<variable list’> ->
	,<variable identifier> <variable list’>|e
Author: Kwok Hong Kelvin Chan */
void variable_list_p(void) {
	switch(lookahead_token.code) {
	case COM_T:
		match(COM_T, NO_ATTR);
		variable_identifier();
		variable_list_p();
		break;
	}
}


/* FIRST(output statement)={OUTPUT}
<output statement> ->
	OUTPUT(<output list>);
Author: Kyle Hinskens */
void output_statement(void) {
	if (lookahead_token.code == KW_T && lookahead_token.attribute.get_int == OUTPUT) {
		match(KW_T,OUTPUT);
		match(LPR_T,NO_ATTR);
		output_list();
		match(RPR_T,NO_ATTR);
		match(EOS_T,NO_ATTR);
		gen_incode("OUTPUT statement parsed"); 
	}
}


/* FIRST(output list)={variable identifier,STR_T,e}
<output list> -> 
	<variable list> | STR_T | e
Author: Kyle Hinskens */
void output_list(void) {
	switch(lookahead_token.code) {
	case STR_T:
		match(STR_T, NO_ATTR);
		gen_incode("Output list (string literal) parsed");
		break;
	case AVID_T:
	case SVID_T:
		variable_list();
		break;
	default:
		gen_incode("Output list (empty) parsed");
		break;
	}
}


/* FIRST(arithmetic expression)={-,+,AVID_T,FPL_T,INL_T,(}
<arithmetic expression> - >
	<unary arithmetic expression>
	| <additive arithmetic expression>
Author: Kwok Hong Kelvin Chan */
void arithmetic_expression(void){
	switch(lookahead_token.code){
	case AVID_T:
	case FPL_T:
	case INL_T:
	case LPR_T:
		additive_arithmetic_expression();
		gen_incode("Arithmetic expression parsed");
		break;
	case ART_OP_T:
		switch(lookahead_token.attribute.arr_op){
		case MINUS:
		case PLUS:	
			unary_arithmetic_expression();
			gen_incode("Arithmetic expression parsed");
			break;
		default:
			syn_printe();
			break;
		}
		break;
	default:
		syn_printe();
		break;
	}
}


/* FIRST(unary arithmetic expression)={-,+}
<unary arithmetic expression> ->
 - <primary arithmetic expression>
	| + <primary arithmetic expression>
Author: Kyle Hinskens */
void unary_arithmetic_expression(void){
	switch(lookahead_token.attribute.arr_op){
		case MINUS:
			match(ART_OP_T,MINUS);
			primary_arithmetic_expression();
			gen_incode("Unary arithmetic expression parsed");
			break;
		case PLUS:
			match(ART_OP_T,PLUS);
			primary_arithmetic_expression();
			gen_incode("Unary arithmetic expression parsed");
			break;
		default:
			syn_printe();
			break;
	}
}


/* FIRST(additive arithmetic expression)={AVID_T, FPL_T,INL_T ,(}
<additive arithmetic expression> ->  
	<multiplicative arithmetic expression> <additive arithmetic expression’>
Author: Kwok Hong Kelvin Chan */
void additive_arithmetic_expression(void){
	switch(lookahead_token.code){
		case AVID_T:
		case FPL_T:
		case INL_T:
		case LPR_T: 
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_p(); 
			break;
	}
}


/* FIRST(additive arithmetic expression’)={+,-, e}
<additive arithmetic expression’> ->
	+ <multiplicative arithmetic expression> <additive arithmetic expression’> 
	| - <multiplicative arithmetic expression> <additive arithmetic expression’> 
	| e
Author: Kyle Hinskens */
void additive_arithmetic_expression_p(void){
	switch(lookahead_token.code){
	case ART_OP_T:
		switch (lookahead_token.attribute.arr_op){
		case PLUS:
			match(ART_OP_T,PLUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_p();
			gen_incode("Additive arithmetic expression parsed");
			break;
		case MINUS:
			match(ART_OP_T,MINUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_p();
			gen_incode("Additive arithmetic expression parsed");
			break;
		}
		break;
	}
}


/* FIRST(multiplicative arithmetic expression)={AVID_T, FPL_T,INL_T ,(}
<multiplicative arithmetic expression> ->  
	<primary arithmetic expression> <multiplicative arithmetic expression’>
Author: Kwok Hong Kelvin Chan */
void multiplicative_arithmetic_expression(void){
	switch(lookahead_token.code){
		case AVID_T:
		case FPL_T:
		case INL_T:
		case LPR_T: 
			primary_arithmetic_expression(); 
			multiplicative_arithmetic_expression_p(); 
			break;
	}
}


/* FIRST(multiplicative arithmetic expression’)={*,/,e}
<multiplicative arithmetic expression’> ->
	* <primary arithmetic expression> <multiplicative arithmetic expression’> 
	| / <primary arithmetic expression> <multiplicative arithmetic expression’> 
	| e
Author: Kyle Hinskens */
void multiplicative_arithmetic_expression_p(void){
	switch(lookahead_token.code){
	case ART_OP_T:
		switch(lookahead_token.attribute.arr_op){
			case MULT:
				match(ART_OP_T,MULT);
				primary_arithmetic_expression();
				multiplicative_arithmetic_expression_p();
				gen_incode("Multiplicative arithmetic expression parsed");
				break;
			case DIV:
				match(ART_OP_T,DIV);
				primary_arithmetic_expression();
				multiplicative_arithmetic_expression_p();
				gen_incode("Multiplicative arithmetic expression parsed");
				break;
		}
	}
}


/* FIRST(primary arithmetic expression)={AVID_T, FPL_T ,INL_T ,(}
<primary arithmetic expression> ->
		 AVID_T 
		|  FPL_T 
		|  INL_T 
		|  (<arithmetic expression>) 
Author: Kwok Hong Kelvin Chan */
void primary_arithmetic_expression(void){
	switch(lookahead_token.code){
	case LPR_T:
		match(LPR_T, NO_ATTR);
		arithmetic_expression();
		match(RPR_T, NO_ATTR);
		break;
	case AVID_T:
		match(AVID_T, NO_ATTR);
		break;
	case FPL_T:
		match(FPL_T, NO_ATTR);
		break;
	case INL_T:
		match(INL_T, NO_ATTR);
		break;
	default:
		syn_printe();
		break;
	}

	gen_incode("Primary arithmetic expression parsed");
}


/* FIRST(string expression)={SVID_T, STR_T}
<string expression> -> 
	<primary string expression> <string expression’>
Author: Kyle Hinskens */
void string_expression(void){
	switch (lookahead_token.code){
	case SVID_T:
	case STR_T:
		primary_string_expression();
		string_expression_p();
		gen_incode("String expression parsed");
		break;
	}
}


/* FIRST(string expression’)={<>,e} 
<string expression’> -> 
	<> <primary string expression> <string expression’> | e
Author: Kwok Hong Kelvin Chan */
void string_expression_p(void){
	switch (lookahead_token.code){
		case SCC_OP_T:
			match(SCC_OP_T, NO_ATTR);
			primary_string_expression();
			string_expression_p();
			break;
	}
}


/* FIRST(primary string expression={SVID_T, STR_T}
<primary string expression> -> 
		SVID_T | STR_T
Author: Kyle Hinskens */
void primary_string_expression(void){
	switch(lookahead_token.code){
		case SVID_T:
			match(SVID_T, NO_ATTR);
			gen_incode("Primary string expression parsed");
			break;
		case STR_T:
			match(STR_T, NO_ATTR);
			gen_incode("Primary string expression parsed");
			break;
		default:
			syn_printe();
			break;
	}
}


/* FIRST(conditional expression)={AVID_T, FPL_T,INL_T,SVID_T, STR_T}
<conditional expression> -> 
	<logical OR expression>
Author: Kwok Hong Kelvin Chan */
void conditional_expression(void){
	logical_OR_expression();
	gen_incode("Conditional expression parsed");
}


/* FIRST(logical OR expression)={AVID_T, FPL_T,INL_T,SVID_T, STR_T}
<logical OR expression> ->  
	<logical AND expression> <logical OR expression’>
Author: Kyle Hinskens */
void logical_OR_expression(void){
	logical_AND_expression();
	logical_OR_expression_p(); 
}


/* FIRST(logical OR expression’)={.OR.,e}
<logical OR expression’> -> 
	.OR. <logical AND expression> <logical OR expression’> | e
Author: Kwok Hong Kelvin Chan */
void logical_OR_expression_p(void){
	switch(lookahead_token.code){
		case LOG_OP_T:
			switch (lookahead_token.attribute.log_op){
				case OR: 
					match(LOG_OP_T, OR);
					logical_AND_expression();
					logical_OR_expression_p(); 
					gen_incode("Logical OR expression parsed"); 
					break;
			}
	}
}


/* FIRST(logical AND expression)={AVID_T, FPL_T,INL_T,SVID_T, STR_T}
<logical AND expression> ->  
	<relational expression> <logical AND expression’>
Author: Kyle Hinskens */
void logical_AND_expression(void){
		relational_expression();
		logical_AND_expression_p();
}


/* FIRST(logical AND expression’)={.AMD., e}
<logical AND expression’> -> 
	.AND. <relational expression> <logical AND expression’> | e
Author: Kwok Hong Kelvin Chan */
void logical_AND_expression_p(void){
	switch(lookahead_token.code){
		case LOG_OP_T:
			switch (lookahead_token.attribute.log_op){
				case AND: 
					match(LOG_OP_T, AND);
					relational_expression(); 
					logical_AND_expression_p();
					gen_incode("Logical AND expression parsed"); 
					break;
			}
	}
}


/* FIRST(relational expression)={AVID_T, FPL_T,INL_T,SVID_T, STR_T}
<relational expression> ->
	<primary a_relational expression> <p_operator> <primary a_relational expression> 
	| <primary s_relational expression> <p_operator> <primary s_relational expression>
Author: Kyle Hinskens */
void relational_expression(void){
	switch(lookahead_token.code){
		case AVID_T:
		case FPL_T:
		case INL_T:
			primary_a_relational_expression();
			switch (lookahead_token.code){
				case REL_OP_T:
					p_operator();
					primary_a_relational_expression();
					break;
				default :
					syn_printe();
			}
			break;
		case SVID_T:
		case STR_T: 
			primary_s_relational_expression();
			switch (lookahead_token.code){
				case REL_OP_T:
					p_operator();
					primary_s_relational_expression();
					break;
				default :
					syn_printe();
			}
			break;
		default:
			syn_printe();
			break;
	}

	gen_incode("Relational expression parsed");
}


/* FIRST(p_operator)={==,!=,>,<}
<operator> -> == | != | > | <
Author: Kwok Hong Kelvin Chan */
void p_operator(void){
	switch(lookahead_token.code){
	case REL_OP_T:
		switch (lookahead_token.attribute.rel_op){
			case EQ:
				match(REL_OP_T, EQ);
				break;
			case NE:
				match(REL_OP_T, NE);
				break;
			case GT:
				match(REL_OP_T, GT);
				break;
			case LT:
				match(REL_OP_T, LT);
				break;
			default:
				syn_printe();
				break;
		}
	}
}


/* FIRST(primary a_relational expression)={AVID_T, FPL_T,INL_T}
<primary a_relational expression> -> 
	AVID_T | FPL_T | INL_T
Author: Kyle Hinskens */
void primary_a_relational_expression(void){
	switch(lookahead_token.code){
		case AVID_T:
			match(AVID_T, NO_ATTR);
			break;
		case FPL_T:
			match(FPL_T, NO_ATTR);
			break;
		case INL_T:
			match(INL_T, NO_ATTR);
			break;
		default:
			syn_printe();
			break;
	}
	
	gen_incode("Primary a_relational expression parsed");
}


/* FIRST(primary s_relational expression)={SVID_T, STR_T }
<primary s_relational expression> -> 
	<primary string expression>
Author: Kwok Hong Kelvin Chan */
void primary_s_relational_expression(void){
	primary_string_expression();
	gen_incode("Primary s_relational expression parsed");
}