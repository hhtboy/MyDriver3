#include "MyMiniFilter.h"

PFLT_FILTER gFilterHandle;

// 필터 등록 구조체
const FLT_OPERATION_REGISTRATION Callbacks[] = {
    { IRP_MJ_CREATE, 0, MiniFilterPreCreateOperation, NULL },
    { IRP_MJ_OPERATION_END }
};

const FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),         // Size
    FLT_REGISTRATION_VERSION,         // Version
    0,                                // Flags
    NULL,                             // ContextRegistration
    Callbacks,                        // Operation callbacks
    MiniFilterUnload,                 // MiniFilterUnload
    NULL, NULL, NULL, NULL, NULL, NULL
};

// 드라이버 진입점
NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status;

    status = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = FltStartFiltering(gFilterHandle);
    if (!NT_SUCCESS(status)) {
        FltUnregisterFilter(gFilterHandle);
    }

    return status;
}

// 필터 언로드
NTSTATUS
MiniFilterUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(Flags);
    FltUnregisterFilter(gFilterHandle);
    return STATUS_SUCCESS;
}

// 파일 생성 전 콜백 함수
FLT_PREOP_CALLBACK_STATUS
MiniFilterPreCreateOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
{
    UNREFERENCED_PARAMETER(CompletionContext);
    UNREFERENCED_PARAMETER(FltObjects); // 추가된 코드
    // 파일명 가져오기
    if (Data->Iopb->TargetFileObject && Data->Iopb->TargetFileObject->FileName.Length > 0) {
        UNICODE_STRING fileName = Data->Iopb->TargetFileObject->FileName;
        DbgPrint("File Opened: %wZ\n", &fileName);
    }

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}
