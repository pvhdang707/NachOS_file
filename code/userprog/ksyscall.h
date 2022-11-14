/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#define __STDC_LIMIT_MACROS
#include "kernel.h"
#include "synchconsole.h"
#include <stdint.h>


#define _maxNumLen 11
char _numberBuffer[_maxNumLen + 2];

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}


void SysReadString(char *buffer,int length)
{
  char ch;
  int i=0;
  do
  {
    ch = (char)kernel->synchConsoleIn->GetChar();
    if (ch!= '\n')
        buffer[i++] = ch;
  }while (ch != '\n' && i < length);
}

void SysPrintString(char *buffer, int limit)
{
  if (buffer != NULL)
  {
    int i = 0;
    while (i < limit && (char)buffer[i] != '\0')
    {
      kernel->synchConsoleOut->PutChar((char)buffer[i]);
      i++;
    }
  }
}



int SysReadNum()
{
  // int max_len = 11; // int [-2147483648 ... 2147483647]
  char buffer[_maxNumLen + 2];
  // ki tu trong buffer =0
  memset(buffer, 0, sizeof(buffer));

  // xu li
  // doc chuoi so vao buffer
  int index = 0;
  char ch = kernel->synchConsoleIn->GetChar();
  while ((ch != '\n'))
  {
    buffer[index++] = ch;
    if (index > _maxNumLen)
    {
      DEBUG(dbgSys, "Number is too long \n");
      return 0;
    }
    ch = kernel->synchConsoleIn->GetChar();
  }
  // chuyen buffer thanh so
  int result = 0; // luu so chuyen tu buffer
  int temp = 0;   // bien tam
  if (index < 11) // max_len = 11 <=> int [-2147483648 ... 2147483647]
  {
    int start;
    if (buffer[0] == '-') // so am
      start = 1;
    else
      start = 0;

    for (int i = start; i < index; i++)
    {
      if (buffer[i] >= '0' && buffer[i] <= '9') // ki tu phai la so
      {
        temp = buffer[i] - 48; // chuyen ki tu so => so
        result = result * 10 + temp;
      }
      else // ki tu khong phai la so
      {
        result = 0;
        break;
      }
    }
  }
  if (buffer[0] == '-') // doi thanh so am
    {result = result * -1;}

  return result;
}

void SysPrintNum(int result)
{
  char buffer[13];
  if (result == 0)
    kernel->synchConsoleOut->PutChar('0');
  else
  {
    if (result < 0) // so am
    {
      kernel->synchConsoleOut->PutChar('-'); // in dau tru
      result = result * (-1);                // chuyen thanh so duong
    }
    int n = 0;
    while (result)
    {
      buffer[n++] = result % 10;
      result /= 10;
    }
    for (int i = n - 1; i >= 0; --i)
      kernel->synchConsoleOut->PutChar(buffer[i] + '0');
  }
}

char SysReadChar()
{
  return kernel->synchConsoleIn->GetChar();
}

void SysPrintChar(char character)
{
  kernel->synchConsoleOut->PutChar(character);
}

int SysCreateFile(char *filename)
{
  if(filename==NULL || kernel->fileSystem->Create(filename, 0)==FALSE)
   {
    
    return -1;
  }
  return 0;
}


#endif /* ! __USERPROG_KSYSCALL_H__ */
