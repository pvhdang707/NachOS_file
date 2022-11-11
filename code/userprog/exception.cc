// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "machine.h"
#include <malloc.h>
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

/*
 * Hàm tăng giá trị thanh ghi PC.
 * Đầu vào: không có.
 * Đầu ra: không có.
 */
void IncreasePC()
{
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

/*
 * Hàm sao chép chuỗi bytes từ user space sang kernel space.
 * Đầu vào:
 * - int virtAddr: Địa chỉ vùng nhớ user space.
 * - int limit: Số bytes tối đa cần sao chép.
 * Đầu ra:
 * - char*: Con trỏ đến vùng nhớ đệm trên Kernel space chứa dữ liệu đã được sao chép từ user space.
 */
char *User2System(int virtAddr, int limit)
{
	int i;
	int oneChar = 0;

	char *kernelBuf = NULL;
	kernelBuf = (char *)malloc(limit + 1); // chứa thêm kí tự ngắt chuỗi
	if (kernelBuf == NULL)
		return kernelBuf;

	memset(kernelBuf, 0, limit + 1);

	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(int(virtAddr + i), 1, &oneChar);
		kernelBuf[i] = (char)oneChar;

		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}
/*
 * Hàm sao chép chuỗi bytes từ kernel space sang user space.
 * Đầu vào:
 * - int virtAddr: Địa chỉ vùng nhớ trên user space.
 * - int len: Số byte tối đa cần sao chép.
 * - char* buffer: Con trỏ đến vùng nhớ trên kernel space.
 * Đầu ra:
 * - int: Số byte đã được sao chép.
 */
int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
	switch (which)
	{
	 case NoException:  // return control to kernel
            kernel->interrupt->setStatus(SystemMode);
            DEBUG(dbgSys, "Switch to system mode\n");
            break;
    case PageFaultException:
    case ReadOnlyException:
    case BusErrorException:
    case AddressErrorException:
    case OverflowException:
    case IllegalInstrException:
    case NumExceptionTypes:
            cerr << "Error " << which << " occurs\n";
            SysHalt();
            ASSERTNOTREACHED();
	}
	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;
		}
		//-----------------------------------------------
		case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		//-----------------------------------------------
		case SC_RandomNum:
		{
			DEBUG(dbgSys, "System call RandomNum invoked!");
			int generatedRandomNum;
			generatedRandomNum = SysRandomNum();
			DEBUG(dbgSys, "RandomNum returning with " << generatedRandomNum << "\n");
			kernel->machine->WriteRegister(2, (int)generatedRandomNum);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		//-----------------------------------------------
		case SC_ReadString:
		{
			DEBUG(dbgSys, "Syscall ReadString invoked!");

			// Lấy tham số từ thanh ghi 4 và 5
			int virtAddr, length;
			virtAddr = kernel->machine->ReadRegister(4);
			length = kernel->machine->ReadRegister(5);
			DEBUG(dbgSys, "SC_ReadString: Kernel space buffer address is " << virtAddr);
			DEBUG(dbgSys, "SC_ReadString: Number of characters to read: " << length);

			// Tạo kernel space buffer
			char *buffer;
			buffer = new char[length +1];
			memset(buffer, 0, length + 1);
			if (buffer == NULL)
			{
				DEBUG(dbgSys, "SC_ReadString: ERROR: Failed to allocate memory for kernel space buffer");
				break;
			}

			// Đọc chuỗi từ console vào trong kernel space buffer
			SysReadString(buffer, length);

			DEBUG(dbgSys, "SC_ReadString: ReadString result: " << buffer << "\n");
			

			// Copy chuỗi từ kernel space buffer sang user space buffer
			System2User(virtAddr, length + 1, buffer);

			// Giải phóng bộ nhớ
			free(buffer);

			// Tăng program counter
			IncreasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		//-----------------------------------------------
		case SC_PrintString:
		{
			DEBUG(dbgSys, "Syscall PrintString invoked!");

			// Lấy tham số từ thanh ghi 4
			int virtAddr;
			virtAddr = kernel->machine->ReadRegister(4);
			DEBUG(dbgSys, "SC_PrintString: Kernel space buffer address is " << virtAddr);

			// Chuyển chuỗi từ user space sang kernel space
			char *buffer;
			buffer = NULL;
			const int MAX_STRING = 2048;
			buffer = User2System(virtAddr, MAX_STRING);

			if (buffer == NULL)
			{
				DEBUG(dbgSys, "SC_PrintString: Failed to copy User2System");
				IncreasePC();
				break;
			}

			DEBUG(dbgSys, "SC_PrintString: kernel space buffer content: " << buffer);

			// Write chuỗi ra console
			SysPrintString(buffer, MAX_STRING);

			// Giải phóng bộ nhớ
			free(buffer);

			// Tăng Program Counter
			IncreasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadNum:
		{
			DEBUG(dbgSys, "System call: Read num \n");
			int result = SysReadNum();

			DEBUG(dbgSys, "ReadNum: gia tri tra ve: " << result << "\n");

			kernel->machine->WriteRegister(2, (int)result);

			IncreasePC();

			return;

			ASSERTNOTREACHED();

			break;
		}

		case SC_PrintNum:
		{

			DEBUG(dbgSys, "PrintNum: PrintNum.\n");
			int character = kernel->machine->ReadRegister(4);

			SysPrintNum(character);

			IncreasePC();

			return;

			ASSERTNOTREACHED();

			break;
		}

		case SC_ReadChar:
		{  
			// thong bao
			DEBUG(dbgSys,  "System call: Read char \n");
			// doc ki tu tu sysconsoleIn
			char result = SysReadChar();
            // luu vao thanh ghi
			kernel->machine->WriteRegister(2, (int)result);
            // thong bao doc thanh cong
			DEBUG(dbgSys, "ReadChar: ki tu tra ve: " << result << "\n");
            // tang thanh ghi pc
			IncreasePC();

			return;

			ASSERTNOTREACHED();

			break;
		}

		case SC_PrintChar:
		{
			// thong bao
			DEBUG(dbgSys, "System call: Print char \n");
            // doc ki tu tu thanh ghi
			char character = (char)kernel->machine->ReadRegister(4);
            // xuat ra console
			SysPrintChar(character);
            // tang thanh ghi pc
			IncreasePC();

			return;

			ASSERTNOTREACHED();

			break;
		}

		//system call create file
		case SC_Create:
		{
			DEBUG(dbgSys, "System call: Create file \n");
			// lay dia chi cua ten file
			int virtAddr = kernel->machine->ReadRegister(4);
			// chuyen tu user space sang kernel space
			char *filename;
			filename = NULL;
			const int MAX_STRING = 255;
			filename = User2System(virtAddr, MAX_STRING);
			//xet truong hop lay dia chi that bai
			if (filename==NULL||strlen(filename)==0||kernel->fileSystem->Create(filename, 0) == FALSE)
			{
				DEBUG(dbgSys, "Create file: that bai \n");
				kernel->machine->WriteRegister(2, -1);
			}
			//neu thanh cong
			else
			{
				kernel->fileSystem->Create(filename, 0);
				DEBUG(dbgSys, "Create file: thanh cong \n");
				kernel->machine->WriteRegister(2, 0);
			}
			
			delete [] filename;
			// tang thanh ghi pc
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		//system call Open
		case SC_Open:
		{
			DEBUG(dbgSys, "System call: Open file \n");
			// lay dia chi cua ten file
			int virtAddr = kernel->machine->ReadRegister(4);
			// chuyen tu user space sang kernel space
			char *filename;
			filename = NULL;
			const int MAX_STRING = 255;
			filename = User2System(virtAddr, MAX_STRING);
			// tim vi tri trong bang mo file
			int slot=kernel->fileSystem->FindFreeSlot();
			//neu khong the mo file
			if(slot==-1)
			{
				DEBUG(dbgSys, "Open file: khong con slot trong bang mo file \n");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				//mo file
				kernel->fileSystem->openFile[slot]=kernel->fileSystem->Open(filename,0);
				kernel->machine->WriteRegister(2, slot);
				DEBUG(dbgSys, "Open file: mo file thanh cong \n");
			}
			delete [] filename;
			// tang thanh ghi pc
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}

		//system call close file
		case SC_Close:
		{
			DEBUG(dbgSys, "System call: Close file \n");
			// lay vi tri file trong bang mo file
			int slot = kernel->machine->ReadRegister(4);
			// kiem tra vi tri hop le
			if(slot<0||slot>=MAX_FILES||kernel->fileSystem->openFile[slot]==NULL)
			{
				DEBUG(dbgSys, "Close file: vi tri file khong hop le \n");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				delete kernel->fileSystem->openFile[slot];
				kernel->fileSystem->openFile[slot] = NULL;
				kernel->machine->WriteRegister(2, 0); 
				DEBUG(dbgSys,"Close file: thanh cong \n");
			}
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		//system call Read
		case SC_Read:
		{
			DEBUG(dbgSys, "System call: Read.\n");
			// lay vi tri file trong bang mo file
			int virtAddr = kernel->machine->ReadRegister(4); //vi tri
			int size = kernel->machine->ReadRegister(5);	//kich thuoc
			int id = kernel->machine->ReadRegister(6);	//openfileID

			// kiem tra vi tri hop le
			if (id < 0 || id >= MAX_FILES || kernel->fileSystem->openFile[id] == NULL)
			{
				//thong bao loi
				DEBUG(dbgSys, "Read that bai!!!\n");
				//
				kernel->machine->WriteRegister(2, -1);

			}
			//Console Out -> k doc
			if (id == 1)
			{
				DEBUG(dbgSys, "Cannot read Console Out!\n");
				kernel->machine->WriteRegister(2, -1);
			}

			// Console In -> doc
			if (id == 0)
			{	
				char* buffer = new char[size];; 
				memset(buffer, 0, size);

				int index = 0; // Index
				char ch;
				//doc ki tu den khi gap '\n' hoac het file
				do
				{
					//doc vao bien ch, hop le thi luu vao buffer
					ch = kernel->synchConsoleIn->GetChar(); 
					if (ch != '\n') 
						buffer[index++] = ch; 
				} while (ch != '\n' && index < size);
				//chuyen tu kernel space sang user space
				System2User(virtAddr, size, buffer); 
				//giai phong bo nho
				free(buffer); 

				kernel->machine->WriteRegister(2, index);

				DEBUG(dbgSys, "Read String thanh cong!\n");
			}
			
			// doc file 
			char* buffer = new char[size];
			memset(buffer, 0, size);

			//lay gia tri seekPosition
			int OldPos = kernel->fileSystem->openFile[id]->GetCurrentPos();
			//doc file
			kernel->fileSystem->openFile[id]->Read(buffer, size);
			//cap nhat seekPosition
			int NewPos = kernel->fileSystem->openFile[id]->GetCurrentPos();

			//chuyen tu kernel space sang user space
			System2User(virtAddr, NewPos - OldPos, buffer);

			kernel->machine->WriteRegister(2, NewPos - OldPos);

			DEBUG(dbgSys, "Read File thanh cong!\n");

			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: Write
    	case SC_Write:
		{
			DEBUG(dbgSys, "System call: Write.\n");
			//lay vi tri file trong bang mo file, kich thuoc, openfileID
			int virtAddr = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int id = kernel->machine->ReadRegister(6);

			//kiem tra hop le
			if (id < 0 || id >= MAX_FILES || kernel->fileSystem->openFile[id] == NULL)
			{
				DEBUG(dbgSys, "Write that bai!\n");
				kernel->machine->WriteRegister(2, -1);
			}

			// Console In -> k ghi
			if (id == 0)
			{	
				DEBUG(dbgSys, "khong the ghi Console In!\n");
				kernel->machine->WriteRegister(2, -1);
			}
			
			// console out -> ghi
			if (id == 1)
			{
				//gan string cho buffer
				char* buffer = User2System(virtAddr, size); 
				int index = 0; 
				//in tung ki tu trong buffer cho den khi gap '\0'
				while (buffer[index] != 0) 
					kernel->synchConsoleOut->PutChar(buffer[index++]); 

				//giai phong bo nho
				free(buffer);
				DEBUG(dbgSys, "Write thanh cong!\n");
			}

			// ghi file thong thuong
			//gan string cho buffer
			char* buffer = User2System(virtAddr, size);
			//lay gia tri seekPosition
			int OldPos = kernel->fileSystem->openFile[id]->GetCurrentPos();
			//ghi file
			kernel->fileSystem->openFile[id]->Write(buffer, strlen(buffer));
			//cap nhat seekPosition
			int NewPos = kernel->fileSystem->openFile[id]->GetCurrentPos();

			kernel->machine->WriteRegister(2, NewPos - OldPos);
			//giai phong bo nho
			free(buffer);
			DEBUG(dbgSys, "Write thanh cong!\n");

			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}

		//system call Seek
		case SC_Seek:
		{
			DEBUG(dbgSys, "System call: Seek.\n");
	
			//lay vi tri, openfileID
			int position = kernel->machine->ReadRegister(4);
			int id = kernel->machine->ReadRegister(5);

			//kiem tra hop le
			if (id < 0 || id >= MAX_FILES || kernel->fileSystem->openFile[id] == NULL)
			{
				DEBUG(dbgSys, "Seek that bai!\n");
				kernel->machine->WriteRegister(2, -1);
			}

			//Console In, Out -> k seek
			if (id == 0 || id == 1)
			{	
				DEBUG(dbgSys, "khong the seek Console!\n");
				kernel->machine->WriteRegister(2, -1);
			}

			// seek file thong thuong
			if (position < 0 || position > kernel->fileSystem->openFile[id]->Length())
			{
				DEBUG(dbgSys, "vi tri khong hop le!\n");
				kernel->machine->WriteRegister(2, -1);
			}
			//-1: seek toi cuoi file
			if (position == -1)
				position = kernel->fileSystem->openFile[id]->Length();

			
			kernel->fileSystem->openFile[id]->Seek(position);
			kernel->machine->WriteRegister(2, position);
			DEBUG(dbgSys, "Seek thanh cong!\n");

			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;

	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
