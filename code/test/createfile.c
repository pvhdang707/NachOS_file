#include "syscall.h"

#define MAX_LENGTH 255

int main()
{
    char filename[MAX_LENGTH];

    PrintString("nhap ten file: \n");
    ReadString(filename, MAX_LENGTH);
    PrintString("\n\n");
    if (Create(filename) == 0)
    {
        PrintString("Create file ");
        PrintString(filename);
        PrintString(" thanh cong!\n");
    }
    else
    {
        PrintString("tao file that bai!\n");
    }
    
    Halt();

}