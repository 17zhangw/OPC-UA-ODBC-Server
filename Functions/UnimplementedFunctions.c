/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "ExposedFunctions.h"

OpcUa_StatusCode ID_UA_FindServersOnNetwork(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                            OpcUa_UInt32 nStartingRecordId, OpcUa_UInt32 nMaxRecordsToReturn, OpcUa_Int32 nNoOfServerCapabilityFilter,
                                            const OpcUa_String* pServerCapabilityFilter, OpcUa_ResponseHeader* pResponseHeader, OpcUa_DateTime* pLastCounterResetTime,
                                            OpcUa_Int32* pNoOfServers, OpcUa_ServerOnNetwork** pServers) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_FindServersOnNetwork");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(pRequestHeader);
	OpcUa_ReferenceParameter(nStartingRecordId);
	OpcUa_ReferenceParameter(nMaxRecordsToReturn);
	OpcUa_ReferenceParameter(nNoOfServerCapabilityFilter);
	OpcUa_ReferenceParameter(pServerCapabilityFilter);
	OpcUa_ReferenceParameter(pResponseHeader);
	OpcUa_ReferenceParameter(pLastCounterResetTime);
	OpcUa_ReferenceParameter(pNoOfServers);
	OpcUa_ReferenceParameter(pServers);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_RegisterServer(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                      const OpcUa_RegisteredServer* pServer, OpcUa_ResponseHeader* pResponseHeader) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_RegisterServer");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(pRequestHeader);
	OpcUa_ReferenceParameter(pServer);
	OpcUa_ReferenceParameter(pResponseHeader);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_RegisterServer2(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                       const OpcUa_RegisteredServer* pServer, OpcUa_Int32 nNoOfDiscoveryConfiguration, const OpcUa_ExtensionObject* pDiscoveryConfiguration,
                                       OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfConfigurationResults, OpcUa_StatusCode** pConfigurationResults,
                                       OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_RegisterServer2");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(pRequestHeader);
	OpcUa_ReferenceParameter(pServer);
	OpcUa_ReferenceParameter(nNoOfDiscoveryConfiguration);
	OpcUa_ReferenceParameter(pDiscoveryConfiguration);
	OpcUa_ReferenceParameter(pResponseHeader);
	OpcUa_ReferenceParameter(pNoOfConfigurationResults);
	OpcUa_ReferenceParameter(pConfigurationResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_Cancel(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                              OpcUa_UInt32 nRequestHandle, OpcUa_ResponseHeader* pResponseHeader, OpcUa_UInt32* pCancelCount) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_Cancel");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(pRequestHeader);
	OpcUa_ReferenceParameter(nRequestHandle);
	OpcUa_ReferenceParameter(pResponseHeader);
	OpcUa_ReferenceParameter(pCancelCount);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_AddNodes(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                OpcUa_Int32 nNoOfNodesToAdd, const OpcUa_AddNodesItem* pNodesToAdd, OpcUa_ResponseHeader* pResponseHeader,
                                OpcUa_Int32* pNoOfResults, OpcUa_AddNodesResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos,
                                OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_AddNodes");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(pRequestHeader);
	OpcUa_ReferenceParameter(nNoOfNodesToAdd);
	OpcUa_ReferenceParameter(pNodesToAdd);
	OpcUa_ReferenceParameter(pResponseHeader);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_AddReferences(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                     OpcUa_Int32 nNoOfReferencesToAdd, const OpcUa_AddReferencesItem* pReferencesToAdd, OpcUa_ResponseHeader* pResponseHeader,
                                     OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos,
                                     OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_AddReferences");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(pRequestHeader);
	OpcUa_ReferenceParameter(nNoOfReferencesToAdd);
	OpcUa_ReferenceParameter(pReferencesToAdd);
	OpcUa_ReferenceParameter(pResponseHeader);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_DeleteNodes(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                   OpcUa_Int32 nNoOfNodesToDelete, const OpcUa_DeleteNodesItem* pNodesToDelete, OpcUa_ResponseHeader* pResponseHeader,
                                   OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_DeleteNodes");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nNoOfNodesToDelete);
	OpcUa_ReferenceParameter(pNodesToDelete);
	OpcUa_ReferenceParameter(pResponseHeader);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_DeleteReferences(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                        OpcUa_Int32 nNoOfReferencesToDelete, const OpcUa_DeleteReferencesItem* pReferencesToDelete,
                                        OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults,
                                        OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_DeleteReferences");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nNoOfReferencesToDelete);
	OpcUa_ReferenceParameter(pReferencesToDelete);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_QueryFirst(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                  const OpcUa_ViewDescription* pView, OpcUa_Int32 nNoOfNodeTypes, const OpcUa_NodeTypeDescription* pNodeTypes,
                                  const OpcUa_ContentFilter* pFilter, OpcUa_UInt32 nMaxDataSetsToReturn, OpcUa_UInt32 nMaxReferencesToReturn,
                                  OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfQueryDataSets, OpcUa_QueryDataSet** pQueryDataSets,
                                  OpcUa_ByteString* pContinuationPoint, OpcUa_Int32* pNoOfParsingResults, OpcUa_ParsingResult** pParsingResults,
                                  OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos, OpcUa_ContentFilterResult* pFilterResult) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_QueryFirst");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(pView);
	OpcUa_ReferenceParameter(nNoOfNodeTypes);
	OpcUa_ReferenceParameter(pNodeTypes);
	OpcUa_ReferenceParameter(pFilter);
	OpcUa_ReferenceParameter(nMaxDataSetsToReturn);
	OpcUa_ReferenceParameter(nMaxReferencesToReturn);
	OpcUa_ReferenceParameter(pNoOfQueryDataSets);
	OpcUa_ReferenceParameter(pQueryDataSets);
	OpcUa_ReferenceParameter(pContinuationPoint);
	OpcUa_ReferenceParameter(pNoOfParsingResults);
	OpcUa_ReferenceParameter(pParsingResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);
	OpcUa_ReferenceParameter(pFilterResult);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_QueryNext(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                 OpcUa_Boolean bReleaseContinuationPoint, const OpcUa_ByteString* pContinuationPoint, OpcUa_ResponseHeader* pResponseHeader,
                                 OpcUa_Int32* pNoOfQueryDataSets, OpcUa_QueryDataSet** pQueryDataSets, OpcUa_ByteString* pRevisedContinuationPoint) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_QueryNext");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(bReleaseContinuationPoint);
	OpcUa_ReferenceParameter(pContinuationPoint);
	OpcUa_ReferenceParameter(pNoOfQueryDataSets);
	OpcUa_ReferenceParameter(pQueryDataSets);
	OpcUa_ReferenceParameter(pRevisedContinuationPoint);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_HistoryRead(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                   const OpcUa_ExtensionObject* pHistoryReadDetails, OpcUa_TimestampsToReturn eTimestampsToReturn, OpcUa_Boolean bReleaseContinuationPoints,
                                   OpcUa_Int32 nNoOfNodesToRead, const OpcUa_HistoryReadValueId* pNodesToRead, OpcUa_ResponseHeader* pResponseHeader,
                                   OpcUa_Int32* pNoOfResults, OpcUa_HistoryReadResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
	OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_HistoryRead");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(pHistoryReadDetails);
	OpcUa_ReferenceParameter(eTimestampsToReturn);
	OpcUa_ReferenceParameter(bReleaseContinuationPoints);
	OpcUa_ReferenceParameter(nNoOfNodesToRead);
	OpcUa_ReferenceParameter(pNodesToRead);
	OpcUa_ReferenceParameter(pResponseHeader);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_HistoryUpdate(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                     OpcUa_Int32 nNoOfHistoryUpdateDetails, const OpcUa_ExtensionObject* pHistoryUpdateDetails,
                                     OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_HistoryUpdateResult** pResults,
                                     OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_HistoryUpdate");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nNoOfHistoryUpdateDetails);
	OpcUa_ReferenceParameter(pHistoryUpdateDetails);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_CreateMonitoredItems(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                            OpcUa_UInt32 nSubscriptionId, OpcUa_TimestampsToReturn eTimestampsToReturn,
                                            OpcUa_Int32 nNoOfItemsToCreate, const OpcUa_MonitoredItemCreateRequest* pItemsToCreate,
                                            OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_MonitoredItemCreateResult** pResults,
                                            OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_CreateMonitoredItems");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nSubscriptionId);
	OpcUa_ReferenceParameter(eTimestampsToReturn);
	OpcUa_ReferenceParameter(nNoOfItemsToCreate);
	OpcUa_ReferenceParameter(pItemsToCreate);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_ModifyMonitoredItems(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                            OpcUa_UInt32 nSubscriptionId, OpcUa_TimestampsToReturn eTimestampsToReturn,
                                            OpcUa_Int32 nNoOfItemsToModify, const OpcUa_MonitoredItemModifyRequest* pItemsToModify,
                                            OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_MonitoredItemModifyResult** pResults,
                                            OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_ModifyMonitoredItems");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nSubscriptionId);
	OpcUa_ReferenceParameter(eTimestampsToReturn);
	OpcUa_ReferenceParameter(nNoOfItemsToModify);
	OpcUa_ReferenceParameter(pItemsToModify);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_SetMonitoringMode(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                         OpcUa_UInt32 nSubscriptionId, OpcUa_MonitoringMode eMonitoringMode, OpcUa_Int32 nNoOfMonitoredItemIds,
                                         const OpcUa_UInt32* pMonitoredItemIds, OpcUa_ResponseHeader* pResponseHeader,
                                         OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_SetMonitoringMode");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nSubscriptionId);
	OpcUa_ReferenceParameter(eMonitoringMode);
	OpcUa_ReferenceParameter(nNoOfMonitoredItemIds);
	OpcUa_ReferenceParameter(pMonitoredItemIds);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_SetTriggering(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                     OpcUa_UInt32 nSubscriptionId, OpcUa_UInt32 nTriggeringItemId, OpcUa_Int32 nNoOfLinksToAdd,
                                     const OpcUa_UInt32* pLinksToAdd, OpcUa_Int32 nNoOfLinksToRemove, const OpcUa_UInt32* pLinksToRemove,
                                     OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfAddResults, OpcUa_StatusCode** pAddResults,
                                     OpcUa_Int32* pNoOfAddDiagnosticInfos, OpcUa_DiagnosticInfo** pAddDiagnosticInfos, OpcUa_Int32* pNoOfRemoveResults,
                                     OpcUa_StatusCode** pRemoveResults, OpcUa_Int32* pNoOfRemoveDiagnosticInfos, OpcUa_DiagnosticInfo** pRemoveDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_SetTriggering");
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(nSubscriptionId);
	OpcUa_ReferenceParameter(nTriggeringItemId);
	OpcUa_ReferenceParameter(nNoOfLinksToAdd);
	OpcUa_ReferenceParameter(pLinksToAdd);
	OpcUa_ReferenceParameter(nNoOfLinksToRemove);
	OpcUa_ReferenceParameter(pLinksToRemove);
	OpcUa_ReferenceParameter(pNoOfAddResults);
	OpcUa_ReferenceParameter(pAddResults);
	OpcUa_ReferenceParameter(pNoOfAddDiagnosticInfos);
	OpcUa_ReferenceParameter(pAddDiagnosticInfos);
	OpcUa_ReferenceParameter(pNoOfRemoveResults);
	OpcUa_ReferenceParameter(pRemoveResults);
	OpcUa_ReferenceParameter(pNoOfRemoveDiagnosticInfos);
	OpcUa_ReferenceParameter(pRemoveDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_DeleteMonitoredItems(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                            OpcUa_UInt32 nSubscriptionId, OpcUa_Int32 nNoOfMonitoredItemIds, const OpcUa_UInt32* pMonitoredItemIds,
                                            OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults,
                                            OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_DeleteMonitoredItems");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nSubscriptionId);
	OpcUa_ReferenceParameter(nNoOfMonitoredItemIds);
	OpcUa_ReferenceParameter(pMonitoredItemIds);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_CreateSubscription(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                          OpcUa_Double nRequestedPublishingInterval, OpcUa_UInt32 nRequestedLifetimeCount,
                                          OpcUa_UInt32 nRequestedMaxKeepAliveCount, OpcUa_UInt32 nMaxNotificationsPerPublish,
                                          OpcUa_Boolean bPublishingEnabled, OpcUa_Byte nPriority, OpcUa_ResponseHeader* pResponseHeader,
                                          OpcUa_UInt32* pSubscriptionId, OpcUa_Double* pRevisedPublishingInterval,
                                          OpcUa_UInt32* pRevisedLifetimeCount, OpcUa_UInt32* pRevisedMaxKeepAliveCount) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_CreateSubscription");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nRequestedPublishingInterval);
	OpcUa_ReferenceParameter(nRequestedLifetimeCount);
	OpcUa_ReferenceParameter(nRequestedMaxKeepAliveCount);
	OpcUa_ReferenceParameter(nMaxNotificationsPerPublish);
	OpcUa_ReferenceParameter(bPublishingEnabled);
	OpcUa_ReferenceParameter(nPriority);
	OpcUa_ReferenceParameter(pSubscriptionId);
	OpcUa_ReferenceParameter(pRevisedPublishingInterval);
	OpcUa_ReferenceParameter(pRevisedLifetimeCount);
	OpcUa_ReferenceParameter(pRevisedMaxKeepAliveCount);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_ModifySubscription(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                          OpcUa_UInt32 nSubscriptionId, OpcUa_Double nRequestedPublishingInterval, OpcUa_UInt32 nRequestedLifetimeCount,
                                          OpcUa_UInt32 nRequestedMaxKeepAliveCount, OpcUa_UInt32 nMaxNotificationsPerPublish, OpcUa_Byte nPriority,
                                          OpcUa_ResponseHeader* pResponseHeader, OpcUa_Double* pRevisedPublishingInterval,
                                          OpcUa_UInt32* pRevisedLifetimeCount, OpcUa_UInt32* pRevisedMaxKeepAliveCount) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_ModifySubscription");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nSubscriptionId);
	OpcUa_ReferenceParameter(nRequestedPublishingInterval);
	OpcUa_ReferenceParameter(nRequestedLifetimeCount);
	OpcUa_ReferenceParameter(nRequestedMaxKeepAliveCount);
	OpcUa_ReferenceParameter(nMaxNotificationsPerPublish);
	OpcUa_ReferenceParameter(nPriority);
	OpcUa_ReferenceParameter(pRevisedPublishingInterval);
	OpcUa_ReferenceParameter(pRevisedLifetimeCount);
	OpcUa_ReferenceParameter(pRevisedMaxKeepAliveCount);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_SetPublishingMode(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                         OpcUa_Boolean bPublishingEnabled, OpcUa_Int32 nNoOfSubscriptionIds, const OpcUa_UInt32* pSubscriptionIds,
                                         OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults,
                                         OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_SetPublishingMode");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(bPublishingEnabled);
	OpcUa_ReferenceParameter(nNoOfSubscriptionIds);
	OpcUa_ReferenceParameter(pSubscriptionIds);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_Publish(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                               OpcUa_Int32 nNoOfSubscriptionAcknowledgements, const OpcUa_SubscriptionAcknowledgement* pSubscriptionAcknowledgements,
                               OpcUa_ResponseHeader* pResponseHeader, OpcUa_UInt32* pSubscriptionId, OpcUa_Int32* pNoOfAvailableSequenceNumbers,
                               OpcUa_UInt32** pAvailableSequenceNumbers, OpcUa_Boolean* pMoreNotifications, OpcUa_NotificationMessage* pNotificationMessage,
                               OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_Publish");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nNoOfSubscriptionAcknowledgements);
	OpcUa_ReferenceParameter(pSubscriptionAcknowledgements);
	OpcUa_ReferenceParameter(pSubscriptionId);
	OpcUa_ReferenceParameter(pNoOfAvailableSequenceNumbers);
	OpcUa_ReferenceParameter(pAvailableSequenceNumbers);
	OpcUa_ReferenceParameter(pMoreNotifications);
	OpcUa_ReferenceParameter(pNotificationMessage);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_Republish(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                 OpcUa_UInt32 nSubscriptionId, OpcUa_UInt32 nRetransmitSequenceNumber,
                                 OpcUa_ResponseHeader* pResponseHeader, OpcUa_NotificationMessage* pNotificationMessage) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_Republish");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nSubscriptionId);
	OpcUa_ReferenceParameter(nRetransmitSequenceNumber);
	OpcUa_ReferenceParameter(pNotificationMessage);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_TransferSubscriptions(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                             OpcUa_Int32 nNoOfSubscriptionIds, const OpcUa_UInt32* pSubscriptionIds, OpcUa_Boolean bSendInitialValues,
                                             OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_TransferResult** pResults,
                                             OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_TransferSubscriptions");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nNoOfSubscriptionIds);
	OpcUa_ReferenceParameter(pSubscriptionIds);
	OpcUa_ReferenceParameter(bSendInitialValues);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}

OpcUa_StatusCode ID_UA_DeleteSubscriptions(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                           OpcUa_Int32 nNoOfSubscriptionIds, const OpcUa_UInt32* pSubscriptionIds, OpcUa_ResponseHeader* pResponseHeader,
                                           OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_DeleteSubscriptions");
	OpcUa_ReferenceParameter(hEndpoint);
	OpcUa_ReferenceParameter(hContext);
	OpcUa_ReferenceParameter(nNoOfSubscriptionIds);
	OpcUa_ReferenceParameter(pSubscriptionIds);
	OpcUa_ReferenceParameter(pNoOfResults);
	OpcUa_ReferenceParameter(pResults);
	OpcUa_ReferenceParameter(pNoOfDiagnosticInfos);
	OpcUa_ReferenceParameter(pDiagnosticInfos);

    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, OpcUa_BadNotImplemented);
    Error: ;
    return OpcUa_BadNotImplemented;
}