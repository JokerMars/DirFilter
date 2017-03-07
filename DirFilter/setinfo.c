#include "callbackRoutines.h"


FLT_PREOP_CALLBACK_STATUS
PreSetInfo(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	NTSTATUS status;
	FILE_INFORMATION_CLASS fileInfoClass = Data->Iopb->Parameters.SetFileInformation.FileInformationClass;
	PVOID fileInfoBuffer = Data->Iopb->Parameters.SetFileInformation.InfoBuffer;


	PAGED_CODE();
	FLT_PREOP_CALLBACK_STATUS retVal = FLT_PREOP_SYNCHRONIZE;

	try
	{
		PCHAR procName = GetProcessName();
		if (!IsMonitoredProcess(procName))
		{
			leave;
		}

		if (fileInfoClass == FileRenameInformation)
		{
			PFILE_RENAME_INFORMATION fileInfo = (PFILE_RENAME_INFORMATION)fileInfoBuffer;
			DbgPrint("\nIRP_MJ_SETINFO\n");
			DbgPrint("\tFile Original Name: %wZ", FltObjects->FileObject->FileName);
			DbgPrint("\tFile Dest Name: %ws\n", fileInfo->FileName);
		}


	}
	finally
	{

	}

	return retVal;
}

FLT_POSTOP_CALLBACK_STATUS
PostSetInfo(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__inout_opt PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
)
{
	return FLT_POSTOP_FINISHED_PROCESSING;
}
