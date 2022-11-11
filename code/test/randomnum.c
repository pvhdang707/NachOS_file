#include "syscall.h"
/*Random number generating test*/

int main() {
    int result;
    result = RandomNum();
    PrintNum(result);
    PrintChar('\n');
    Halt();
}