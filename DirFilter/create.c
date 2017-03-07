#include "callbackRoutines.h"

FLT_PREOP_CALLBACK_STATUS
PreCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	ULONG createOptions;
	PFLT_FILE_NAME_INFORMATION nameInfo = NULL;
	FLT_PREOP_CALLBACK_STATUS retVal = FLT_PREOP_SUCCESS_NO_CALLBACK;

	NTSTATUS status;
	HANDLE hFile = NULL;
	IO_STATUS_BLOCK ioStatus;

	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING path = { 0 };
	

	PAGED_CODE();

	try
	{
		//
		//进程监控
		//

		PCHAR procName = GetProcessName();
		if (!IsMonitoredProcess(procName))
		{
			leave;
		}


		//
		//分页读，内存操作，此时不是具体写入
		//

		if (Data->Iopb->OperationFlags&SL_OPEN_PAGING_FILE)
		{
			//DbgPrint("\tIgnoring paging file open\n");
			leave;
		}

		//
		// 打开卷
		//

		if (Data->Iopb->TargetFileObject->Flags&FO_VOLUME_OPEN)
		{
			//DbgPrint("\tIgnoring volume open\n");
			leave;
		}

		//
		//打开文件夹
		//

		if (Data->Iopb->OperationFlags&SL_OPEN_TARGET_DIRECTORY)
		{
			leave;
		}


		//
		//文件创建行为
		//

		createOptions = Data->Iopb->Parameters.Create.Options >> 24;
		/*if (createOptions != FILE_SUPERSEDE&&
			createOptions != FILE_OVERWRITE&&
			createOptions != FILE_OVERWRITE_IF&&
			createOptions != FILE_CREATE&&
			createOptions != FILE_OPEN_IF)
		{
			leave;
		}*/

		status = FltGetFileNameInformation(Data,
			FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_FILESYSTEM_ONLY,
			&nameInfo
		);
		if (!NT_SUCCESS(status))
		{
			leave;
		}
		status = FltParseFileNameInformation(nameInfo);
		if (!NT_SUCCESS(status))
		{
			leave;
		}

		//
		// 文件后缀名
		//

		if (!IsMonitoredExtension(&(nameInfo->Extension)))
		{
			leave;
		}



		DbgPrint("\nIRP_MJ_CREATE:\n");
		DbgPrint("\tCreate Options: %d\n", createOptions);
		DbgPrint("\tProcess Name: %s\n", procName);
		DbgPrint("\tFile Name: %wZ\n", nameInfo->Name);
		DbgPrint("\tFile Extension: %wZ\n", nameInfo->Extension);

		
		path.MaximumLength = nameInfo->Name.MaximumLength + 10 * sizeof(WCHAR);
		path.Buffer = ExAllocatePool(NonPagedPool, path.MaximumLength);

		RtlAppendUnicodeStringToString(&path, &nameInfo->Name);
		RtlAppendUnicodeToString(&path, L":flag");

		DbgPrint("\tFile Name: %wZ\n", path);

		InitializeObjectAttributes(&oa, &path, OBJ_CASE_INSENSITIVE, NULL, NULL);


		status = FltCreateFile(FltObjects->Filter,
			FltObjects->Instance,
			&hFile,
			GENERIC_READ,
			&oa,
			&ioStatus,
			NULL,
			FILE_ATTRIBUTE_HIDDEN,
			FILE_SHARE_READ,
			FILE_CREATE,
			FILE_WRITE_THROUGH,
			NULL,
			0,
			IO_IGNORE_SHARE_ACCESS_CHECK
		);

		if (!NT_SUCCESS(status))
		{
			DbgPrint("****IoStatus Status: %d\n", ioStatus.Status);
			leave;
		}

		DbgPrint("\tFile Created!\n");



		retVal = FLT_PREOP_SUCCESS_WITH_CALLBACK;

	}
	finally
	{
		if (nameInfo)
		{
			FltReleaseFileNameInformation(nameInfo);
		}

		if (path.Buffer)
		{
			ExFreePool(path.Buffer);
		}

		if (hFile)
		{
			FltClose(hFile);
		}

	}

	return retVal;
}


FLT_POSTOP_CALLBACK_STATUS
PostCreate(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__inout_opt PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
)
{
	KIRQL irql = KeGetCurrentIrql();

	DbgPrint("\tIRQL: %d\n", irql);
	


	return FLT_POSTOP_FINISHED_PROCESSING;
}