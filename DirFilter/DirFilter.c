/*++

Module Name:

    DirFilter.c

Abstract:

    This is the main module of the DirFilter miniFilter driver.

Environment:

    Kernel mode

--*/

#include "common.h"
#include "callbackRoutines.h"

PFLT_FILTER gFilterHandle;
ULONG_PTR OperationStatusCtx = 1;

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002

ULONG gTraceFlags = 0;


#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(gTraceFlags,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))

/*************************************************************************
    Prototypes
*************************************************************************/

EXTERN_C_START

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    );


NTSTATUS
DirFilterUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
    );


EXTERN_C_END

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, DirFilterUnload)

#endif

//
//  operation registration
//

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {

	{
		IRP_MJ_CREATE,
		0,
		PreCreate,
		PostCreate
	},

	{
		IRP_MJ_SET_INFORMATION,
		0,
		PreSetInfo,
		PostSetInfo
	},

	{
		IRP_MJ_CLEANUP,
		0,
		PreCleanup,
		NULL
	},

    { IRP_MJ_OPERATION_END }
};


//
// 上下文结构
//

CONST FLT_CONTEXT_REGISTRATION Context_Array[]={
	{
		FLT_STREAM_CONTEXT,
		0,
		NULL,
		STREAM_CONTEXT_SIZE,
		STREAM_CONTEXT_TAG
	},

	{FLT_CONTEXT_END}
};



//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof( FLT_REGISTRATION ),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    Context_Array,                               //  Context
    Callbacks,                          //  Operation callbacks

    DirFilterUnload,                           //  MiniFilterUnload

    NULL,                    //  InstanceSetup
    NULL,            //  InstanceQueryTeardown
    NULL,            //  InstanceTeardownStart
    NULL,         //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent

};






/*************************************************************************
    MiniFilter initialization and unload routines.
*************************************************************************/

NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This is the initialization routine for this miniFilter driver.  This
    registers with FltMgr and initializes all global data structures.

Arguments:

    DriverObject - Pointer to driver object created by the system to
        represent this driver.

    RegistryPath - Unicode string identifying where the parameters for this
        driver are located in the registry.

Return Value:

    Routine can return non success error codes.

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER( RegistryPath );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("DirFilter!DriverEntry: Entered\n") );

	//
	//初始化加密进程链表
	//

	InitializeCipherProcList();
	InitializeCipherExtensionList();




    //
    //  Register with FltMgr to tell it our callback routines
    //

    status = FltRegisterFilter( DriverObject,
                                &FilterRegistration,
                                &gFilterHandle );

    FLT_ASSERT( NT_SUCCESS( status ) );

    if (NT_SUCCESS( status )) {

        //
        //  Start filtering i/o
        //

        status = FltStartFiltering( gFilterHandle );

        if (!NT_SUCCESS( status )) {

            FltUnregisterFilter( gFilterHandle );
        }
    }

    return status;
}

NTSTATUS
DirFilterUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
    )
/*++

Routine Description:

    This is the unload routine for this miniFilter driver. This is called
    when the minifilter is about to be unloaded. We can fail this unload
    request if this is not a mandatory unload indicated by the Flags
    parameter.

Arguments:

    Flags - Indicating if this is a mandatory unload.

Return Value:

    Returns STATUS_SUCCESS.

--*/
{
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("DirFilter!DirFilterUnload: Entered\n") );

	//
	// 清理资源
	//

	ClearCipherProcList();

	ClearCipherExtensionList();

    FltUnregisterFilter( gFilterHandle );

    return STATUS_SUCCESS;
}


