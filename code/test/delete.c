#include "syscall.h"
int main()
{
    char filename[255];

    // Read filename
    PrintString("Nhap ten file muon xoa: ");
    ReadString(filename, 255);
    PrintString("\n");

    if (Remove(filename) == 0)
    {
        PrintString("Xoa ");
        PrintString(filename);
        PrintString(" hoan tat!\n");
    }
    else
    {
        PrintString("Xoa file that bai!\n");
    }
    
    Halt();
 
}