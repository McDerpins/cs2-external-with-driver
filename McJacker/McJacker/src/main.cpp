#include "../lib/pch.h"
#include "../lib/util/util.h"
#include "../lib/namespace/namespace.h"


int main() {
    
    // if we're not in debug don't spawn a console
#ifndef _DEBUG
    FreeConsole();
#endif // !_DEBUG

    // Name of the game we wanna hack
    const wchar_t* targetModuleName = L"cs2.exe";

    // Get that game's process ID
    int pid = GetProcessIdByName(targetModuleName);
    if (pid == 0) {
        MessageBoxW(NULL, L"Process not found", L"Error", MB_OK | MB_ICONERROR);
        return 1; // failure
    }

    // Create driver handle
    const HANDLE driver = CreateFile(L"\\\\.\\McDriver", GENERIC_READ, 0, nullptr, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    if (driver == INVALID_HANDLE_VALUE) {

        MessageBoxW(NULL, L"Failed to create our driver handle", L"Error", MB_OK | MB_ICONERROR);
        return 1; // failure
    }

    // "attach" to process
    if (driver::attach_to_process(driver, pid) == true) {
#ifdef _DEBUG
        std::cout << "Attached successfully\n" << std::endl;
#endif
        // get module base
        if (const std::uintptr_t client = GetModuleBaseAddress(pid, L"client.dll"); client != 0) {

#ifdef _DEBUG
            std::cout << "client base found sucessfully\n" << std::endl;
#endif
            // Initialize overlay
            c_overlay overlay(driver, client);

            // Start our hack loop
            while (overlay.msg_loop() && !overlay.ready) {
              
            }

        }
    }

	return 1;
}

