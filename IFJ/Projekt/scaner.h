//hlavicka pro lexikalni analyzator


//******************************************************************
//              -------- main,run a vestavene funkce doresit
//Typy tokenu

#ifndef SCANER_H
#define	SCANER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "str.h"
#include "error.h"

typedef enum
{
	T_boolean,		// 0 klicova slova-nemozu byt identifikatory
	T_break,
	T_class,
	T_continue,
	T_do,
	T_double_kw,
	T_else,
	T_false,
	T_for,
	T_if,
	T_int_kw,    // 10
	T_return,
	T_string_kw,
	T_static,
	T_true,
	T_void_kw,
	T_while,
	//konec klucovich slov---------------
	T_id,			//identifikator
	T_string_T,			//retazec
	T_int_T,		//   cele cislo
	T_double_T,	    // 20   desatine cislo
	T_EOF,			//konec suboru
	T_semicolon,	// ;
	T_dot,			// .
	T_comma, 		// ,
	T_plus,			// +
	T_min,			// -
	T_mul,			// *
	T_div,			// /
	T_less,			//   <
	T_lessEqual, 	// 30<=
	T_greater, 		// >
	T_greaterEqual, // >=
	T_equal,		// ==
	T_assign,		// =
	T_notEqual, 	// 35 !=
	T_lParenthesis,	// (
	T_rParenthesis,	// )
	T_lBracket,		// {
	T_rBracket,		// }
	T_LEX_ERROR,  // 40
}T_Type;


//Stavy konecneho automatu
typedef enum
{
	S_start, 		//pociatocny stav
	S_id,			//identifikator
	S_int,			//cele cislo
	S_des1,
	S_des2,
	S_exp1,			//exponent
	S_exp2,
	S_exp3,
	S_str,			//retazec
	S_EscSeq,
	S_slash,		//riadkovy/blokovy  komentar
	S_LeftBeak,	// (lavy zobak) < alebo <=
	S_RightBeak,// (pravy zobak) > alebo >=
	S_EqOrAss,	// rovnost == alebo priradenie =
	S_NotEqual,	// nerovnost !=
	S_error,	//state pre error
	S_extend,	//kvalifikovany id
	S_extend1,
	S_linecom,	//radkovy komentar
	S_blockcom,	//blokovy komentar-zaciatok
	S_rblockcom	//blokovy komentar-konec
}States;

typedef union T_value //Samotna data
{
  char *s;
  int i;
  double d;
  void *ptr;
}T_value;

typedef struct tToken
{
	T_Type type;
	T_value value;
	char *class_name;
} tToken;

//hlavicka funkce simulujici lexikalni analyzator
void setSourceFile(FILE *f);
tToken getNextToken();
void typeToString(T_Type type);

#endif
