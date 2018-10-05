// lexikalni analyzator

#include "scaner.h"

// promenna pro ulozeni vstupniho souboru
FILE *source;

const char* token_debug[] =
{
    "boolean",      // 0 klicova slova-nemozu byt identifikatory
    "break",
    "class",
    "continue",
    "do",
    "double_kw",
    "else",
    "false",
    "for",
    "if",
    "int_kw",    // 10
    "return",
    "string_kw",
    "static",
    "true",
    "void_kw",
    "while",
    //konec klucovich slov---------------
    "id",           //identifikator
    "string_T",         //retazec
    "int_T",        //   cele cislo
    "double_T",     // 20   desatine cislo
    "EOF",          //konec suboru
    "semicolon",    // ;
    "dot",          // .
    "comma",        // ,
    "plus",         // +
    "min",          // -
    "mul",          // *
    "div",          // /
    "less",         //   <
    "lessEqual",    // 30<=
    "greater",      // >
    "greaterEqual", // >=
    "equal",        // ==
    "assign",       // =
    "notEqual",     // 35 !=
    "lParenthesis", // (
    "rParenthesis", // )
    "lBracket",     // {
    "rBracket",     // }
    "LEX_ERROR"     // 40
};

void setSourceFile(FILE *f)
{
  source = f;
}

void typeToString(T_Type type)
{
    printf("_token: %s\n", token_debug[type]);
}

// hlavni funkce lexikalniho analyzatoru
tToken getNextToken()
{
   string attr;

   States state = S_start;
   tToken stoken;
   int c;
   strInit(&attr);
   strClear(&attr);

   while (1)
   {
     c = fgetc(source);
     switch (state)
     {
//------------------------------------------------------------------
            case S_start:
                        switch(c)
                        {
                              case '+':
                                    stoken.type =T_plus;
                                    return stoken;
                              case '-':
                                    stoken.type =T_min;
                                    return stoken;
                              case '*':
                                    stoken.type =T_mul;
                                    return stoken;
                              case '(':
                                    stoken.type =T_lParenthesis;
                                    return stoken;
                              case ')':
                                    stoken.type =T_rParenthesis;
                                    return stoken;
                              case '{':
                                    stoken.type =T_lBracket;
                                    return stoken;
                              case '}':
                                    stoken.type =T_rBracket;
                                    return stoken;
                              case ',':
                                    stoken.type =T_comma;
                                    return stoken;
                              case ';':
                                    stoken.type =T_semicolon;
                                    return stoken;
                              case '.':
                                    stoken.type =T_dot;
                                    return stoken;
                              case EOF:                     //ak je znak konec suboru
                                    stoken.type =T_EOF;
                                    return stoken;
                              case '/':
                                    state=S_slash;
                                    break;
                              case '!':
                                    state=S_NotEqual;
                                    break;
                              case '=':
                                    state=S_EqOrAss;
                                    break;
                              case '<':
                                    state=S_LeftBeak;
                                    break;
                              case '>':
                                    state=S_RightBeak;
                                    break;
                              case '\"':
                                    state=S_str;
                                    break;
                              default:
                                    if((isalpha(c)) || (c == '_') || (c == '$')) //identifikator alebo kluove slovo
                                    {
                                          state = S_id;
                                          strAddChar(&attr,c);
                                    }else if(isdigit(c))    //cislo
                                    {
                                          state = S_int;
                                          strAddChar(&attr,c);
                                    }else if(isspace(c))    // vynechva prazny znak
                                          break;
                                    else
                                          state = S_error;
                        }
            break;
//---------------------------------------------------------
            case S_id:
                        if((isalpha(c)) || (isdigit(c)) || (c == '_') || (c == '$'))
                        {
                              strAddChar(&attr,c);
                              state = S_id;
                        }else if(c == '.')
                        {
                              strAddChar(&attr,c);
                              state = S_extend;
                        }else
                        {
                              ungetc(c,source);
                              if(strCmpConstStr(&attr,"boolean") == 0)
                              {
                                    stoken.type =T_boolean;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"break") == 0)
                              {
                                    stoken.type =T_break;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"class") == 0)
                              {
                                    stoken.type =T_class;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"continue") == 0)
                              {
                                    stoken.type =T_continue;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"do") == 0)
                              {
                                    stoken.type =T_do;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"double") == 0)
                              {
                                    stoken.type =T_double_kw;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"else") == 0)
                              {
                                    stoken.type =T_else;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"false") == 0)
                              {
                                    stoken.type =T_false;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"for") == 0)
                              {
                                    stoken.type =T_for;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"if") == 0)
                              {
                                    stoken.type =T_if;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"int") == 0)
                              {
                                    stoken.type =T_int_kw;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"return") == 0)
                              {
                                    stoken.type =T_return;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"String") == 0)
                              {
                                    stoken.type =T_string_kw;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"static") == 0)
                              {
                                    stoken.type =T_static;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"true") == 0)
                              {
                                    stoken.type =T_true;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"void") == 0)
                              {
                                    stoken.type =T_void_kw;
                                    return stoken;
                              }else if(strCmpConstStr(&attr,"while") == 0)
                              {
                                    stoken.type =T_while;
                                    return stoken;
                              }else
                              {
                                    stoken.type =T_id;
                                    stoken.value.s=my_malloc(sizeof(char)*strlen(strGetStr(&attr)));
                                    strcpy(stoken.value.s, strGetStr(&attr));
                                    stoken.class_name=NULL;
                                    return stoken;
                              }
                        }
            break;
//---------------------------------------------------------
            case S_extend:
                if (isspace(c))
                {
                    state = S_error;
                    break;
                }else if ((isalpha(c)) || (c == '$') || (c == '_'))
                {
                    strAddChar(&attr,c);
                    state = S_extend1;
                }else
                {
                    ungetc(c,source);
                    //overit ci to je klucove slovo, ak ano vyhodit error
                }
            break;
//---------------------------------------------------------
            case S_extend1:
                if (isspace(c))
                {
                    ungetc(c,source);
                    stoken.type =T_id;
                    char *class=strtok(strGetStr(&attr), ".");
                    char *id=strtok(NULL, "\0");
                    stoken.class_name=my_malloc(sizeof(char)*strlen(class));
                    strcpy(stoken.class_name, class);
                    stoken.value.s=my_malloc(sizeof(char)*strlen(id));
                    strcpy(stoken.value.s, id);
                    return stoken;
                }
                if ((isalpha(c)) || isdigit(c) || (c == '$') || (c == '_') )
                {
                    strAddChar(&attr,c);
                    state = S_extend1;
                }else
                {
                    ungetc(c,source);
                    //overeni ci to neni klucove slovo
                    stoken.type =T_id;
                    char *class=strtok(strGetStr(&attr), ".");
                    char *id=strtok(NULL, "\0");
                    stoken.class_name=my_malloc(sizeof(char)*strlen(class));
                    strcpy(stoken.class_name, class);
                    stoken.value.s=my_malloc(sizeof(char)*strlen(id));
                    strcpy(stoken.value.s, id);
                    return stoken;
                }
            break;
//---------------------------------------------------------
            case S_int:
                  if(isdigit(c))
                  {
                        strAddChar(&attr,c);
                        state = S_int;
                  }else if(c == '.')  //ak dojde bodka bude to desatine cislo
                  {
                        strAddChar(&attr,c);
                        state = S_des1;
                  }else if(c == 'e' || c == 'E') //bude to cislo z exponentom
                  {
                        strAddChar(&attr,c);
                        state = S_exp1;
                  }else if(isalpha(c) || (c == '_') || (c == '$'))
                  {
                        state = S_error;
                        break;
                  }else
                  {
                        ungetc(c,source);
                        stoken.type =T_int_T;
                        stoken.value.i= atoi(strGetStr(&attr));
                        return stoken;
                  }
            break;
//---------------------------------------------------------
            case S_des1:      //za bodkov musi byt aspon jedno cislo
                  if(isdigit(c))
                  {
                        strAddChar(&attr,c);
                        state = S_des2;
                  }else
                  {
                        state = S_error;
                  }
            break;
//---------------------------------------------------------
            case S_des2:
                  if(isdigit(c))
                  {
                        strAddChar(&attr,c);
                        state = S_des2;
                  }else if(c == 'E' || c == 'e')
                  {
                        strAddChar(&attr,c);
                        state = S_exp1;
                  }else
                  {
                        ungetc(c,source);
                        stoken.type =T_double_T;
                        stoken.value.d= atof(strGetStr(&attr));
                        return stoken;
                  }
            break;
//---------------------------------------------------------
            case S_exp1: //za exponentom musi nasledovat cislo alebo +/-
                  if(c == '+' || c == '-')
                  {
                        strAddChar(&attr,c);
                        state = S_exp2;
                  }else if(isdigit(c))
                  {
                        strAddChar(&attr,c);
                        state = S_exp3;
                  }else
                  {
                        state = S_error;
                  }
                  break;
//---------------------------------------------------------
            case S_exp2:
                  if(isdigit(c))
                  {
                        strAddChar(&attr,c);
                        state = S_exp3;
                  }else
                  {
                        state = S_error;
                  }
                  break;
//---------------------------------------------------------
            case S_exp3:
                  if(isdigit(c))
                  {
                        strAddChar(&attr,c);
                        state = S_exp3;
                  }else
                  {
                        ungetc(c,source); //ak nacitam nieco ine ako cislo vrati ho
                        stoken.type =T_double_T;
                        stoken.value.d= atof(strGetStr(&attr));
                        return stoken;
                  }
                  break;
//---------------------------------------------------------
            case S_slash:
                  if(c == '/')
                  {
                        state = S_linecom;
                  }else if(c == '*')
                  {
                        state = S_blockcom;
                  }else
                  {
                        ungetc(c,source);
                        stoken.type =T_div;
                        return stoken;
                  }
                  break;
//------------------------------------------------------------
            case S_linecom:
                  if(c == '\n' || c == EOF)
                  {
                        state = S_start;
                  }else
                        state = S_linecom;
                  break;
//--------------------------------------------------------------
            case S_blockcom:
                  if(c == '\n')
                  {
                        state = S_blockcom;
                  }else if(c == '*')
                  {
                        state = S_rblockcom;
                  }else
                        state = S_blockcom;
                  break;
//--------------------------------------------------------------
            case S_rblockcom:
                  if(c == '/')
                  {
                        state = S_start;
                  }else
                        state = S_blockcom;
                  break;
//--------------------------------------------------------------
            case S_NotEqual:
                  if(c == '=')
                  {
                        stoken.type =T_notEqual;
                        return stoken;
                  }else
                  {
                        state = S_error;
                  }
                  break;
//--------------------------------------------------------------
            case S_EqOrAss:
                  if(c == '=')
                  {
                        stoken.type =T_equal;
                        return stoken;
                  }else
                  {
                        ungetc(c,source);
                        stoken.type =T_assign;
                        return stoken;
                  }
                  break;
//--------------------------------------------------------------
            case S_LeftBeak:
                  if(c == '=')
                  {
                        stoken.type =T_lessEqual;
                        return stoken;
                  }else
                  {
                        ungetc(c,source);
                        stoken.type =T_less;
                        return stoken;
                  }
                  break;
//--------------------------------------------------------------
            case S_RightBeak:
                  if(c == '=')
                  {
                        stoken.type =T_greaterEqual;
                        return stoken;
                  }else
                  {
                        ungetc(c,source);
                        stoken.type =T_greater;
                        return stoken;
                  }
                  break;
//--------------------------------------------------------------
            case S_str:
                  if(c == '\"')
                  {
                        //strAddChar(&attr,c);
                        stoken.type =T_string_T;
                        stoken.value.s=my_malloc(sizeof(char)*strlen(strGetStr(&attr)));
                        strcpy(stoken.value.s, strGetStr(&attr));
                        return stoken;
                  }else if(c == '\\')
                  {
                        state = S_EscSeq;
                  }else if(c == EOF || c == '\n')
                  {
                        state = S_error;
                        break;
                  }else if(c > 31)
                  {
                        strAddChar(&attr,c);
                        state = S_str;
                  }
                  break;
//--------------------------------------------------------------
            case S_EscSeq:
                  if(c == '\"')
                  {
                       strAddChar(&attr,'\"');
                       state = S_str;
                  }else if(c == '\\')
                  {
                        strAddChar(&attr,'\\');
                        state = S_str;
                  }else if(c == 'n')
                  {
                        strAddChar(&attr,'\n');
                        state = S_str;
                  }else if(c == 't')
                  {
                        strAddChar(&attr,'\t');
                        state = S_str;
                  }else if(c == EOF)
                  {
                        state = S_error;
                        break;
                  }else if(c >= '0' && c <= '3')
                  {
                        string pom;
                        strInit(&pom);
                        strAddChar(&pom,c);
                        char c_pom = fgetc(source);
                        if(c_pom >= '0' && c_pom <= '7')
                        {
                              strAddChar(&pom,c_pom);
                        }
                        c_pom = fgetc(source);
                        if(c_pom >= '0' && c_pom <= '7')
                        {
                              strAddChar(&pom,c_pom);
                        }
                        int o = (int)strtol(pom.str,NULL,8);
                        strFree(&pom);
                        strAddChar(&attr,o);
                        state = S_str;
                  }else
                  {
                        state = S_error;
                  }
                  break;
//--------------------------------------------------------------
            case S_error:
                  stoken.type =T_LEX_ERROR;
                  return stoken;
         }
      }
}
