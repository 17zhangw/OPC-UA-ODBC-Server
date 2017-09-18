/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "AddressSpaceCopier.h"

// variation on the safe string copy macro
#define ID_SafeStringCopy(Arg) \
    if (OpcUa_String_GetRawString(&S->Arg) != NULL) { \
        OpcUa_String_AttachCopy(&T->Arg, OpcUa_String_GetRawString(&S->Arg)); \
    }

OpcUa_Boolean ID_AreTypesEqual(OpcUa_EncodeableType * Type1, OpcUa_EncodeableType * Type2) {
    if (Type1 == NULL || Type2 == NULL) {
        return OpcUa_False;
    }
    
    if (Type1->TypeId != Type2->TypeId) {
        return OpcUa_False;
    }
    
    if (Type1->BinaryEncodingTypeId != Type2->BinaryEncodingTypeId) {
        return OpcUa_False;
    }
    
    // theoretically, if two types are the same => namespaceUri must point to the same pointer
    // verify by doing pointer inequality and check for actual string inequality
    if (Type1->NamespaceUri != Type2->NamespaceUri) {
        if (Type1->NamespaceUri == NULL || Type2->NamespaceUri == NULL) {
            return OpcUa_False;
        }

        if (strcmp(Type1->NamespaceUri, Type2->NamespaceUri) != 0) {
            return OpcUa_False;
        }
    }
    
    if (strcmp(Type1->TypeName, Type2->TypeName) != 0) {
        return OpcUa_False;
    }

    return OpcUa_True;
}

// Copy ApplicationDescription Object
void ID_CopyApplicationDescription(OpcUa_ApplicationDescription * S, OpcUa_ApplicationDescription * T) {
    ID_SafeStringCopy(ApplicationName.Locale);
    ID_SafeStringCopy(ApplicationName.Text);
    T->ApplicationType = S->ApplicationType;
    ID_SafeStringCopy(ApplicationUri);
    ID_SafeStringCopy(DiscoveryProfileUri);
    ID_SafeStringCopy(GatewayServerUri);
    ID_SafeStringCopy(ProductUri);
    T->NoOfDiscoveryUrls = S->NoOfDiscoveryUrls;
    for (int i = 0; i < S->NoOfDiscoveryUrls; i++) {
        ID_SafeStringCopy(DiscoveryUrls[i]);
    }
}

// Copy BuildInfo
void ID_CopyBuildInfo(OpcUa_BuildInfo * S, OpcUa_BuildInfo * T) {
    T->BuildDate.dwHighDateTime = S->BuildDate.dwHighDateTime;
    T->BuildDate.dwLowDateTime = S->BuildDate.dwLowDateTime;
    ID_SafeStringCopy(BuildNumber);
    ID_SafeStringCopy(ManufacturerName);
    ID_SafeStringCopy(ProductName);
    ID_SafeStringCopy(ProductUri);
    ID_SafeStringCopy(SoftwareVersion);
}

// Copy DiagnosticInfo
void ID_CopyDiagnosticInfo(OpcUa_DiagnosticInfo * S, OpcUa_DiagnosticInfo * T) {
    T->Locale = S->Locale;
    T->LocalizedText = S->LocalizedText;
    T->NamespaceUri = S->NamespaceUri;
    T->SymbolicId = S->SymbolicId;
    T->InnerStatusCode = S->InnerStatusCode;
    ID_SafeStringCopy(AdditionalInfo)
    if (S->InnerDiagnosticInfo != NULL) {
        if (T->InnerDiagnosticInfo == NULL) {
            T->InnerDiagnosticInfo = malloc(sizeof(OpcUa_DiagnosticInfo));
            OpcUa_DiagnosticInfo_Initialize(T->InnerDiagnosticInfo);
        }
        
        ID_CopyDiagnosticInfo(S->InnerDiagnosticInfo, T->InnerDiagnosticInfo);
    }
}

OpcUa_StatusCode ID_AddressSpace_CopyExtensionObjectBody(OpcUa_ExtensionObject * Source, OpcUa_ExtensionObject * Target) {
    OpcUa_EncodeableType * SType = Source->Body.EncodeableObject.Type;
    if (ID_AreTypesEqual(SType, &OpcUa_Argument_EncodeableType)) {
        OpcUa_Argument * T = Target->Body.EncodeableObject.Object;
        OpcUa_Argument * S = Source->Body.EncodeableObject.Object;

        T->ValueRank = S->ValueRank;
        T->NoOfArrayDimensions = S->NoOfArrayDimensions;
        T->ArrayDimensions = malloc(sizeof(OpcUa_UInt32) * T->NoOfArrayDimensions);
        memcpy(T->ArrayDimensions, S->ArrayDimensions, sizeof(OpcUa_UInt32) * T->NoOfArrayDimensions);

        ID_OpcUa_NodeId_CopyTo(&S->DataType, &T->DataType);
        
        ID_SafeStringCopy(Name);
        ID_SafeStringCopy(Description.Locale);
        ID_SafeStringCopy(Description.Text);
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_TimeZoneDataType_EncodeableType)) {
        OpcUa_TimeZoneDataType * TArg = Target->Body.EncodeableObject.Object;
        OpcUa_TimeZoneDataType * SArg = Source->Body.EncodeableObject.Object;
        TArg->DaylightSavingInOffset = SArg->DaylightSavingInOffset;
        TArg->Offset = SArg->Offset;
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_BuildInfo_EncodeableType)) {
        OpcUa_BuildInfo * TArg = Target->Body.EncodeableObject.Object;
        OpcUa_BuildInfo * SArg = Source->Body.EncodeableObject.Object;
        ID_CopyBuildInfo(SArg, TArg);
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_EUInformation_EncodeableType)) {
        OpcUa_EUInformation * T = Target->Body.EncodeableObject.Object;
        OpcUa_EUInformation * S = Source->Body.EncodeableObject.Object;
        
        T->UnitId = S->UnitId;
        ID_SafeStringCopy(NamespaceUri);
        ID_SafeStringCopy(Description.Locale);
        ID_SafeStringCopy(Description.Text);
        ID_SafeStringCopy(DisplayName.Locale);
        ID_SafeStringCopy(DisplayName.Text);
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_SamplingIntervalDiagnosticsDataType_EncodeableType)) {
        OpcUa_SamplingIntervalDiagnosticsDataType * T = Target->Body.EncodeableObject.Object;
        OpcUa_SamplingIntervalDiagnosticsDataType * S = Source->Body.EncodeableObject.Object;
        T->DisabledMonitoredItemCount = S->DisabledMonitoredItemCount;
        T->MaxMonitoredItemCount = S->MaxMonitoredItemCount;
        T->MonitoredItemCount = S->MonitoredItemCount;
        T->SamplingInterval = S->SamplingInterval;
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_SubscriptionDiagnosticsDataType_EncodeableType)) {
        OpcUa_SubscriptionDiagnosticsDataType * T = Target->Body.EncodeableObject.Object;
        OpcUa_SubscriptionDiagnosticsDataType * S = Source->Body.EncodeableObject.Object;
        memcpy(T, S, sizeof(OpcUa_SubscriptionDiagnosticsDataType));
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_ApplicationDescription_EncodeableType)) {
        OpcUa_ApplicationDescription * T = Target->Body.EncodeableObject.Object;
        OpcUa_ApplicationDescription * S = Source->Body.EncodeableObject.Object;
        ID_CopyApplicationDescription(S, T);
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_ServiceCounterDataType_EncodeableType)) {
        OpcUa_ServiceCounterDataType * T = Target->Body.EncodeableObject.Object;
        OpcUa_ServiceCounterDataType * S = Source->Body.EncodeableObject.Object;
        T->ErrorCount = S->ErrorCount;
        T->TotalCount = S->TotalCount;
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_SessionSecurityDiagnosticsDataType_EncodeableType)) {
        OpcUa_SessionSecurityDiagnosticsDataType * T = Target->Body.EncodeableObject.Object;
        OpcUa_SessionSecurityDiagnosticsDataType * S = Source->Body.EncodeableObject.Object;
        T->SecurityMode = S->SecurityMode;
        T->NoOfClientUserIdHistory = S->NoOfClientUserIdHistory;
        
        ID_SafeStringCopy(Encoding);
        ID_SafeStringCopy(TransportProtocol);
        ID_SafeStringCopy(SecurityPolicyUri);
        ID_SafeStringCopy(ClientUserIdOfSession);
        ID_SafeStringCopy(AuthenticationMechanism);
        for (int i = 0; i < S->NoOfClientUserIdHistory; i++) {
            ID_SafeStringCopy(ClientUserIdHistory[i]);
        }
        
        ID_OpcUa_NodeId_CopyTo(&S->SessionId, &T->SessionId);
        ID_OpcUa_ByteString_CopyTo(&S->ClientCertificate, &T->ClientCertificate);;
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_SessionDiagnosticsDataType_EncodeableType)) {
        OpcUa_SessionDiagnosticsDataType * T = Target->Body.EncodeableObject.Object;
        OpcUa_SessionDiagnosticsDataType * S = Source->Body.EncodeableObject.Object;

        memcpy(&T->AddNodesCount, &S->AddNodesCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->AddReferencesCount, &S->AddReferencesCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->BrowseCount, &S->BrowseCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->BrowseNextCount, &S->BrowseNextCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->CallCount, &S->CallCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->ClientConnectionTime, &S->ClientConnectionTime, sizeof(OpcUa_DateTime));
        memcpy(&T->ClientLastContactTime, &S->ClientLastContactTime, sizeof(OpcUa_DateTime));
        memcpy(&T->CreateMonitoredItemsCount, &S->CreateMonitoredItemsCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->CreateSubscriptionCount, &S->CreateSubscriptionCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->DeleteMonitoredItemsCount, &S->DeleteMonitoredItemsCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->DeleteNodesCount, &S->DeleteNodesCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->DeleteReferencesCount, &S->DeleteReferencesCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->DeleteSubscriptionsCount, &S->DeleteSubscriptionsCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->HistoryReadCount, &S->HistoryReadCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->HistoryUpdateCount, &S->HistoryUpdateCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->ModifyMonitoredItemsCount, &S->ModifyMonitoredItemsCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->ModifySubscriptionCount, &S->ModifySubscriptionCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->PublishCount, &S->PublishCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->QueryFirstCount, &S->QueryFirstCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->QueryNextCount, &S->QueryNextCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->ReadCount, &S->ReadCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->RegisterNodesCount, &S->RegisterNodesCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->RepublishCount, &S->RepublishCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->SetMonitoringModeCount, &S->SetMonitoringModeCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->SetPublishingModeCount, &S->SetPublishingModeCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->SetTriggeringCount, &S->SetTriggeringCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->TotalRequestCount, &S->TotalRequestCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->TransferSubscriptionsCount, &S->TransferSubscriptionsCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->TranslateBrowsePathsToNodeIdsCount, &S->TranslateBrowsePathsToNodeIdsCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->UnregisterNodesCount, &S->UnregisterNodesCount, sizeof(OpcUa_ServiceCounterDataType));
        memcpy(&T->WriteCount, &S->WriteCount, sizeof(OpcUa_ServiceCounterDataType));

        T->CurrentMonitoredItemsCount = S->CurrentMonitoredItemsCount;
        T->CurrentPublishRequestsInQueue = S->CurrentPublishRequestsInQueue;
        T->CurrentSubscriptionsCount = S->CurrentSubscriptionsCount;
        T->MaxResponseMessageSize = S->MaxResponseMessageSize;
        T->UnauthorizedRequestCount = S->UnauthorizedRequestCount;
        T->ActualSessionTimeout = S->ActualSessionTimeout;

        ID_SafeStringCopy(ServerUri);
        ID_SafeStringCopy(SessionName);
        ID_SafeStringCopy(EndpointUrl);
        T->NoOfLocaleIds = S->NoOfLocaleIds;
        for (int i = 0; i < S->NoOfLocaleIds; i++) {
            ID_SafeStringCopy(LocaleIds[i]);
        }
        
        ID_OpcUa_NodeId_CopyTo(&S->SessionId, &T->SessionId);
        ID_CopyApplicationDescription(&S->ClientDescription, &T->ClientDescription);
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_ServerDiagnosticsSummaryDataType_EncodeableType)) {
        OpcUa_ServerDiagnosticsSummaryDataType * T = Target->Body.EncodeableObject.Object;
        OpcUa_ServerDiagnosticsSummaryDataType * S = Source->Body.EncodeableObject.Object;
        memcpy(T, S, sizeof(OpcUa_ServerDiagnosticsSummaryDataType));
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_ServerStatusDataType_EncodeableType)) {
        OpcUa_ServerStatusDataType * T = Target->Body.EncodeableObject.Object;
        OpcUa_ServerStatusDataType * S = Source->Body.EncodeableObject.Object;
        memcpy(&T->CurrentTime, &S->CurrentTime, sizeof(OpcUa_DateTime));
        memcpy(&T->StartTime, &S->StartTime, sizeof(OpcUa_DateTime));
        T->SecondsTillShutdown = S->SecondsTillShutdown;
        T->State = S->State;
        ID_SafeStringCopy(ShutdownReason.Locale);
        ID_SafeStringCopy(ShutdownReason.Text);
        ID_CopyBuildInfo(&S->BuildInfo, &T->BuildInfo);
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_StatusResult_EncodeableType)) {
        OpcUa_StatusResult * T = Target->Body.EncodeableObject.Object;
        OpcUa_StatusResult * S = Source->Body.EncodeableObject.Object;
        T->StatusCode = S->StatusCode;
        ID_CopyDiagnosticInfo(&S->DiagnosticInfo, &T->DiagnosticInfo);
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_UserNameIdentityToken_EncodeableType)) {
        OpcUa_UserNameIdentityToken * T = Target->Body.EncodeableObject.Object;
        OpcUa_UserNameIdentityToken * S = Source->Body.EncodeableObject.Object;
        ID_SafeStringCopy(EncryptionAlgorithm);
        ID_SafeStringCopy(PolicyId);
        ID_SafeStringCopy(UserName);
        
        T->Password.Length = S->Password.Length;
        T->Password.Data = malloc(sizeof(OpcUa_Byte) * T->Password.Length);
        memcpy(T->Password.Data, S->Password.Data, sizeof(OpcUa_Byte) * T->Password.Length);
        return OpcUa_Good;
    } else if (ID_AreTypesEqual(SType, &OpcUa_AnonymousIdentityToken_EncodeableType)) {
        OpcUa_AnonymousIdentityToken * T = Target->Body.EncodeableObject.Object;
        OpcUa_AnonymousIdentityToken * S = Source->Body.EncodeableObject.Object;
        ID_SafeStringCopy(PolicyId);
        return OpcUa_Good;
    }
    
    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Error: copy function not implemented for type '%s'\n", SType->TypeName);
    return OpcUa_BadNotImplemented;
}