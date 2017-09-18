/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "Definitions/id_opcua.h"

#ifdef _WIN32
#include <conio.h>

#ifdef _DEBUG
#include <crtdbg.h>
#include <vld.h> // Visual Leak Detector
#endif

#define sleep(xMilliseconds) Sleep(xMilliseconds)
#endif

#include "AddressSpace/AddressSpace.h"
#include "Functions/UASQLFunctions.h"
#include "UASpace/Initialize.h"
#include "Indexer/Aliaser.h"
#include "Functions/ExposedFunctions.h"
#include "Indexer/Continuer.h"
#include "SessionManager/SecurityManager.h"
#include "SessionManager/SessionManager.h"

ID_AddressSpaceHandle GlobalAddressSpaceHandle;
ID_IndexerHandle GlobalIndexerHandle;
ID_SessionManagerHandle GlobalSessionManagerHandle;
ID_AliaserHandle GlobalAliaserHandle;
ID_ContinuerHandle GlobalContinuerHandle;
ID_SecurityHandle GlobalSecurityHandle;

// timer
OpcUa_Timer Timer;
OpcUa_StatusCode OPCUA_DLLCALL Timer_Callback (OpcUa_Void * pvCallbackData, OpcUa_Timer hTimer, OpcUa_UInt32 msecElapsed) {
	OpcUa_ReferenceParameter(pvCallbackData);
	OpcUa_ReferenceParameter(hTimer);

    ID_SessionManager_ValidateSessionLifetime(GlobalSessionManagerHandle, msecElapsed);
    
    // write to serverstatus timestamp
    OpcUa_NodeId ServerStatus = ID_OPCNodeId(OpcUaId_Server_ServerStatus);
    ID_VariableNode * ServerStatusVar = ID_AddressSpace_GetNode(GlobalAddressSpaceHandle, &ServerStatus);
	if (ServerStatusVar != NULL) {
		OpcUa_ServerStatusDataType * SSStructure = ServerStatusVar->Value.Value.ExtensionObject->Body.EncodeableObject.Object;
		SSStructure->CurrentTime = OpcUa_DateTime_UtcNow();

		OpcUa_QualifiedName CTName;
		OpcUa_QualifiedName_Initialize(&CTName);
		CTName.NamespaceIndex = 0;
		OpcUa_String_AttachCopy(&CTName.Name, "CurrentTime");
        ID_VariableNode * CurrentTime = ID_AddressSpace_HuntForReferenceWithNode(GlobalAddressSpaceHandle, (ID_BaseAttributes*)ServerStatusVar, CTName);
		CurrentTime->Value.Value.DateTime = SSStructure->CurrentTime;
		OpcUa_QualifiedName_Clear(&CTName);
	}

	return OpcUa_Good;
}

// implemented function declarations
OpcUa_ServiceType UA_FindServersServiceType = {OpcUaId_FindServersRequest, &OpcUa_FindServersResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginFindServers, (OpcUa_PfnInvokeService*)ID_UA_FindServers};
OpcUa_ServiceType UA_GetEndpointsServiceType = {OpcUaId_GetEndpointsRequest, &OpcUa_GetEndpointsResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginGetEndpoints, (OpcUa_PfnInvokeService*)ID_UA_GetEndpoints};
OpcUa_ServiceType UA_CreateSessionServiceType = {OpcUaId_CreateSessionRequest, &OpcUa_CreateSessionResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginCreateSession, (OpcUa_PfnInvokeService*)ID_UA_CreateSession};
OpcUa_ServiceType UA_ActivateSessionServiceType = {OpcUaId_ActivateSessionRequest, &OpcUa_ActivateSessionResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginActivateSession, (OpcUa_PfnInvokeService*)ID_UA_ActivateSession};
OpcUa_ServiceType UA_CloseSessionServiceType = {OpcUaId_CloseSessionRequest, &OpcUa_CloseSessionResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginCloseSession, (OpcUa_PfnInvokeService*)ID_UA_CloseSession};
OpcUa_ServiceType UA_BrowseServiceType = {OpcUaId_BrowseRequest, &OpcUa_BrowseResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginBrowse, (OpcUa_PfnInvokeService*)ID_UA_Browse};
OpcUa_ServiceType UA_BrowseNextServiceType = {OpcUaId_BrowseNextRequest, &OpcUa_BrowseNextResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginBrowseNext, (OpcUa_PfnInvokeService*)ID_UA_BrowseNext};
OpcUa_ServiceType UA_TranslateBrowsePathsToNodeIdServiceType = {OpcUaId_TranslateBrowsePathsToNodeIdsRequest, &OpcUa_TranslateBrowsePathsToNodeIdsResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginTranslateBrowsePathsToNodeIds, (OpcUa_PfnInvokeService*)ID_UA_TranslateBrowsePathsToNodeIds};
OpcUa_ServiceType UA_RegisterNodesServiceType = {OpcUaId_RegisterNodesRequest, &OpcUa_RegisterNodesResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginRegisterNodes, (OpcUa_PfnInvokeService*)ID_UA_RegisterNodes};
OpcUa_ServiceType UA_UnregisterNodesServiceType = {OpcUaId_UnregisterNodesRequest, &OpcUa_UnregisterNodesResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginUnregisterNodes, (OpcUa_PfnInvokeService*)ID_UA_UnregisterNodes};
OpcUa_ServiceType UA_ReadServiceType = {OpcUaId_ReadRequest, &OpcUa_ReadResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginRead, (OpcUa_PfnInvokeService*)ID_UA_Read};
OpcUa_ServiceType UA_WriteServiceType = {OpcUaId_WriteRequest, &OpcUa_WriteResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginWrite, (OpcUa_PfnInvokeService*)ID_UA_Write};
OpcUa_ServiceType UA_CallServiceType = {OpcUaId_CallRequest, &OpcUa_CallResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginCall, (OpcUa_PfnInvokeService*)ID_UA_Call};

// not implemented function declarations
OpcUa_ServiceType UA_FindServersOnNetworkServiceType = {OpcUaId_FindServersOnNetworkRequest, &OpcUa_FindServersOnNetworkResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginFindServersOnNetwork, (OpcUa_PfnInvokeService*)ID_UA_FindServersOnNetwork};
OpcUa_ServiceType UA_RegisterServerServiceType = {OpcUaId_RegisterServerRequest, &OpcUa_RegisterServerResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginRegisterServer, (OpcUa_PfnInvokeService*)ID_UA_RegisterServer};
OpcUa_ServiceType UA_RegisterServer2ServiceType = {OpcUaId_RegisterServer2Request, &OpcUa_RegisterServer2Response_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginRegisterServer2, (OpcUa_PfnInvokeService*)ID_UA_RegisterServer2};
OpcUa_ServiceType UA_CancelServiceType = {OpcUaId_CancelRequest, &OpcUa_CancelResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginCancel, (OpcUa_PfnInvokeService*)ID_UA_Cancel};
OpcUa_ServiceType UA_AddNodesServiceType = {OpcUaId_AddNodesRequest, &OpcUa_AddNodesResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginAddNodes, (OpcUa_PfnInvokeService*)ID_UA_AddNodes};
OpcUa_ServiceType UA_AddReferencesServiceType = {OpcUaId_AddReferencesRequest, &OpcUa_AddReferencesResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginAddReferences, (OpcUa_PfnInvokeService*)ID_UA_AddReferences};
OpcUa_ServiceType UA_DeleteNodesServiceType = {OpcUaId_DeleteNodesRequest, &OpcUa_DeleteNodesResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginDeleteNodes, (OpcUa_PfnInvokeService*)ID_UA_DeleteNodes};
OpcUa_ServiceType UA_DeleteReferencesServiceType = {OpcUaId_DeleteReferencesRequest, &OpcUa_DeleteReferencesResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginDeleteReferences, (OpcUa_PfnInvokeService*)ID_UA_DeleteReferences};
OpcUa_ServiceType UA_QueryFirstServiceType = {OpcUaId_QueryFirstRequest, &OpcUa_QueryFirstResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginQueryFirst, (OpcUa_PfnInvokeService*)ID_UA_QueryFirst};
OpcUa_ServiceType UA_QueryNextServiceType = {OpcUaId_QueryNextRequest, &OpcUa_QueryNextResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginQueryNext, (OpcUa_PfnInvokeService*)ID_UA_QueryNext};
OpcUa_ServiceType UA_HistoryReadServiceType = {OpcUaId_HistoryReadRequest, &OpcUa_HistoryReadResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginHistoryRead, (OpcUa_PfnInvokeService*)ID_UA_HistoryRead};
OpcUa_ServiceType UA_HistoryUpdateServiceType = {OpcUaId_HistoryUpdateRequest, &OpcUa_HistoryUpdateResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginHistoryUpdate, (OpcUa_PfnInvokeService*)ID_UA_HistoryUpdate};
OpcUa_ServiceType UA_CreateMonitoredItemsServiceType = {OpcUaId_CreateMonitoredItemsRequest, &OpcUa_CreateMonitoredItemsResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginCreateMonitoredItems, (OpcUa_PfnInvokeService*)ID_UA_CreateMonitoredItems};
OpcUa_ServiceType UA_ModifyMonitoredItemsServiceType = {OpcUaId_ModifyMonitoredItemsRequest, &OpcUa_ModifyMonitoredItemsResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginModifyMonitoredItems, (OpcUa_PfnInvokeService*)ID_UA_ModifyMonitoredItems};
OpcUa_ServiceType UA_SetMonitoringModeServiceType = {OpcUaId_SetMonitoringModeRequest, &OpcUa_SetMonitoringModeResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginSetMonitoringMode, (OpcUa_PfnInvokeService*)ID_UA_SetMonitoringMode};
OpcUa_ServiceType UA_SetTriggeringServiceType = {OpcUaId_SetTriggeringRequest, &OpcUa_SetTriggeringResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginSetTriggering, (OpcUa_PfnInvokeService*)ID_UA_SetTriggering};
OpcUa_ServiceType UA_DeleteMonitoredItemsServiceType = {OpcUaId_DeleteMonitoredItemsRequest, &OpcUa_DeleteMonitoredItemsResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginDeleteMonitoredItems, (OpcUa_PfnInvokeService*)ID_UA_DeleteMonitoredItems};
OpcUa_ServiceType UA_CreateSubscriptionServiceType = {OpcUaId_CreateSubscriptionRequest, &OpcUa_CreateSubscriptionResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginCreateSubscription, (OpcUa_PfnInvokeService*)ID_UA_CreateSubscription};
OpcUa_ServiceType UA_ModifySubscriptionServiceType = {OpcUaId_ModifySubscriptionRequest, &OpcUa_ModifySubscriptionResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginModifySubscription, (OpcUa_PfnInvokeService*)ID_UA_ModifySubscription};
OpcUa_ServiceType UA_SetPublishingModeServiceType = {OpcUaId_SetPublishingModeRequest, &OpcUa_SetPublishingModeResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginSetPublishingMode, (OpcUa_PfnInvokeService*)ID_UA_SetPublishingMode};
OpcUa_ServiceType UA_PublishServiceType = {OpcUaId_PublishRequest, &OpcUa_PublishResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginPublish, (OpcUa_PfnInvokeService*)ID_UA_Publish};
OpcUa_ServiceType UA_RepublishServiceType = {OpcUaId_RepublishRequest, &OpcUa_RepublishResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginRepublish, (OpcUa_PfnInvokeService*)ID_UA_Republish};
OpcUa_ServiceType UA_TransferSubscriptionsServiceType = {OpcUaId_TransferSubscriptionsRequest, &OpcUa_TransferSubscriptionsResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginTransferSubscriptions, (OpcUa_PfnInvokeService*)ID_UA_TransferSubscriptions};
OpcUa_ServiceType UA_DeleteSubscriptionsServiceType = {OpcUaId_DeleteSubscriptionsRequest, &OpcUa_DeleteSubscriptionsResponse_EncodeableType, (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginDeleteSubscriptions, (OpcUa_PfnInvokeService*)ID_UA_DeleteSubscriptions};
OpcUa_ServiceType * UA_SupportedServiceTypes[] = {&UA_FindServersServiceType, &UA_GetEndpointsServiceType,&UA_CreateSessionServiceType, &UA_ActivateSessionServiceType, &UA_CloseSessionServiceType,
                                                  &UA_BrowseServiceType, &UA_BrowseNextServiceType, &UA_TranslateBrowsePathsToNodeIdServiceType, &UA_RegisterNodesServiceType,
                                                  &UA_UnregisterNodesServiceType, &UA_ReadServiceType, &UA_WriteServiceType, &UA_CallServiceType,
                                                  &UA_FindServersOnNetworkServiceType, &UA_RegisterServerServiceType, &UA_RegisterServer2ServiceType,
                                                  &UA_CancelServiceType, &UA_AddNodesServiceType, &UA_AddReferencesServiceType,
                                                  &UA_DeleteNodesServiceType, &UA_DeleteReferencesServiceType, &UA_QueryFirstServiceType,
                                                  &UA_QueryNextServiceType, &UA_HistoryReadServiceType, &UA_HistoryUpdateServiceType,
                                                  &UA_CreateMonitoredItemsServiceType, &UA_ModifyMonitoredItemsServiceType, &UA_SetMonitoringModeServiceType,
                                                  &UA_SetTriggeringServiceType, &UA_DeleteMonitoredItemsServiceType, &UA_CreateSubscriptionServiceType,
                                                  &UA_ModifySubscriptionServiceType, &UA_SetPublishingModeServiceType, &UA_PublishServiceType,
                                                  &UA_RepublishServiceType, &UA_TransferSubscriptionsServiceType, &UA_DeleteSubscriptionsServiceType, OpcUa_Null};

#ifdef __linux__
#include <sys/select.h>
#include <opcua_thread.h>
#include <opcua_timer.h>
#include <opcua_securelistener.h>
#include <malloc.h>
#include <opcua_core.h>
#include <unistd.h>
int _kbhit(void) {
    struct timeval tv;
    fd_set fd;
    memset(&tv, 0, sizeof(tv));
    memset(&fd, 0, sizeof(fd));
    FD_SET(0, &fd);
    return select(1, &fd, NULL, NULL, &tv)>0;
}
#endif

// checks for a keypress
OpcUa_Boolean ID_CheckForKeypress() {
    if (!_kbhit()) {
        OpcUa_Thread_Sleep(5000);
    } else {
        if (getchar()=='x') {
            return 1;
        }
    }
    
    return 0;
}

// callback function for changes in connection
OpcUa_StatusCode ID_Endpoint_CallbackFunction(OpcUa_Endpoint hEndpoint, OpcUa_Void* pvCallbackData, OpcUa_Endpoint_Event eEvent, OpcUa_StatusCode uStatus,
                                              OpcUa_UInt32 uSecureChannelId, OpcUa_ByteString* pbsClientCertificate, OpcUa_String* pSecurityPolicy, OpcUa_UInt16 uSecurityMode) {
	OpcUa_ReferenceParameter(uSecurityMode);
	OpcUa_ReferenceParameter(pSecurityPolicy);
	OpcUa_ReferenceParameter(uStatus);
	OpcUa_ReferenceParameter(pvCallbackData);
	OpcUa_ReferenceParameter(hEndpoint);
	
	if (eEvent == eOpcUa_Endpoint_Event_SecureChannelOpened) {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "Secure Channel Opened with Secure Channel ID %d", uSecureChannelId);
        ID_SessionManager_AssociateSecureChannelIdWithCertificate(GlobalSessionManagerHandle, uSecureChannelId, pbsClientCertificate);
    } else if (eEvent == eOpcUa_Endpoint_Event_SecureChannelRenewed) {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "Secure Channel Renewed with Secure Channel ID %d", uSecureChannelId);
        // link securechannelid with client certificate
        ID_SessionManager_AssociateSecureChannelIdWithCertificate(GlobalSessionManagerHandle, uSecureChannelId, pbsClientCertificate);
    } else if (eEvent == eOpcUa_Endpoint_Event_SecureChannelClosed || eEvent == eOpcUa_Endpoint_Event_Invalid || eEvent == eOpcUa_Endpoint_Event_DecoderError) {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "Secure Channel Closed with Secure Channel ID %d", uSecureChannelId); 
        ID_SessionManager_UnassociateSecureChannelId(GlobalSessionManagerHandle, uSecureChannelId);
    }
    
    return OpcUa_Good; 
}

#ifdef _WIN32
extern void ID_memoryDebugSQLFunction();

int __cdecl main(int argc, char** argv) {
#elif defined(__linux__)
int main(int argc, char** argv) {
#endif

#if defined(_WIN32) && defined(_DEBUG)
	// disable several str functions from force filling with 0xFE
	// to cover the presumably empty space described by the passed
	// in parameter to the function...
	// this is a fix (somewhat sloppy) combined with adjusted values
	// in cJSON to fix the cJSON memory leak issue that seemingly differs
	// between the Linux and Windows instances
	//_CrtSetDebugFillThreshold(0);

	//memoryDebugSQLFunction();
#endif

    // timeout handler
    int timeout = -1;
    if (argc == 2) {
        timeout = atoi(argv[1]);
    }

    // initialize proxystubconfiguration and platformlayerhandle
    OpcUa_ProxyStubConfiguration UaServer_g_pProxyStubConfiguration;
    OpcUa_Handle UaServer_g_PlatformLayerHandle;
    
    UaServer_g_pProxyStubConfiguration.bProxyStub_Trace_Enabled              = OpcUa_True;   //to deactivate Tracer set this variable Opc Ua False.
    UaServer_g_pProxyStubConfiguration.uProxyStub_Trace_Level                = OPCUA_TRACE_OUTPUT_LEVEL_WARNING;
    UaServer_g_pProxyStubConfiguration.iSerializer_MaxAlloc                  = -1;
    UaServer_g_pProxyStubConfiguration.iSerializer_MaxStringLength           = -1;
    UaServer_g_pProxyStubConfiguration.iSerializer_MaxByteStringLength       = -1;
    UaServer_g_pProxyStubConfiguration.iSerializer_MaxArrayLength            = -1;
    UaServer_g_pProxyStubConfiguration.iSerializer_MaxMessageSize            = -1;
    UaServer_g_pProxyStubConfiguration.iSerializer_MaxRecursionDepth         = -1;
    UaServer_g_pProxyStubConfiguration.bSecureListener_ThreadPool_Enabled    = OpcUa_False;
    UaServer_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MinThreads = -1;
    UaServer_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MaxThreads = -1;
    UaServer_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MaxJobs    = -1;
    UaServer_g_pProxyStubConfiguration.bSecureListener_ThreadPool_BlockOnAdd = OpcUa_True;
    UaServer_g_pProxyStubConfiguration.uSecureListener_ThreadPool_Timeout    = OPCUA_INFINITE;
    UaServer_g_pProxyStubConfiguration.bTcpListener_ClientThreadsEnabled     = OpcUa_False;
    UaServer_g_pProxyStubConfiguration.iTcpListener_DefaultChunkSize         = -1;
    UaServer_g_pProxyStubConfiguration.iTcpConnection_DefaultChunkSize       = -1;
    UaServer_g_pProxyStubConfiguration.iTcpTransport_MaxMessageLength        = -1;
    UaServer_g_pProxyStubConfiguration.iTcpTransport_MaxChunkCount           = -1;
    UaServer_g_pProxyStubConfiguration.bTcpStream_ExpectWriteToBlock         = OpcUa_True;

    /* initialize platform layer */
    OpcUa_P_Initialize(&UaServer_g_PlatformLayerHandle);
    OpcUa_ProxyStub_Initialize(UaServer_g_PlatformLayerHandle, &UaServer_g_pProxyStubConfiguration);
    
    // reserve ODBC identifiers
    ID_IndexerHandle Indexer;
    ID_Indexer_Create(&Indexer);
    ID_Initialize_ReserveIndexes(Indexer);
    GlobalIndexerHandle = Indexer;
    
    // all global handles
    ID_AddressSpaceHandle Handle;
    ID_AddressSpace_Create(&Handle);
    GlobalAddressSpaceHandle = Handle;

    ID_AliaserHandle Aliaser;
    ID_Aliaser_Create(&Aliaser);
    GlobalAliaserHandle = Aliaser;
    
    ID_ContinuerHandle ContinuerHandle;
    ID_Continuer_Create(&ContinuerHandle);
    GlobalContinuerHandle = ContinuerHandle;
    
    ID_SecurityHandle SecurityHandle;
    ID_UASecurityManager_Create(&SecurityHandle);
    GlobalSecurityHandle = SecurityHandle;
    
    ID_SessionManagerHandle SessionManagerHandle;
    ID_SessionManager_Create(&SessionManagerHandle);
    GlobalSessionManagerHandle = SessionManagerHandle;
    
    #ifdef _DEBUG
    OpcUa_Boolean IsDebug = OpcUa_True;
    #else
    OpcUa_Boolean IsDebug = OpcUa_False;
    #endif

    // intiialize AddressSpace
    ID_Initialize_OPCAddressSpace(Handle, Indexer, IsDebug);

    // construct fastlist access
    ID_AddressSpace_ConstructFastList(Handle);

    // create timer
    OpcUa_Timer_Create(&Timer, 5, &Timer_Callback, NULL, NULL);
    
    // initialize endpoint
    OpcUa_Endpoint Endpoint = OpcUa_Null;
    OpcUa_Endpoint_Create(&Endpoint, OpcUa_Endpoint_SerializerType_Binary, UA_SupportedServiceTypes);
    OpcUa_Endpoint_Open(Endpoint, ID_ENDPOINT_URL, OpcUa_True, &ID_Endpoint_CallbackFunction, OpcUa_Null, &SecurityHandle->ServerCertificate, &SecurityHandle->ServerPrivateKey, &SecurityHandle->StoreConfig, 2, SecurityHandle->SecurityPolicyConfigurations);
    printf("Endpoint started...\n");
    
    if (timeout == -1) {
        while (!ID_CheckForKeypress()) {
        
        }
    } else {
        sleep(timeout);
    }
      
    // Cleanup server
    OpcUa_Endpoint_Close(Endpoint);
    OpcUa_Endpoint_Delete(&Endpoint);
    OpcUa_Timer_Delete(&Timer);
    ID_Indexer_Delete(&Indexer);
    ID_Aliaser_Delete(&Aliaser);
    ID_Continuer_Destroy(&ContinuerHandle);
    ID_SessionManager_Destroy(&SessionManagerHandle);
    ID_UASecurityManager_Destroy(&SecurityHandle);
    ID_AddressSpace_Destroy(&Handle);
    
    OpcUa_P_Clean(&UaServer_g_PlatformLayerHandle);
    OpcUa_ProxyStub_Clear();
	
    return (EXIT_SUCCESS);
}