#include "syscall.h"
int main()
{
    int i, size, openFileId;
    char c, filename[255];

    // Read filename
    PrintString("nhap ten file: ");
    ReadString(filename, 255);
    PrintString("\n");

    // Open file
    openFileId = Open(filename);

    // in noi dung file
    if (openFileId >= 0)
    {
        PrintString("noi dung:\n");

        // lay file size
        size = Seek(-1, openFileId);
        Seek(0, openFileId);

        // in ra console
        for (i = 0; i < size; i++)
        {
            Read(&c, 1, openFileId);
            PrintChar(c);
        }

        PrintString("\n\n");

        // Close file
        Close(openFileId);
    }
    else
    {
        PrintString("khong the mo file!\n");
    }

    Halt();
  
}