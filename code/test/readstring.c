#include "syscall.h"
/*String input and output test*/

int main() {
    char buffer[256];
    PrintString("String length: (<= 255):\n");
    ReadString(buffer, ReadNum());
    PrintString("String input :\n");
    PrintString(buffer);
    PrintString("\n");
    Halt();
}