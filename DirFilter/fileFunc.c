#include "fileFunc.h"

LIST_ENTRY CipherExtensionList;
UNICODE_STRING MonitoredDir;



BOOLEAN InsertNodeToList(PWSTR ext)
{
	if (ext == NULL)
	{
		return FALSE;
	}

	PEXT_NODE node = NULL;
	node = ExAllocatePool(NonPagedPool, sizeof(EXT_NODE));
	if (node == NULL)
	{
		DbgPrint("\tInsufficient Resources.\n");
		return FALSE;
	}

	RtlZeroMemory(node, sizeof(EXT_NODE));
	RtlInitUnicodeString(&node->ext, ext);


	InsertHeadList(&CipherExtensionList, &node->next);

	return TRUE;
}


VOID OutputExtNodes(PLIST_ENTRY head)
{
	PLIST_ENTRY pEntry;
	PEXT_NODE pNode;

	for (pEntry = head->Flink; pEntry != head; pEntry = pEntry->Flink)
	{
		pNode = CONTAINING_RECORD(pEntry, EXT_NODE, next);

		DbgPrint("\tThe Data of Ext List is: %wZ\n", pNode->ext);
	}
}


BOOLEAN SearchExtension(PUNICODE_STRING ext, PLIST_ENTRY head)
{
	PLIST_ENTRY pEntry;
	PEXT_NODE pNode;

	for (pEntry = head->Flink; pEntry != head; pEntry = pEntry->Flink)
	{
		pNode = CONTAINING_RECORD(pEntry, EXT_NODE, next);

		if (RtlCompareUnicodeString(ext, &(pNode->ext), FALSE) == 0)
		{
			return TRUE;
		}

	}

	return FALSE;
}


BOOLEAN IsMonitoredExtension(PUNICODE_STRING ext)
{
	if (ext == NULL)
	{
		return FALSE;
	}

	if (SearchExtension(ext, &CipherExtensionList))
	{
		return TRUE;
	}

	return FALSE;
}


VOID InitializeCipherExtensionList()
{
	BOOLEAN isOk;

	RtlInitUnicodeString(&MonitoredDir, L"C:\\Test");

	InitializeListHead(&CipherExtensionList);
	isOk = InsertNodeToList(L"dwg");
	isOk = InsertNodeToList(L"tmp");

	if (isOk)
	{
		OutputExtNodes(&CipherExtensionList);
	}

}

VOID ClearCipherExtensionList()
{
	ClearNodeList(&CipherExtensionList);
}


VOID ClearNodeList(PLIST_ENTRY head)
{
	PEXT_NODE pNode;
	PLIST_ENTRY pEntry;

	try
	{
		while (!IsListEmpty(head))
		{
			pEntry = RemoveTailList(head);
			pNode = CONTAINING_RECORD(pEntry, EXT_NODE, next);

			if(pNode->ext.Length!=0)
			{
				DbgPrint("\tRemove From List: %wZ\n", pNode->ext);
			}

			//RtlFreeUnicodeString(&(pNode->ext));
			ExFreePool(pNode);
		}



	}
	finally
	{

	}


}


BOOLEAN IsMonitoredPath(PUNICODE_STRING dosName)
{
	if (dosName->Length < MonitoredDir.Length || dosName->Buffer == NULL)
	{
		return FALSE;
	}
	
	if (RtlPrefixUnicodeString(&MonitoredDir, dosName, TRUE))
	{
		return TRUE;
	}

	return FALSE;

}


BOOLEAN AddFileFlag(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects
)
{

	NTSTATUS status;
	HANDLE hFile = NULL;
	IO_STATUS_BLOCK ioStatus;

	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING path = { 0 };
	PFLT_FILE_NAME_INFORMATION nameInfo = NULL;


	try
	{

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



		DbgPrint("\nIRP_MJ_CREATE\n");
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
			DbgPrint("\tIoStatus.Info: %d\n", ioStatus.Information);
			DbgPrint("****IoStatus Status: %d\n", ioStatus.Status);
			leave;
		}

		DbgPrint("\tFile Created!\n");



		return TRUE;
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


	return FALSE;
}


ULONG CreateOrOpenFileWithFlag(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	ULONG CreateDisposition
)
{
	NTSTATUS status;
	HANDLE hFile = NULL;
	IO_STATUS_BLOCK ioStatus;

	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING path = { 0 };
	PFLT_FILE_NAME_INFORMATION nameInfo = NULL;

	ULONG retVal = RET_NORMAL_FAIL;

	try
	{

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
			retVal = RET_NOT_FILE_TYPE;
			leave;
		}


		if (CreateDisposition == FILE_OPEN)
			DbgPrint("\nIRP_MJ_CREATE\n");
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
			CreateDisposition,
			FILE_WRITE_THROUGH,
			NULL,
			0,
			IO_IGNORE_SHARE_ACCESS_CHECK
		);

		if (!NT_SUCCESS(status))
		{
			DbgPrint("\tIoStatus.Info: %d\n", ioStatus.Information);
			DbgPrint("\tIoStatus Status: %d\n", ioStatus.Status);
			DbgPrint("********CREATE OR OPEN Failed**********\n");

			retVal = RET_CREATE_OPEN_FAIL;

			leave;
		}

		if (CreateDisposition == FILE_CREATE)
			DbgPrint("\tFile Created!\n");
		else
			DbgPrint("\tFile Opened!\n");

		retVal = RET_CREATE_OPEN_SUCCEED;
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