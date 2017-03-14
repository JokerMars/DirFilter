#pragma once
#ifndef _CALLBACK_ROUTINE_H
#define _CALLBACK_ROUTINE_H


#include "common.h"
#include "proc.h"
#include "fileFunc.h"
#include "ctx.h"


FLT_PREOP_CALLBACK_STATUS
PreCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);


FLT_POSTOP_CALLBACK_STATUS
PostCreate(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__inout_opt PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
);


FLT_PREOP_CALLBACK_STATUS
PreSetInfo(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS
PostSetInfo(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__inout_opt PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
);


FLT_PREOP_CALLBACK_STATUS
PreCleanup(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);




#endif