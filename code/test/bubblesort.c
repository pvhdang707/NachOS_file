#include "syscall.h"

int GoodOrder(int curr, int next, int asc) {
    if (asc == 1)
        return (curr <= next) ? 1 : 0;
    else
        return (curr >= next) ? 1 : 0;
}

void BubbleSort(int* a, int n, int asc) {
    int left = 0, right = n - 1;
    while (left <= right) {
        int isSwapped = 0;
        int lastSwapPos = left + 1;
        int j;
        for (j = left; j < right; ++j) {
            if (!GoodOrder(a[j], a[j+1], asc)) {
                int temp;
                temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
                isSwapped = 1;
                lastSwapPos = j + 1;
            }
        }
        if (isSwapped == 0)
            break;
        right = lastSwapPos;
    }
}

int main() {
    
    unsigned int n;
    int a[100];
    int isAscending, i;
    
    // Nhap kich thuoc mang
    do {
        PrintString("Nhap kich thuoc mang (n): ");
        n = ReadNum();
    // Thong bao khi kich thuoc mang nhap sai
        if (n < 1 || n > 100)
            PrintString("Kich thuoc mang nam ngoai pham vi hop le \n");

    } while (n < 1 || n > 100);

    // Nhap gia tri 
    
    
    for (i = 0; i < n; i++){
        PrintString("Nhap gia tri thu ");
        PrintNum(i);
        PrintString(":\t");
        a[i] = ReadNum();
    }
    // Lua chon tang hoac giam mang
    PrintString("Tang Dan hoac Giam Dan ( Nhap 1 : Tang Dan , Nhap 2: Giam Dan ): ");
    do {
        isAscending = ReadNum();
        if (!(isAscending == 1 || isAscending == 2))
            PrintString("CHI CHON 1 HOAC 2 !!!)\n");
    } while (isAscending != 1 && isAscending != 2);

    BubbleSort(a, n, isAscending);

    PrintString("Ket Qua: ");
    for (i = 0; i < n; i++) {
        PrintNum(a[i]); PrintChar('\t');
    }
    PrintChar('\n');

    Halt();
}