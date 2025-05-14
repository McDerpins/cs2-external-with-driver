#include <ntifs.h>
#pragma warning(disable: 4100)
extern "C" {
	NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);

	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProces, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);
}

void debug_print(PCSTR text) 
{
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, text));
}

namespace driver {
	namespace codes {
		constexpr ULONG attach =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

		constexpr ULONG read =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

		constexpr ULONG write =
			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
	}

	struct Request {
		HANDLE process_id;

		PVOID target;
		PVOID buffer;

		SIZE_T size;
		SIZE_T return_size;
	};

	NTSTATUS create(PDEVICE_OBJECT device_object, PIRP irp)
	{
		UNREFERENCED_PARAMETER(device_object);
		debug_print("[+] IRP_MJ_CREATE called\n");
		irp->IoStatus.Status = STATUS_SUCCESS;  // Explicitly set success status
		irp->IoStatus.Information = 0;         // No additional information

		IoCompleteRequest(irp, IO_NO_INCREMENT);
		debug_print("[+] IRP_MJ_CREATE completed\n");

		return STATUS_SUCCESS;
	}

	NTSTATUS close(PDEVICE_OBJECT device_object, PIRP irp)
	{
		UNREFERENCED_PARAMETER(device_object);

		irp->IoStatus.Status = STATUS_SUCCESS;  // Explicitly set success status
		irp->IoStatus.Information = 0;         // No additional information

		IoCompleteRequest(irp, IO_NO_INCREMENT);

		return STATUS_SUCCESS;
	}

	NTSTATUS device_control(PDEVICE_OBJECT device_object, PIRP irp)
	{
		UNREFERENCED_PARAMETER(device_object);

		KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[+] device_control called\n"));

		PIO_STACK_LOCATION stack_irp = IoGetCurrentIrpStackLocation(irp);
		if (!stack_irp || !irp->AssociatedIrp.SystemBuffer) {
			KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[-] Invalid IRP parameters\n"));
			irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return STATUS_INVALID_PARAMETER;
		}

		if (stack_irp->Parameters.DeviceIoControl.InputBufferLength < sizeof(Request)) {
			KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[-] Input buffer too small\n"));
			irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return STATUS_BUFFER_TOO_SMALL;
		}

		KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[+] IOCTL received: 0x%X\n",
			stack_irp->Parameters.DeviceIoControl.IoControlCode));

		auto request = reinterpret_cast<Request*>(irp->AssociatedIrp.SystemBuffer);

		NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
		//NTSTATUS testStatus = STATUS_SUCCESS;
		static PEPROCESS target_process = nullptr;
		debug_print("Request received:\n");
		DbgPrint("  Target: 0x%p\n", request->target);
		DbgPrint("  Buffer: 0x%p\n", request->buffer);
		DbgPrint("  Size: %llu\n", request->size);

		switch (stack_irp->Parameters.DeviceIoControl.IoControlCode) {
		case codes::attach:
			KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[+] Attach IOCTL received\n"));
			KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[+] Attaching to Process ID: %u\n", request->process_id));
			status = PsLookupProcessByProcessId(request->process_id, &target_process);
			if (!NT_SUCCESS(status)) {
				KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[-] Failed to lookup process: 0x%X\n", status));
				target_process = nullptr;
			}
			break;

		case codes::read:
			KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[+] Read IOCTL received\n"));

		
			if (MmIsAddressValid(request->target) && MmIsAddressValid(request->buffer)) {
				KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[+] Address is valid\n"));
			}

			KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
				"[+] target_process 1: 0x%p, target: 0x%p, buffer: 0x%p, size: %llu\n",
				target_process, request->target, request->buffer, request->size));

			if (target_process != nullptr) {
				status = MmCopyVirtualMemory(
					target_process,
					request->target,
					PsGetCurrentProcess(),
					request->buffer,
					request->size,
					KernelMode,
					&request->return_size);
			}
			if (status != STATUS_SUCCESS) {
				KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[-] mmcopyvirtual memory was not successful"));
				KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[-] status is  Decimal: %d, Hex: 0x%X, Unsigned: %u\n", status,status,status ));

				KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
					"[+] target_process2: 0x%p, target: 0x%p, buffer: 0x%p, size: %llu\n",
					target_process, request->target, request->buffer, request->size));

			}
			if (!target_process) {
				debug_print("NO TARGET PROCESSSS");

			}
			KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[-] MmCopyVirtualMemory status: 0x%08X\n", status));
			
			break;

		case codes::write:
			KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[+] Write IOCTL received\n"));
			if (target_process) {
				status = MmCopyVirtualMemory(
					PsGetCurrentProcess(),
					request->buffer,
					target_process,
					request->target,
					request->size,
					KernelMode,
					&request->return_size);
			}
			break;

		default:
			KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[-] Unsupported IOCTL: 0x%X\n",
				stack_irp->Parameters.DeviceIoControl.IoControlCode));
			status = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}

		irp->IoStatus.Status = status;
		irp->IoStatus.Information = (NT_SUCCESS(status) ? sizeof(Request) : 0);

		IoCompleteRequest(irp, IO_NO_INCREMENT);

		return status;
	}

} // namespace driver

NTSTATUS driver_main(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(registry_path);
	debug_print("[+] driver main function\n");
	UNICODE_STRING device_name = {};
	RtlInitUnicodeString(&device_name, L"\\Device\\McDriver");

	PDEVICE_OBJECT device_object = nullptr;
	NTSTATUS status = IoCreateDevice(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);

	if (status != STATUS_SUCCESS)
	{
		KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IoCreateDevice failed with status: 0x%X\n", status));

		debug_print("[-] Failed to create driver device\n");
		return status;
	}

	debug_print("[+] Driver device created\n");

	UNICODE_STRING symbolic_link = {};
	RtlInitUnicodeString(&symbolic_link, L"\\DosDevices\\McDriver");

	status = IoCreateSymbolicLink(&symbolic_link, &device_name);
	if (status != STATUS_SUCCESS)
	{
		debug_print("[-] Failed to create symbolic link");
		return status;
	}

	debug_print("[+] symbolic Link created\n");

	SetFlag(device_object->Flags, DO_BUFFERED_IO);

	driver_object->MajorFunction[IRP_MJ_CREATE] = driver::create;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = driver::close;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = driver::device_control;

	ClearFlag(device_object->Flags, DO_DEVICE_INITIALIZING);

	debug_print("[+] Driver initialized\n");


	return status;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);

	debug_print("[+] this is a change!\n");

	UNICODE_STRING driver_name = {};

	RtlInitUnicodeString(&driver_name, L"\\Driver\\McDriver");
	debug_print("[+] creating  driver....!\n");
	return IoCreateDriver(&driver_name, &driver_main);
}