#pragma once
#include "namespace.h"

// shared between user mode and kernel mode
struct Request {
    HANDLE process_id;

    PVOID target;
    PVOID buffer;

    SIZE_T size;
    SIZE_T return_size;
};

namespace driver {
    bool attach_to_process(HANDLE driver_handle, const DWORD pid) {
        Request r;
        r.process_id = reinterpret_cast<HANDLE>(pid);

        return DeviceIoControl(driver_handle, codes::attach, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
    }

}