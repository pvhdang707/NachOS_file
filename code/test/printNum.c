#include "syscall.h"

int main()
{
    int temp;
    temp=ReadNum();
    PrintNum(temp);
    PrintChar('\n');
    
    Halt();
}