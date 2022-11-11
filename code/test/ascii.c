#include "syscall.h"

int main() {
    // Các kí tự đọc được trong khoảng từ 33 đến 126.
    int i;
    PrintString("\n==== ASCII TABLE =====\n");
    PrintString("Code\tCharacter\n");
    for (i = 33; i < 127; i++) {
        PrintNum(i);
        PrintChar('\t');
        PrintChar((char)i);
        PrintChar('\n');
    }
    PrintString("-----------------------\n");
    Halt();
}