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