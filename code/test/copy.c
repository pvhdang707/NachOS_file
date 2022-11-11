#include "syscall.h"
int main()
{
    
    int i, size, openFileId_1, openFileId_2;
    char c, filename_1[255], filename_2[255];

    // Read filename 1
    PrintString("nhap ten file 1: ");
    ReadString(filename_1, 255);
    PrintString("\n");


    // Open file 1
    openFileId_1 = Open(filename_1);
  
    // Copy noi dung
    if (openFileId_1 >= 0)
    {
        // Read filename 2
        PrintString("nhap ten file 2: ");
        ReadString(filename_2,  255);
        PrintString("\n");
       

        // Create file 2
        Create(filename_2);

        // Open file 2
        openFileId_2 = Open(filename_2);

        // lay file size
        size = Seek(-1, openFileId_1);
        Seek(0, openFileId_1);

        // Copy 
        for (i = 0; i < size; i++)
        {
            Read(&c, 1, openFileId_1);
            Write(&c, 1, openFileId_2);
        }

        // Close file
        Close(openFileId_1);
        Close(openFileId_2);

        PrintString("Copy noi dung hoan tat!\n");
    }
    else
    {
        PrintString("khong the mo file!\n");
    }

    Halt();
}