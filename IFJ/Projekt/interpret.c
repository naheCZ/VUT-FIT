/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   interpret.c
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */
//
#include <stdio.h>
#include "str.h"
#include "ilist.h"
#include "ial.h"
#include "functions.h"
#include "error.h"


tError inter(tListOfInstr *instrList)
{

  int debug_interpret = 3;
  listFirst(instrList);
  tInstr *ins = NULL;
  tData *op1 = NULL;
  tData *op2 = NULL;
  tData *res = NULL;
  int l = 0;




  while (1)
  {
    ins = listGetData(instrList);
    int i = 1;
    if (debug_interpret > 1) printf("Nactena instrukce %d\n",ins->instCode);
    switch (ins->instCode)
    {
    case I_Stop:
    if (debug_interpret > 1) printf("konec interpetu\n");
      // instrukce konce programu
       return OK;
      break;

    case I_Lab:
      // instrukce navesti
      // nic se neprovadi
      break;

    case I_Assign:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;

        printf("%s\n", res->name);
        printf("%d\n", op1->data_type);
        printf("%d\n", res->data_type);

        if(res->data_type == DOUBLE && op1->data_type == DOUBLE)
        {
          if (debug_interpret > 1) printf("Prirazeni\n");
          res->data_type = DOUBLE;
          res->value.d = op1->value.d;
        }
        else if(res->data_type == DOUBLE && op1->data_type == INT)
        {
          if (debug_interpret > 1) printf("Prirazeni\n");
          res->data_type = DOUBLE;
          res->value.d = op1->value.i;
        }
        else if(res->data_type == INT && op1->data_type == INT)
        {
          if (debug_interpret > 1) printf("Prirazeni\n");
          res->value.i = op1->value.i;
        }
        else if(res->data_type == INT && op1->data_type == DOUBLE)
        {
          if (debug_interpret > 1) printf("Prirazeni\n");
          res->value.i = op1->value.d;
        }
        else if(res->data_type == STRING && op1->data_type == STRING)
        {
          if (debug_interpret > 1) printf("Prirazeni\n");

	        int len = length_char(op1->value.s);
	        char *tmp = my_malloc(len + 1);
	        if (tmp == NULL) {
		        return E_INTERNAL;
	        }
	        memcpy(tmp, op1->value.s, len + 1);

	        my_free(res->value.s);
	        res->value.s = tmp;

         }
         else
           return E_SEMANTIC_INCOMPATIBLE;

        break;

      case I_Add:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;

        if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
        {
          res->data_type = DOUBLE;
          res->value.d = op1->value.d + op2->value.d;
        }
        else if((op1->data_type == INT) && (op2->data_type == INT))
        {
          res->value.i = op1->value.i + op2->value.i;
          if (debug_interpret > 1) printf("Scitani %d\n", res->value.i);
        }
        else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
        {
          res->data_type = DOUBLE;
          res->value.d = op1->value.i + op2->value.d;
        }
        else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
        {
          res->data_type = DOUBLE;
          res->value.d = op1->value.d + op2->value.i;
        }
        else if((op1->data_type == STRING) && (op2->data_type == STRING))
        {
          res->value.s = concat_char(op1->value.s,op2->value.s);
        }
        else
        {
           return E_SEMANTIC_INCOMPATIBLE;
        }

        break;

        case I_Mul:
          op1 = (tData*)ins->op1;
          op2 = (tData*)ins->op2;
          res = (tData*)ins->result;

          if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
          {
            res->data_type = DOUBLE;
            res->value.d = op1->value.d * op2->value.d;
          }
          else if((op1->data_type == INT) && (op2->data_type == INT))
          {
            res->value.i = op1->value.i * op2->value.i;
          }
          else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
          {
            res->data_type = DOUBLE;
            res->value.d = op1->value.i * op2->value.d;
          }
          else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
          {
            res->data_type = DOUBLE;
            res->value.d = op1->value.d * op2->value.i;
          }
          else
          {
             return E_SEMANTIC_INCOMPATIBLE;
          }
          break;

          case I_Div:
            op1 = (tData*)ins->op1;
            op2 = (tData*)ins->op2;
            res = (tData*)ins->result;
            if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
            {
              if(op2->value.d == 0){
                 return E_RUN_DIV_BY_ZERO;
              }
              res->data_type = DOUBLE;
              res->value.d = op1->value.d / op2->value.d;
            }
            else if((op1->data_type == INT) && (op2->data_type == INT))
            {
              if(op2->value.i == 0){
                return E_RUN_DIV_BY_ZERO;
              }
              if ((op1->value.i % op2->value.i) == 0) {
                res->data_type = INT;
                res->value.i = op1->value.i / op2->value.i;
              } else {
                res->data_type = DOUBLE;
                res->value.d = (double) op1->value.i / op2->value.i;
              }
            }
            else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
            {
              if(op2->value.d == 0){
                return E_RUN_DIV_BY_ZERO;
              }
              res->data_type = DOUBLE;
              res->value.d = op1->value.i / op2->value.d;
            }
            else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
            {
              if(op2->value.i == 0){
                return E_RUN_DIV_BY_ZERO;
              }
              res->data_type = DOUBLE;
              res->value.d = op1->value.d / op2->value.i;
            }
            else
            {
              return E_SEMANTIC_INCOMPATIBLE;
            }

          break;

      case I_Sub:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;

        if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
        {
          res->data_type = DOUBLE;
          res->value.d = op1->value.d - op2->value.d;
        }
        else if((op1->data_type == INT) && (op2->data_type == INT))
        {
          res->value.i = op1->value.i - op2->value.i;
        }
        else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
        {
          res->data_type = DOUBLE;
          res->value.d = ((double) op1->value.i) - op2->value.d;
        }
        else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
        {
          res->data_type = DOUBLE;
          res->value.d = op1->value.d - op2->value.i;
        }

        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }

        break;

      case I_Less:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;

        if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.d < op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == INT))
        {
          if(op1->value.i < op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.i < op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;
            }
        }
        else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
        {
          res->data_type = INT;
          if(op1->value.d < op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == STRING) && (op2->data_type == STRING))
        {
          res->data_type = INT;
          if(strcmp(op1->value.s,op2->value.s)< 0)
			res->value.i = 1;
          else
			res->value.i = 0;

        }

        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }

        break;

      case I_Greater:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;

        if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.d > op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == INT))
        {
          if(op1->value.i > op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.i > op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
        {
          res->data_type = INT;
          if(op1->value.d > op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == STRING) && (op2->data_type == STRING))
        {
          res->data_type = INT;
          if(strcmp(op1->value.s,op2->value.s)> 0)
			res->value.i = 1;
          else
			res->value.i = 0;

        }
        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }
        break;

      case I_LessEq:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;

        if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.d <= op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == INT))
        {
          if(op1->value.i <= op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.i <= op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
        {
          res->data_type = INT;
          if(op1->value.d <= op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == STRING) && (op2->data_type == STRING))
        {
          res->data_type = INT;
          if(strcmp(op1->value.s,op2->value.s)<= 0)
			res->value.i = 1;
          else
			res->value.i = 0;

        }

        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }


        break;

      case I_GreaterEq:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;

        if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.d >= op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == INT))
        {
          if(op1->value.i >= op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.i >= op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
        {
          res->data_type = INT;
          if(op1->value.d >= op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == STRING) && (op2->data_type == STRING))
        {
          res->data_type = INT;
          if(strcmp(op1->value.s,op2->value.s)>= 0)
        res->value.i = 1;
          else
        res->value.i = 0;

        }
        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }
        break;

      case I_Equal:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;

        if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.d == op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == INT))
        {
          if(op1->value.i == op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.i == op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
        {
          res->data_type = INT;
          if(op1->value.d == op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == STRING) && (op2->data_type == STRING))
        {
          res->data_type = INT;
          if(strcmp(op1->value.s,op2->value.s)== 0)
        res->value.i = 1;
          else
        res->value.i = 0;

        }

        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }

        break;

      case I_NotEqual:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;

        if((op1->data_type == DOUBLE) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.d != op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == INT))
        {
          if(op1->value.i != op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == INT) && (op2->data_type == DOUBLE))
        {
          res->data_type = INT;
          if(op1->value.i != op2->value.d)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == DOUBLE) && (op2->data_type == INT))
        {
          res->data_type = INT;
          if(op1->value.d != op2->value.i)
          res->value.i = 1;
          else
            {res->value.i = 0;}
        }
        else if((op1->data_type == STRING) && (op2->data_type == STRING))
        {
          res->data_type = INT;
          if(strcmp(op1->value.s,op2->value.s)!= 0)
        res->value.i = 1;
          else
        res->value.i = 0;

        }

        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }
        break;

      case I_Lenght: /* spocita delku retezce */
        op1 = (tData*)ins->op1;
        res = (tData*)ins->result;
        if(op1->data_type == STRING)
        {
          res->value.i = length_char(op1->value.s);
        }
        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }
        break;

      case I_Print:
        op1 = (tData*)ins->op1;
        if(op1->data_type == INT)
        {
          printf("%d",op1->value.i);
        }
        else if(op1->data_type == DOUBLE)
        {
          printf("%g",op1->value.d);
        }
        else if(op1->data_type == STRING)
        {
          printf("%s",op1->value.s);
        }
        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }
        break;

      case I_Compare:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;
        if((op1->data_type == STRING) && (op2->data_type == STRING))
        {
          res->value.i = strcmp(op1->value.s,op2->value.s);
        }
        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }
        break;

      case I_Find:
        op1 = (tData*)ins->op1;
        op2 = (tData*)ins->op2;
        res = (tData*)ins->result;
        if((op1->data_type == STRING) && (op2->data_type == STRING))
        {
          res->value.i = find_char(op1->value.s,op2->value.s);
        }
        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }
        break;

      case I_Sort:
        op1 = (tData*)ins->op1;
        res = (tData*)ins->result;
        if(op1->data_type == STRING)
        {
          res->value.s = sort_char(op1->value.s);
        }
        else
        {
          return E_SEMANTIC_INCOMPATIBLE;
        }
        break;


       case I_IfGoTo:
         op1 = (tData*)ins->op1;
         op2 = (tData*)ins->op2;

         if(op1->data_type == INT){
           if (op1->value.i != 0) {
             if (debug_interpret > 1) printf("I_IfGoTo - skáču¨\n");
             listFirst(instrList);

             i = 0;
             while(i+1 != op2->value.i){
               listNext(instrList);
               i++;
             }
           }
         }else if(op1->data_type == DOUBLE){
            if (op1->value.d != 0) {
           if (debug_interpret > 1) printf("I_IfGoTo - skáču¨\n");
           listFirst(instrList);

           i = 0;
           while(i+1 != op2->value.i){
             listNext(instrList);
             i++;
           }
         }
       }else if(op1->data_type == STRING){
         if (op1->value.s == 0) {
         if (debug_interpret > 1) printf("I_IfNGoTo - skáču¨\n");
         listFirst(instrList);

         i = 0;
         while(i+1 != op2->value.i){
           listNext(instrList);
           i++;
         }
       }
     } else
          if (debug_interpret > 1) printf("I_IfGoTo - neskáču¨\n");

         break;

       case I_GoTo:
         op2 = (tData*)ins->op2;
         l=ins->line;
         if (debug_interpret > 1) printf("I_GoTo - skáču na %d a %d\n",op2->value.i,l);
         listFirst(instrList);
         i = 0;
         while(i+1 != op2->value.i){
           listNext(instrList);
           i++;
         }
         break;

       case I_IfNGoTo:
         op1 = (tData*)ins->op1;
         op2 = (tData*)ins->op2;
         printf("cond: %d\n",op1->value.i);
         if(op1->data_type == INT){
           if (op1->value.i == 0) {
             if (debug_interpret > 1) printf("I_IfNGoTo - skáču¨\n");
             listFirst(instrList);

             i = 0;
             while(i+1 != op2->value.i){
               listNext(instrList);
               i++;
             }
           }
         }else if(op1->data_type == DOUBLE){
           if (op1->value.d == 0) {
           if (debug_interpret > 1) printf("I_IfNGoTo - skáču¨\n");
           listFirst(instrList);

           i = 0;
           while(i+1 != op2->value.i){
             listNext(instrList);
             i++;
           }
         }
       }else if(op1->data_type == STRING){
         if (op1->value.s == 0) {
         if (debug_interpret > 1) printf("I_IfNGoTo - skáču¨\n");
         listFirst(instrList);

         i = 0;
         while(i+1 != op2->value.i){
           listNext(instrList);
           i++;
         }
       }
     }else
      if (debug_interpret > 1) printf("I_IfNGoTo - neskáču¨\n");
         break;


       case I_ReadString:
         op1 = (tData*)ins->op1;
         op1->data_type = STRING;
         int c;

         string *str = my_malloc(sizeof(string));
         strInit(str);
         while((c=getchar()) != EOF && c != '\n')
         {
          strAddChar(str,c);
         }

         op1->value.s = str->str;
         break;

        case I_ReadInt:
          op1 = (tData*)ins->op1;
          op1->data_type = INT;
          if (scanf("%d", &op1->value.i) <= 0){
            return E_RUN_IN_DIGIT;
          }
          break;

        case I_ReadDouble:
          op1 = (tData*)ins->op1;
          op1->data_type = DOUBLE;
          if (scanf("%lf", &op1->value.d) <= 0){
            return E_RUN_IN_DIGIT;
          }
          break;

        case I_NOP:
          break;

        case I_Func:
          res = (tData*)ins->result;
          l=ins->line;
          if (debug_interpret > 1) printf("Funkce - skáču na %d ze radku %d\n",res->value.i,l);
          listFirst(instrList);
          i = 0;
          while(i+1 != res->value.i){
            listNext(instrList);
            i++;
          }
          break;

        case I_Ret:
          res = (tData*)ins->result;
          if (debug_interpret > 1) printf("Return - skáču na %d\n",l);
          listFirst(instrList);
          i = 0;
          while(i+1 != l){
            listNext(instrList);
            i++;
          }
          break;

    }
if (debug_interpret > 1) printf("konec instrukce\n");
    // prejdeme na dalsi instrukci
    // POZOR! Pokud byl proveden skok, nemelo by se posouvat na dalsi instrukci!
    // Protoze ale vime, ze skok mohl byt proveden pouze na navesti a v ramci
    // navesti se nic neprovadi, muzeme si dovolit prejit na dalsi instrukci i
    // v tomto pripade, pouze tim urychlime beh programu
    listNext(instrList);

  }
}
