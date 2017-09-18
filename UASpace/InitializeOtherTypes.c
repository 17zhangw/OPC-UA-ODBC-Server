/*
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "Initialize.h"
#include "../Indexer/Indexer.h"
    
static OpcUa_NodeId ID_StringNode = ID_OPCNodeId(OpcUaId_String);
static OpcUa_NodeId ID_ByteStringNode = ID_OPCNodeId(OpcUaId_ByteString);

#pragma warning(push)
#pragma warning(disable:4204 4221)
void ID_Initialize_AllStandardVariableTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_NodeId ID_PropertyTypeNode = ID_PropertyType;
    OpcUa_NodeId ID_BaseDataTypeNode = ID_BaseDataType;
    OpcUa_NodeId ID_HasSubtypeNode = ID_HasSubtype;
    OpcUa_NodeId ID_ModellingRuleOptionalNode = ID_ModellingRuleOptional;
    OpcUa_NodeId ID_ModellingRuleExposesItsArrayNode = ID_ModellingRuleExposesItsArray;

    OpcUa_NodeId BaseVariableType = ID_OPCNodeId(OpcUaId_BaseVariableType);
    ID_VariantStructure PropertyTypeStructure[1] = {{OpcUaType_Null, 0, NULL, NULL}};
    ID_VariantStructure BaseVariableTypeStructure[1] = {{OpcUaType_Null, 0, NULL, NULL}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, NULL, ID_PropertyTypeNode, 0, "PropertyType", OpcUa_False, ID_VALUERANK_ANY_INDICATOR, ID_BaseDataTypeNode, NULL, PropertyTypeStructure, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, NULL, BaseVariableType, 0, "BaseVariableType", OpcUa_True, ID_VALUERANK_ANY_INDICATOR, ID_BaseDataTypeNode, NULL, BaseVariableTypeStructure, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    ID_AddressSpace_AddBiReference(Handle, &BaseVariableType, ID_NodeType_VariableType, &ID_PropertyTypeNode, ID_NodeType_VariableType, &ID_HasSubtypeNode);
    
    OpcUa_NodeId BaseDataVariableType = ID_OPCNodeId(OpcUaId_BaseDataVariableType);
    ID_VariantStructure BaseDataVariableTypeStructure[1] = { { OpcUaType_Null, 0, NULL, NULL } };
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, NULL, BaseDataVariableType, 0, "BaseDataVariableType", OpcUa_False, ID_VALUERANK_ANY_INDICATOR, ID_BaseDataTypeNode, &BaseVariableType, BaseDataVariableTypeStructure, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    OpcUa_NodeId ServerVendorCapabilityType = ID_OPCNodeId(OpcUaId_ServerVendorCapabilityType);
    ID_VariantStructure ServerVendorCapabilityTypeStructure[1] = {{OpcUaType_Null, 0, NULL, NULL}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, NULL, ServerVendorCapabilityType, 0, "ServerVendorCapabilityType", OpcUa_True, 0, ID_BaseDataTypeNode, &BaseDataVariableType, ServerVendorCapabilityTypeStructure, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    
    // DataTypeDictionaryType
    OpcUa_NodeId DataTypeDictionaryType = ID_OPCNodeId(OpcUaId_DataTypeDictionaryType);
    int DTDTChildNameNamespaces[2] = {0,0};
    char * DTDTChildNames[2] = { "DataTypeVersion", "NamespaceUri" };
    int DTDTChildNodeIds[2] = { OpcUaId_DataTypeDictionaryType_DataTypeVersion, OpcUaId_DataTypeDictionaryType_NamespaceUri };
    int DTDTChildDataTypeIds[2] = { OpcUaId_String, OpcUaId_String };
    int DTDTChildInheritType[2] = { OpcUaId_PropertyType, OpcUaId_PropertyType };
    int DTDTChildLengths[2] = { 0, 0 };
    ID_VariantStructure DataTypeDictionaryTypeStructure[1] = {{OpcUaType_ByteString, 0, NULL, NULL}};
    ID_VariantStructure DataTypeDictionaryTypeChildrenStructure[2] = {{OpcUaType_String, 0, NULL, NULL, &ID_ModellingRuleOptionalNode},{OpcUaType_String,0,NULL,NULL,&ID_ModellingRuleOptionalNode}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, NULL, DataTypeDictionaryType, 0, "DataTypeDictionaryType", OpcUa_False, 0, ID_ByteStringNode, &BaseDataVariableType, DataTypeDictionaryTypeStructure, 2, DTDTChildNameNamespaces, DTDTChildNames, DTDTChildNodeIds, DTDTChildDataTypeIds, DTDTChildInheritType, DTDTChildLengths, DataTypeDictionaryTypeChildrenStructure);

    // DataTypeDescriptionType
    OpcUa_NodeId DataTypeDescriptionType = ID_OPCNodeId(OpcUaId_DataTypeDescriptionType);
    int DescChildNameNamespaces[2] = { 0, 0 };
    char * DescChildNames[2] = { "DataTypeVersion", "DictionaryFragment" };
    int DescChildNodeIds[2] = { OpcUaId_DataTypeDescriptionType_DataTypeVersion, OpcUaId_DataTypeDescriptionType_DictionaryFragment };
    int DescChildDataTypeIds[2] = { OpcUaId_String, OpcUaId_ByteString };
    int DescChildInheritType[2] = { OpcUaId_PropertyType, OpcUaId_PropertyType };
    int DescChildLengths[2] = { 0, 0 };
    ID_VariantStructure DataTypeDescriptionTypeStructure[1] = {{OpcUaType_ByteString, 0, NULL, NULL}};
    ID_VariantStructure DataTypeDescriptionTypeChildrenStructure[2] = {{OpcUaType_String, 0, NULL, NULL,&ID_ModellingRuleOptionalNode},{OpcUaType_String,0, NULL, NULL,&ID_ModellingRuleOptionalNode}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, NULL, DataTypeDescriptionType, 0, "DataTypeDescriptionType", OpcUa_False, 0, ID_ByteStringNode, &BaseDataVariableType, DataTypeDescriptionTypeStructure, 2, DescChildNameNamespaces, DescChildNames, DescChildNodeIds, DescChildDataTypeIds, DescChildInheritType, DescChildLengths, DataTypeDescriptionTypeChildrenStructure);
    
    // BuildInfoType
    ID_VariableTypeNode * BuildInfoNode;
    OpcUa_NodeId BuildInfoType = ID_OPCNodeId(OpcUaId_BuildInfoType);
    int BuildInfoNameNamespaces[6] = {0,0,0,0,0,0};
    char * BuildInfoTypeNames[6] = {"ProductUri","ManufacturerName","ProductName","SoftwareVersion","BuildNumber","BuildDate"};
    int BuildInfoNodeIds[6] = {OpcUaId_BuildInfoType_ProductUri, OpcUaId_BuildInfoType_ManufacturerName, OpcUaId_BuildInfoType_ProductName, OpcUaId_BuildInfoType_SoftwareVersion, OpcUaId_BuildInfoType_BuildNumber, OpcUaId_BuildInfoType_BuildDate};
    int BuildInfoDataTypeIds[6] = {OpcUaId_String,OpcUaId_String,OpcUaId_String,OpcUaId_String,OpcUaId_String,OpcUaId_UtcTime};
    int BuildInfoInheritType[6] = {OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType};
    int BuildInfoChildLengths[6] = {0,0,0,0,0,0};
    OpcUa_NodeId BuildInfoTypeTypeId = ID_OPCNodeId(OpcUaId_BuildInfo);
    OpcUa_ExpandedNodeId BuildInfoTypeExpandedNodeId;
    OpcUa_ExpandedNodeId_Initialize(&BuildInfoTypeExpandedNodeId);
    BuildInfoTypeExpandedNodeId.ServerIndex = 0; ID_OpcUa_NodeId_CopyTo(&BuildInfoTypeTypeId, &BuildInfoTypeExpandedNodeId.NodeId);
    ID_VariantStructure BuildInfoTypeStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &BuildInfoTypeExpandedNodeId, &OpcUa_BuildInfo_EncodeableType}};
    ID_VariantStructure BuildInfoTypeChildrenStructure[6] = {{OpcUaType_String,0,NULL,NULL,NULL},{OpcUaType_String,0,NULL,NULL,NULL},{OpcUaType_String,0,NULL,NULL,NULL},
                                                          {OpcUaType_String,0,NULL,NULL,NULL},{OpcUaType_String,0,NULL,NULL,NULL},{OpcUaType_DateTime,0,NULL,NULL,NULL}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &BuildInfoNode, BuildInfoType, 0, "BuildInfoType", OpcUa_False, 0, BuildInfoTypeTypeId, &BaseDataVariableType, BuildInfoTypeStructure, 6, BuildInfoNameNamespaces, BuildInfoTypeNames, BuildInfoNodeIds, BuildInfoDataTypeIds, BuildInfoInheritType, BuildInfoChildLengths, BuildInfoTypeChildrenStructure);
    
    // ServerStatusType
    ID_VariableTypeNode * ServerStatusTypeNode;
    OpcUa_NodeId ServerStatusType = ID_OPCNodeId(OpcUaId_ServerStatusType);
    int ServerStatusTypeNameNamespaces[6] = {0,0,0,0,0};
    char * ServerStatusTypeNames[6] = {"StartTime","CurrentTime","State","BuildInfo","SecondsTillShutdown","ShutdownReason"};
    int ServerStatusTypeIds[6] = {OpcUaId_ServerStatusType_StartTime,OpcUaId_ServerStatusType_CurrentTime,OpcUaId_ServerStatusType_State, OpcUaId_ServerStatusType_BuildInfo, OpcUaId_ServerStatusType_SecondsTillShutdown,OpcUaId_ServerStatusType_ShutdownReason};
    int ServerStatusDataTypeIds[6] = {OpcUaId_UtcTime,OpcUaId_UtcTime,OpcUaId_ServerState,OpcUaId_BuildInfo,OpcUaId_UInt32,OpcUaId_LocalizedText};
    int ServerStatusInheritType[6] = {OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BuildInfoType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType};
    int ServerStatusLengths[6] = {0,0,0,0,0,0};
    OpcUa_NodeId ServerStatusTypeDataTypeId = ID_OPCNodeId(OpcUaId_ServerStatusDataType);
    OpcUa_ExpandedNodeId ServerStatusTypeExpandedNodeId = ID_OPCExpandedNodeId(OpcUaId_ServerStatusDataType_Encoding_DefaultBinary);

    ID_VariantStructure ServerStatusTypeStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &ServerStatusTypeExpandedNodeId, &OpcUa_ServerStatusDataType_EncodeableType}};
    ID_VariantStructure ServerStatusTypeChildrenStructure[6] = {{OpcUaType_DateTime,0,NULL,NULL,NULL},{OpcUaType_DateTime,0,NULL,NULL,NULL},
                                                            {OpcUaType_UInt32,0,NULL,NULL,NULL},
                                                            {OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&BuildInfoTypeExpandedNodeId,&OpcUa_BuildInfo_EncodeableType,NULL},
                                                            {OpcUaType_UInt32,0,NULL,NULL,NULL},{OpcUaType_LocalizedText,0,NULL,NULL,NULL}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &ServerStatusTypeNode, ServerStatusType, 0, "ServerStatusType", OpcUa_False, 0, ServerStatusTypeDataTypeId, &BaseDataVariableType, ServerStatusTypeStructure, 6, ServerStatusTypeNameNamespaces, ServerStatusTypeNames, ServerStatusTypeIds, ServerStatusDataTypeIds, ServerStatusInheritType, ServerStatusLengths, ServerStatusTypeChildrenStructure);

    // ServerDiagnosticsSummaryType
    OpcUa_NodeId ServerDiagnosticsSummaryType = ID_OPCNodeId(OpcUaId_ServerDiagnosticsSummaryType);
    int ServerDiagnosticsNameNamespaces[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    char * ServerDiagnosticsNames[12] = {"ServerViewCount","CurrentSessionCount","CumulatedSessionCount","SecurityRejectedSessionCount","RejectedSessionCount","SessionTimeoutCount","SessionAbortCount","PublishingIntervalCount","CurrentSubscriptionCount","CumulatedSubscriptionCount","SecurityRejectedRequestsCount","RejectedRequestsCount"};
    int ServerDiagnosticsIds[12] = {OpcUaId_ServerDiagnosticsSummaryType_ServerViewCount,OpcUaId_ServerDiagnosticsSummaryType_CurrentSessionCount,OpcUaId_ServerDiagnosticsSummaryType_CumulatedSessionCount,
                                    OpcUaId_ServerDiagnosticsSummaryType_SecurityRejectedSessionCount,OpcUaId_ServerDiagnosticsSummaryType_RejectedSessionCount,OpcUaId_ServerDiagnosticsSummaryType_SessionTimeoutCount,
                                    OpcUaId_ServerDiagnosticsSummaryType_SessionAbortCount,OpcUaId_ServerDiagnosticsSummaryType_PublishingIntervalCount,OpcUaId_ServerDiagnosticsSummaryType_CurrentSubscriptionCount,
                                    OpcUaId_ServerDiagnosticsSummaryType_CumulatedSubscriptionCount,OpcUaId_ServerDiagnosticsSummaryType_SecurityRejectedRequestsCount,OpcUaId_ServerDiagnosticsSummaryType_RejectedRequestsCount};
    int ServerDiagnosticsDataTypeIds[12] = {OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32};
    int ServerDiagnosticsInheritType[12] = {OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BuildInfoType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BuildInfoType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType};
    int ServerDiagnosticsLengths[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    OpcUa_NodeId ServerDiagnosticsTypeTypeId = ID_OPCNodeId(OpcUaId_ServerDiagnosticsSummaryDataType);
    OpcUa_ExpandedNodeId ServerDiagnosticsTypeExpandedNodeId = ID_OPCExpandedNodeId(OpcUaId_ServerDiagnosticsSummaryDataType_Encoding_DefaultBinary);
    ID_VariantStructure ServerDiagnosticsSummaryTypeStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &ServerDiagnosticsTypeExpandedNodeId, &OpcUa_ServerDiagnosticsSummaryDataType_EncodeableType}};
    ID_VariantStructure ServerDiagnosticsSummaryTypeChildrenStructure[12] = {{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},
                                                                          {OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, NULL, ServerDiagnosticsSummaryType, 0, "ServerDiagnosticsSummaryType", OpcUa_False, 0, ServerDiagnosticsTypeTypeId, &BaseDataVariableType, ServerDiagnosticsSummaryTypeStructure, 12, ServerDiagnosticsNameNamespaces, ServerDiagnosticsNames, ServerDiagnosticsIds, ServerDiagnosticsDataTypeIds, ServerDiagnosticsInheritType, ServerDiagnosticsLengths, ServerDiagnosticsSummaryTypeChildrenStructure);
   
    // SamplingIntervalDiagnosticsType
    ID_VariableTypeNode * SamplingIntervalDiagnosticsTypeNode;
    OpcUa_NodeId SamplingIntervalDiagnosticsType = ID_OPCNodeId(OpcUaId_SamplingIntervalDiagnosticsType);
    int SIDTNameNamespaces[4] = {0,0,0,0};
    char * SIDTNames[4] = {"SamplingInterval","SampledMonitoredItemCount","MaxSampledMonitoredItemCount","DisabledMonitoredItemsSamplingCount"};
    int SIDTIds[4] = {OpcUaId_SamplingIntervalDiagnosticsType_SamplingInterval,OpcUaId_SamplingIntervalDiagnosticsType_SampledMonitoredItemsCount,OpcUaId_SamplingIntervalDiagnosticsType_MaxSampledMonitoredItemsCount,OpcUaId_SamplingIntervalDiagnosticsType_DisabledMonitoredItemsSamplingCount};
    int SIDTDataTypeIds[4] = {OpcUaId_Duration,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32};
    int SIDTInheritType[4] = {OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType};
    int SIDTLengths[4] = {0,0,0,0};
    OpcUa_NodeId SamplingIntervalDiagnosticsTypeDataType = ID_OPCNodeId(OpcUaId_SamplingIntervalDiagnosticsDataType);
    OpcUa_ExpandedNodeId SamplingIntervalDiagnosticsTypeExpandedId = ID_OPCExpandedNodeId(OpcUaId_SamplingIntervalDiagnosticsDataType_Encoding_DefaultBinary);
    ID_VariantStructure SIDTStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SamplingIntervalDiagnosticsTypeExpandedId, &OpcUa_SamplingIntervalDiagnosticsDataType_EncodeableType}};
    ID_VariantStructure SIDTChildrenStructure[4] = {{OpcUaType_DateTime, 0, NULL, NULL, NULL},{OpcUaType_UInt32, 0, NULL, NULL, NULL},{OpcUaType_UInt32, 0, NULL, NULL, NULL},{OpcUaType_UInt32, 0, NULL, NULL, NULL}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &SamplingIntervalDiagnosticsTypeNode, SamplingIntervalDiagnosticsType, 0, "SamplingIntervalDiagnosticsType", OpcUa_False, 0, SamplingIntervalDiagnosticsTypeDataType, &BaseDataVariableType, SIDTStructure, 4, SIDTNameNamespaces, SIDTNames, SIDTIds, SIDTDataTypeIds, SIDTInheritType, SIDTLengths, SIDTChildrenStructure);
    
    // SamplingIntervalDiagnosticsArrayType
    ID_VariableTypeNode * SamplingIntervalDiagnosticsArrayType;
    OpcUa_NodeId SamplingIntervalDiagnosticsArrayTypeNodeId = ID_OPCNodeId(OpcUaId_SamplingIntervalDiagnosticsArrayType);
    int SIDATNameNamespaces[1] = {0};
    char * SIDATNames[1] = {"SamplingIntervalDiagnostics"};
    int SIDATIds[1] = {OpcUaId_SamplingIntervalDiagnosticsArrayType_SamplingIntervalDiagnostics};
    int SIDATDataTypeIds[1] = {OpcUaId_SamplingIntervalDiagnosticsDataType};
    int SIDATInheritType[1] = {OpcUaId_SamplingIntervalDiagnosticsType};
    int SIDATLengths[1] = {0};
    ID_VariantStructure SIDTATStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SamplingIntervalDiagnosticsTypeExpandedId, &OpcUa_SamplingIntervalDiagnosticsDataType_EncodeableType}};
    ID_VariantStructure SIDTATChildrenStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SamplingIntervalDiagnosticsTypeExpandedId, &OpcUa_SamplingIntervalDiagnosticsDataType_EncodeableType, &ID_ModellingRuleExposesItsArrayNode}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &SamplingIntervalDiagnosticsArrayType, SamplingIntervalDiagnosticsArrayTypeNodeId, 0, "SamplingIntervalDiagnosticsArrayType", OpcUa_False, ID_LENGTH_UNKNOWN_INDICATOR, SamplingIntervalDiagnosticsTypeDataType, &BaseDataVariableType, SIDTATStructure, 1, SIDATNameNamespaces, SIDATNames, SIDATIds, SIDATDataTypeIds, SIDATInheritType, SIDATLengths, SIDTATChildrenStructure);

    // SubscriptionDiagnosticsType
    ID_VariableTypeNode * SubscriptionDiagnosticsTypeNode;
    OpcUa_NodeId SubscriptionDiagnosticsType = ID_OPCNodeId(OpcUaId_SubscriptionDiagnosticsType);
    int SDTNameNamespaces[31] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char * SDTNames[31] = {"SessionId","SubscriptionId","Priority","PublishingInterval","MaxKeepAliveCount","MaxLifetimeCount",
                           "MaxNotificationsPerPublish","PublishingEnabled","ModifyCount","EnableCount","DisableCount",
                           "RepublishRequestCount","RepublishMessageRequestCount","RepublishMessageCount","TransferRequestCount",
                           "TransferredToAltClientCount","TransferredToSameClientCount","PublishRequestCount","DataChangeNotificationsCount",
                           "EventNotificationsCount","NotificationsCount","LatePublishRequestCount","CurrentKeepAliveCount",
                           "CurrentLifetimeCount","UnacknowledgedMessageCount","DiscardedMessageCount","MonitoredItemCount",
                           "DisabledMonitoredItemCount","MonitoringQueueOverflowCount","NextSequenceNumber","EventQueueOverflowCount"};
    int SDTIds[31] = {2173,2174,2175,2176,2177,8888,2179,2180,2181,2182,2183,2184,2185,2186,2187,2188,2189,2190,2191,2998,2193,
                      8889,8890,8891,8892,8893,8894,8895,8896,8897,8902};
    int SDTDataTypeIds[31] = {OpcUaId_NodeId,OpcUaId_UInt32,OpcUaId_Byte,OpcUaId_Duration,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,
                              OpcUaId_Boolean,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,
                              OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,
                              OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,OpcUaId_UInt32,
                              OpcUaId_UInt32,OpcUaId_UInt32};
    int SDTInheritTypes[31] = {OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,
                              OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,
                              OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,
                              OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,
                              OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,
                              OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,
                              OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,
                              OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType,OpcUaId_BaseDataVariableType};
    int SDTLengths[31] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    OpcUa_NodeId SubscriptionDiagnosticsTypeTypeId = ID_OPCNodeId(OpcUaId_SubscriptionDiagnosticsDataType);
    OpcUa_ExpandedNodeId SubscriptionDiagnosticsTypeExpandedNodeId = ID_OPCExpandedNodeId(OpcUaId_SubscriptionDiagnosticsDataType_Encoding_DefaultBinary);
    ID_VariantStructure SubscriptionDiagnosticsTypeStructure[1] = {{OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&SubscriptionDiagnosticsTypeExpandedNodeId,&OpcUa_SubscriptionDiagnosticsDataType_EncodeableType}};
    ID_VariantStructure SubscriptionDiagnosticsTypeChildStructure[31] = {{OpcUaType_NodeId},{OpcUaType_UInt32},{OpcUaType_Byte},{OpcUaType_Double},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_Boolean},
                                                                     {OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},
                                                                     {OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},
                                                                     {OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &SubscriptionDiagnosticsTypeNode, SubscriptionDiagnosticsType, 0, "SubscriptionDiagnosticsType", OpcUa_False, 0, SubscriptionDiagnosticsTypeTypeId, &BaseDataVariableType, SubscriptionDiagnosticsTypeStructure, 31, SDTNameNamespaces, SDTNames, SDTIds, SDTDataTypeIds, SDTInheritTypes, SDTLengths, SubscriptionDiagnosticsTypeChildStructure);
    
    // SubscriptionDiagnosticsArrayType
    ID_VariableTypeNode * SubscriptionDiagnosticsArrayType;
    OpcUa_NodeId SubscriptionDiagnosticsArrayTypeNodeId = ID_OPCNodeId(OpcUaId_SubscriptionDiagnosticsArrayType);
    int SDATNameNamespaces[1] = {0};
    char * SDATNames[1] = {"SubscriptionDiagnostics"};
    int SDATIds[1] = {OpcUaId_SubscriptionDiagnosticsArrayType_SubscriptionDiagnostics};
    int SDATDataTypeIds[1] = {OpcUaId_SubscriptionDiagnosticsDataType};
    int SDATInheritType[1] = {OpcUaId_SubscriptionDiagnosticsType};
    int SDATLengths[1] = {0};
    ID_VariantStructure SubscriptionDiagnosticsArrayTypeStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SubscriptionDiagnosticsTypeExpandedNodeId, &OpcUa_SubscriptionDiagnosticsDataType_EncodeableType}};
    ID_VariantStructure SubscriptionDiagnosticsArrayTypeChildrenStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SubscriptionDiagnosticsTypeExpandedNodeId, &OpcUa_SubscriptionDiagnosticsDataType_EncodeableType, &ID_ModellingRuleExposesItsArrayNode}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &SubscriptionDiagnosticsArrayType, SubscriptionDiagnosticsArrayTypeNodeId, 0, "SubscriptionDiagnosticsArrayType", OpcUa_False, ID_LENGTH_UNKNOWN_INDICATOR, SubscriptionDiagnosticsTypeTypeId, &BaseDataVariableType, SubscriptionDiagnosticsArrayTypeStructure, 1, SDATNameNamespaces, SDATNames, SDATIds, SDATDataTypeIds, SDATInheritType, SDATLengths, SubscriptionDiagnosticsArrayTypeChildrenStructure);

    // SessionDiagnosticsVariableType
    ID_VariableTypeNode * SessionDiagnosticsVariableTypeNode;
    OpcUa_NodeId SessionDiagnosticsVariableType = ID_OPCNodeId(OpcUaId_SessionDiagnosticsVariableType);
    int SDNNameNamespaces[43] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char * SessionDiagnosticsNames[43] = {"SessionId","SessionName","ClientDescription","ServerUri","EndpointUrl","LocaleIds",
                                          "MaxResponseMessageSize","ActualSessionTimeout","ClientConnectionTime","ClientLastContactTime",
                                          "CurrentSubscriptionsCount","CurrentMonitoredItemsCount","CurrentPublishRequestsInQueue",
                                          "TotalRequestCount","UnauthorizedRequestCount","ReadCount","HistoryReadCount","WriteCount",
                                          "HistoryUpdateCount","CallCount","CreateMonitoredItemsCount","ModifyMonitoredItemsCount",
                                          "SetMonitoringModeCount","SetTriggeringCount","DeleteMonitoredItemsCount","CreateSubscriptionCount",
                                          "ModifySubscriptionCount","SetPublishingModeCount","PublishCount","RepublishCount",
                                          "TransferSubscriptionsCount","DeleteSubscriptionsCount","AddNodesCount","AddReferencesCount",
                                          "DeleteNodesCount","DeleteReferencesCount","BrowseCount","BrowseNextCount",
                                          "TranslateBrowsePathsToNodeIdsCount","QueryFirstCount","QueryNextCount",
                                          "RegisterNodesCount","UnregisterNodesCount"};
    int SDNIds[43] = {2198,2199,2200,2201,2202,2203,3050,2204,2205,2206,2207,2208,2209,8900,11892,2217,2218,2219,2220,2221,
                      2222,2223,2224,2225,2226,2227,2228,2229,2230,2231,2232,2233,2234,2235,2236,2237,2238,2239,2240,2241,2242,2730,2731};
    int SDNDataTypeIds[43] = {OpcUaId_NodeId,OpcUaId_String,OpcUaId_ApplicationDescription,OpcUaId_String,OpcUaId_String,
                              OpcUaId_LocaleId,OpcUaId_UInt32,OpcUaId_Duration,OpcUaId_UtcTime,OpcUaId_UtcTime,OpcUaId_UInt32,OpcUaId_UInt32,
                              OpcUaId_UInt32,OpcUaId_ServiceCounterDataType,OpcUaId_UInt32,OpcUaId_ServiceCounterDataType,871,871,871,871,871,
                              871,871,871,871,871,871,871,871,871,871,871,871,871,871,871,871,871,871,871,871,871,871};
    int SDNInheritTypes[43] = {63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
                               63,63,63,63,63,63,63,63,63,63,63,63,63};
    int SDNLengths[43] = {0,0,0,0,0,ID_LENGTH_UNKNOWN_INDICATOR,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    OpcUa_NodeId SessionDiagnosticsDataType = ID_OPCNodeId(OpcUaId_SessionDiagnosticsDataType);
    OpcUa_ExpandedNodeId SessionDiagnosticsDataTypeExpandedNodeId = ID_OPCExpandedNodeId(OpcUaId_SessionDiagnosticsDataType_Encoding_DefaultBinary);
    ID_VariantStructure SessionDiagnosticsVariableTypeStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SessionDiagnosticsDataTypeExpandedNodeId, &OpcUa_SessionDiagnosticsDataType_EncodeableType}};
    OpcUa_ExpandedNodeId ApplicationDescriptionExpandedNodeId = ID_OPCExpandedNodeId(OpcUaId_ApplicationDescription_Encoding_DefaultBinary);

    OpcUa_Int32 TEO = OpcUaType_ExtensionObject;
    OpcUa_UInt32 EOEEO = OpcUa_ExtensionObjectEncoding_EncodeableObject; 
    OpcUa_EncodeableType * SCDT = &OpcUa_ServiceCounterDataType_EncodeableType;
    OpcUa_ExpandedNodeId ServiceCounterId = ID_OPCExpandedNodeId(OpcUaId_ServiceCounterDataType_Encoding_DefaultBinary);
    ID_VariantStructure SessionDiagnosticsVariableTypeChildrenStructure[43] = {{OpcUaType_NodeId},{OpcUaType_String},{OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&ApplicationDescriptionExpandedNodeId,&OpcUa_ApplicationDescription_EncodeableType},
                                                                            {OpcUaType_String},{OpcUaType_String},{OpcUaType_String},{OpcUaType_UInt32},{OpcUaType_Double},{OpcUaType_DateTime},{OpcUaType_DateTime},{OpcUaType_UInt32},{OpcUaType_UInt32},{OpcUaType_UInt32},
                                                                            {TEO,EOEEO,&ServiceCounterId,SCDT},{OpcUaType_UInt32},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},
                                                                            {TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},
                                                                            {TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},
                                                                            {TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},
                                                                            {TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT},{TEO,EOEEO,&ServiceCounterId,SCDT}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &SessionDiagnosticsVariableTypeNode, SessionDiagnosticsVariableType, 0, "SessionDiagnosticsVariableType",OpcUa_False, 0, SessionDiagnosticsDataType, &BaseDataVariableType, SessionDiagnosticsVariableTypeStructure, 43, SDNNameNamespaces, SessionDiagnosticsNames, SDNIds, SDNDataTypeIds, SDNInheritTypes, SDNLengths, SessionDiagnosticsVariableTypeChildrenStructure);
    
    // SessionDiagnosticsArrayType
    ID_VariableTypeNode * SessionDiagnosticsArrayType;
    OpcUa_NodeId SessionDiagnosticsArrayTypeNodeId = ID_OPCNodeId(OpcUaId_SessionDiagnosticsArrayType);
    int SessDATNameNamespaces[1] = { 0 };
    char * SessDATNames[1] = { "SessionDiagnostics" };
    int SessDATIds[1] = { OpcUaId_SessionDiagnosticsArrayType_SessionDiagnostics };
    int SessDATDataTypeIds[1] = { OpcUaId_SessionDiagnosticsDataType };
    int SessDATInheritType[1] = { OpcUaId_SessionDiagnosticsVariableType };
    int SessDATLengths[1] = { 0 };
    ID_VariantStructure SessDATStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SessionDiagnosticsDataTypeExpandedNodeId, &OpcUa_SessionDiagnosticsDataType_EncodeableType}};
    ID_VariantStructure SessDATChildStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SessionDiagnosticsDataTypeExpandedNodeId, &OpcUa_SessionDiagnosticsDataType_EncodeableType, &ID_ModellingRuleExposesItsArrayNode}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &SessionDiagnosticsArrayType, SessionDiagnosticsArrayTypeNodeId, 0, "SessionDiagnosticsArrayType", OpcUa_False, ID_LENGTH_UNKNOWN_INDICATOR, SessionDiagnosticsDataType, &BaseDataVariableType, SessDATStructure, 1, SessDATNameNamespaces, SessDATNames, SessDATIds, SessDATDataTypeIds, SessDATInheritType, SessDATLengths, SessDATChildStructure);
    
    // SessionSecurityDiagnosticsType
    ID_VariableTypeNode * SessionSecurityDiagnosticsTypeNode;
    OpcUa_NodeId SessionSecurityDiagnosticsType = ID_OPCNodeId(OpcUaId_SessionSecurityDiagnosticsType);
    int SessionSecurityNameNamespaces[9] = {0,0,0,0,0,0,0,0,0};
    char * SessionSecurityNames[9] = {"SessionId","ClientUserIdOfSession","ClientUserIdHistory","AuthenticationMechanism","Encoding",
                                      "TransportProtocol","SecurityMode","SecurityPolicyUri","ClientCertificate"};
    int SessionSecurityIds[9] = {2245,2246,2247,2248,2249,2250,2251,2252,3058};
    int SessionSecurityDataTypeIds[9] = {OpcUaId_NodeId,12,12,12,12,12,302,12,15};
    int SessionSecurityInheritIds[9] = {63,63,63,63,63,63,63,63,63};
    int SessionSecurityLengths[9] = {0,0,ID_LENGTH_UNKNOWN_INDICATOR,0,0,0,0,0,0};
    OpcUa_NodeId SessionSecurityDiagnosticsDataType = ID_OPCNodeId(OpcUaId_SessionSecurityDiagnosticsDataType);
    OpcUa_ExpandedNodeId SessionSecurityDiagnosticsTypeExpandedNodeId = ID_OPCExpandedNodeId(OpcUaId_SessionSecurityDiagnosticsDataType_Encoding_DefaultBinary);
    ID_VariantStructure SessionSecurityDiagnosticsTypeStructure[1] = {{OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&SessionSecurityDiagnosticsTypeExpandedNodeId,&OpcUa_SessionSecurityDiagnosticsDataType_EncodeableType}};
    ID_VariantStructure SessionSecurityDiagnosticsTypeChildStructure[9] = {{OpcUaType_NodeId},{OpcUaType_String},{OpcUaType_String},{OpcUaType_String},{OpcUaType_String},{OpcUaType_String},{OpcUaType_Int32},{OpcUaType_String},{OpcUaType_ByteString}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &SessionSecurityDiagnosticsTypeNode, SessionSecurityDiagnosticsType, 0, "SessionSecurityDiagnosticsType", OpcUa_False, 0, SessionSecurityDiagnosticsDataType, &BaseDataVariableType, SessionSecurityDiagnosticsTypeStructure, 9, SessionSecurityNameNamespaces, SessionSecurityNames, SessionSecurityIds, SessionSecurityDataTypeIds, SessionSecurityInheritIds, SessionSecurityLengths, SessionSecurityDiagnosticsTypeChildStructure);
    
    // SessionSecurityDiagnosticsArrayType
    ID_VariableTypeNode * SessionSecurityDiagnosticsArrayType;
    OpcUa_NodeId SessionSecurityDiagnosticsArrayTypeNodeId = ID_OPCNodeId(OpcUaId_SessionSecurityDiagnosticsArrayType);
    int SSDATNamespaces[1] = {0};
    char * SSDATNames[1] = {"SessionSecurityDiagnostics"};
    int SSDATIds[1] = {OpcUaId_SessionSecurityDiagnosticsArrayType_SessionSecurityDiagnostics};
    int SSDATDataTypeIds[1] = {OpcUaId_SessionSecurityDiagnosticsDataType};
    int SSDATInheritIds[1] = {OpcUaId_SessionSecurityDiagnosticsType};
    int SSDATLengths[1] = {0};
    ID_VariantStructure SSDATStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SessionSecurityDiagnosticsTypeExpandedNodeId, &OpcUa_SessionSecurityDiagnosticsDataType_EncodeableType}};
    ID_VariantStructure SSDATChildStructure[1] = {{OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &SessionSecurityDiagnosticsTypeExpandedNodeId, &OpcUa_SessionSecurityDiagnosticsDataType_EncodeableType, &ID_ModellingRuleExposesItsArrayNode}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, &SessionSecurityDiagnosticsArrayType, SessionSecurityDiagnosticsArrayTypeNodeId, 0, "SessionSecurityDiagnosticsArrayType", OpcUa_False, ID_LENGTH_UNKNOWN_INDICATOR, SessionSecurityDiagnosticsDataType, &BaseDataVariableType, SSDATStructure, 1, SSDATNamespaces, SSDATNames, SSDATIds, SSDATDataTypeIds, SSDATInheritIds, SSDATLengths, SSDATChildStructure);
    
    // OptionSetType
    // please refer to Part 5 1.03 7.19 for reasoning behind UInt32
    OpcUa_NodeId OptionSetType = ID_OPCNodeId(OpcUaId_OptionSetType);
    int OptionSetNameNamespaces[2] = {0,0};
    char * OptionSetTypeNames[2] = {"OptionSetValues","BitMask"};
    int OptionSetTypeIds[2] = {OpcUaId_OptionSetType_OptionSetValues,OpcUaId_OptionSetType_BitMask};
    int OptionSetDataTypeIds[2] = {OpcUaId_LocalizedText,OpcUaId_Boolean};
    int OptionSetInheritIds[2] = {OpcUaId_PropertyType,OpcUaId_PropertyType};
    int OptionSetLengths[2] = {ID_LENGTH_UNKNOWN_INDICATOR,ID_LENGTH_UNKNOWN_INDICATOR};
    OpcUa_NodeId OptionSetDataType = ID_OPCNodeId(OpcUaId_UInt32);
    ID_VariantStructure OptionSetTypeStructure[1] = {{OpcUaType_UInt32,0, NULL, NULL}};
    ID_VariantStructure OptionSetTypeChildStructure[2] = {{OpcUaType_LocalizedText},{OpcUaType_Boolean}};
    ID_AddressSpaceHelper_CreateVariableTypeNode(Handle, Indexer, NULL, OptionSetType, 0, "OptionSetType", OpcUa_False, 0, OptionSetDataType, &BaseVariableType, OptionSetTypeStructure, 2, OptionSetNameNamespaces, OptionSetTypeNames, OptionSetTypeIds, OptionSetDataTypeIds,OptionSetInheritIds, OptionSetLengths, OptionSetTypeChildStructure);
} 
#pragma warning(pop)

// these are initializing "methodtypes" that can be used to create actual "method" objects
#pragma warning(push)
#pragma warning(disable: 4204)
void ID_Initialize_AllStandardMethods(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_Int NumAllocs = 5;
    ID_SETUP_METHOD_LOCALIZED(NumAllocs);
    
    OpcUa_NodeId NullId = ID_OPCNodeId(0);
    ID_MethodNode * GetMonitoredItems;
    OpcUa_NodeId GetMonitoredItemsId = ID_OPCNodeId(ID_OpcUaId_GetMonitoredItemsMethodType);
    OpcUa_NodeId GetMonitoredItemsInput = ID_OPCNodeId(ID_OpcUaId_GetMonitoredItemsMethodType_InputArguments);
    OpcUa_NodeId GetMonitoredItemsOutput = ID_OPCNodeId(ID_OpcUaId_GetMonitoredItemsMethodType_OutputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Subscription's ID");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 0, "Server Handles");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 1, "Client Handles");
    ID_InputArgsInfo GetMonitoredItemsInputArgs[1] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"subscriptionId",&InputDescs[0]}};
    ID_InputArgsInfo GetMonitoredItemsOutputArgs[2] = {{OpcUa_ValueRanks_OneDimension,2048,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"serverHandles",&OutputDescs[0]},
                                                    {OpcUa_ValueRanks_OneDimension,2048,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"clientHandles",&OutputDescs[1]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &GetMonitoredItems, GetMonitoredItemsId, 0, "GetMonitoredItems", 1, GetMonitoredItemsInput, GetMonitoredItemsInputArgs, 2, GetMonitoredItemsOutput, GetMonitoredItemsOutputArgs);
    
    ID_MethodNode * ResendData;
    OpcUa_NodeId ResendDataId = ID_OPCNodeId(ID_OpcUaId_ResendDataMethodType);
    OpcUa_NodeId ResendDataInput = ID_OPCNodeId(ID_OpcUaId_ResendDataMethodType_InputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Subscription ID");
    ID_InputArgsInfo ResendDataInputArgs[1] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"subscriptionId",&InputDescs[0]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &ResendData, ResendDataId, 0, "ResendData", 1, ResendDataInput, ResendDataInputArgs, 0, NullId, NULL);
    
    ID_MethodNode * SetSubscriptionDurable;
    OpcUa_NodeId SetSubscriptionDurableId = ID_OPCNodeId(ID_OpcUaId_SetSubscriptionDurableMethodType);
    OpcUa_NodeId SetSubscriptionDurableInput = ID_OPCNodeId(ID_OpcUaId_SetSubscriptionDurableMethodType_InputArguments);
    OpcUa_NodeId SetSubscriptionDurableOutput = ID_OPCNodeId(ID_OpcUaId_SetSubscriptionDurableMethodType_OutputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Subscription's ID");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "Lifetime of Subscription in Hours");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 0, "Server revised lifetime of subscription");
    ID_InputArgsInfo SetSubscriptionDurableInputArgs[2] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"subscriptionId",&InputDescs[0]},
                                                        {OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"lifetimeInHours",&InputDescs[1]}};
    ID_InputArgsInfo SetSubscriptionDurableOutputArgs[1] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"revisedLifetimeInHours",&OutputDescs[0]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &SetSubscriptionDurable, SetSubscriptionDurableId, 0, "SetSubscriptionDurable", 2, SetSubscriptionDurableInput, SetSubscriptionDurableInputArgs, 1, SetSubscriptionDurableOutput, SetSubscriptionDurableOutputArgs);
    
    ID_MethodNode * RequestServerStateChange;
    OpcUa_NodeId RequestServerStateChangeId = ID_OPCNodeId(ID_OpcUaId_RequestServerStateChangeMethodType);
    OpcUa_NodeId RequestServerStateChangeInput = ID_OPCNodeId(ID_OpcUaId_RequestServerStateChangeMethodType_InputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Requested Server's Target State");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "Estimated time needed to enter RUNNING state");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 2, "Seconds till the server shutdown");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 3, "Localized text describing the reason for shutdown");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 4, "Indicates whether the server should be restarted");
    ID_InputArgsInfo RequestServerStateChangeInputArgs[5] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_ServerState},"state",&InputDescs[0]},
                                                          {OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_DateTime},"estimatedReturnTime",&InputDescs[1]},
                                                          {OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"secondsTillShutdown",&InputDescs[2]},
                                                          {OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_LocalizedText},"reason",&InputDescs[3]},
                                                          {OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_Boolean},"restart",&InputDescs[4]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &RequestServerStateChange, RequestServerStateChangeId, 0, "RequestServerStateChange", 5, RequestServerStateChangeInput, RequestServerStateChangeInputArgs, 0, NullId, NULL);
    
    ID_MethodNode * Open;
    OpcUa_NodeId OpenId = ID_OPCNodeId(ID_OpcUaId_OpenMethodType);
    OpcUa_NodeId OpenInput = ID_OPCNodeId(ID_OpcUaId_OpenMethodType_InputArguments);
    OpcUa_NodeId OpenOutput = ID_OPCNodeId(ID_OpcUaId_OpenMethodType_OutputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Mode File should be opened in");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 0, "Number representing the file handle");
    ID_InputArgsInfo OpenInputArgs[1] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_Byte},"mode",&InputDescs[0]}};
    ID_InputArgsInfo OpenOutputArgs[1] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"fileHandle",&OutputDescs[0]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &Open, OpenId, 0, "Open", 1, OpenInput, OpenInputArgs, 1, OpenOutput, OpenOutputArgs);
    
    ID_MethodNode * Close;
    OpcUa_NodeId CloseId = ID_OPCNodeId(ID_OpcUaId_CloseMethodType);
    OpcUa_NodeId CloseInput = ID_OPCNodeId(ID_OpcUaId_CloseMethodType_InputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Number representing the file handle");
    ID_InputArgsInfo CloseInputArgs[1] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"fileHandle",&InputDescs[0]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &Close, CloseId, 0, "Close", 1, CloseInput, CloseInputArgs, 0, NullId, NULL);
    
    ID_MethodNode * Read;
    OpcUa_NodeId ReadId = ID_OPCNodeId(ID_OpcUaId_ReadMethodType);
    OpcUa_NodeId ReadInput = ID_OPCNodeId(ID_OpcUaId_ReadMethodType_InputArguments);
    OpcUa_NodeId ReadOutput = ID_OPCNodeId(ID_OpcUaId_ReadMethodType_OutputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Number representing the file handle");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "Length in bytes to read");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 0, "Returned data");
    ID_InputArgsInfo ReadInputArgs[2] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32}, "fileHandle",&InputDescs[0]},{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_Int32}, "length",&InputDescs[1]}};
    ID_InputArgsInfo ReadOutputArgs[1] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_ByteString}, "data",&OutputDescs[0]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &Read, ReadId, 0, "Read", 2, ReadInput, ReadInputArgs, 1, ReadOutput, ReadOutputArgs);
    
    ID_MethodNode * Write;
    OpcUa_NodeId WriteId = ID_OPCNodeId(ID_OpcUaId_WriteMethodType);
    OpcUa_NodeId WriteInput = ID_OPCNodeId(ID_OpcUaId_WriteMethodType_InputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Number representing the file handle");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "Input Byte Data");
    ID_InputArgsInfo WriteInputArgs[2] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"fileHandle",&InputDescs[0]},
                                       {OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_ByteString},"data",&InputDescs[1]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &Write, WriteId, 0, "Write", 2, WriteInput, WriteInputArgs, 0, NullId, NULL);
    
    ID_MethodNode * GetPosition;
    OpcUa_NodeId GetPositionId = ID_OPCNodeId(ID_OpcUaId_GetPositionMethodType);
    OpcUa_NodeId GetPositionInput = ID_OPCNodeId(ID_OpcUaId_GetPositionMethodType_InputArguments);
    OpcUa_NodeId GetPositionOutput = ID_OPCNodeId(ID_OpcUaId_GetPositionMethodType_OutputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Number representing the file handle");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 0, "Current Position Cursor");
    ID_InputArgsInfo GetPositionInputArgs[1] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"fileHandle",&InputDescs[0]}};
    ID_InputArgsInfo GetPositionOutputArgs[1] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt64},"position",&OutputDescs[0]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &GetPosition, GetPositionId, 0, "GetPosition", 1, GetPositionInput, GetPositionInputArgs, 1, GetPositionOutput, GetPositionOutputArgs);
    
    ID_MethodNode * SetPosition;
    OpcUa_NodeId SetPositionId = ID_OPCNodeId(ID_OpcUaId_SetPositionMethodType);
    OpcUa_NodeId SetPositionInput = ID_OPCNodeId(ID_OpcUaId_SetPositionMethodType_InputArguments);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Number representing the file handle");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "Position to set the cursor to");
    ID_InputArgsInfo SetPositionInputArgs[2] = {{OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt32},"fileHandle",&InputDescs[0]},
                                            {OpcUa_ValueRanks_Scalar,0,{OpcUa_IdType_Numeric,0,OpcUaId_UInt64},"position",&InputDescs[1]}};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &SetPosition, SetPositionId, 0, "SetPosition", 2, SetPositionInput, SetPositionInputArgs, 0, NullId, NULL);

    ID_MethodNode * ExportNamespace;
    OpcUa_NodeId ExportNamespaceId = {OpcUa_IdType_Numeric, ID_NAMESPACE_INDEX, ID_Indexer_GetNextIndex(Indexer)};
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &ExportNamespace, ExportNamespaceId, 0, "ExportNamespace", 0, NullId, NULL, 0, NullId, NULL);
    ID_ExportNamespaceTypeNode.Identifier.Numeric = ExportNamespaceId.Identifier.Numeric;
    ID_ExportNamespaceTypeNode.NamespaceIndex = ExportNamespaceId.NamespaceIndex;
    
    ID_FREE_METHOD_LOCALIZED(NumAllocs);
}
#pragma warning(pop)

void ID_Initialize_AllStandardViews(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
	OpcUa_ReferenceParameter(Handle);
	OpcUa_ReferenceParameter(Indexer);
}

void ID_Initialize_AllStandardReferenceTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_NodeId References = ID_OPCNodeId(OpcUaId_References);
    OpcUa_NodeId HierarchicalReferences = ID_OPCNodeId(OpcUaId_HierarchicalReferences);
    OpcUa_NodeId NonHierarchicalReferences = ID_OPCNodeId(OpcUaId_NonHierarchicalReferences);
    OpcUa_NodeId HasChild = ID_OPCNodeId(OpcUaId_HasChild);
    OpcUa_NodeId Organizes = ID_OPCNodeId(OpcUaId_Organizes);
    OpcUa_NodeId HasEventSource = ID_OPCNodeId(OpcUaId_HasEventSource);
    OpcUa_NodeId HasModellingRule = ID_OPCNodeId(OpcUaId_HasModellingRule);
    OpcUa_NodeId HasEncoding = ID_OPCNodeId(OpcUaId_HasEncoding);
    OpcUa_NodeId HasDescription = ID_OPCNodeId(OpcUaId_HasDescription);
    OpcUa_NodeId HasTypeDefinition = ID_OPCNodeId(OpcUaId_HasTypeDefinition);
    OpcUa_NodeId GeneratesEvent = ID_OPCNodeId(OpcUaId_GeneratesEvent);
    OpcUa_NodeId AlwaysGeneratesEvent = ID_OPCNodeId(OpcUaId_AlwaysGeneratesEvent);
    OpcUa_NodeId Aggregates = ID_OPCNodeId(OpcUaId_Aggregates);
    OpcUa_NodeId HasSubtype = ID_OPCNodeId(OpcUaId_HasSubtype);
    OpcUa_NodeId HasProperty = ID_OPCNodeId(OpcUaId_HasProperty);
    OpcUa_NodeId HasComponent = ID_OPCNodeId(OpcUaId_HasComponent);
    OpcUa_NodeId HasNotifier = ID_OPCNodeId(OpcUaId_HasNotifier);
    OpcUa_NodeId HasOrderedComponent = ID_OPCNodeId(OpcUaId_HasOrderedComponent);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, NULL, NULL, OpcUa_True, OpcUa_True, 0, "References", "", References);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &References, NULL, OpcUa_False, OpcUa_True, 0, "HierarchicalReferences", "", HierarchicalReferences);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &References, NULL, OpcUa_True, OpcUa_True, 0, "NonHierarchicalReferences", "", NonHierarchicalReferences);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &HierarchicalReferences, NULL, OpcUa_False, OpcUa_True, 0, "HasChild", "", HasChild);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &HierarchicalReferences, NULL, OpcUa_False, OpcUa_False, 0, "Organizes", "", Organizes);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &HierarchicalReferences, NULL, OpcUa_False, OpcUa_False, 0, "HasEventSource", "", HasEventSource);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &NonHierarchicalReferences, NULL, OpcUa_False, OpcUa_False, 0, "HasModellingRule", "ModellingRuleOf", HasModellingRule);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &NonHierarchicalReferences, NULL, OpcUa_False, OpcUa_False, 0, "HasEncoding", "EncodingOf", HasEncoding);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &NonHierarchicalReferences, NULL, OpcUa_False, OpcUa_False, 0, "HasDescription", "DescriptionOf", HasDescription);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &NonHierarchicalReferences, NULL, OpcUa_False, OpcUa_False, 0, "HasTypeDefinition", "TypeDefinitionOf", HasTypeDefinition);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &NonHierarchicalReferences, NULL, OpcUa_False, OpcUa_False, 0, "GeneratesEvent", "", GeneratesEvent);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &GeneratesEvent, NULL, OpcUa_False, OpcUa_False, 0, "AlwaysGeneratesEvent", "AlwaysGeneratedBy", AlwaysGeneratesEvent);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &HasChild, NULL, OpcUa_False, OpcUa_True, 0, "Aggregates", "", Aggregates);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &HasChild, NULL, OpcUa_False, OpcUa_False, 0, "HasSubtype", "SubtypeOf", HasSubtype);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &Aggregates, NULL, OpcUa_False, OpcUa_False, 0, "HasProperty", "PropertyOf", HasProperty);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &Aggregates, NULL, OpcUa_False, OpcUa_False, 0, "HasComponent", "ComponentOf", HasComponent);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &HasEventSource, NULL, OpcUa_False, OpcUa_False, 0, "HasNotifier", "NotifierOf", HasNotifier);
    ID_AddressSpaceHelper_CreateReferenceTypeNode(Handle, Indexer, &HasComponent, NULL, OpcUa_False, OpcUa_False, 0, "HasOrderedComponent", "OrderedComponentOf", HasOrderedComponent);
}

void ID_Initialize_AllStandardDataTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_NodeId NullId = ID_OPCNodeId(0);
    OpcUa_NodeId BaseDataType = ID_OPCNodeId(OpcUaId_BaseDataType);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, NULL, NULL, OpcUa_True, 0, "BaseDataType", BaseDataType, 0, NULL, NULL, NullId, NULL, NULL);
    
    OpcUa_NodeId Boolean = ID_OPCNodeId(OpcUaId_Boolean);
    OpcUa_NodeId LocalByteStringId = ID_OPCNodeId(OpcUaId_ByteString);
    OpcUa_NodeId DateTime = ID_OPCNodeId(OpcUaId_DateTime);
    OpcUa_NodeId DataValue = ID_OPCNodeId(OpcUaId_DataValue);
    OpcUa_NodeId DiagnosticInfo = ID_OPCNodeId(OpcUaId_DiagnosticInfo);
    OpcUa_NodeId Enumeration = ID_OPCNodeId(OpcUaId_Enumeration);
    OpcUa_NodeId ExpandedNodeId = ID_OPCNodeId(OpcUaId_ExpandedNodeId);
    OpcUa_NodeId Guid = ID_OPCNodeId(OpcUaId_Guid);
    OpcUa_NodeId LocalizedText = ID_OPCNodeId(OpcUaId_LocalizedText);
    OpcUa_NodeId NodeId = ID_OPCNodeId(OpcUaId_NodeId);
    OpcUa_NodeId Number = ID_OPCNodeId(OpcUaId_Number);
    OpcUa_NodeId QualifiedName = ID_OPCNodeId(OpcUaId_QualifiedName);
    OpcUa_NodeId LocalStringId = ID_OPCNodeId(OpcUaId_String);
    OpcUa_NodeId Structure = ID_OPCNodeId(OpcUaId_Structure);
    OpcUa_NodeId XmlElement = ID_OPCNodeId(OpcUaId_XmlElement);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "Boolean", Boolean, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "ByteString", LocalByteStringId, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "DataTime", DateTime, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "DataValue", DataValue, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "DiagnosticInfo", DiagnosticInfo, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_True, 0, "Enumeration", Enumeration, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "ExpandedNodeId", ExpandedNodeId, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "Guid", Guid, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "LocalizedText", LocalizedText, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "NodeId", NodeId, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_True, 0, "Number", Number, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "QualifiedName", QualifiedName, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "String", LocalStringId, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_True, 0, "Structure", Structure, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &BaseDataType, NULL, OpcUa_False, 0, "XmlElement", XmlElement, 0, NULL, NULL, NullId, NULL, NULL);
    
    // structure
    OpcUa_NodeId Argument = ID_OPCNodeId(OpcUaId_Argument); 
    OpcUa_NodeId ArgumentEncoding = ID_OPCNodeId(OpcUaId_Argument_Encoding_DefaultBinary);
    OpcUa_NodeId UserIdentityToken = ID_OPCNodeId(OpcUaId_UserIdentityToken); 
    OpcUa_NodeId UserIdentityTokenEncoding = ID_OPCNodeId(OpcUaId_UserIdentityToken_Encoding_DefaultBinary);
    OpcUa_NodeId AddNodesItem = ID_OPCNodeId(OpcUaId_AddNodesItem); 
    OpcUa_NodeId AddNodesItemEncoding = ID_OPCNodeId(OpcUaId_AddNodesItem_Encoding_DefaultBinary);
    OpcUa_NodeId AddReferencesItem = ID_OPCNodeId(OpcUaId_AddReferencesItem);
    OpcUa_NodeId AddReferencesItemEncoding = ID_OPCNodeId(OpcUaId_AddReferencesItem_Encoding_DefaultBinary);
    OpcUa_NodeId DeleteNodesItem = ID_OPCNodeId(OpcUaId_DeleteNodesItem);
    OpcUa_NodeId DeleteNodesItemEncoding = ID_OPCNodeId(OpcUaId_DeleteNodesItem_Encoding_DefaultBinary);
    OpcUa_NodeId DeleteReferencesItem = ID_OPCNodeId(OpcUaId_DeleteReferencesItem);
    OpcUa_NodeId DeleteReferencesItemEncoding = ID_OPCNodeId(OpcUaId_DeleteReferencesItem_Encoding_DefaultBinary);
    OpcUa_NodeId ApplicationDescription = ID_OPCNodeId(OpcUaId_ApplicationDescription);
    OpcUa_NodeId ApplicationDescriptionEncoding = ID_OPCNodeId(OpcUaId_ApplicationDescription_Encoding_DefaultBinary);
    OpcUa_NodeId BuildInfo = ID_OPCNodeId(OpcUaId_BuildInfo);
    OpcUa_NodeId BuildInfoEncoding = ID_OPCNodeId(OpcUaId_BuildInfo_Encoding_DefaultBinary);
    OpcUa_NodeId RedundantServerDataType = ID_OPCNodeId(OpcUaId_RedundantServerDataType);
    OpcUa_NodeId RedundantServerDataTypeEncoding = ID_OPCNodeId(OpcUaId_RedundantServerDataType_Encoding_DefaultBinary);
    OpcUa_NodeId SamplingIntervalDiagnosticsDataType = ID_OPCNodeId(OpcUaId_SamplingIntervalDiagnosticsDataType);
    OpcUa_NodeId SamplingIntervalDiagnosticsDataTypeEncoding = ID_OPCNodeId(OpcUaId_SamplingIntervalDiagnosticsDataType_Encoding_DefaultBinary);
    OpcUa_NodeId ServerDiagnosticsSummaryDataType = ID_OPCNodeId(OpcUaId_ServerDiagnosticsSummaryDataType);
    OpcUa_NodeId ServerDiagnosticsSummaryDataTypeEncoding = ID_OPCNodeId(OpcUaId_ServerDiagnosticsSummaryDataType_Encoding_DefaultBinary);
    OpcUa_NodeId ServerStatusDataType = ID_OPCNodeId(OpcUaId_ServerStatusDataType);
    OpcUa_NodeId ServerStatusDataTypeEncoding = ID_OPCNodeId(OpcUaId_ServerStatusDataType_Encoding_DefaultBinary);
    OpcUa_NodeId SessionDiagnosticsDataType = ID_OPCNodeId(OpcUaId_SessionDiagnosticsDataType);
    OpcUa_NodeId SessionDiagnosticsDataTypeEncoding = ID_OPCNodeId(OpcUaId_SessionDiagnosticsDataType_Encoding_DefaultBinary);
    OpcUa_NodeId SessionSecurityDiagnosticsDataType = ID_OPCNodeId(OpcUaId_SessionSecurityDiagnosticsDataType);
    OpcUa_NodeId SessionSecurityDiagnosticsDataTypeEncoding = ID_OPCNodeId(OpcUaId_SessionSecurityDiagnosticsDataType_Encoding_DefaultBinary);
    OpcUa_NodeId ServiceCounterDataType = ID_OPCNodeId(OpcUaId_ServiceCounterDataType);
    OpcUa_NodeId ServiceCounterDataTypeEncoding = ID_OPCNodeId(OpcUaId_ServiceCounterDataType_Encoding_DefaultBinary);
    OpcUa_NodeId StatusResult = ID_OPCNodeId(OpcUaId_StatusResult);
    OpcUa_NodeId StatusResultEncoding = ID_OPCNodeId(OpcUaId_StatusResult_Encoding_DefaultBinary);
    OpcUa_NodeId SubscriptionDiagnosticsDataType = ID_OPCNodeId(OpcUaId_SubscriptionDiagnosticsDataType);
    OpcUa_NodeId SubscriptionDiagnosticsDataTypeEncoding = ID_OPCNodeId(OpcUaId_SubscriptionDiagnosticsDataType_Encoding_DefaultBinary);
    OpcUa_NodeId ModelChangeStructureDataType = ID_OPCNodeId(OpcUaId_ModelChangeStructureDataType);
    OpcUa_NodeId ModelChangeStructureDataTypeEncoding = ID_OPCNodeId(OpcUaId_ModelChangeStructureDataType_Encoding_DefaultBinary);
    OpcUa_NodeId SemanticChangeStructureDataType = ID_OPCNodeId(OpcUaId_SemanticChangeStructureDataType);
    OpcUa_NodeId SemanticChangeStructureDataTypeEncoding = ID_OPCNodeId(OpcUaId_SemanticChangeStructureDataType_Encoding_DefaultBinary);
    OpcUa_NodeId SignedSoftwareCertificate = ID_OPCNodeId(OpcUaId_SignedSoftwareCertificate);
    OpcUa_NodeId SignedSoftwareCertificateEncoding = ID_OPCNodeId(OpcUaId_SignedSoftwareCertificate_Encoding_DefaultBinary);
    OpcUa_NodeId TimeZoneDataType = ID_OPCNodeId(OpcUaId_TimeZoneDataType);
    OpcUa_NodeId TimeZoneDataTypeEncoding = ID_OPCNodeId(OpcUaId_TimeZoneDataType_Encoding_DefaultBinary);
    OpcUa_NodeId EnumValueType = ID_OPCNodeId(OpcUaId_EnumValueType);
    OpcUa_NodeId EnumValueTypeEncoding = ID_OPCNodeId(OpcUaId_EnumValueType_Encoding_DefaultBinary);
    OpcUa_NodeId OptionSet = ID_OPCNodeId(OpcUaId_OptionSet);
    OpcUa_NodeId OptionSetEncoding = ID_OPCNodeId(OpcUaId_OptionSet_Encoding_DefaultBinary);
    OpcUa_NodeId Union = ID_OPCNodeId(OpcUaId_Union);
    OpcUa_NodeId UnionEncoding = ID_OPCNodeId(OpcUaId_Union_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "Argument", Argument, 0, NULL, NULL, NullId, NULL, &ArgumentEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "UserIdentityToken", UserIdentityToken, 0, NULL, NULL, NullId, NULL, &UserIdentityTokenEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "AddNodesItem", AddNodesItem, 0, NULL, NULL, NullId, NULL, &AddNodesItemEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "AddReferencesItem", AddReferencesItem, 0, NULL, NULL, NullId, NULL, &AddReferencesItemEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "DeleteNodesItem", DeleteNodesItem, 0, NULL, NULL, NullId, NULL, &DeleteNodesItemEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "DeleteReferencesItem", DeleteReferencesItem, 0, NULL, NULL, NullId, NULL, &DeleteReferencesItemEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ApplicationDescription", ApplicationDescription, 0, NULL, NULL, NullId, NULL, &ApplicationDescriptionEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "BuildInfo", BuildInfo, 0, NULL, NULL, NullId, NULL, &BuildInfoEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "RedundantServerDataType", RedundantServerDataType, 0, NULL, NULL, NullId, NULL, &RedundantServerDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "SamplingIntervalDiagnosticsDataType", SamplingIntervalDiagnosticsDataType, 0, NULL, NULL, NullId, NULL, &SamplingIntervalDiagnosticsDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ServerDiagnosticsSummaryDataType", ServerDiagnosticsSummaryDataType, 0, NULL, NULL, NullId, NULL, &ServerDiagnosticsSummaryDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ServerStatusDataType", ServerStatusDataType, 0, NULL, NULL, NullId, NULL, &ServerStatusDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "SessionDiagnosticsDataType", SessionDiagnosticsDataType, 0, NULL, NULL, NullId, NULL, &SessionDiagnosticsDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "SessionSecurityDiagnosticsDataType", SessionSecurityDiagnosticsDataType, 0, NULL, NULL, NullId, NULL, &SessionSecurityDiagnosticsDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ServiceCounterDataType", ServiceCounterDataType, 0, NULL, NULL, NullId, NULL, &ServiceCounterDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "StatusResult", StatusResult, 0, NULL, NULL, NullId, NULL, &StatusResultEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "SubscriptionDiagnosticsDataType", SubscriptionDiagnosticsDataType, 0, NULL, NULL, NullId, NULL, &SubscriptionDiagnosticsDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ModelChangeStructureDataType", ModelChangeStructureDataType, 0, NULL, NULL, NullId, NULL, &ModelChangeStructureDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "SemanticChangeStructureDataType", SemanticChangeStructureDataType, 0, NULL, NULL, NullId, NULL, &SemanticChangeStructureDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "SignedSoftwareCertificate", SignedSoftwareCertificate, 0, NULL, NULL, NullId, NULL, &SignedSoftwareCertificateEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "TimeZoneDataType", TimeZoneDataType, 0, NULL, NULL, NullId, NULL, &TimeZoneDataTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "EnumValueType", EnumValueType, 0, NULL, NULL, NullId, NULL, &EnumValueTypeEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "OptionSet", OptionSet, 0, NULL, NULL, NullId, NULL, &OptionSetEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "Union", Union, 0, NULL, NULL, NullId, NULL, &UnionEncoding);
    
    // enumeration
    OpcUa_NodeId IdType = ID_OPCNodeId(OpcUaId_IdType);
    OpcUa_NodeId IdTypeEnum = ID_OPCNodeId(OpcUaId_IdType_EnumStrings);
    char * IdTypeLocale[4] = { "en-US", "en-US", "en-US", "en-US" };
    char * IdTypeText[4] = { "Numeric", "String", "Guid", "Opaque" };
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Enumeration, NULL, OpcUa_False, 0, "IdType", IdType, 4, IdTypeLocale, IdTypeText, IdTypeEnum, NULL, NULL);
    
    OpcUa_NodeId NamingRuleType = ID_OPCNodeId(OpcUaId_NamingRuleType);
    OpcUa_NodeId NamingRuleTypeEnum = ID_OPCNodeId(OpcUaId_NamingRuleType_EnumValues);
    char * NamingRuleTypeLocale[3] = { "en-US", "en-US", "en-US" };
    char * NamingRuleTypeText[3] = { "Mandatory", "Optional", "Constraint" };
    int NamingRuleTypeNum[3] = { 1, 2, 3 };
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Enumeration, NULL, OpcUa_False, 0, "NamingRuleType", NamingRuleType, 3, NamingRuleTypeLocale, NamingRuleTypeText, NamingRuleTypeEnum, NamingRuleTypeNum, NULL);
    
    OpcUa_NodeId NodeClass = ID_OPCNodeId(OpcUaId_NodeClass);
    OpcUa_NodeId NodeClassEnum = ID_OPCNodeId(OpcUaId_NodeClass_EnumValues);
    char * NodeClassLocale[8] = { "en-US", "en-US", "en-US", "en-US", "en-US", "en-US", "en-US", "en-US" };
    char * NodeClassText[8] = { "Object","Variable","Method","ObjectType","VariableType","ReferenceType","DataType","View" };
    int NodeClassNum[8] = { 1,2,4,8,16,32,64,128 };
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Enumeration, NULL, OpcUa_False, 0, "NodeClass", NodeClass, 8, NodeClassLocale, NodeClassText, NodeClassEnum, NodeClassNum, NULL);
    
    OpcUa_NodeId SecurityTokenRequestType = ID_OPCNodeId(OpcUaId_SecurityTokenRequestType);
    OpcUa_NodeId SecurityTokenRequestTypeEnum = ID_OPCNodeId(OpcUaId_SecurityTokenRequestType_EnumStrings);
    char * SecurityTokenLocale[2] = { "en-US", "en-US" };
    char * SecurityTokenText[2] = { "Issue", "Renew" };
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Enumeration, NULL, OpcUa_False, 0, "SecurityTokenRequestType", SecurityTokenRequestType, 2, SecurityTokenLocale, SecurityTokenText, SecurityTokenRequestTypeEnum, NULL, NULL);
    
    OpcUa_NodeId MessageSecurityMode = ID_OPCNodeId(OpcUaId_MessageSecurityMode);
    OpcUa_NodeId MessageSecurityModeEnum = ID_OPCNodeId(OpcUaId_MessageSecurityMode_EnumStrings);
    char * MessageSecurityModeLocale[4] = {"en-US","en-US","en-US","en-US"};
    char * MessageSecurityModeText[4] = {"Invalid","None","Sign","SignAndEncrypt"};
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Enumeration, NULL, OpcUa_False, 0, "MessageSecurityMode", MessageSecurityMode, 4, MessageSecurityModeLocale, MessageSecurityModeText, MessageSecurityModeEnum, NULL, NULL);
    
    OpcUa_NodeId RedundancySupport = ID_OPCNodeId(OpcUaId_RedundancySupport);
    OpcUa_NodeId RedundancySupportEnum = ID_OPCNodeId(OpcUaId_RedundancySupport_EnumStrings);
    char * RedundancySupportLocale[6] = {"en-US","en-US","en-US","en-US","en-US","en-US"};
    char * RedundancySupportText[6] = {"None","Cold","Warm","Hot","Transparent","HotAndMirrored"};
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Enumeration, NULL, OpcUa_False, 0, "RedundancySupport", RedundancySupport, 6, RedundancySupportLocale, RedundancySupportText, RedundancySupportEnum, NULL, NULL);
    
    OpcUa_NodeId ServerState = ID_OPCNodeId(OpcUaId_ServerState);
    OpcUa_NodeId ServerStateEnum = ID_OPCNodeId(OpcUaId_ServerState_EnumStrings);
    char * ServerStateLocale[8] = {"en-US","en-US","en-US","en-US","en-US","en-US","en-US","en-US"};
    char * ServerStateText[8] = {"Running","Failed","NoConfiguration","Suspended","Shutdown","Test","CommunicationFault","Unknown"};
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Enumeration, NULL, OpcUa_False, 0, "ServerState", ServerState, 8, ServerStateLocale, ServerStateText, ServerStateEnum, NULL, NULL);
    
    OpcUa_NodeId Image = ID_OPCNodeId(OpcUaId_Image);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &ID_ByteStringNode, NULL, OpcUa_True, 0, "Image", Image, 0, NULL, NULL, NullId, NULL, NULL);

    OpcUa_NodeId Integer = ID_OPCNodeId(OpcUaId_Integer);
    OpcUa_NodeId UInteger = ID_OPCNodeId(OpcUaId_UInteger);
    OpcUa_NodeId Double = ID_OPCNodeId(OpcUaId_Double);
    OpcUa_NodeId Float = ID_OPCNodeId(OpcUaId_Float);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Number, NULL, OpcUa_True, 0, "Integer", Integer, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Number, NULL, OpcUa_True, 0, "UInteger", UInteger, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Number, NULL, OpcUa_False, 0, "Double", Double, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Number, NULL, OpcUa_False, 0, "Float", Float, 0, NULL, NULL, NullId, NULL, NULL);

    OpcUa_NodeId Duration = ID_OPCNodeId(OpcUaId_Duration);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Double, NULL, OpcUa_False, 0, "Duration", Duration, 0, NULL, NULL, NullId, NULL, NULL);

    OpcUa_NodeId SByte = ID_OPCNodeId(OpcUaId_SByte);
    OpcUa_NodeId Int16 = ID_OPCNodeId(OpcUaId_Int16);
    OpcUa_NodeId Int32 = ID_OPCNodeId(OpcUaId_Int32);
    OpcUa_NodeId Int64 = ID_OPCNodeId(OpcUaId_Int64);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Integer, NULL, OpcUa_False, 0, "SByte", SByte, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Integer, NULL, OpcUa_False, 0, "Int16", Int16, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Integer, NULL, OpcUa_False, 0, "Int32", Int32, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Integer, NULL, OpcUa_False, 0, "Int64", Int64, 0, NULL, NULL, NullId, NULL, NULL);

    OpcUa_NodeId UtcTime = ID_OPCNodeId(OpcUaId_UtcTime);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &DateTime, NULL, OpcUa_False, 0, "UtcTime", UtcTime, 0, NULL, NULL, NullId, NULL, NULL);

    OpcUa_NodeId LocaleId = ID_OPCNodeId(OpcUaId_LocaleId);
    OpcUa_NodeId NumericRange = ID_OPCNodeId(OpcUaId_NumericRange);
    OpcUa_NodeId NormalizedString = ID_OPCNodeId(OpcUaId_NormalizedString);
    OpcUa_NodeId DecimalString = ID_OPCNodeId(OpcUaId_DecimalString);
    OpcUa_NodeId DurationString = ID_OPCNodeId(OpcUaId_DurationString);
    OpcUa_NodeId TimeString = ID_OPCNodeId(OpcUaId_TimeString);
    OpcUa_NodeId DateString = ID_OPCNodeId(OpcUaId_DateString);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &ID_StringNode, NULL, OpcUa_False, 0, "LocaleId", LocaleId, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &ID_StringNode, NULL, OpcUa_False, 0, "NumericRange", NumericRange, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &ID_StringNode, NULL, OpcUa_False, 0, "NormalizedString", NormalizedString, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &ID_StringNode, NULL, OpcUa_False, 0, "DecimalString", DecimalString, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &ID_StringNode, NULL, OpcUa_False, 0, "DurationString", DurationString, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &ID_StringNode, NULL, OpcUa_False, 0, "TimeString", TimeString, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &ID_StringNode, NULL, OpcUa_False, 0, "DateString", DateString, 0, NULL, NULL, NullId, NULL, NULL);
    
    OpcUa_NodeId Byte = ID_OPCNodeId(OpcUaId_Byte);
    OpcUa_NodeId UInt16 = ID_OPCNodeId(OpcUaId_UInt16);
    OpcUa_NodeId UInt32 = ID_OPCNodeId(OpcUaId_UInt32);
    OpcUa_NodeId UInt64 = ID_OPCNodeId(OpcUaId_UInt64);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &UInteger, NULL, OpcUa_False, 0, "Byte", Byte, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &UInteger, NULL, OpcUa_False, 0, "UInt16", UInt16, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &UInteger, NULL, OpcUa_False, 0, "UInt32", UInt32, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &UInteger, NULL, OpcUa_False, 0, "UInt64", UInt64, 0, NULL, NULL, NullId, NULL, NULL);
    
    OpcUa_NodeId ImageBMP = ID_OPCNodeId(OpcUaId_ImageBMP);
    OpcUa_NodeId ImageGIF = ID_OPCNodeId(OpcUaId_ImageGIF);
    OpcUa_NodeId ImageJPG = ID_OPCNodeId(OpcUaId_ImageJPG);
    OpcUa_NodeId ImagePNG = ID_OPCNodeId(OpcUaId_ImagePNG);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Image, NULL, OpcUa_False, 0, "ImageBMP", ImageBMP, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Image, NULL, OpcUa_False, 0, "ImageGIF", ImageGIF, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Image, NULL, OpcUa_False, 0, "ImageJPG", ImageJPG, 0, NULL, NULL, NullId, NULL, NULL);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Image, NULL, OpcUa_False, 0, "ImagePNG", ImagePNG, 0, NULL, NULL, NullId, NULL, NULL);

    OpcUa_NodeId BitFieldMaskDataType = ID_OPCNodeId(OpcUaId_BitFieldMaskDataType);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &UInt64, NULL, OpcUa_False, 0, "BitFieldMaskDataType", BitFieldMaskDataType, 0, NULL, NULL, NullId, NULL, NULL);

    OpcUa_NodeId AnonymousIdentityToken = ID_OPCNodeId(OpcUaId_AnonymousIdentityToken);
    OpcUa_NodeId AnonymousIdentityTokenEncoding = ID_OPCNodeId(OpcUaId_AnonymousIdentityToken_Encoding_DefaultBinary);
    OpcUa_NodeId UserNameIdentityToken = ID_OPCNodeId(OpcUaId_UserNameIdentityToken);
    OpcUa_NodeId UserNameidentityTokenEncoding = ID_OPCNodeId(OpcUaId_UserNameIdentityToken_Encoding_DefaultBinary);
    OpcUa_NodeId X509IdentityToken = ID_OPCNodeId(OpcUaId_X509IdentityToken);
    OpcUa_NodeId X509IdentityTokenEncoding = ID_OPCNodeId(OpcUaId_X509IdentityToken_Encoding_DefaultBinary);
    OpcUa_NodeId WssIdentityToken = ID_OPCNodeId(OpcUaId_IssuedIdentityToken);
    OpcUa_NodeId WssIdentityTokenEncoding = ID_OPCNodeId(OpcUaId_IssuedIdentityToken_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &UserIdentityToken, NULL, OpcUa_False, 0, "AnonymousIdentityToken", AnonymousIdentityToken, 0, NULL, NULL, NullId, NULL, &AnonymousIdentityTokenEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &UserIdentityToken, NULL, OpcUa_False, 0, "UserNameIdentityToken", UserNameIdentityToken, 0, NULL, NULL, NullId, NULL, &UserNameidentityTokenEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &UserIdentityToken, NULL, OpcUa_False, 0, "X509IdentityToken", X509IdentityToken, 0, NULL, NULL, NullId, NULL, &X509IdentityTokenEncoding);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &UserIdentityToken, NULL, OpcUa_False, 0, "IssuedIdentityToken", WssIdentityToken, 0, NULL, NULL, NullId, NULL, &WssIdentityTokenEncoding);
    
    OpcUa_NodeId NetworkGroupDataType = ID_OPCNodeId(OpcUaId_NetworkGroupDataType);
    OpcUa_NodeId NetworkGroupDataTypeEncoding = ID_OPCNodeId(OpcUaId_NetworkGroupDataType_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "NetworkGroupDataType", NetworkGroupDataType, 0, NULL, NULL, NullId, NULL, &NetworkGroupDataTypeEncoding);
    
    OpcUa_NodeId EndpointUrlListDataType = ID_OPCNodeId(OpcUaId_EndpointUrlListDataType);
    OpcUa_NodeId EndpointUrlListDataTypeEncoding = ID_OPCNodeId(OpcUaId_EndpointUrlListDataType_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "EndpointUrlListDataType", EndpointUrlListDataType, 0, NULL, NULL, NullId, NULL, &EndpointUrlListDataTypeEncoding);
    
    OpcUa_NodeId EUInformation = ID_OPCNodeId(OpcUaId_EUInformation);
    OpcUa_NodeId EUInformationEncoding = ID_OPCNodeId(OpcUaId_EUInformation_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "EUInformation", EUInformation, 0, NULL, NULL, NullId, NULL, &EUInformationEncoding);

    // part 4 listed specifications
    OpcUa_NodeId ApplicationInstanceCertificate = ID_OPCNodeId(OpcUaId_ApplicationInstanceCertificate);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ApplicationInstanceCertificate", ApplicationInstanceCertificate, 0, NULL, NULL, NullId, NULL, NULL);
    
    OpcUa_NodeId BrowseResult = ID_OPCNodeId(OpcUaId_BrowseResult);
    OpcUa_NodeId BrowseResultEncoding = ID_OPCNodeId(OpcUaId_BrowseResult_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "BrowseResult", BrowseResult, 0, NULL, NULL, NullId, NULL, &BrowseResultEncoding);
    
    OpcUa_NodeId ContentFilter = ID_OPCNodeId(OpcUaId_ContentFilter);
    OpcUa_NodeId ContentFilterEncoding = ID_OPCNodeId(OpcUaId_ContentFilter_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ContentFilter", ContentFilter, 0, NULL, NULL, NullId, NULL, &ContentFilterEncoding);
    
    OpcUa_NodeId ContentFilterResult = ID_OPCNodeId(OpcUaId_ContentFilterResult);
    OpcUa_NodeId ContentFilterResultEncoding = ID_OPCNodeId(OpcUaId_ContentFilterResult_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ContentFilterResult", ContentFilterResult, 0, NULL, NULL, NullId, NULL, &ContentFilterResultEncoding);
    
    OpcUa_NodeId FilterOperator = ID_OPCNodeId(OpcUaId_FilterOperator);
    OpcUa_NodeId FilterOperatorEnum = ID_OPCNodeId(OpcUaId_FilterOperator_EnumStrings);
    char * FilterOperatorLocale[18];
    for (int i = 0; i < 18; i++) { FilterOperatorLocale[i] = "en-US"; };
    char * FilterOperatorText[18] = {"Equals","IsNull","GreaterThan","LessThan","GreaterThanOrEqual","LessThanOrEqual","Like","Not","Between","InList","And","Or","Cast","InView","OfType","RelatedTo","BitwiseAnd","BitwiseOr"};
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Enumeration, NULL, OpcUa_False, 0, "FilterOperator", FilterOperator, 18, FilterOperatorLocale, FilterOperatorText, FilterOperatorEnum, NULL, NULL);
    
    OpcUa_NodeId ElementOperand = ID_OPCNodeId(OpcUaId_ElementOperand);
    OpcUa_NodeId ElementOperandEncoding = ID_OPCNodeId(OpcUaId_ElementOperand_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ElementOperand", ElementOperand, 0, NULL, NULL, NullId, NULL, &ElementOperandEncoding);
    
    OpcUa_NodeId LiteralOperand = ID_OPCNodeId(OpcUaId_LiteralOperand);
    OpcUa_NodeId LOEncoding = ID_OPCNodeId(OpcUaId_LiteralOperand_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "LiteralOperand", LiteralOperand, 0, NULL, NULL, NullId, NULL, &LOEncoding);
    
    OpcUa_NodeId AttributeOperand = ID_OPCNodeId(OpcUaId_AttributeOperand);
    OpcUa_NodeId AOEncoding = ID_OPCNodeId(OpcUaId_AttributeOperand_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "AttributeOperand", AttributeOperand, 0, NULL, NULL, NullId, NULL, &AOEncoding);
    
    OpcUa_NodeId SimpleAttributeOperand = ID_OPCNodeId(OpcUaId_SimpleAttributeOperand);
    OpcUa_NodeId SAOEncoding = ID_OPCNodeId(OpcUaId_SimpleAttributeOperand_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "SimpleAttributeOperand", SimpleAttributeOperand, 0, NULL, NULL, NullId, NULL, &SAOEncoding);

    OpcUa_NodeId DiscoveryConfiguration = ID_OPCNodeId(OpcUaId_DiscoveryConfiguration);
    OpcUa_NodeId DCEncoding = ID_OPCNodeId(OpcUaId_DiscoveryConfiguration_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "DiscoveryConfiguration", DiscoveryConfiguration, 0, NULL, NULL, NullId, NULL, &DCEncoding);
    
    OpcUa_NodeId MdnsDiscoveryConfiguration = ID_OPCNodeId(OpcUaId_MdnsDiscoveryConfiguration);
    OpcUa_NodeId MDCEncoding = ID_OPCNodeId(OpcUaId_MdnsDiscoveryConfiguration_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "MdnsDiscoveryConfiguration", MdnsDiscoveryConfiguration, 0, NULL, NULL, NullId, NULL, &MDCEncoding);
    
    OpcUa_NodeId EndpointDescription = ID_OPCNodeId(OpcUaId_EndpointDescription);
    OpcUa_NodeId EDEncoding = ID_OPCNodeId(OpcUaId_EndpointDescription_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "EndpointDescription", EndpointDescription, 0, NULL, NULL, NullId, NULL, &EDEncoding);
    
    OpcUa_NodeId MonitoringParameters = ID_OPCNodeId(OpcUaId_MonitoringParameters);
    OpcUa_NodeId MPEncoding = ID_OPCNodeId(OpcUaId_MonitoringParameters_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "MonitoringParameters", MonitoringParameters, 0, NULL, NULL, NullId, NULL, &MPEncoding);
    
    OpcUa_NodeId DataChangeFilter = ID_OPCNodeId(OpcUaId_DataChangeFilter);
    OpcUa_NodeId DCFEncoding = ID_OPCNodeId(OpcUaId_DataChangeFilter_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "DataChangeFilter", DataChangeFilter, 0, NULL, NULL, NullId, NULL, &DCFEncoding);
    
    OpcUa_NodeId EventFilter = ID_OPCNodeId(OpcUaId_EventFilter);
    OpcUa_NodeId EFEncoding = ID_OPCNodeId(OpcUaId_EventFilter_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "EventFilter", EventFilter, 0, NULL, NULL, NullId, NULL, &EFEncoding);
    
    OpcUa_NodeId AggregateFilter = ID_OPCNodeId(OpcUaId_AggregateFilter);
    OpcUa_NodeId AFEncoding = ID_OPCNodeId(OpcUaId_AggregateFilter_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "AggregateFilter", AggregateFilter, 0, NULL, NULL, NullId, NULL, &AFEncoding);
    
    OpcUa_NodeId AggregateFilterResult = ID_OPCNodeId(OpcUaId_AggregateFilterResult);
    OpcUa_NodeId AFREncoding = ID_OPCNodeId(OpcUaId_AggregateFilterResult_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "AggregateFilterResult", AggregateFilterResult, 0, NULL, NULL, NullId, NULL, &AFREncoding);
    
    OpcUa_NodeId ObjectAttributes = ID_OPCNodeId(OpcUaId_ObjectAttributes);
    OpcUa_NodeId OAEncoding = ID_OPCNodeId(OpcUaId_ObjectAttributes_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ObjectAttributes", ObjectAttributes, 0, NULL, NULL, NullId, NULL, &OAEncoding);
    
    OpcUa_NodeId VariableAttributes = ID_OPCNodeId(OpcUaId_VariableAttributes);
    OpcUa_NodeId VAEncoding = ID_OPCNodeId(OpcUaId_VariableAttributes_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "VariableAttributes", VariableAttributes, 0, NULL, NULL, NullId, NULL, &VAEncoding);
    
    OpcUa_NodeId MethodAttributes = ID_OPCNodeId(OpcUaId_MethodAttributes);
    OpcUa_NodeId MAEncoding = ID_OPCNodeId(OpcUaId_MethodAttributes_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "MethodAttributes", MethodAttributes, 0, NULL, NULL, NullId, NULL, &MAEncoding);
    
    OpcUa_NodeId DataTypeEncodingId = ID_OPCNodeId(0);
    OpcUa_NodeId ObjectTypeAttributes = ID_OPCNodeId(OpcUaId_ObjectTypeAttributes);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_ObjectTypeAttributes_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ObjectTypeAttributes", ObjectTypeAttributes, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);
    
    OpcUa_NodeId VariableTypeAttributes = ID_OPCNodeId(OpcUaId_VariableTypeAttributes);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_VariableTypeAttributes_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "VariableTypeAttributes", VariableTypeAttributes, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);
    
    OpcUa_NodeId ReferenceTypeAttributes = ID_OPCNodeId(OpcUaId_ReferenceTypeAttributes);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_ReferenceTypeAttributes_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ReferenceTypeAttributes", ReferenceTypeAttributes, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);
    
    OpcUa_NodeId DataTypeAttributes = ID_OPCNodeId(OpcUaId_DataTypeAttributes);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_DataTypeAttributes_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "DataTypeAttributes", DataTypeAttributes, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);

    OpcUa_NodeId ViewAttributes = ID_OPCNodeId(OpcUaId_ViewAttributes);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_ViewAttributes_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ViewAttributes", ViewAttributes, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);

    OpcUa_NodeId DataChangeNotification = ID_OPCNodeId(OpcUaId_DataChangeNotification);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_DataChangeNotification_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "DataChangeNotification", DataChangeNotification, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);

    OpcUa_NodeId EventNotificationList = ID_OPCNodeId(OpcUaId_EventNotificationList);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_EventNotificationList_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "EventNotificationList", EventNotificationList, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);

    OpcUa_NodeId StatusChangeNotification = ID_OPCNodeId(OpcUaId_StatusChangeNotification);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_StatusChangeNotification_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "StatusChangeNotification", StatusChangeNotification, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);

    OpcUa_NodeId NotificationMessage = ID_OPCNodeId(OpcUaId_NotificationMessage);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_NotificationMessage_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "NotificationMessage", NotificationMessage, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);

    OpcUa_NodeId Range = ID_OPCNodeId(OpcUaId_Range);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_Range_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "Range", Range, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);
    
    OpcUa_NodeId QueryDataSet = ID_OPCNodeId(OpcUaId_QueryDataSet);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_QueryDataSet_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "QueryDataSet", QueryDataSet, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);
    
    OpcUa_NodeId ReadValueId = ID_OPCNodeId(OpcUaId_ReadValueId);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_ReadValueId_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ReadValueId", ReadValueId, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);
    
    OpcUa_NodeId ReferenceDescription = ID_OPCNodeId(OpcUaId_ReferenceDescription);
    DataTypeEncodingId.Identifier.Numeric = OpcUaId_ReferenceDescription_Encoding_DefaultBinary;
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ReferenceDescription", ReferenceDescription, 0, NULL, NULL, NullId, NULL, &DataTypeEncodingId);
    
    OpcUa_NodeId RelativePath = ID_OPCNodeId(OpcUaId_RelativePath);
    OpcUa_NodeId RelativePathEncoding = ID_OPCNodeId(OpcUaId_RelativePath_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "RelativePath", RelativePath, 0, NULL, NULL, NullId, NULL, &RelativePathEncoding);
    
    OpcUa_NodeId RegisteredServer = ID_OPCNodeId(OpcUaId_RegisteredServer);
    OpcUa_NodeId RegisteredServerEncoding = ID_OPCNodeId(OpcUaId_RegisteredServer_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "RegisteredServer", RegisteredServer, 0, NULL, NULL, NullId, NULL, &RegisteredServerEncoding);
    
    OpcUa_NodeId RequestHeader = ID_OPCNodeId(OpcUaId_RequestHeader);
    OpcUa_NodeId RequestHeaderEncoding = ID_OPCNodeId(OpcUaId_RequestHeader_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "RequestHeader", RequestHeader, 0, NULL, NULL, NullId, NULL, &RequestHeaderEncoding);
    
    OpcUa_NodeId ResponseHeader = ID_OPCNodeId(OpcUaId_ResponseHeader);
    OpcUa_NodeId ResponseHeaderEncoding = ID_OPCNodeId(OpcUaId_ResponseHeader_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ResponseHeader", ResponseHeader, 0, NULL, NULL, NullId, NULL, &ResponseHeaderEncoding);
    
    OpcUa_NodeId ServiceFault = ID_OPCNodeId(OpcUaId_ServiceFault);
    OpcUa_NodeId ServiceFaultEncoding = ID_OPCNodeId(OpcUaId_ServiceFault_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ServiceFault", ServiceFault, 0, NULL, NULL, NullId, NULL, &ServiceFaultEncoding);
    
    OpcUa_NodeId SignatureData = ID_OPCNodeId(OpcUaId_SignatureData);
    OpcUa_NodeId SignatureDataEncoding = ID_OPCNodeId(OpcUaId_SignatureData_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "SignatureData", SignatureData, 0, NULL, NULL, NullId, NULL, &SignatureDataEncoding);

    OpcUa_NodeId TimestampsToReturn = ID_OPCNodeId(OpcUaId_TimestampsToReturn);
    OpcUa_NodeId TimestampsToReturnEnum = ID_OPCNodeId(OpcUaId_TimestampsToReturn_EnumStrings);
    char * TimestampsToReturnLocale[4] = {"en-US","en-US","en-US","en-US"};
    char * TimestampsToReturnText[4] = {"Source","Server","Both","Neither"};
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Enumeration, NULL, OpcUa_False, 0, "TimestampsToReturn", TimestampsToReturn, 4, TimestampsToReturnLocale, TimestampsToReturnText, TimestampsToReturnEnum, NULL, NULL);
    
    OpcUa_NodeId UserTokenPolicy = ID_OPCNodeId(OpcUaId_UserTokenPolicy);
    OpcUa_NodeId UserTokenPolicyEncoding = ID_OPCNodeId(OpcUaId_UserTokenPolicy_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "UserTokenPolicy", UserTokenPolicy, 0, NULL, NULL, NullId, NULL, &UserTokenPolicyEncoding);
    
    OpcUa_NodeId ViewDescription = ID_OPCNodeId(OpcUaId_ViewDescription);
    OpcUa_NodeId ViewDescriptionEncoding = ID_OPCNodeId(OpcUaId_ViewDescription_Encoding_DefaultBinary);
    ID_AddressSpaceHelper_CreateDataTypeNode(Handle, Indexer, &Structure, NULL, OpcUa_False, 0, "ViewDescription", ViewDescription, 0, NULL, NULL, NullId, NULL, &ViewDescriptionEncoding);
}