#include "callbackRoutines.h"

FLT_PREOP_CALLBACK_STATUS
PreCleanup(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	NTSTATUS status;
	PSTREAM_CONTEXT pStreamCtx = NULL;
	ULONG retVal;

	try
	{
		//
		// 进程监控
		//

		PCHAR procName = GetProcessName();
		if (!IsMonitoredProcess(procName))
		{
			leave;
		}

		//
		// 获取上下文
		//

		status = FltGetStreamContext(FltObjects->Instance, FltObjects->FileObject, &pStreamCtx);
		if (!NT_SUCCESS(status))
		{
			leave;
		}

		DbgPrint("\nIRP_CLEAN_UP\n");
		DbgPrint("\tProcess Name: %s\n", procName);
		DbgPrint("\tStreamContext: Flag: %d, Write Data: %d\n", pStreamCtx->bIsThereAFlag, pStreamCtx->bHasWrittenData);

		//
		// 只有没有：flag的监控文件类型，才需要添加标志，同时只有在写入文件数据时，才添加
		//

		//if (pStreamCtx->bIsThereAFlag == FALSE)
		{
			retVal = CreateOrOpenFileWithFlag(Data, FltObjects, FILE_CREATE);
			if (retVal == RET_CREATE_OPEN_SUCCEED)
			{
				pStreamCtx->bIsThereAFlag = TRUE;
				pStreamCtx->bHasWrittenData = FALSE;

				DbgPrint("\tAdd File Flag\n");
			}
		}


	}
	finally
	{

	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}
