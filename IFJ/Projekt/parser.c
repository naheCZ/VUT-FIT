/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   parser.c
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

#include "parser.h"

tError parse()
{
	generate(I_NOP, NULL, NULL, NULL); // vlozeni prazdne instrukce pro zichr kvuli skokum
	return program();
}

tError program()
{
	if (debug > 1) printf("program\n");

	if (token.type == T_EOF)
	{
		generate(I_Stop, NULL, NULL, NULL);
		return OK;
	}
	else if (token.type == T_class)
	{
		if (class() == OK)
		{
			get_token();
			return program();
		}
		else
			return E_SYNTAX;
	}
	else
		return E_SYNTAX;
}

tError class()
{
		if (debug > 1) printf("class\n");
	get_token_and_check(T_id);

	loadClass(token.value.s);							// nacteni jmena tridy

	get_token_and_check(T_lBracket);
	if (var_or_func() != OK)
		return E_SYNTAX;

	return var_or_func();
}

tError var_or_func()
{
	if (debug > 1) printf("var_or_func\n");
	get_token();
	if (token.type == T_rBracket)   				 // konec tridy
		return OK;
	check_token(T_static);
	get_token();														 // nejaky dat typ, pro fci nebo var
	if (token.type == T_int_kw || token.type == T_double_kw ||  token.type == T_string_kw || token.type == T_void_kw)
	{
		curr_type = get_data_type();  				 // zjistim datovy typ
		get_token_and_check(T_id);
		loadName(token.value.s);  						 // ulozim id do curr_name
		get_token();
		if (token.type == T_semicolon || token.type == T_assign)             // je to glob promenna
        {
            curr_info = VAR;
            if (insert_to_global() != OK)  // deklarace glob promenne
                return E_SYNTAX;
 
            if (token.type == T_assign)            // init
            {
                if (expression(EXPR) != OK)
                    return E_SYNTAX;
                tData *var = tReadByInfo(global_table, curr_name, curr_class, VAR);
                tData *value;
                if(expr_result->class_name == NULL)
					value = tReadByInfo(get_table(curr_func, curr_class), expr_result->name, curr_class, VAR);
				else
					value = tReadByInfo(get_table(curr_func, curr_class), expr_result->name, expr_result->class_name, VAR);
                generate(I_Assign, value, NULL, var);
                get_token_and_check(T_semicolon);
                return OK;
            }
            return var_or_func();                            // jdeme na dalsi fci nebo glob var
        }
		else if (token.type == T_lParenthesis) // je to funkce
		{
			curr_info = FUNC;
			loadFunc(curr_name);
			if (insert_to_global() != OK)
				return E_SYNTAX;
			if (params() != OK)     					   // zpracujeme parametry
				return E_SYNTAX;
			return func();       								 // zpracujeme telo funkce
		}
		else
			return E_SYNTAX;
	}
	else
		return E_SYNTAX;
}

tError params()
{
	if (debug > 1) printf("params\n");
	get_token();
	if (token.type == T_rParenthesis)	// funkce nema parametry
		return OK;
	else if (token.type == T_int_kw || token.type == T_double_kw || token.type == T_string_kw || token.type == T_void_kw)
	{
		curr_type = get_data_type();   // zjistim datovy typ
		get_token_and_check(T_id);
		loadName(token.value.s);  // ulozim id do docasne promenne

		// vlozeni parametru

		get_token();
		if (token.type == T_comma)   // jde se na dalsi parametr
			return params();
		else if (token.type == T_rParenthesis) // funkce uz nema parametry
		{
			return OK;
		}
		else
			return E_SYNTAX;
	}
	else
		return E_SYNTAX;
}

tError func()
{
	if (debug > 1) printf("func\n");

	get_token_and_check(T_lBracket);
	// todo: zacatek teto funkce
	get_token();
	if (stmt_list() != OK)
		return E_SYNTAX;

	// todo: zkontrolovani returnu, u voidu nemusi!!
	check_token(T_rBracket);
	return OK;
}

tError stmt_list()
{
	if (debug > 1) printf("stmt_list\n");

	if (token.type == T_rBracket)
		return OK;
	else
	{
		if (stmt() != OK)
			return E_SYNTAX;

		return stmt_list();
	}
	return E_SYNTAX;
}

tError stmt()
{
	if (debug > 1) printf("stmt\n");
  tData *cond = NULL;

	switch (token.type) {
		case T_lBracket: //-------------------- blok { }
			get_token();
			// todo: mozna bude potreba pridat blok
			if (stmt_list() != OK)
				return E_SYNTAX;
			check_token(T_rBracket);
			get_token();
			return OK;

		case T_int_kw:  //---------------------- var definice
		case T_double_kw:
		case T_string_kw:
			curr_type = get_data_type();
			if (var_dec() != OK)
				return E_SYNTAX;

			get_token();
				if (debug > 1) printf("stmt %d\n", token.type);
			return OK;

		case T_if:     // ------------------------ if
			if (expression(COND) != OK)  // provedeni vyrazu
				return E_SYNTAX;
			cond = tRead(get_table(curr_func, curr_class), expr_result->name);

			tData *label_else = tmp_var(INT, NULL);

			generate(I_IfNGoTo, cond, label_else, NULL);

			get_token();
			if (stmt() != OK)
				return E_SYNTAX;
			tData *label_end_if = tmp_var(INT, NULL);

			generate(I_GoTo, NULL, label_end_if, NULL);
			set_value_int(label_else->name, line);  //zacatek else

			if (els() != OK)
				return E_SYNTAX;

			set_value_int(label_end_if->name, line);
			if (debug > 1) printf("label_else = %d   label_end_if = %d\n", label_else->value.i, label_end_if->value.i);
			return OK;
			break;

		case T_while:
		{
			tData *label_cond = tmp_var(INT, NULL);
			set_value_int(label_cond->name, line);
			tData *label_end_while = tmp_var(INT, NULL);

			if (expression(COND) != OK)  // provedeni vyrazu
				return E_SYNTAX;
			cond = tRead(get_table(curr_func, curr_class), expr_result->name);

			generate(I_IfNGoTo, cond, label_end_while, NULL);

			get_token();
			if (stmt() != OK)
				return E_SYNTAX;

			generate(I_GoTo, NULL, label_cond, NULL);

			set_value_int(label_end_while->name, line);
		}
		return OK;

		case T_return:
        {   
			tData *func = tRead(global_table, curr_func);
            if (func->data_type == VOID) {
                get_token();
                generate(I_Ret, NULL, NULL, NULL);
            }
            else if (func->data_type == INT || func->data_type == DOUBLE || func->data_type == STRING) {
                get_token();
                tData *result = tmp_var(expr_result->type, &expr_result->value);
                generate(I_Ret, NULL, NULL, result);
            }
            else
                return E_SYNTAX;
            return OK;
          }
		
		case T_id:
			loadName(token.value.s);
			get_token();
			if (token.type == T_assign)			// je to promenna
			{
				if (expression(EXPR) != OK)
					return E_SYNTAX;
				
				tData *var = tReadByInfo(get_table(curr_func, curr_class), curr_name, curr_class, VAR);
				tData *value;
				
				if(expr_result->class_name == NULL)
					value = tReadByInfo(get_table(curr_func, curr_class), expr_result->name, curr_class, VAR);
				else
					value = tReadByInfo(get_table(curr_func, curr_class), expr_result->name, expr_result->class_name, VAR);
				
				generate(I_Assign, value, NULL, var);
				get_token();
				return OK;
			}
			else if (token.type == T_lParenthesis)		// je to funkce
			{

			}
			else
				return E_SYNTAX;
			return OK;

		default:
			return E_SYNTAX;

	}
	return OK;
}

tError var_dec()
{
	if (debug > 1) printf("var_dec\n");

	get_token_and_check(T_id);
	loadName(token.value.s);  // ulozim id do docasne promenne
	curr_info = VAR;

	if (insert_to_local() != OK)
		return E_SEMANTIC_UNDEFINED;
	get_token();
	return init();
}

tError init()
{
	if (debug > 1) printf("init\n");

	if (token.type == T_assign)
	{
		if (expression(EXPR) != OK)
			return E_SYNTAX;
		
		tData *var = tReadByInfo(get_table(curr_func, curr_class), curr_name, curr_class, VAR);
		tData *value;
		
		if(expr_result->class_name == NULL)
			value = tReadByInfo(get_table(curr_func, curr_class), expr_result->name, curr_class, VAR);
		else
			value = tReadByInfo(get_table(curr_func, curr_class), expr_result->name, expr_result->class_name, VAR);
		
		generate(I_Assign, value, NULL, var);
		return OK;
	}
	else
		return OK; // napr jenom deklarace
}

tError els()
{

	if (debug > 1) printf("else\n");
	if (token.type == T_else) // je else cast
	{
		get_token();
		return stmt();
	}
	else											// neni else cast
		return OK;
}

tError expression(int expr_or_cond)
{
	if (debug > 1) printf("expression\n");
 	tError e;

	if ((e = expr(expr_or_cond)) != OK)
	{
		throwError(e);
	}

	if (expr_result != NULL)
		if (debug > 1) printf("expr_result neni prazdny %d\n", expr_result->value.i);

	return OK;
}



//------------------------ pomocné funkce na vkladani do TS --------------------------------

tTable* get_table(char* funcName, char* className)
{
  tData* pom;

  pom = tReadByInfo(global_table, funcName, className, FUNC);

  if(pom == NULL)
    throwError(E_INTERNAL);

  return stackTop(pom->local_table_stack);
}

int insert_tmp_var(T_data_type data_type, char* name)
{
	tData *data, *tmp_data;
	data = my_malloc(sizeof(tData));
	tmp_data = my_malloc(sizeof(tData));

	if (tmp_data == NULL || data == NULL) 				// kontrola
		throwError(E_INTERNAL);
	tmp_data = tReadByInfo(global_table, curr_func, curr_class, FUNC);    // nalezeni spravne fce, pro kterou chceme lokalni tabulku
	if (tmp_data == NULL)
		throwError(E_SEMANTIC_UNDEFINED);

	if (tSearch(stackTop(tmp_data->local_table_stack), name) != NULL) // kontrol, jesti uz neni promenna ve fci,
			throwError(E_SEMANTIC_UNDEFINED);

	if ((data->name = my_malloc(sizeof(char) * strlen(name))) == NULL)
		throwError(E_INTERNAL);

	data->info = VAR;
	strcpy(data->name, name);
	data->data_type = data_type;
	data->class_name = curr_class;

	tInsert(stackTop(tmp_data->local_table_stack), name, *data);

	return OK;
}

int set_value_int(char* name, int int_val)
{
	tData* tmp_data;
	 // do tmp_data nactu funkci pro kterou chci najit lok. tab.
	if ((tmp_data = tReadByInfo(global_table, curr_func, curr_class, FUNC)) == NULL)
	 	throwError(E_INTERNAL);
	tData* data = tReadByInfo(stackTop(tmp_data->local_table_stack), name, curr_class, VAR);
	if (data == NULL)
		throwError(E_INTERNAL);

	data->value.i = int_val;

	return OK;
}

int set_value_double(char* name, double double_val)
{
	tData* tmp_data;
	 // do tmp_data nactu funkci pro kterou chci najit lok. tab.
	if ((tmp_data = tReadByInfo(global_table, curr_func, curr_class, FUNC)) == NULL)
	 	throwError(E_INTERNAL);

	tData* data = tReadByInfo(stackTop(tmp_data->local_table_stack), name, curr_class, VAR);
	if (data == NULL)
		throwError(E_INTERNAL);

	data->value.d = double_val;

	return OK;
}

int set_value_string(char* name, char* string_val)
{
	tData* tmp_data;
	 // do tmp_data nactu funkci pro kterou chci najit lok. tab.
	if ((tmp_data = tReadByInfo(global_table, curr_func, curr_class, FUNC)) == NULL)
		throwError(E_INTERNAL);

	tData* data = tReadByInfo(stackTop(tmp_data->local_table_stack), name, curr_class, VAR);
	if (data == NULL)
		throwError(E_INTERNAL);

	data->value.s = my_malloc(sizeof(char) * strlen(string_val));

	if (data->value.s == NULL)
		throwError(E_INTERNAL);
	strcpy(data->value.s, string_val);

	return OK;
}

int set_value_string_id(char* name)
{
	tData* tmp_data;
	 // do tmp_data nactu funkci pro kterou chci najit lok. tab.
	if ((tmp_data = tReadByInfo(global_table, curr_func, curr_class, FUNC)) == NULL)
		throwError(E_INTERNAL);

	tData* data = tReadByInfo(stackTop(tmp_data->local_table_stack), name, curr_class, VAR);
	if (data == NULL)
		throwError(E_INTERNAL);

	return OK;
}

tError insert_to_local()
{
	tData *data, *tmp_data;
	data = my_malloc(sizeof(tItem));
	tmp_data = my_malloc(sizeof(tItem));

	if (tmp_data == NULL || data == NULL) 				// kontrola
		return E_INTERNAL;
	if (curr_info != VAR || curr_type == VOID)
		return E_SEMANTIC_UNDEFINED;
	tmp_data = tReadByInfo(global_table, curr_func, curr_class, FUNC);    // nalezeni spravne fce, pro kterou chceme lokalni tabulku
	if (tmp_data == NULL)
		return E_SEMANTIC_UNDEFINED;

	if (tSearch(stackTop(tmp_data->local_table_stack), curr_name) != NULL) // kontrol, jesti uz neni promenna ve fci, todo: pridat kontrolu paramatru
		return E_SEMANTIC_UNDEFINED;

	if ((data->name = my_malloc(sizeof(char) * strlen(curr_name))) == NULL)
		throwError(E_INTERNAL);
	if ((data->class_name = my_malloc(sizeof(char) * strlen(curr_class))) == NULL)
		throwError(E_INTERNAL);

	data->info = curr_info;
	strcpy(data->name, curr_name);
	data->data_type = curr_type;
	strcpy(data->class_name, curr_class);

	tInsert(stackTop(tmp_data->local_table_stack), curr_name, *data);

	data = tRead(stackTop(tmp_data->local_table_stack), curr_name);    // nalezeni spravne fce, pro kterou chceme lokalni tabulku

/*------------------- TEST VKLADANI PRVKU ------------------
	tItem *item;
	item = tSearch(global_table, curr_name);
	printf("---- %s %s %d \n",item->data.class_name, item->data.name, item->data.info);

	char c[2] = {'a', '\0'};
	if (item->data.data_type == STRING)
	{
		item = tSearch(global_table, c);
		printf("---- %s %s %d \n",item->data.class_name, item->data.name, item->data.info);
	}
*/
	return OK;
}

tError insert_to_global()
{
	tData *data;
	tItem *item;
	tTable *tmp_local_table;
	data = my_malloc(sizeof(tItem));
	if (data == NULL)
		return E_INTERNAL;

	if (curr_info == FUNC)
		item = tSearchByInfo(global_table, curr_name, curr_class, FUNC);
	else if (curr_info == VAR)
		item = tSearchByInfo(global_table, curr_name, curr_class, VAR);

	if (item != NULL)
		return E_SEMANTIC_UNDEFINED;

	if (curr_info == FUNC)
	{
		initTable(&tmp_local_table);
		data->local_table_stack = stackInit();
		stackPush(data->local_table_stack, tmp_local_table);
	}

	if ((data->name = my_malloc(sizeof(char) * strlen(curr_name))) == NULL)
		throwError(E_INTERNAL);
	if ((data->class_name = my_malloc(sizeof(char) * strlen(curr_class))) == NULL)
		throwError(E_INTERNAL);

	data->info = curr_info;
	strcpy(data->name, curr_name);
	data->data_type = curr_type;
	strcpy(data->class_name, curr_class);

	tInsert(global_table, curr_name, *data);

/*------------------- TEST VKLADANI PRVKU ------------------
	item = tSearch(global_table, curr_name);
	printf("---- %s %s %d \n",item->data.class_name, item->data.name, item->data.info);

	char c[2] = {'a', '\0'};
	if (item->data.data_type == STRING)
	{
		item = tSearch(global_table, c);
		printf("---- %s %s %d \n",item->data.class_name, item->data.name, item->data.info);
	}
*/
	return OK;
}


T_data_type get_data_type()
{
	switch (token.type) {
		case T_int_kw: return INT;
		case T_double_kw: return DOUBLE;
		case T_string_kw: return STRING;
		case T_void_kw: return VOID;
		default: throwError(E_SYNTAX);
	}
	return VOID;
}

// funkce pro načtení dalšího tokenu ze scaneru
void get_token()
{
	token = getNextToken();
	if (debug >2) typeToString(token.type);
	if (token.type == T_LEX_ERROR)
		throwError(E_LEX);
}

// funkce prověří, jestli náš token je ten, který jsme očekávali
void check_token(T_Type token_type)
{
	if (token.type != token_type)
		throwError(E_SYNTAX);
}

int loadName(char *s)
{
	curr_name = my_malloc(sizeof(char) * strlen(s));
	if (curr_name == NULL)
		throwError(E_INTERNAL);
	strcpy(curr_name, s);
	return 0;
}

int loadClass(char *s)
{
	curr_class = my_malloc(sizeof(char) * strlen(s));
	if (curr_class == NULL)
		throwError(E_INTERNAL);
	strcpy(curr_class, s);
	return 0;
}

int loadFunc(char *s)
{
	curr_func = my_malloc(sizeof(char) * strlen(s));
	if (curr_func == NULL)
		throwError(E_INTERNAL);
	strcpy(curr_func, s);
	return 0;
}

// funkce načte a prověří další token
void get_token_and_check(T_Type tok)
{
	get_token();
	check_token(tok);
}

int generate_key()
{
	static int i = 0;
	i++;
	return i;
}
