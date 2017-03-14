#pragma once
#ifndef _FILE_FUNC_H
#define _FILE_FUNC_H

#include "common.h"

extern LIST_ENTRY CipherExtensionList;

extern UNICODE_STRING MonitoredDir;


typedef struct _EXT_NODE
{
	UNICODE_STRING ext;
	LIST_ENTRY next;
}EXT_NODE, *PEXT_NODE;

BOOLEAN InsertNodeToList(PWSTR ext);

VOID OutputExtNodes(PLIST_ENTRY head);

BOOLEAN SearchExtension(PUNICODE_STRING ext, PLIST_ENTRY head);

BOOLEAN IsMonitoredExtension(PUNICODE_STRING ext);

VOID InitializeCipherExtensionList();

VOID ClearCipherExtensionList();

VOID ClearNodeList(PLIST_ENTRY head);

/*
添加附加数据流，同时，过滤文件类型
*/
BOOLEAN AddFileFlag(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects
);

/*
	给文件添加flag或者，检查是否存在flag
*/
ULONG CreateOrOpenFileWithFlag(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	ULONG CreateDisposition
);


#define RET_NORMAL_FAIL				0
#define RET_NOT_FILE_TYPE			1
#define RET_CREATE_OPEN_FAIL		2
#define RET_CREATE_OPEN_SUCCEED		3


#endif