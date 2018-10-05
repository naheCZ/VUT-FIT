/*
  Autor: Roman Nahálka, xnahal01
  Zmìny: 85%
  Datum: 15.12.2016
*/
/*******************************************************************************
   main.c: LCD + keyboard demo
   Copyright (C) 2009 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): Zdenek Vasicek <vasicek AT stud.fit.vutbr.cz>

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.

   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$


*******************************************************************************/

#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>
#include <stdio.h>
#include <string.h>

#define LENGTH 33

char last_ch; //naposledy precteny znak
char lastKey; //Naposledy stisknuta klavesa
int zeroCount = 0; //Pocitani nestiknuti klavesy
int mode = 0; //Zadavani textu nebo efekty
int blickMode = 0;
int leftRotation = 0;
int rightRotation = 0;
int firstKey = 0; //Nastavi se, pokud uz byla stisknuta klavesa
char text[LENGTH];
char rotation[LENGTH];
int myIndex = 0;


/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
*******************************************************************************/
void print_user_help(void)
{
}

//Funkce na kontrolu klaves, zjisti, jestli se jedna o klavesy s pismeny.
int control(char ch)
{
  switch(ch)
  {
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return 1;
    
    default:
      return 0;
  }
}

char getFirstChar(char ch)
{
  switch(ch)
  {
    case '2':
      return 'A';
    case '3':
      return 'D';
    case '4':
      return 'G';
    case '5':
      return 'J';
    case '6':
      return 'M';
    case '7':
      return 'P';
    case '8':
      return 'T';
    case '9':
      return 'W';
    default:
      return ch;
  }
}

int isCharLast(char ch)
{
  switch(ch)
  {
    case 'C':
    case 'F':
    case 'I':
    case 'L':
    case 'O':
    case 'S':
    case 'V':
    case 'Z':
      return 1;
    default:
      return 0;  
  }
}

void deleteChar(char ch)
{  
  if(myIndex <= 0)
    myIndex = 0;
  
  else
  {
  
    if(text[myIndex] != 0)
      myIndex++;
    
    myIndex--;
    text[myIndex] = 0;
    LCD_clear();
    LCD_append_string(text);
  }
}

void setRotation()
{
  memset(rotation, 0, LENGTH - 1);
  strcpy(rotation, text);
  int i;
  for(i = 0; i < LENGTH - 1; i++) //Nulove znaky naplnime mezerami
  {
    if(rotation[i] == 0)
      rotation[i] = ' ';
  }
}

void editMode(char ch)
{
  if(ch == 0 && ch == last_ch && firstKey != 0)
    {
      zeroCount++;
      last_ch = ch;  
    }
    
  else if(ch == lastKey && control(ch) && zeroCount != 0 && firstKey == 1)
  {
    zeroCount = 0;
    lastKey = ch;
    last_ch = ch;
    
    if(text[myIndex] >= '2' && text[myIndex] <= '9')
      text[myIndex] = getFirstChar(ch);
    
    else if(isCharLast(text[myIndex]))
      text[myIndex] = ch;
    
    else
      text[myIndex]++;
      
    LCD_clear();
    LCD_append_string(text);
  }
  
  else if(ch == lastKey && ch == '1' && zeroCount != 0)
  {
    zeroCount = 0;
    lastKey = ch;
    last_ch = ch;
    
    deleteChar(ch);
  }
  
  else if(ch == lastKey && ch == '0' && zeroCount != 0)
  {
    zeroCount = 0;
    lastKey = ch;
    last_ch = ch;
    
    if(text[myIndex] != 0)
      myIndex++;
      
    text[myIndex] = ' ';
    LCD_append_char(text[myIndex]);
  }
  
  
  else if (ch != last_ch) 
  {
     last_ch = ch;
     
    if(ch == '1')
    {
        zeroCount = 0;
        firstKey = 0;
        lastKey = ch;
        
        deleteChar(ch);
    }
    
    else if(ch == '0')
    {
      zeroCount = 0;
      firstKey = 0;
      lastKey = ch;
      
      if(text[myIndex] != 0)
        myIndex++;
        
      text[myIndex] = ' ';
      LCD_append_char(text[myIndex]);
    }
    
    else if(ch == '*')
    {
      LCD_clear();
      LCD_write_string(text);
      
      mode = 0;
      firstKey = 0;
      lastKey = ch;
      zeroCount = 0;
    }
    
    else if(ch != 0 && control(ch))
    {
      if(text[myIndex] != 0)
        myIndex++;
      
      zeroCount = 0;
      firstKey = 1;
      lastKey = ch;

      text[myIndex] = getFirstChar(ch);
      LCD_append_char(text[myIndex]);  
    }
    
    else if(ch == 0)
      zeroCount = 1;
  }
  
  if(zeroCount >= 16)
  {
    zeroCount = 0;
    firstKey = 0;
    myIndex++;
  }
}

void readMode(char ch)
{
  if(ch == 0)
  {
    last_ch = ch;
    
    if (blickMode)
    {
      LCD_clear();
      delay_ms(200);
      LCD_append_string(text);
    }
    
    else if(leftRotation)
    {
       int i;
       char tmp;
       
       for(i = 0; i < LENGTH - 1; i++)
       {         
          if(i == 0)
          {
            tmp = rotation[i];
            rotation[i] = rotation[i + 1];          
          }
          
          else if(i == LENGTH - 2)
            rotation[i] = tmp;
            
          else
            rotation[i] = rotation[i + 1];
       }
       
       LCD_clear();
       LCD_append_string(rotation);
       delay_ms(250);
    }
    
    else if(rightRotation)
    {
      int i;
      char tmp;
      
      for(i = LENGTH - 2; i >= 0; i--)
      {
         if(i == LENGTH - 2)
         {
            tmp = rotation[i];
            rotation[i] = rotation[i - 1];
         }
         
         else if(i == 0)
          rotation[i] = tmp;
          
         else
          rotation[i] = rotation[i - 1];
      }
      
      LCD_clear();
      LCD_append_string(rotation);
      delay_ms(250);
    }
  }
  
  else if (ch != last_ch) 
  {
    last_ch = ch;
    
    if(ch == '#')
    {
      mode = 1;
      leftRotation = 0;
      rightRotation = 0;
      blickMode = 0;
    }
    
    else if (ch != 0 && !control(ch)) 
    {
      if(ch == 'A' && blickMode == 0)
      {
        blickMode = 1;
        leftRotation = 0;
        rightRotation = 0;
      }
      
      else if(ch == 'A' && blickMode == 1)
        blickMode = 0;
        
      else if(ch == 'B' && leftRotation == 0)
      {
        blickMode = 0;
        rightRotation = 0;
        leftRotation = 1;
        setRotation();
      }
      
      else if(ch == 'B' && leftRotation == 1)
      {
        leftRotation = 0;
        LCD_clear();
        LCD_append_string(text);
      }
      
      else if(ch == 'C' && rightRotation == 0)
      {
        blickMode =  0;
        leftRotation = 0;
        rightRotation = 1;
        setRotation();
      }
      
      else if(ch == 'C' && rightRotation == 1)
      {
        rightRotation = 0;
        LCD_clear();
        LCD_append_string(text);
      } 
    }
  }
}

/*******************************************************************************
 * Obsluha klavesnice
*******************************************************************************/
int keyboard_idle()
{
  char ch;
  ch = key_decode(read_word_keyboard_4x4());
  
  if(mode)
    editMode(ch);
    
  else
    readMode(ch);     
  
  return 0;
}



/*******************************************************************************
 * Dekodovani a vykonani uzivatelskych prikazu
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return CMD_UNKNOWN;
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized()
{
  LCD_init();
  LCD_clear();
}


/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
  text[LENGTH - 1] = '\0';

  initialize_hardware();
  keyboard_init();

  set_led_d6(1);                       // rozsviceni D6
  set_led_d5(1);                       // rozsviceni D5

  while (1)
  {
    delay_ms(75);
    keyboard_idle();                   // obsluha klavesnice
    terminal_idle();
  }

  return 0;
}
 