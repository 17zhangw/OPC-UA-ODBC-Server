/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef EXPOSEDFUNCTIONS_H
#define EXPOSEDFUNCTIONS_H

#include <opcua.h>
#include <opcua_serverapi.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    #define ID_CHECK_SESSION() \
        ID_Session * UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken); \
        if (UserSession == NULL) { \
            uStatus = OpcUa_BadSessionIdInvalid; \
            goto Error; \
        } \
        \
        OpcUa_UInt32 SecureChannelId; \
        OpcUa_Endpoint_GetMessageSecureChannelId(hEndpoint, hContext, &SecureChannelId); \
        if (UserSession->SecureChannelId != SecureChannelId) { \
            uStatus = OpcUa_BadSecureChannelIdInvalid; \
            goto Error; \
        } \
        \
        if (ID_SessionManager_IsSessionValid(GlobalSessionManagerHandle, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken, hEndpoint, hContext, OpcUa_False) == OpcUa_False) { \
            uStatus = OpcUa_BadSessionNotActivated; \
            ID_Continuer_RemoveContinuationPointWithSession(GlobalContinuerHandle, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken); \
            ID_SessionManager_RemoveSession(GlobalSessionManagerHandle, (OpcUa_NodeId *)&pRequestHeader->AuthenticationToken); \
            goto Error; \
        } \
        \
        if (pRequestHeader->Timestamp.dwHighDateTime == 0 && pRequestHeader->Timestamp.dwLowDateTime == 0) { \
            uStatus = OpcUa_BadInvalidTimestamp; \
            goto Error; \
        } \

    OpcUa_StatusCode ID_UA_FindServers(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
							           const OpcUa_String* pEndpointUrl, OpcUa_Int32 nNoOfLocaleIds, const OpcUa_String* pLocaleIds,
								       OpcUa_Int32 nNoOfServerUris, const OpcUa_String* pServerUris, OpcUa_ResponseHeader* pResponseHeader,
									   OpcUa_Int32* pNoOfServers, OpcUa_ApplicationDescription** pServers);
    
    OpcUa_StatusCode ID_UA_GetEndpoints(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
										const OpcUa_String* pEndpointUrl, OpcUa_Int32 nNoOfLocaleIds, const OpcUa_String* pLocaleIds,
										OpcUa_Int32 nNoOfProfileUris, const OpcUa_String* pProfileUris, OpcUa_ResponseHeader* pResponseHeader,
										OpcUa_Int32* pNoOfEndpoints, OpcUa_EndpointDescription** pEndpoints);
    
    OpcUa_StatusCode ID_UA_CreateSession(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
										 const OpcUa_ApplicationDescription* pClientDescription, const OpcUa_String* pServerUri,
										 const OpcUa_String* pEndpointUrl, const OpcUa_String* pSessionName, const OpcUa_ByteString* pClientNonce,
										 const OpcUa_ByteString* pClientCertificate, OpcUa_Double nRequestedSessionTimeout,
										 OpcUa_UInt32 nMaxResponseMessageSize, OpcUa_ResponseHeader* pResponseHeader, OpcUa_NodeId* pSessionId,
										 OpcUa_NodeId* pAuthenticationToken, OpcUa_Double* pRevisedSessionTimeout, OpcUa_ByteString* pServerNonce,
										 OpcUa_ByteString* pServerCertificate, OpcUa_Int32* pNoOfServerEndpoints, 
										 OpcUa_EndpointDescription** pServerEndpoints, OpcUa_Int32* pNoOfServerSoftwareCertificates,
										 OpcUa_SignedSoftwareCertificate** pServerSoftwareCertificates, OpcUa_SignatureData* pServerSignature,
										 OpcUa_UInt32* pMaxRequestMessageSize);
    
    OpcUa_StatusCode ID_UA_ActivateSession(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
							               const OpcUa_SignatureData* pClientSignature, OpcUa_Int32 nNoOfClientSoftwareCertificates,
										   const OpcUa_SignedSoftwareCertificate* pClientSoftwareCertificates, OpcUa_Int32 nNoOfLocaleIds,
                                           const OpcUa_String* pLocaleIds, const OpcUa_ExtensionObject* pUserIdentityToken,
                                           const OpcUa_SignatureData* pUserTokenSignature, OpcUa_ResponseHeader* pResponseHeader,
                                           OpcUa_ByteString* pServerNonce, OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults,
                                           OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_CloseSession(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                        OpcUa_Boolean bDeleteSubscriptions, OpcUa_ResponseHeader* pResponseHeader);
    
    OpcUa_StatusCode ID_UA_Browse(OpcUa_Endpoint hEndpoint,OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                  const OpcUa_ViewDescription* pView, OpcUa_UInt32 nRequestedMaxReferencesPerNode, OpcUa_Int32 nNoOfNodesToBrowse,
                                  const OpcUa_BrowseDescription* pNodesToBrowse, OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults,
                                  OpcUa_BrowseResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_BrowseNext(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext,const OpcUa_RequestHeader* pRequestHeader,
                                      OpcUa_Boolean bReleaseContinuationPoints, OpcUa_Int32 nNoOfContinuationPoints, 
                                      const OpcUa_ByteString* pContinuationPoints, OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults,
                                      OpcUa_BrowseResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_TranslateBrowsePathsToNodeIds(OpcUa_Endpoint hEndpoint,OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                                         OpcUa_Int32 nNoOfBrowsePaths, const OpcUa_BrowsePath* pBrowsePaths, 
                                                         OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, 
                                                         OpcUa_BrowsePathResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, 
                                                         OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_RegisterNodes(OpcUa_Endpoint hEndpoint,OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                         OpcUa_Int32 nNoOfNodesToRegister, const OpcUa_NodeId* pNodesToRegister, 
                                         OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfRegisteredNodeIds,
                                         OpcUa_NodeId** pRegisteredNodeIds);
    
    OpcUa_StatusCode ID_UA_UnregisterNodes(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                           OpcUa_Int32 nNoOfNodesToUnregister, const OpcUa_NodeId* pNodesToUnregister,
                                           OpcUa_ResponseHeader* pResponseHeader);
    
    OpcUa_StatusCode ID_UA_Read(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader, OpcUa_Double nMaxAge,
                                OpcUa_TimestampsToReturn eTimestampsToReturn, OpcUa_Int32 nNoOfNodesToRead, const OpcUa_ReadValueId* pNodesToRead,
                                OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_DataValue** pResults,
                                OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_Write(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                 OpcUa_Int32 nNoOfNodesToWrite, const OpcUa_WriteValue* pNodesToWrite, OpcUa_ResponseHeader* pResponseHeader,
                                 OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, 
                                 OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_Call(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                OpcUa_Int32 nNoOfMethodsToCall, const OpcUa_CallMethodRequest* pMethodsToCall, 
                                OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_CallMethodResult** pResults,
                                OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    /* begin unimplemented functions */
    OpcUa_StatusCode ID_UA_FindServersOnNetwork(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                                OpcUa_UInt32 nStartingRecordId, OpcUa_UInt32 nMaxRecordsToReturn, OpcUa_Int32 nNoOfServerCapabilityFilter,
                                                const OpcUa_String* pServerCapabilityFilter, OpcUa_ResponseHeader* pResponseHeader, OpcUa_DateTime* pLastCounterResetTime,
                                                OpcUa_Int32* pNoOfServers, OpcUa_ServerOnNetwork** pServers);
    
    OpcUa_StatusCode ID_UA_RegisterServer(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                          const OpcUa_RegisteredServer* pServer, OpcUa_ResponseHeader* pResponseHeader);
    
    OpcUa_StatusCode ID_UA_RegisterServer2(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                           const OpcUa_RegisteredServer* pServer, OpcUa_Int32 nNoOfDiscoveryConfiguration, const OpcUa_ExtensionObject* pDiscoveryConfiguration,
                                           OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfConfigurationResults, OpcUa_StatusCode** pConfigurationResults,
                                           OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_Cancel(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                  OpcUa_UInt32 nRequestHandle, OpcUa_ResponseHeader* pResponseHeader, OpcUa_UInt32* pCancelCount);
    
    OpcUa_StatusCode ID_UA_AddNodes(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                    OpcUa_Int32 nNoOfNodesToAdd, const OpcUa_AddNodesItem* pNodesToAdd, OpcUa_ResponseHeader* pResponseHeader,
                                    OpcUa_Int32* pNoOfResults, OpcUa_AddNodesResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos,
                                    OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_AddReferences(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                         OpcUa_Int32 nNoOfReferencesToAdd, const OpcUa_AddReferencesItem* pReferencesToAdd, OpcUa_ResponseHeader* pResponseHeader,
                                         OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos,
                                         OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_DeleteNodes(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                       OpcUa_Int32 nNoOfNodesToDelete, const OpcUa_DeleteNodesItem* pNodesToDelete, OpcUa_ResponseHeader* pResponseHeader,
                                       OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_DeleteReferences(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                            OpcUa_Int32 nNoOfReferencesToDelete, const OpcUa_DeleteReferencesItem* pReferencesToDelete,
                                            OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults,
                                            OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_QueryFirst(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                      const OpcUa_ViewDescription* pView, OpcUa_Int32 nNoOfNodeTypes, const OpcUa_NodeTypeDescription* pNodeTypes,
                                      const OpcUa_ContentFilter* pFilter, OpcUa_UInt32 nMaxDataSetsToReturn, OpcUa_UInt32 nMaxReferencesToReturn,
                                      OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfQueryDataSets, OpcUa_QueryDataSet** pQueryDataSets,
                                      OpcUa_ByteString* pContinuationPoint, OpcUa_Int32* pNoOfParsingResults, OpcUa_ParsingResult** pParsingResults,
                                      OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos, OpcUa_ContentFilterResult* pFilterResult);
    
    OpcUa_StatusCode ID_UA_QueryNext(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                     OpcUa_Boolean bReleaseContinuationPoint, const OpcUa_ByteString* pContinuationPoint, OpcUa_ResponseHeader* pResponseHeader,
                                     OpcUa_Int32* pNoOfQueryDataSets, OpcUa_QueryDataSet** pQueryDataSets, OpcUa_ByteString* pRevisedContinuationPoint);
    
    OpcUa_StatusCode ID_UA_HistoryRead(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                       const OpcUa_ExtensionObject* pHistoryReadDetails, OpcUa_TimestampsToReturn eTimestampsToReturn, OpcUa_Boolean bReleaseContinuationPoints,
                                       OpcUa_Int32 nNoOfNodesToRead, const OpcUa_HistoryReadValueId* pNodesToRead, OpcUa_ResponseHeader* pResponseHeader,
                                       OpcUa_Int32* pNoOfResults, OpcUa_HistoryReadResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_HistoryUpdate(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                         OpcUa_Int32 nNoOfHistoryUpdateDetails, const OpcUa_ExtensionObject* pHistoryUpdateDetails,
                                         OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_HistoryUpdateResult** pResults,
                                         OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_CreateMonitoredItems(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                                OpcUa_UInt32 nSubscriptionId, OpcUa_TimestampsToReturn eTimestampsToReturn,
                                                OpcUa_Int32 nNoOfItemsToCreate, const OpcUa_MonitoredItemCreateRequest* pItemsToCreate,
                                                OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_MonitoredItemCreateResult** pResults,
                                                OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_ModifyMonitoredItems(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                                OpcUa_UInt32 nSubscriptionId, OpcUa_TimestampsToReturn eTimestampsToReturn,
                                                OpcUa_Int32 nNoOfItemsToModify, const OpcUa_MonitoredItemModifyRequest* pItemsToModify,
                                                OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_MonitoredItemModifyResult** pResults,
                                                OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_SetMonitoringMode(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                             OpcUa_UInt32 nSubscriptionId, OpcUa_MonitoringMode eMonitoringMode, OpcUa_Int32 nNoOfMonitoredItemIds,
                                             const OpcUa_UInt32* pMonitoredItemIds, OpcUa_ResponseHeader* pResponseHeader,
                                             OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_SetTriggering(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                         OpcUa_UInt32 nSubscriptionId, OpcUa_UInt32 nTriggeringItemId, OpcUa_Int32 nNoOfLinksToAdd,
                                         const OpcUa_UInt32* pLinksToAdd, OpcUa_Int32 nNoOfLinksToRemove, const OpcUa_UInt32* pLinksToRemove,
                                         OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfAddResults, OpcUa_StatusCode** pAddResults,
                                         OpcUa_Int32* pNoOfAddDiagnosticInfos, OpcUa_DiagnosticInfo** pAddDiagnosticInfos, OpcUa_Int32* pNoOfRemoveResults,
                                         OpcUa_StatusCode** pRemoveResults, OpcUa_Int32* pNoOfRemoveDiagnosticInfos, OpcUa_DiagnosticInfo** pRemoveDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_DeleteMonitoredItems(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                                OpcUa_UInt32 nSubscriptionId, OpcUa_Int32 nNoOfMonitoredItemIds, const OpcUa_UInt32* pMonitoredItemIds,
                                                OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults,
                                                OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_CreateSubscription(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                              OpcUa_Double nRequestedPublishingInterval, OpcUa_UInt32 nRequestedLifetimeCount,
                                              OpcUa_UInt32 nRequestedMaxKeepAliveCount, OpcUa_UInt32 nMaxNotificationsPerPublish,
                                              OpcUa_Boolean bPublishingEnabled, OpcUa_Byte nPriority, OpcUa_ResponseHeader* pResponseHeader,
                                              OpcUa_UInt32* pSubscriptionId, OpcUa_Double* pRevisedPublishingInterval,
                                           OpcUa_UInt32* pRevisedLifetimeCount, OpcUa_UInt32* pRevisedMaxKeepAliveCount);
    
    OpcUa_StatusCode ID_UA_ModifySubscription(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                              OpcUa_UInt32 nSubscriptionId, OpcUa_Double nRequestedPublishingInterval, OpcUa_UInt32 nRequestedLifetimeCount,
                                              OpcUa_UInt32 nRequestedMaxKeepAliveCount, OpcUa_UInt32 nMaxNotificationsPerPublish, OpcUa_Byte nPriority,
                                              OpcUa_ResponseHeader* pResponseHeader, OpcUa_Double* pRevisedPublishingInterval,
                                              OpcUa_UInt32* pRevisedLifetimeCount, OpcUa_UInt32* pRevisedMaxKeepAliveCount);
    
    OpcUa_StatusCode ID_UA_SetPublishingMode(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                             OpcUa_Boolean bPublishingEnabled, OpcUa_Int32 nNoOfSubscriptionIds, const OpcUa_UInt32* pSubscriptionIds,
                                             OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults,
                                             OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_Publish(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                   OpcUa_Int32 nNoOfSubscriptionAcknowledgements, const OpcUa_SubscriptionAcknowledgement* pSubscriptionAcknowledgements,
                                   OpcUa_ResponseHeader* pResponseHeader, OpcUa_UInt32* pSubscriptionId, OpcUa_Int32* pNoOfAvailableSequenceNumbers,
                                   OpcUa_UInt32** pAvailableSequenceNumbers, OpcUa_Boolean* pMoreNotifications, OpcUa_NotificationMessage* pNotificationMessage,
                                   OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);

    OpcUa_StatusCode ID_UA_Republish(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                     OpcUa_UInt32 nSubscriptionId, OpcUa_UInt32 nRetransmitSequenceNumber,
                                     OpcUa_ResponseHeader* pResponseHeader, OpcUa_NotificationMessage* pNotificationMessage);
    
    OpcUa_StatusCode ID_UA_TransferSubscriptions(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                                 OpcUa_Int32 nNoOfSubscriptionIds, const OpcUa_UInt32* pSubscriptionIds, OpcUa_Boolean bSendInitialValues,
                                                 OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_TransferResult** pResults,
                                                 OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    OpcUa_StatusCode ID_UA_DeleteSubscriptions(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                               OpcUa_Int32 nNoOfSubscriptionIds, const OpcUa_UInt32* pSubscriptionIds, OpcUa_ResponseHeader* pResponseHeader,
                                               OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos);
    
    void ID_UA_InitializeResponseHeader(OpcUa_ResponseHeader * ResponseHeader, const OpcUa_RequestHeader * RequestHeader, OpcUa_StatusCode Status);
    void ID_FillServerEndpoints(OpcUa_EndpointDescription * Endpoint);

#ifdef __cplusplus
}
#endif

#endif /* EXPOSEDFUNCTIONS_H */

