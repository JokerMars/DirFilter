#include "ctx.h"


NTSTATUS
iCtx_CreateStreamContext(
	PCFLT_RELATED_OBJECTS FltObjects,
	PSTREAM_CONTEXT *newStreamContext
)
{
	NTSTATUS status;
	PSTREAM_CONTEXT streamContext;

	PAGED_CODE();

	status = FltAllocateContext(FltObjects->Filter,
		FLT_STREAM_CONTEXT,
		STREAM_CONTEXT_SIZE,
		NonPagedPool,
		&streamContext);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	RtlZeroMemory(streamContext, STREAM_CONTEXT_SIZE);

	*newStreamContext = streamContext;

	return STATUS_SUCCESS;
}


/*
	查找流上下文，找不到就创建一个，
*/
NTSTATUS
Ctx_FindOrCreateStreamContext(
	_In_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ BOOLEAN CreateIfNotFound,
	_Out_ PSTREAM_CONTEXT *StreamContext,
	_Out_ BOOLEAN *ContextCreated
)
{
	NTSTATUS status;
	PSTREAM_CONTEXT newStreamContext;
	PSTREAM_CONTEXT oldStreamContext;

	PAGED_CODE();

	*StreamContext = NULL;
	if (ContextCreated != NULL)*ContextCreated = FALSE;

	status = FltGetStreamContext(Data->Iopb->TargetInstance,
		Data->Iopb->TargetFileObject, &newStreamContext);

	if (!NT_SUCCESS(status) &&
		(status == STATUS_NOT_FOUND) &&
		CreateIfNotFound)
	{
		status = iCtx_CreateStreamContext(FltObjects, &newStreamContext);
		if (!NT_SUCCESS(status))
		{
			return status;
		}

		status = FltSetStreamContext(Data->Iopb->TargetInstance,
			Data->Iopb->TargetFileObject,
			FLT_SET_CONTEXT_KEEP_IF_EXISTS,
			newStreamContext,
			&oldStreamContext);
		if (!NT_SUCCESS(status))
		{
			FltReleaseContext(newStreamContext);

			if (status != STATUS_FLT_CONTEXT_ALREADY_DEFINED)
			{
				return status;
			}

			newStreamContext = oldStreamContext;
			status = STATUS_SUCCESS;
		}
		else
		{
			if (ContextCreated != NULL)*ContextCreated = TRUE;
		}
	}
	
	*StreamContext = newStreamContext;

	return status;

}


