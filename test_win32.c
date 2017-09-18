/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4201)
#include "Definitions/id_opcua.h"
#include "Functions\UASQLFunctions.h"
#include "SessionManager\SessionManager.h"
#include "AddressSpace\AddressSpace.h"
#include "Indexer\Indexer.h"

extern ID_AddressSpaceHandle GlobalAddressSpaceHandle;
extern ID_IndexerHandle GlobalIndexerHandle;
extern ID_SessionManagerHandle GlobalSessionManagerHandle;

#define ID_SET_INPUT_ARG_STRING(xInputArgs, xIndex, xStr) \
xInputArgs[xIndex].ArrayType = OpcUa_VariantArrayType_Scalar; \
xInputArgs[xIndex].Datatype = OpcUaType_String; \
OpcUa_String_Initialize(&xInputArgs[xIndex].Value.String); \
OpcUa_String_AttachCopy(&xInputArgs[xIndex].Value.String, xStr); \

void ID_memoryDebugSQLFunction() {
	OpcUa_ProxyStubConfiguration UaTestServer_g_pProxyStubConfiguration;
	OpcUa_Handle UaTestServer_g_PlatformLayerHandle;

	UaTestServer_g_pProxyStubConfiguration.bProxyStub_Trace_Enabled = OpcUa_True;   //to deactivate Tracer set this variable Opc Ua False.
	UaTestServer_g_pProxyStubConfiguration.uProxyStub_Trace_Level = OPCUA_TRACE_OUTPUT_LEVEL_WARNING;
	UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxAlloc = -1;
	UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxStringLength = -1;
	UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxByteStringLength = -1;
	UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxArrayLength = -1;
	UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxMessageSize = -1;
	UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxRecursionDepth = -1;
	UaTestServer_g_pProxyStubConfiguration.bSecureListener_ThreadPool_Enabled = OpcUa_False;
	UaTestServer_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MinThreads = -1;
	UaTestServer_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MaxThreads = -1;
	UaTestServer_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MaxJobs = -1;
	UaTestServer_g_pProxyStubConfiguration.bSecureListener_ThreadPool_BlockOnAdd = OpcUa_True;
	UaTestServer_g_pProxyStubConfiguration.uSecureListener_ThreadPool_Timeout = OPCUA_INFINITE;
	UaTestServer_g_pProxyStubConfiguration.bTcpListener_ClientThreadsEnabled = OpcUa_False;
	UaTestServer_g_pProxyStubConfiguration.iTcpListener_DefaultChunkSize = -1;
	UaTestServer_g_pProxyStubConfiguration.iTcpConnection_DefaultChunkSize = -1;
	UaTestServer_g_pProxyStubConfiguration.iTcpTransport_MaxMessageLength = -1;
	UaTestServer_g_pProxyStubConfiguration.iTcpTransport_MaxChunkCount = -1;
	UaTestServer_g_pProxyStubConfiguration.bTcpStream_ExpectWriteToBlock = OpcUa_True;

	/* initialize platform layer */
	OpcUa_P_Initialize(&UaTestServer_g_PlatformLayerHandle);
	OpcUa_ProxyStub_Initialize(UaTestServer_g_PlatformLayerHandle, &UaTestServer_g_pProxyStubConfiguration);

    ID_AddressSpace_Create(&GlobalAddressSpaceHandle);
    ID_Indexer_Create(&GlobalIndexerHandle);
    ID_SessionManager_Create(&GlobalSessionManagerHandle);

	OpcUa_String SessionName;
	OpcUa_String_Initialize(&SessionName);
	OpcUa_String_AttachCopy(&SessionName, "Session");

	OpcUa_ApplicationDescription AppDesc;
	OpcUa_ApplicationDescription_Initialize(&AppDesc);

	OpcUa_ByteString ByteStr;
	OpcUa_ByteString_Initialize(&ByteStr);

    ID_Session * session = ID_SessionManager_AddSession(GlobalSessionManagerHandle, 1, &SessionName, &AppDesc, &ByteStr, &ByteStr);
	session->IsActivated = OpcUa_True;
	
	int NoInput = 1;
	OpcUa_Variant * InputArgs = malloc(sizeof(OpcUa_Variant) * NoInput);
	OpcUa_Variant_InitializeArray(InputArgs, NoInput);
    ID_SET_INPUT_ARG_STRING(InputArgs, 0, "Driver={SQL Server};Database=testdb;Server=localhost;UID=esj;PWD=esjesjNo!");

	OpcUa_Variant * OutputArgs = NULL;
	int NoOutputArgs = 0;
	OpcUa_StatusCode * StatusCodes = malloc(sizeof(OpcUa_StatusCode) * NoInput);
	memset(StatusCodes, 0x00, sizeof(OpcUa_StatusCode) * NoInput);
    ID_UASQLConnectA(&session->AuthenticationToken, NoInput, InputArgs, &NoOutputArgs, &OutputArgs, StatusCodes);

	OpcUa_Variant_ClearArray(InputArgs, NoInput);
	OpcUa_Variant_ClearArray(OutputArgs, NoOutputArgs);
	free(InputArgs);
	free(OutputArgs);

	OpcUa_String_Clear(&SessionName);
    ID_SessionManager_Destroy(&GlobalSessionManagerHandle);
    ID_AddressSpace_Destroy(&GlobalAddressSpaceHandle);
    ID_Indexer_Delete(&GlobalIndexerHandle);

	OpcUa_P_Clean(&UaTestServer_g_PlatformLayerHandle);
	OpcUa_ProxyStub_Clear();
}
#pragma warning(pop)
#endif