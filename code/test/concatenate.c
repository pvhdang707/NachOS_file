#include "syscall.h"
int main()
{
    int i, size, ID1, ID2;
    char c, filename_1[255], filename_2[255];

    //file 1
    PrintString("Nhap ten file 1: ");
    ReadString(filename_1, 255);

    ID1 = Open(filename_1);
    if (ID1 == -1)
    {
        PrintString("Khong the mo file!\n");
        Halt();
        return 0;
    }

    //file 2
    PrintString("Nhap ten file 2: ");
    ReadString(filename_2, 255);
    ID2 = Open(filename_2);
    if (ID2 == -1)
    {
        PrintString("Khong the mo file!\n");
        Halt();
        return 0;
    }

    //tien hanh noi file 1 vao file 2
    //lay kich thuoc file 1
    size = Seek(-1, ID1);
    Seek(0, ID1);

    Seek(-1, ID2);

    //noi file 1 vao file 2
    for (i = 0; i < size; i++)
    {
        Read(&c, 1, ID1);
        Write(&c, 1, ID2);
    }

    // Close file
    Close(ID1);
    Close(ID2);

    PrintString("Noi thanh cong!!!");
    PrintString("\n");

    Halt();

}