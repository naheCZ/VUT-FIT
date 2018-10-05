/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   expressions.c
 * Datum:    11.11.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

#include "expressions.h"

#define BUFFER_SIZE 48

int debug_expr = 7;


const char* Prec_symb_debug[] =
{
  "P_PLUS",        // 0 +
  "P_MINUS",       // 1 -
  "P_MUL",         // 2 *
  "P_DIV",         // 3 /
  "P_LESS",        // 4 <
  "P_GREAT",       // 5 >
  "P_LESS_EQUAL",  // 6 <=
  "P_GREAT_EQUAL", // 7 >=
  "P_EQUAL",       // 8 ==
  "P_NOT_EQUAL",   // 9 !=
  "P_LEFT_PAR",    // 10 (
  "P_RIGHT_PAR",   // 11 )
  "P_COMMA",       // 12 ,
  "P_STRING",      // 13 string
  "P_INT",         // 14 int
  "P_DOUBLE",      // 15 double
  "P_FUNCTION",    // 16 f //TODO
  "P_DOLLAR",      // 17 $
  "P_ID",          // 18 ID
  "P_ASSIGN",      // 19 =
  "Z_LESS",        // 20 < na zasobniku
  "Z_NOT_TERM",    // 21 E na zasobniku
  "less",	         // 22 < prec_tab
  "great",	       // 23 > prec_tab
  "equal",	       // 24 = prec_tab
  "empty"	       // 25 E prec_tab
};

void symbolToString(Prec_symb symbol)
{
    printf("_symbol: %s\n", Prec_symb_debug[symbol]);
}

//precedencni tabulka
int prec_tab[19][19] = {
//             +      -      *      /      <      >      <=     >=     ==     !=     (      )      ,     str    int    dbl    f       $     id      =
/*  +   */  {great, great, less,  less,  great, great, great, great, great, great, less,  great, great, less,  less,  less,  less,  great, less},
/*  -   */  {great, great, less,  less,  great, great, great, great, great, great, less,  great, great, empty, less,  less,  less,  great, less},
/*  *   */  {great, great, great, great, great, great, great, great, great, great, less,  great, great, empty, less,  less,  less,  great, less},
/*  /   */  {great, great, great, great, great, great, great, great, great, great, less,  great, great, empty, less,  less,  less,  great, less},
/*  <   */  {less,  less,  less,  less,  great, great, great, great, great, great, less,  great, great, less,  less,  less,  less,  great, less},
/*  >   */  {less,  less,  less,  less,  great, great, great, great, great, great, less,  great, great, less,  less,  less,  less,  great, less},
/*  <=  */  {less,  less,  less,  less,  great, great, great, great, great, great, less,  great, great, less,  less,  less,  less,  great, less},
/*  >=  */  {less,  less,  less,  less,  great, great, great, great, great, great, less,  great, great, less,  less,  less,  less,  great, less},
/*  ==  */  {less,  less,  less,  less,  great, great, great, great, great, great, less,  great, great, less,  less,  less,  less,  great, less},
/*  !=  */  {less,  less,  less,  less,  great, great, great, great, great, great, less,  great, great, less,  less,  less,  less,  great, less},
/*  (   */  {less,  less,  less,  less,  less,  less,  less,  less,  less,  less,  less,  equal, equal, less,  less,  less,  less,  empty, less},
/*  )   */  {great, great, great, great, great, great, great, great, great, great, empty, great, great, less,  empty, empty, less,  great, empty},
/*  ,   */  {less,  less,  less,  less,  less,  less,  less,  less,  less,  less,  less,  equal, equal, less,  less,  less,  less,  empty, less},
/*  str */  {great, empty, empty, empty, great, great, great, great, great, great, empty, great, equal, empty, empty, empty, empty, great, empty},
/*  int */  {great, great, great, great, great, great, great, great, great, great, empty, great, equal, empty, empty, empty, empty, great, empty},
/*  dbl */  {great, great, great, great, great, great, great, great, great, great, empty, great, equal, empty, empty, empty, empty, great, empty},
/*  f   */  {empty, empty, empty, empty, empty, empty, empty, empty, empty, empty, equal, empty, empty, empty, empty, empty, empty, empty, empty},
/*  $   */  {less,  less,  less,  less,  less,  less,  less,  less,  less,  less,  less,  less,  empty, less,  less,  less,  less,  empty, less},
/*  id  */  {great, great, great, great, great, great, great, great, great, great, empty, great, equal, empty, empty, empty, empty, great, empty},
};
//get prec symbol: prec_tab[zasobnik][vstup]

//prace se zasobnikem
void Stack_init(Stack *L) {
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void Stack_dispose(Stack *L) {
	Stack_Elem* pom = NULL;

	while((pom = L->First) != NULL) {
		L->First = L->First->Rptr;
		my_free(pom);
	}
	L->Act = NULL;
	L->Last = NULL;
}

void Stack_insert_last(Stack *L, Stack_Elem* val) {
	Stack_Elem* new;

	new = my_malloc(sizeof(struct Stack_Elem));
	if(new == NULL)
		throwError(E_INTERNAL);//Chyba alokace pameti
	else {
		new->symbol = val->symbol;
		new->value = val->value;
		new->type = val->type;
		new->Rptr = NULL;
		new->Lptr = L->Last;
		new->name = val->name;
		new->defined = val->defined;
		new->class_name = val->class_name;

		if(L->First == NULL)
			L->First = new;
		else
			L->Last->Rptr = new;
		L->Last = new;
	}
}

void Stack_act_last(Stack *L) {
	L->Act = L->Last;
}

void Stack_value_last(Stack *L, Stack_Elem** val) {
	if(L->First == NULL)
		throwError(E_INTERNAL);//chyba alokace pameti
	else
		*val = L->Last;
}

void Stack_delete_last(Stack *L) {
	if(L->First != NULL) {
		Stack_Elem* pom = L->Last;
		if(L->Last == L->Act)
			L->Act = NULL;
		if(L->First == L->Last) {
			L->First = NULL;
			L->Last = NULL;
		}
		else {
			L->Last = L->Last->Lptr;
			L->Last->Rptr = NULL;
		}
		my_free(pom);
	}
}

void Stack_delete_before(Stack *L) {
	if(L->Act !=NULL && L->First != NULL && L->Act != L->First) {
		Stack_Elem* pom;
		pom = L->Act->Lptr;
		L->Act->Lptr = pom->Lptr;

		if(pom == L->First)
			L->First = L->Act;
		else
			pom->Lptr->Rptr = L->Act;
		my_free(pom);
	}
}

void Stack_push_after(Stack *L, Stack_Elem* val) {
	if(L->Act != NULL) {
		Stack_Elem* new;
		new = my_malloc(sizeof(struct Stack_Elem));
		if(new == NULL)
			throwError(E_INTERNAL);//chyba alokace pameti
		else {
			new->symbol = val->symbol;
			new->type = val->type;
			new->value = val->value;
			new->class_name = val->class_name;
			new->Lptr = L->Act;
			new->Rptr = L->Act->Rptr;
			L->Act->Rptr = new;
			if(L->Act == L->Last)
				L->Last = new;
			else
				new->Rptr->Lptr = new;
		}
	}
}

void Stack_push_before(Stack *L, Stack_Elem* val) {
	if(L->Act != NULL) {
		Stack_Elem* new;
		new = my_malloc(sizeof(struct Stack_Elem));
		if (new == NULL)
			throwError(E_INTERNAL);//chyba alokace pameti
		else {
			new->symbol = val->symbol;
			new->type = val->type;
			new->value = val->value;
			new->class_name = val->class_name;
			new->Lptr = L->Act->Lptr;
			new->Rptr = L->Act;
			if(L->Act == L->First)
				L->First = new;
			else {
				new->Lptr->Rptr = new;
				L->Act->Lptr = new;
			}
		}
	}
}

void Stack_value_act(Stack *L, Stack_Elem** val) {
	if(L->Act == NULL)
		throwError(E_SYNTAX);
	else {
		*val = L->Act;
	}
}

void Stack_act_before(Stack *L) {
	if (L->Act != NULL)
		L->Act = L->Act->Lptr;
}

Stack_Elem* pom; //pomocna promenna pro prevod tokenu na term
//prevod tokenu na term
Stack_Elem* tok_to_term(tToken token) {

  pom = my_malloc(sizeof(struct Stack_Elem));
  if (pom == NULL)
    throwError(E_INTERNAL);

  switch (token.type) {
    case T_plus: pom->symbol = P_PLUS; break;
    case T_min: pom->symbol = P_MINUS; break;
    case T_mul: pom->symbol = P_MUL; break;
    case T_div: pom->symbol = P_DIV; break;
    case T_less: pom->symbol = P_LESS; break;
    case T_greater: pom->symbol = P_GREAT; break;
    case T_lessEqual: pom->symbol = P_LESS_EQUAL; break;
    case T_greaterEqual: pom->symbol = P_GREAT_EQUAL; break;
    case T_equal: pom->symbol = P_EQUAL; break;
    case T_notEqual: pom->symbol = P_NOT_EQUAL; break;
    case T_lParenthesis: pom->symbol = P_LEFT_PAR; break;
    case T_rParenthesis: pom->symbol = P_RIGHT_PAR; break;
    case T_comma: pom->symbol = P_COMMA; break;
    case T_string_T:
    pom->symbol = P_STRING;
    pom->value.s = token.value.s;
    pom->type = STRING; break;
    case T_int_T:
    pom->symbol = P_INT;
    pom->value.i = token.value.i;
    pom->type = INT; break;
    case T_double_T:
    pom->symbol = P_DOUBLE;
    pom->value.d = token.value.d;
    pom->type = DOUBLE; break;
    case T_id:
    pom->symbol = P_ID;
    pom->value = token.value;
    pom->class_name = token.class_name; break;
    //FUNKCE?   case T_func: pom = P_FUNCTION; break; //TODO
    case T_semicolon: pom->symbol = P_DOLLAR; break;
    default: throwError(E_SYNTAX);
  }
  return pom;
}

	tData* term_to_data(Stack_Elem* elem) {
  tData *data = my_malloc(sizeof(tData));
	if (data == NULL)
		throwError(E_INTERNAL);

	data->data_type = elem->type;
	data->value = elem->value;
	data->name = elem->name;
	data->defined = elem->defined;
	data->class_name = elem->class_name;

  return data;
}

//Docasne promenne
tData* tmp_var(T_data_type typ, T_value *hodnota) {
	static unsigned id = 0;

  char buffer[BUFFER_SIZE];

	sprintf(buffer, "@T%d", id);
	insert_tmp_var(typ, buffer);

	tData* tmp_data = tReadByInfo(global_table, curr_func, curr_class, FUNC);
	tData* data = tRead(stackTop(tmp_data->local_table_stack), buffer);

	if (hodnota != NULL) {
		switch (typ) {
			case INT: set_value_int(data->name, hodnota->i); break;
			case DOUBLE: set_value_double(data->name, hodnota->d); break;
			case STRING: set_value_string(data->name, hodnota->s); break;
			default: throwError(E_INTERNAL);
		}
	}

	id++;

	return data;
}

//hlavni funkce
tError expr(int podminka) { //0 obycejny vyraz, 1 podminka, 2 funkce
	if (debug_expr > 6) printf("------------------------ EXPRESSIONS ------------------------\n");

  Stack stack;
  Stack_init(&stack);

	Stack_Elem* vstup = my_malloc(sizeof(struct Stack_Elem));
	if(vstup == NULL) throwError(E_INTERNAL);
  Stack_Elem* zasobnik = my_malloc(sizeof(struct Stack_Elem));
  if(zasobnik == NULL) throwError(E_INTERNAL);
	Stack_Elem* p_dollar = my_malloc(sizeof(struct Stack_Elem));
  if(p_dollar == NULL) throwError(E_INTERNAL);
  p_dollar->symbol = P_DOLLAR;
	Stack_Elem* z_less = my_malloc(sizeof(struct Stack_Elem));
  if(z_less == NULL) throwError(E_INTERNAL);
  z_less->symbol = Z_LESS;
	Stack_Elem* insert = my_malloc(sizeof(struct Stack_Elem));
  if(insert == NULL) throwError(E_INTERNAL);

  Stack_insert_last(&stack, p_dollar); //$ dno zasobniku

	int pom_tab = 0, last = 0, zavorka = 0;

	Stack_value_last(&stack, &zasobnik);
	vstup = tok_to_term(getNextToken()); //prevod

	pom_tab = prec_tab[zasobnik->symbol][vstup->symbol];
	if(podminka) {
    if(vstup->symbol != P_LEFT_PAR)
      throwError(E_SYNTAX); //za ifem neni (
	}

	tData *op1, *op2, *res;
	tData *pomoc = my_malloc(sizeof(struct tData));
	if (pomoc == NULL)
		throwError(E_INTERNAL);

	if (debug_expr > 0) printf("inicializace vsech promennych OK\n");
  int i = 1; //debugovani krok

  while(!last) {
		if (debug_expr > 6) printf("Krok %d: ", i);	if (debug_expr > 6) i++;
    if(pom_tab == empty) {
			if (debug_expr > 6) printf("CHBYA, DISPOSE Stack\n");
      Stack_dispose(&stack);
      return E_SYNTAX;
    }
    else if(pom_tab == less) {
      if(podminka) {
        if(vstup->symbol == P_LEFT_PAR)
          zavorka++;
        else if(vstup->symbol == P_RIGHT_PAR)
          zavorka--;
      }

    	Stack_value_last(&stack, &zasobnik);
    	if(zasobnik->symbol != Z_NOT_TERM) {
		  	if (debug_expr > 6) printf("LESS no term\n");
    		Stack_insert_last(&stack, z_less);
    		Stack_insert_last(&stack, vstup);

				Stack_value_last(&stack, &insert);

    		Stack_value_last(&stack, &zasobnik);
    		vstup = tok_to_term(getNextToken()); //prevod

    		pom_tab = prec_tab[zasobnik->symbol][vstup->symbol];
    		if (debug_expr > 6) printf("_____Zasobnik: %d, Vstup: %d, Prec_tab: %d\n", zasobnik->symbol, vstup->symbol, pom_tab);
    	}
    	else {
				if (debug_expr > 6) printf("LESS term\n");
      	Stack_act_last(&stack);
      	Stack_push_before(&stack, z_less);
      	Stack_act_last(&stack);
      	Stack_push_after(&stack, vstup);

				Stack_value_last(&stack, &zasobnik);
      	vstup = tok_to_term(getNextToken()); //prevod

      	pom_tab = prec_tab[zasobnik->symbol][vstup->symbol];
				if (debug_expr > 6) printf("_____Zasobnik: %d, Vstup: %d, Prec_tab: %d\n", zasobnik->symbol, vstup->symbol, pom_tab);
    	}
    }
		else if(pom_tab == great) {
      if(zasobnik->symbol == P_INT || zasobnik->symbol == P_DOUBLE || zasobnik->symbol == P_STRING || zasobnik->symbol == P_ID) {
			  if (debug_expr > 6) printf("GREAT ");

				Stack_value_last(&stack, &insert);

				if (insert->symbol == P_INT || insert->symbol == P_DOUBLE || insert->symbol == P_STRING) {
					op1 = term_to_data(insert);
					res = tmp_var(op1->data_type, &op1->value);
					switch(insert->symbol) {
						case P_INT: set_value_int(res->name, op1->value.i);
						if (debug_expr > 6) printf("konstanta INT\n"); break;
						case P_DOUBLE: set_value_double(res->name, op1->value.d);
						if (debug_expr > 6) printf("konstanta DOUBLE\n"); break;
						case P_STRING: set_value_string(res->name, op1->value.s);
						if (debug_expr > 6) printf("konstanta STRING\n"); break;
						default: throwError(E_SYNTAX);
					}
					insert->name = res->name;
					insert->type = res->data_type;
					insert->value = res->value;
					insert->class_name = res->class_name;
				}
				else if(insert->symbol == P_ID) {
				  if (debug_expr > 6) printf("identifikator\n");
					pomoc = tRead(global_table, curr_func);
					if(insert->class_name == NULL)
						pomoc = tReadByInfo(stackTop(pomoc->local_table_stack), insert->value.s, curr_class, VAR);
					else
						pomoc = tReadByInfo(stackTop(pomoc->local_table_stack), insert->value.s, insert->class_name, VAR);
					op1 = pomoc;
					insert->defined = true;
					/*switch (op1->data_type) {
						case INT: set_value_int(op1->name, op1->value.i); break;
						case DOUBLE: set_value_double(op1->name, op1->value.d); break;
						case STRING: set_value_string_id(op1->name); break;
						default: throwError(E_SYNTAX);
					}*/
					insert->name = op1->name;
					insert->type = op1->data_type;
					insert->value = op1->value;
					insert->class_name = op1->class_name;
					printf("Dostal jsem se sem!\n");
				}
				else
					throwError(E_SYNTAX);

				Stack_delete_last(&stack);
        Stack_delete_last(&stack);

				insert->symbol = Z_NOT_TERM;

				Stack_insert_last(&stack, insert);

        Stack_act_last(&stack);
        Stack_act_before(&stack);

        Stack_value_act(&stack, &zasobnik);

        pom_tab = prec_tab[zasobnik->symbol][vstup->symbol];
				if (debug_expr > 6) printf("_____Zasobnik: %d, Vstup: %d, Prec_tab: %d\n", zasobnik->symbol, vstup->symbol, pom_tab);
      }
      else if(zasobnik->symbol == P_PLUS || zasobnik->symbol == P_MINUS || zasobnik->symbol == P_MUL || zasobnik->symbol == P_DIV
				|| zasobnik->symbol == P_LESS || zasobnik->symbol == P_GREAT || zasobnik->symbol == P_LESS_EQUAL
				|| zasobnik->symbol == P_GREAT_EQUAL || zasobnik->symbol == P_EQUAL || zasobnik->symbol == P_NOT_EQUAL) {
				if (debug_expr > 6) printf("GREAT ");

				Stack_value_last(&stack, &insert);

				if(!insert->defined)
					op2 = term_to_data(insert);
				else {
					pomoc = tRead(global_table, curr_func);
					if(insert->class_name == NULL)
						pomoc = tReadByInfo(stackTop(pomoc->local_table_stack), insert->name, curr_class, VAR);
					else
						pomoc = tReadByInfo(stackTop(pomoc->local_table_stack), insert->name, insert->class_name, VAR);
					op2 = pomoc;
				}

        Stack_delete_last(&stack);
        Stack_delete_last(&stack);

				Stack_value_last(&stack, &insert);

				if(!insert->defined)
					op1 = term_to_data(insert);
				else {
					pomoc = tRead(global_table, curr_func);
					if(insert->class_name == NULL)
						pomoc = tReadByInfo(stackTop(pomoc->local_table_stack), insert->name, curr_class, VAR);
					else
						pomoc = tReadByInfo(stackTop(pomoc->local_table_stack), insert->name, insert->class_name, VAR);
					op1 = pomoc;
				}

        Stack_delete_last(&stack);
        Stack_delete_last(&stack);

				res = tmp_var(op1->data_type, NULL);
				pomoc = res;

				switch(zasobnik->symbol) {
					case P_PLUS: if (debug_expr > 6) printf("plus\n");
					generate(I_Add, op1, op2, res); break;
					case P_MINUS: if (debug_expr > 6) printf("minus\n");
					generate(I_Sub, op1, op2, res); break;
					case P_MUL: if (debug_expr > 6) printf("mul\n");
					generate(I_Mul, op1, op2, res); break;
					case P_DIV: if (debug_expr > 6) printf("div\n");
					generate(I_Div, op1, op2, res); break;
					case P_LESS: if (debug_expr > 6) printf("less\n");
					generate(I_Less, op1, op2, res); break;
					case P_GREAT: if (debug_expr > 6) printf("great\n");
					generate(I_Greater, op1, op2, res); break;
					case P_LESS_EQUAL: if (debug_expr > 6) printf("less_equal\n");
					generate(I_LessEq, op1, op2, res); break;
					case P_GREAT_EQUAL: if (debug_expr > 6) printf("great_equal\n");
					generate(I_GreaterEq, op1, op2, res); break;
					case P_EQUAL: if (debug_expr > 6) printf("equal\n");
					generate(I_Equal, op1, op2, res); break;
					case P_NOT_EQUAL: if (debug_expr > 6) printf("not_equal\n");
					generate(I_NotEqual, op1, op2, res); break;
					default: throwError(E_SYNTAX);
				}

				insert->symbol = Z_NOT_TERM;
				insert->type = res->data_type;
				insert->value = res->value;
				insert->name = res->name;
				insert->defined = true;
				insert->class_name = res->class_name;

				Stack_insert_last(&stack, insert);

        Stack_act_last(&stack);
        Stack_act_before(&stack);

        Stack_value_act(&stack, &zasobnik);
        pom_tab = prec_tab[zasobnik->symbol][vstup->symbol];
				if (debug_expr > 6) printf("_____Zasobnik: %d, Vstup: %d, Prec_tab: %d\n", zasobnik->symbol, vstup->symbol, pom_tab);
      }
			else if(zasobnik->symbol == P_RIGHT_PAR) {
				if (debug_expr > 6) printf("GREAT prava zavorka )\n");

				Stack_delete_last(&stack);

				Stack_act_last(&stack);
				Stack_delete_before(&stack);
				Stack_delete_before(&stack);

				Stack_act_last(&stack);
				Stack_act_before(&stack);

				Stack_value_act(&stack, &zasobnik);
				if(!zavorka)
					pom_tab = prec_tab[zasobnik->symbol][vstup->symbol];
				else
					vstup->symbol = P_DOLLAR;

				if (debug_expr > 6) printf("_____Zasobnik: %d, Vstup: %d, Prec_tab: %d\n", zasobnik->symbol, vstup->symbol, pom_tab);
      }
      else if(zasobnik->symbol == P_FUNCTION) {
				if (debug_expr > 6) printf("GREAT function\n");
        //TODO funkce
				if (debug_expr > 6) printf("_____Zasobnik: %d, Vstup: %d, Prec_tab: %d\n", zasobnik->symbol, vstup->symbol, pom_tab);
      }
    }
    else if(pom_tab == equal) {
	  	if (debug_expr > 6) printf("EQUAL\n");

      if(podminka) {
        if(vstup->symbol == P_RIGHT_PAR)
          zavorka--;
        else if(vstup->symbol == P_LEFT_PAR)
          zavorka++;
      }

      Stack_insert_last(&stack, vstup);
      Stack_value_last(&stack, &zasobnik);
			if(podminka) {
        if(zavorka)
          vstup = tok_to_term(getNextToken()); //prevod
			  if(!zavorka) {
          vstup->symbol = P_DOLLAR; //posledni operace
			  }
		  }
			else
				vstup = tok_to_term(getNextToken());
      pom_tab = prec_tab[zasobnik->symbol][vstup->symbol];
			if (debug_expr > 6) printf("_____Zasobnik: %d, Vstup: %d, Prec_tab: %d\n", zasobnik->symbol, vstup->symbol, pom_tab);
    }
    else {
			if (debug_expr > 6) printf("Neni v prec_tab\n");
      Stack_dispose(&stack);
      return E_SYNTAX; //neznama chyba
    }
    //Konec zpracovani check
		if(vstup->symbol == P_DOLLAR) {
		  Stack_value_last(&stack, &insert);
		  if(insert->symbol == Z_NOT_TERM) {
			Stack_act_last(&stack);
		    Stack_act_before(&stack);
		    Stack_value_act(&stack, &insert);
		    if(insert->symbol == P_DOLLAR) {
		      last = 1;
		    }
		  }
		}
	printf("jsme na konci %d\n", last);
  }
	if (debug_expr > 6) printf("KONEC expressions\n");
	//Vysledek celeho vyrazu (prirazeni nebo podminka)
	Stack_value_last(&stack, &expr_result);
	//if (debug_expr > 6) printf("RESULT typ: %d, hodnota int: %d, hodnota dbl: %d, hodnota str: %c\n", expr_result->symbol, expr_result->value.i, expr_result->value.d, expr_result->value.s);

	Stack_dispose(&stack);
  return OK;
}
