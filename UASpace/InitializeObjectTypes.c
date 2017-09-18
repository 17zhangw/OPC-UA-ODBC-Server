/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "Initialize.h"

void ID_Initialize_AllStandardObjectTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    ID_Initialize_StandardObjectTypes(Handle, Indexer);
    ID_Initialize_StandardEventTypes(Handle, Indexer);
}

#pragma warning(push)
#pragma warning(disable: 4204 4221)
void ID_Initialize_StandardObjectTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;
    OpcUa_NodeId ID_HasComponentNode = ID_HasComponent;
    OpcUa_NodeId ID_HasModellingRuleNode = ID_HasModellingRule;
    OpcUa_NodeId ID_ModellingRuleOptionalNode = ID_ModellingRuleOptional;
    OpcUa_NodeId ID_ModellingRuleOptionalPlaceholderNode = ID_ModellingRuleOptionalPlaceholder;

    // base object type and data type encoding type initialized at earlier stage
    OpcUa_NodeId BaseObjectTypeId = ID_OPCNodeId(OpcUaId_BaseObjectType);
    ID_ObjectTypeNode * BaseObjectTypeNode = ID_AddressSpace_GetObjectTypeNode(Handle, &BaseObjectTypeId);
    
    ID_ObjectTypeNode * ModellingRuleTypeNode;
    OpcUa_NodeId ModellingRuleType = ID_OPCNodeId(OpcUaId_ModellingRuleType);
    ID_PropertyAddition Additions[1] = {{0, "NamingRule",OpcUaId_ModellingRuleType_NamingRule,0,0,OpcUaId_NamingRuleType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Int32,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &ModellingRuleTypeNode, 0, "ModellingRuleType", OpcUa_False, ModellingRuleType, 1, Additions);
    
    ID_ObjectTypeNode * FolderTypeNode;
    OpcUa_NodeId FolderType = ID_OPCNodeId(OpcUaId_FolderType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &FolderTypeNode, 0, "FolderType", OpcUa_False, FolderType, 0, NULL);
    
    OpcUa_NodeId DataTypeSystemType = ID_OPCNodeId(OpcUaId_DataTypeSystemType);
    OpcUa_NodeId AggregateFunctionType = ID_OPCNodeId(OpcUaId_AggregateFunctionType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, NULL, 0, "DataTypeSystemType", OpcUa_False, DataTypeSystemType, 0, NULL);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, NULL, 0, "AggregateFunctionType", OpcUa_False, AggregateFunctionType, 0, NULL);
    
    ID_ObjectTypeNode * VendorServerInfoNode;
    OpcUa_NodeId VendorServerInfoType = ID_OPCNodeId(OpcUaId_VendorServerInfoType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &VendorServerInfoNode, 0, "VendorServerInfoType", OpcUa_False, VendorServerInfoType, 0, NULL);
  
    ID_ObjectTypeNode * ServerRedundancyTypeNode;
    OpcUa_NodeId ServerRedundancyType = ID_OPCNodeId(OpcUaId_ServerRedundancyType);
    ID_PropertyAddition SRTAdditions[1] = {{0, "RedundancySupport",OpcUaId_ServerRedundancyType_RedundancySupport,0,0,OpcUaId_RedundancySupport,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Int32,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &ServerRedundancyTypeNode, 0, "ServerRedundancyType", OpcUa_False, ServerRedundancyType, 1, SRTAdditions);
    
    OpcUa_NodeId TransparentRedundancyType = ID_OPCNodeId(OpcUaId_TransparentRedundancyType);
    OpcUa_ExpandedNodeId RSDTNode = ID_OPCExpandedNodeId(OpcUaId_RedundantServerDataType_Encoding_DefaultBinary);
    ID_PropertyAddition TRTAdditions[2] = {{0, "CurrentServerId",OpcUaId_TransparentRedundancyType_CurrentServerId,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                           {0, "RedundantServerArray",OpcUaId_TransparentRedundancyType_RedundantServerArray,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_RedundantServerDataType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&RSDTNode,&OpcUa_RedundantServerDataType_EncodeableType}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, ServerRedundancyTypeNode, NULL, 0, "TransparentRedundancyType", OpcUa_False, TransparentRedundancyType, 2, TRTAdditions);
    
    ID_ObjectTypeNode * NonTransparentObjectTypeNode;
    OpcUa_NodeId NonTransparentObjectType = ID_OPCNodeId(OpcUaId_NonTransparentRedundancyType);
    ID_PropertyAddition NTOTAdditions[2] = {{0, "ServerUriArray",OpcUaId_NonTransparentRedundancyType_ServerUriArray,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, ServerRedundancyTypeNode, &NonTransparentObjectTypeNode, 0, "NonTransparentObjectType", OpcUa_False, NonTransparentObjectType, 1, NTOTAdditions);
    
    OpcUa_NodeId NonTransparentNetworkRedundancyType = ID_OPCNodeId(OpcUaId_NonTransparentNetworkRedundancyType);
    OpcUa_ExpandedNodeId NGDTNode = ID_OPCExpandedNodeId(OpcUaId_NetworkGroupDataType_Encoding_DefaultBinary);
    ID_PropertyAddition NTNRAdditions[1] = {{0, "ServerNetworkGroups",OpcUaId_NonTransparentNetworkRedundancyType_ServerNetworkGroups,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_NetworkGroupDataType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&NGDTNode,&OpcUa_NetworkGroupDataType_EncodeableType}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, NonTransparentObjectTypeNode, NULL, 0, "NonTransparentNetworkRedundancyType", OpcUa_False, NonTransparentNetworkRedundancyType, 1, NTNRAdditions);
    
    ID_ObjectTypeNode * OperationLimitsTypeNode;
    OpcUa_NodeId OperationLimitsType = ID_OPCNodeId(OpcUaId_OperationLimitsType);
    ID_PropertyAddition OLTAdditions[12] = {{0, "MaxNodesPerRead",OpcUaId_OperationLimitsType_MaxNodesPerRead,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerHistoryReadData",OpcUaId_OperationLimitsType_MaxNodesPerHistoryReadData,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerHistoryReadEvents",OpcUaId_OperationLimitsType_MaxNodesPerHistoryReadEvents,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerWrite",OpcUaId_OperationLimitsType_MaxNodesPerWrite,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerHistoryUpdateData",OpcUaId_OperationLimitsType_MaxNodesPerHistoryUpdateData,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerHistoryUpdateEvents",OpcUaId_OperationLimitsType_MaxNodesPerHistoryUpdateEvents,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerMethodCall",OpcUaId_OperationLimitsType_MaxNodesPerMethodCall,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerBrowse",OpcUaId_OperationLimitsType_MaxNodesPerBrowse,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerRegisterNodes",OpcUaId_OperationLimitsType_MaxNodesPerRegisterNodes,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerTranslateBrowsePathsToNodeIds",OpcUaId_OperationLimitsType_MaxNodesPerTranslateBrowsePathsToNodeIds,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxNodesPerNodeManagement",OpcUaId_OperationLimitsType_MaxNodesPerNodeManagement,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                            {0, "MaxMonitoredItemsPerCall",OpcUaId_OperationLimitsType_MaxMonitoredItemsPerCall,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, FolderTypeNode, &OperationLimitsTypeNode, 0, "OperationLimitsType", OpcUa_False, OperationLimitsType, 12, OLTAdditions);
    
    // FileType and dependencies
    ID_ObjectTypeNode * FileTypeNode;
    OpcUa_NodeId FileType = ID_OPCNodeId(OpcUaId_FileType);
    ID_PropertyAddition FileTypeAdditions[5] = {{0, "Size",OpcUaId_FileType_Size,0,0,OpcUaId_UInt64,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt64,0,NULL,NULL},
                                                {0, "Writable",OpcUaId_FileType_Writable,0,0,OpcUaId_Boolean,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Boolean,0,NULL,NULL},
                                                {0, "UserWritable",OpcUaId_FileType_UserWritable,0,0,OpcUaId_Boolean,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Boolean,0,NULL,NULL},
                                                {0, "OpenCount",OpcUaId_FileType_OpenCount,0,0,OpcUaId_UInt16,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt16,0,NULL,NULL},
                                                {0, "MimeType",OpcUaId_FileType_MimeType,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleOptionalNode,NULL,OpcUaType_String,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &FileTypeNode, 0, "FileType", OpcUa_False, FileType, 5, FileTypeAdditions);
    
    OpcUa_NodeId OpenTypeId = ID_OPCNodeId(ID_OpcUaId_OpenMethodType); OpcUa_NodeId OpenId = ID_OPCNodeId(OpcUaId_FileType_Open);
    OpcUa_NodeId OpenInputId = ID_OPCNodeId(OpcUaId_FileType_Open_InputArguments); OpcUa_NodeId OpenOutputId = ID_OPCNodeId(OpcUaId_FileType_Open_OutputArguments);
    ID_MethodNode * Open = ID_AddressSpace_CloneMethod(Handle, Indexer, &OpenTypeId, &OpenId, &OpenInputId, &OpenOutputId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Open, 0, "Open");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Open, "Open");
    ID_AddressSpace_AddUniReference(Handle, &OpenId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &FileType, ID_NodeType_ObjectType, &OpenId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId CloseTypeId = ID_OPCNodeId(ID_OpcUaId_CloseMethodType); OpcUa_NodeId CloseId = ID_OPCNodeId(OpcUaId_FileType_Close);
    OpcUa_NodeId CloseInputId = ID_OPCNodeId(OpcUaId_FileType_Close_InputArguments);
    ID_MethodNode * Close = ID_AddressSpace_CloneMethod(Handle, Indexer, &CloseTypeId, &CloseId, &CloseInputId, NULL);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Close, 0, "Close");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Close, "Close");
    ID_AddressSpace_AddUniReference(Handle, &CloseId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &FileType, ID_NodeType_ObjectType, &CloseId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId ReadTypeId = ID_OPCNodeId(ID_OpcUaId_ReadMethodType); OpcUa_NodeId ReadId = ID_OPCNodeId(OpcUaId_FileType_Read);
    OpcUa_NodeId ReadInputId = ID_OPCNodeId(OpcUaId_FileType_Read_InputArguments); OpcUa_NodeId ReadOutputId = ID_OPCNodeId(OpcUaId_FileType_Read_OutputArguments);
    ID_MethodNode * Read = ID_AddressSpace_CloneMethod(Handle, Indexer, &ReadTypeId, &ReadId, &ReadInputId, &ReadOutputId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Read, 0, "Read");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Read, "Read");
    ID_AddressSpace_AddUniReference(Handle, &ReadId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &FileType, ID_NodeType_ObjectType, &ReadId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId WriteTypeId = ID_OPCNodeId(ID_OpcUaId_WriteMethodType); OpcUa_NodeId WriteId = ID_OPCNodeId(OpcUaId_FileType_Write);
    OpcUa_NodeId WriteInputId = ID_OPCNodeId(OpcUaId_FileType_Write_InputArguments);
    ID_MethodNode * Write = ID_AddressSpace_CloneMethod(Handle, Indexer, &WriteTypeId, &WriteId, &WriteInputId, NULL);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Write, 0, "Write");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Write, "Write");
    ID_AddressSpace_AddUniReference(Handle, &WriteId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &FileType, ID_NodeType_ObjectType, &WriteId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId GetPositionTypeId = ID_OPCNodeId(ID_OpcUaId_GetPositionMethodType); OpcUa_NodeId GetPositionId = ID_OPCNodeId(OpcUaId_FileType_GetPosition);
    OpcUa_NodeId GetPositionInputId = ID_OPCNodeId(OpcUaId_FileType_GetPosition_InputArguments); OpcUa_NodeId GetPositionOutputId = ID_OPCNodeId(OpcUaId_FileType_GetPosition_OutputArguments);
    ID_MethodNode * GetPosition = ID_AddressSpace_CloneMethod(Handle, Indexer, &GetPositionTypeId, &GetPositionId, &GetPositionInputId, &GetPositionOutputId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(GetPosition, 0, "GetPosition");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(GetPosition, "GetPosition");
    ID_AddressSpace_AddUniReference(Handle, &GetPositionId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &FileType, ID_NodeType_ObjectType, &GetPositionId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId SetPositionTypeId = ID_OPCNodeId(ID_OpcUaId_SetPositionMethodType); OpcUa_NodeId SetPositionId = ID_OPCNodeId(OpcUaId_FileType_SetPosition);
    OpcUa_NodeId SetPositionInputId = ID_OPCNodeId(OpcUaId_FileType_SetPosition_InputArguments);
    ID_MethodNode * SetPosition = ID_AddressSpace_CloneMethod(Handle, Indexer, &SetPositionTypeId, &SetPositionId, &SetPositionInputId, NULL);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(SetPosition, 0, "SetPosition");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(SetPosition, "SetPosition");
    ID_AddressSpace_AddUniReference(Handle, &SetPositionId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &FileType, ID_NodeType_ObjectType, &SetPositionId, ID_NodeType_Method, &ID_ModellingRuleMandatoryNode);
    
    // AddressSpaceFileType
    ID_ObjectTypeNode * AddressSpaceFileTypeNode;
    OpcUa_NodeId AddressSpaceFileTypeId = ID_OPCNodeId(OpcUaId_AddressSpaceFileType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, FileTypeNode, &AddressSpaceFileTypeNode, 0, "AddressSpaceFileType", OpcUa_False, AddressSpaceFileTypeId, 0, NULL);
    OpcUa_NodeId ExportNamespaceTypeId = ID_ExportNamespaceTypeNode; OpcUa_NodeId ExportNamespaceId = ID_OPCNodeId(OpcUaId_AddressSpaceFileType_ExportNamespace);
    ID_MethodNode * ExportNamespace = ID_AddressSpace_CloneMethod(Handle, Indexer, &ExportNamespaceTypeId, &ExportNamespaceId, NULL, NULL);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ExportNamespace, 0, "ExportNamespace");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ExportNamespace, "ExportNamespace");
    ID_AddressSpace_AddUniReference(Handle, &ExportNamespaceId, ID_NodeType_Method, &ID_ModellingRuleOptionalNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &AddressSpaceFileTypeId, ID_NodeType_ObjectType, &ExportNamespaceId, ID_NodeType_Method, &ID_HasComponentNode);
    
    // NamespaceMetadataType
    ID_ObjectTypeNode * NamespaceMetadataTypeNode;
    OpcUa_NodeId NamespaceMetadataTypeId = ID_OPCNodeId(OpcUaId_NamespaceMetadataType);
    ID_PropertyAddition NamespaceMetadataChildStructure[7] = {{0, "NamespaceUri",OpcUaId_NamespaceMetadataType_NamespaceUri,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                              {0, "NamespaceVersion",OpcUaId_NamespaceMetadataType_NamespaceVersion,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                              {0, "NamespacePublicationDate",OpcUaId_NamespaceMetadataType_NamespacePublicationDate,0,0,OpcUaId_DateTime,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_DateTime,0,NULL,NULL},
                                                              {0, "IsNamespaceSubset",OpcUaId_NamespaceMetadataType_IsNamespaceSubset,0,0,OpcUaId_Boolean,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Boolean,0,NULL,NULL},
                                                              {0, "StaticNodeIdTypes",OpcUaId_NamespaceMetadataType_StaticNodeIdTypes,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_IdType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Int32,0,NULL,NULL},
                                                              {0, "StaticNumericNodeIdRange",OpcUaId_NamespaceMetadataType_StaticNumericNodeIdRange,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_NumericRange,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                              {0, "StaticStringNodeIdPattern",OpcUaId_NamespaceMetadataType_StaticStringNodeIdPattern,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &NamespaceMetadataTypeNode, 0, "NamespaceMetadataType", OpcUa_False, NamespaceMetadataTypeId, 7, NamespaceMetadataChildStructure);
    OpcUa_NodeId AddressSpaceFileTypeChildId = ID_OPCNodeId(OpcUaId_NamespaceMetadataType_NamespaceFile);
    
    // NamespaceMetadataType.NamespaceFile
    ID_ObjectNode * AddressSpaceFileTypeChild = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, AddressSpaceFileTypeNode, &AddressSpaceFileTypeChildId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(AddressSpaceFileTypeChild, 0, "NamespaceFile");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(AddressSpaceFileTypeChild, "NamespaceFile");
    ID_AddressSpace_AddUniReference(Handle, &AddressSpaceFileTypeChildId, ID_NodeType_Object, &ID_ModellingRuleOptionalNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &NamespaceMetadataTypeId, ID_NodeType_ObjectType, &AddressSpaceFileTypeChildId, ID_NodeType_Object, &ID_HasComponentNode);
    
    // Namespaces + dependencies
    ID_ObjectTypeNode * NamespacesTypeNode;
    OpcUa_NodeId NamespacesTypeId = ID_OPCNodeId(OpcUaId_NamespacesType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &NamespacesTypeNode, 0, "NamespacesType", OpcUa_False, NamespacesTypeId, 0, NULL);
    
    OpcUa_NodeId NamespaceIdentifierId = ID_OPCNodeId(OpcUaId_NamespacesType_NamespaceIdentifier_Placeholder);
    ID_ObjectNode * NamespaceIdentifierChild = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, NamespaceMetadataTypeNode, &NamespaceIdentifierId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(NamespaceIdentifierChild, 0, "<NamespaceIdentifier>");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(NamespaceIdentifierChild, "<NamespaceIdentifier>");
    ID_AddressSpace_AddUniReference(Handle, &NamespaceIdentifierId, ID_NodeType_Object, &ID_ModellingRuleOptionalPlaceholderNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &NamespacesTypeId, ID_NodeType_ObjectType, &NamespaceIdentifierId, ID_NodeType_Object, &ID_HasComponentNode);
    
    // SessionDiagnosticsObjectType + dependencies
    ID_ObjectTypeNode * SessionDiagnosticsObjectTypeNode;
    OpcUa_NodeId SessionDiagnosticsObjectType = ID_OPCNodeId(OpcUaId_SessionDiagnosticsObjectType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &SessionDiagnosticsObjectTypeNode, 0, "SessionDiagnosticsObjectType", OpcUa_False, SessionDiagnosticsObjectType, 0, NULL);
    
    OpcUa_NodeId SessionDiagnosticsVariableType = ID_OPCNodeId(OpcUaId_SessionDiagnosticsVariableType);
    OpcUa_NodeId SessionDiagnosticsChildId = ID_OPCNodeId(OpcUaId_SessionDiagnosticsObjectType_SessionDiagnostics);
    ID_AddressSpaceHelper_CreateVariable(Handle, Indexer, &SessionDiagnosticsObjectType, ID_NodeType_ObjectType, &SessionDiagnosticsChildId, 0, "SessionDiagnostics", &SessionDiagnosticsVariableType, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId SessionSecurityDiagnosticsVariableType = ID_OPCNodeId(OpcUaId_SessionSecurityDiagnosticsType);
    OpcUa_NodeId SessionSecurityDiagnosticsChildId = ID_OPCNodeId(OpcUaId_SessionDiagnosticsObjectType_SessionSecurityDiagnostics);
    ID_AddressSpaceHelper_CreateVariable(Handle, Indexer, &SessionDiagnosticsObjectType, ID_NodeType_ObjectType, &SessionSecurityDiagnosticsChildId, 0, "SessionSecurityDiagnostics", &SessionSecurityDiagnosticsVariableType, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId SubscriptionDiagnosticsArray = ID_OPCNodeId(OpcUaId_SubscriptionDiagnosticsArrayType);
    OpcUa_NodeId SubscriptionDiagnosticsArrayChildId = ID_OPCNodeId(OpcUaId_SessionDiagnosticsObjectType_SubscriptionDiagnosticsArray);
    ID_AddressSpaceHelper_CreateVariable(Handle, Indexer, &SessionDiagnosticsObjectType, ID_NodeType_ObjectType, &SubscriptionDiagnosticsArrayChildId, 0, "SubscriptionDiagnosticsArray", &SubscriptionDiagnosticsArray, &ID_ModellingRuleMandatoryNode);
    
    // SessionDiagnosticsSummaryType + dependencies
    ID_ObjectTypeNode * SessionsDiagnosticsSummaryTypeNode;
    OpcUa_NodeId SessionsDiagnosticsSummaryType = ID_OPCNodeId(OpcUaId_SessionsDiagnosticsSummaryType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &SessionsDiagnosticsSummaryTypeNode, 0, "SessionsDiagnosticsSummaryType", OpcUa_False, SessionsDiagnosticsSummaryType, 0, NULL);
    
    OpcUa_NodeId SessionDiagnosticsArray = ID_OPCNodeId(OpcUaId_SessionDiagnosticsArrayType);
    OpcUa_NodeId SessionDiagnosticsArrayId = ID_OPCNodeId(OpcUaId_SessionsDiagnosticsSummaryType_SessionDiagnosticsArray);
    ID_AddressSpaceHelper_CreateVariable(Handle, Indexer, &SessionsDiagnosticsSummaryType, ID_NodeType_ObjectType, &SessionDiagnosticsArrayId, 0, "SessionDiagnosticsArray", &SessionDiagnosticsArray, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId SessionSecurityDiagnosticsArray = ID_OPCNodeId(OpcUaId_SessionSecurityDiagnosticsArrayType);
    OpcUa_NodeId SessionSecurityDiagnosticsArrayId = ID_OPCNodeId(OpcUaId_SessionsDiagnosticsSummaryType_SessionSecurityDiagnosticsArray);
    ID_AddressSpaceHelper_CreateVariable(Handle, Indexer, &SessionsDiagnosticsSummaryType, ID_NodeType_ObjectType, &SessionSecurityDiagnosticsArrayId, 0, "SessionSecurityDiagnosticsArray", &SessionSecurityDiagnosticsArray, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId SessionDiagnosticsObjectId = ID_OPCNodeId(OpcUaId_SessionsDiagnosticsSummaryType_ClientName_Placeholder);
    ID_ObjectNode * SessionDiagnosticsChildObject = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, SessionDiagnosticsObjectTypeNode, &SessionDiagnosticsObjectId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(SessionDiagnosticsChildObject, 0, "<ClientName>");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(SessionDiagnosticsChildObject, "<ClientName>");
    ID_AddressSpace_AddUniReference(Handle, &SessionDiagnosticsObjectId, ID_NodeType_Object, &ID_ModellingRuleOptionalPlaceholderNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &SessionsDiagnosticsSummaryType, ID_NodeType_ObjectType, &SessionDiagnosticsObjectId, ID_NodeType_Object, &ID_HasComponentNode);
    
    // ServerDiagnosticsType + dependencies
    ID_ObjectTypeNode * ServerDiagnosticsTypeNode;
    OpcUa_NodeId ServerDiagnosticsType = ID_OPCNodeId(OpcUaId_ServerDiagnosticsType);
    ID_PropertyAddition PropertyStructure[1] = {{0, "EnabledFlag",OpcUaId_ServerDiagnosticsType_EnabledFlag,0,0,OpcUaId_Boolean,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Boolean,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &ServerDiagnosticsTypeNode, 0, "ServerDiagnosticsType", OpcUa_False, ServerDiagnosticsType, 1, PropertyStructure);
    
    OpcUa_NodeId ServerDiagnosticsSummary = ID_OPCNodeId(OpcUaId_ServerDiagnosticsSummaryType);
    OpcUa_NodeId ServerDiagnosticsSummaryId = ID_OPCNodeId(OpcUaId_ServerDiagnosticsType_ServerDiagnosticsSummary);
    ID_AddressSpaceHelper_CreateVariable(Handle, Indexer, &ServerDiagnosticsType, ID_NodeType_ObjectType, &ServerDiagnosticsSummaryId, 0, "ServerDiagnosticsSummary", &ServerDiagnosticsSummary, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId SamplingIntervalDiagnosticsArray = ID_OPCNodeId(OpcUaId_SamplingIntervalDiagnosticsArrayType);
    OpcUa_NodeId SamplingIntervalDiagnosticsArrayId = ID_OPCNodeId(OpcUaId_ServerDiagnosticsType_SamplingIntervalDiagnosticsArray);
    ID_AddressSpaceHelper_CreateVariable(Handle, Indexer, &ServerDiagnosticsType, ID_NodeType_ObjectType, &SamplingIntervalDiagnosticsArrayId, 0, "SamplingIntervalDiagnosticsArray", &SamplingIntervalDiagnosticsArray, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId SubscriptionDiagnosticsArrayId = ID_OPCNodeId(OpcUaId_ServerDiagnosticsType_SubscriptionDiagnosticsArray);
    ID_AddressSpaceHelper_CreateVariable(Handle, Indexer, &ServerDiagnosticsType, ID_NodeType_ObjectType, &SubscriptionDiagnosticsArrayId, 0, "SubscriptionDiagnosticsArray", &SubscriptionDiagnosticsArray, &ID_ModellingRuleMandatoryNode);
    
    OpcUa_NodeId SessionsDiagnosticsSummaryId = ID_OPCNodeId(OpcUaId_ServerDiagnosticsType_SessionsDiagnosticsSummary);
    ID_ObjectNode * SessionsDiagnosticsSummary = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, SessionsDiagnosticsSummaryTypeNode, &SessionsDiagnosticsSummaryId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(SessionsDiagnosticsSummary, 0, "SessionsDiagnosticsSummary");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(SessionsDiagnosticsSummary, "SessionsDiagnosticsSummary");
    ID_AddressSpace_AddUniReference(Handle, &SessionsDiagnosticsSummaryId, ID_NodeType_Object, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerDiagnosticsType, ID_NodeType_ObjectType, &SessionsDiagnosticsSummaryId, ID_NodeType_Object, &ID_HasComponentNode);
    
    // ServerCapabilities + dependencies
    ID_ObjectTypeNode * ServerCapabilitiesTypeNode;
    OpcUa_NodeId ServerCapabilitiesType = ID_OPCNodeId(OpcUaId_ServerCapabilitiesType);
    OpcUa_ExpandedNodeId SignedSoftwareCertificateId = ID_OPCExpandedNodeId(OpcUaId_SignedSoftwareCertificate_Encoding_DefaultBinary);
    ID_PropertyAddition ServerCapabilitiesChild[10] = {{0, "ServerProfileArray",OpcUaId_ServerCapabilitiesType_ServerProfileArray,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                       {0, "LocaleIdArray",OpcUaId_ServerCapabilitiesType_LocaleIdArray,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_LocaleId,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                       {0, "MinSupportedSampleRate",OpcUaId_ServerCapabilitiesType_MinSupportedSampleRate,0,0,OpcUaId_Duration,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Double,0,NULL,NULL},
                                                       {0, "MaxBrowseContinuationPoints",OpcUaId_ServerCapabilitiesType_MaxBrowseContinuationPoints,0,0,OpcUaId_UInt16,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt16,0,NULL,NULL},
                                                       {0, "MaxQueryContinuationPoints",OpcUaId_ServerCapabilitiesType_MaxQueryContinuationPoints,0,0,OpcUaId_UInt16,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt16,0,NULL,NULL},
                                                       {0, "MaxHistoryContinuationPoints",OpcUaId_ServerCapabilitiesType_MaxHistoryContinuationPoints,0,0,OpcUaId_UInt16,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt16,0,NULL,NULL},
                                                       {0, "SoftwareCertificates",OpcUaId_ServerCapabilitiesType_SoftwareCertificates,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_SignedSoftwareCertificate,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&SignedSoftwareCertificateId,&OpcUa_SignedSoftwareCertificate_EncodeableType},
                                                       {0, "MaxArrayLength",OpcUaId_ServerCapabilitiesType_MaxArrayLength,0,0,OpcUaId_UInt16,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt16,0,NULL,NULL},
                                                       {0, "MaxStringLength",OpcUaId_ServerCapabilitiesType_MaxStringLength,0,0,OpcUaId_UInt16,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt16,0,NULL,NULL},
                                                       {0, "MaxByteStringLength",OpcUaId_ServerCapabilitiesType_MaxByteStringLength,0,0,OpcUaId_UInt16,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt16,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &ServerCapabilitiesTypeNode, 0, "ServerCapabilitiesType", OpcUa_False, ServerCapabilitiesType, 10, ServerCapabilitiesChild);
    
    // override OperationLimits force mandatory
    OpcUa_NodeId OperationLimitsId = ID_OPCNodeId(OpcUaId_ServerCapabilitiesType_OperationLimits);
    ID_ObjectNode * OperationLimitsChild = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, OperationLimitsTypeNode, &OperationLimitsId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(OperationLimitsChild, 0, "OperationLimits");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(OperationLimitsChild, "OperationLimits");
    ID_AddressSpace_AddUniReference(Handle, &OperationLimitsId, ID_NodeType_Object, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerCapabilitiesType, ID_NodeType_ObjectType, &OperationLimitsId, ID_NodeType_Object, &ID_HasComponentNode);
    
    OpcUa_NodeId ModellingRules = ID_OPCNodeId(OpcUaId_ServerCapabilitiesType_ModellingRules);
    ID_ObjectNode * ModellingRulesChild = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &ModellingRules);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ModellingRulesChild, 0, "ModellingRules");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ModellingRulesChild, "ModellingRules");
    ID_AddressSpace_AddUniReference(Handle, &ModellingRules, ID_NodeType_Object, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerCapabilitiesType, ID_NodeType_ObjectType, &ModellingRules, ID_NodeType_Object, &ID_HasComponentNode);
    
    OpcUa_NodeId AggregateFunctions = ID_OPCNodeId(OpcUaId_ServerCapabilitiesType_AggregateFunctions);
    ID_ObjectNode * AggregateFunctionsChild = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &AggregateFunctions);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(AggregateFunctionsChild, 0, "AggregateFunctions");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(AggregateFunctionsChild, "AggregateFunctions");
    ID_AddressSpace_AddUniReference(Handle, &AggregateFunctions, ID_NodeType_Object, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerCapabilitiesType, ID_NodeType_ObjectType, &AggregateFunctions, ID_NodeType_Object, &ID_HasComponentNode);
    
    // ServerType + dependencies
    ID_ObjectTypeNode * ServerTypeNode;
    OpcUa_NodeId ServerType = ID_OPCNodeId(OpcUaId_ServerType);
    ID_PropertyAddition ServerChildStructure[5] = {{0, "ServerArray",OpcUaId_ServerType_ServerArray,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                   {0, "NamespaceArray",OpcUaId_ServerType_NamespaceArray,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                   {0, "ServiceLevel",OpcUaId_ServerType_ServiceLevel,0,0,OpcUaId_Byte,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Byte,0,NULL,NULL},
                                                   {0, "Auditing",OpcUaId_ServerType_Auditing,0,0,OpcUaId_Boolean,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Boolean,0,NULL,NULL},
                                                   {0, "EstimatedReturnTime",OpcUaId_ServerType_EstimatedReturnTime,0,0,OpcUaId_DateTime,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_DateTime,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &ServerTypeNode, 0, "ServerType", OpcUa_False, ServerType, 5, ServerChildStructure);
    
    OpcUa_NodeId ServerStatusNodeId = ID_OPCNodeId(OpcUaId_ServerStatusType);
    ID_VariableTypeNode * ServerStatusTypeNode = ID_AddressSpace_GetVariableTypeNode(Handle, &ServerStatusNodeId);
    OpcUa_NodeId ServerStatusChild = ID_OPCNodeId(OpcUaId_ServerType_ServerStatus);
    ID_VariableNode * ServerStatusNode = ID_AddressSpace_CreateVariableFromVariableType(Handle, Indexer, ServerStatusTypeNode, &ServerStatusChild);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ServerStatusNode, 0, "ServerStatus");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ServerStatusNode, "ServerStatus");
    ID_AddressSpace_AddUniReference(Handle, &ServerStatusChild, ID_NodeType_Variable, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &ServerStatusChild, ID_NodeType_Variable, &ID_HasComponentNode);
    
    OpcUa_NodeId ServerCapabilitiesChildId = ID_OPCNodeId(OpcUaId_ServerType_ServerCapabilities);
    ID_ObjectNode * ServerCapabilitiesNode = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, ServerCapabilitiesTypeNode, &ServerCapabilitiesChildId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ServerCapabilitiesNode, 0, "ServerCapabilities");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ServerCapabilitiesNode, "ServerCapabilities");
    ID_AddressSpace_AddUniReference(Handle, &ServerCapabilitiesChildId, ID_NodeType_Object, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &ServerCapabilitiesChildId, ID_NodeType_Object, &ID_HasComponentNode);
    
    OpcUa_NodeId ServerDiagnosticsChildId = ID_OPCNodeId(OpcUaId_ServerType_ServerDiagnostics);
    ID_ObjectNode * ServerDiagnosticsNode = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, ServerDiagnosticsTypeNode, &ServerDiagnosticsChildId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ServerDiagnosticsNode, 0, "ServerDiagnostics");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ServerDiagnosticsNode, "ServerDiagnostics");
    ID_AddressSpace_AddUniReference(Handle, &ServerDiagnosticsChildId, ID_NodeType_Object, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &ServerDiagnosticsChildId, ID_NodeType_Object, &ID_HasComponentNode);
    
    OpcUa_NodeId VendorServerInfoChildId = ID_OPCNodeId(OpcUaId_ServerType_VendorServerInfo);
    ID_ObjectNode * VendorServerInfoChild = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, VendorServerInfoNode, &VendorServerInfoChildId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(VendorServerInfoChild, 0, "VendorServerInfo");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(VendorServerInfoChild, "VendorServerInfo");
    ID_AddressSpace_AddUniReference(Handle, &VendorServerInfoChildId, ID_NodeType_Object, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &VendorServerInfoChildId, ID_NodeType_Object, &ID_HasComponentNode);
    
    OpcUa_NodeId ServerRedundancyChildId = ID_OPCNodeId(OpcUaId_ServerType_ServerRedundancy);
    ID_ObjectNode * ServerRedundancyNode = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, ServerRedundancyTypeNode, &ServerRedundancyChildId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ServerRedundancyNode, 0, "ServerRedundancy");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ServerRedundancyNode, "ServerRedundancy");
    ID_AddressSpace_AddUniReference(Handle, &ServerRedundancyChildId, ID_NodeType_Object, &ID_ModellingRuleMandatoryNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &ServerRedundancyChildId, ID_NodeType_Object, &ID_HasComponentNode);
    
    OpcUa_NodeId GetMonitoredItemsId = ID_OPCNodeId(OpcUaId_ServerType_GetMonitoredItems);
    OpcUa_NodeId GetMonitoredItemsInputId = ID_OPCNodeId(OpcUaId_ServerType_GetMonitoredItems_InputArguments);
    OpcUa_NodeId GetMonitoredItemsOutputId = ID_OPCNodeId(OpcUaId_ServerType_GetMonitoredItems_OutputArguments);
    OpcUa_NodeId GetMonitoredItemsTypeId = ID_OPCNodeId(ID_OpcUaId_GetMonitoredItemsMethodType);
    ID_MethodNode * GetMonitoredItems = ID_AddressSpace_CloneMethod(Handle, Indexer, &GetMonitoredItemsTypeId, &GetMonitoredItemsId, &GetMonitoredItemsInputId, &GetMonitoredItemsOutputId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(GetMonitoredItems, 0, "GetMonitoredItems");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(GetMonitoredItems, "GetMonitoredItems");
    ID_AddressSpace_AddUniReference(Handle, &GetMonitoredItemsId, ID_NodeType_Method, &ID_ModellingRuleOptionalNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &GetMonitoredItemsId, ID_NodeType_Method, &ID_HasComponentNode);
    
    OpcUa_NodeId ResendDataId = ID_OPCNodeId(OpcUaId_ServerType_ResendData);
    OpcUa_NodeId ResendDataInputId = ID_OPCNodeId(OpcUaId_ServerType_ResendData_InputArguments);
    OpcUa_NodeId ResendDataTypeId = ID_OPCNodeId(ID_OpcUaId_ResendDataMethodType);
    ID_MethodNode * ResendData = ID_AddressSpace_CloneMethod(Handle, Indexer, &ResendDataTypeId, &ResendDataId, &ResendDataInputId, NULL);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ResendData, 0, "ResendData");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ResendData, "ResendData");
    ID_AddressSpace_AddUniReference(Handle, &ResendDataId, ID_NodeType_Method, &ID_ModellingRuleOptionalNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &ResendDataId, ID_NodeType_Method, &ID_HasComponentNode);
    
    OpcUa_NodeId SetSubscriptionDurableId = ID_OPCNodeId(OpcUaId_ServerType_SetSubscriptionDurable);
    OpcUa_NodeId SetSubscriptionDurableInputId = ID_OPCNodeId(OpcUaId_ServerType_SetSubscriptionDurable_InputArguments);
    OpcUa_NodeId SetSubscriptionDurableOutputId = ID_OPCNodeId(OpcUaId_ServerType_SetSubscriptionDurable_OutputArguments);
    OpcUa_NodeId SetSubscriptionDurableTypeId = ID_OPCNodeId(ID_OpcUaId_SetSubscriptionDurableMethodType);
    ID_MethodNode * SetSubscriptionDurable = ID_AddressSpace_CloneMethod(Handle, Indexer, &SetSubscriptionDurableTypeId, &SetSubscriptionDurableId, &SetSubscriptionDurableInputId, &SetSubscriptionDurableOutputId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(SetSubscriptionDurable, 0, "SetSubscriptionDurable");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(SetSubscriptionDurable, "SetSubscriptionDurable");
    ID_AddressSpace_AddUniReference(Handle, &SetSubscriptionDurableId, ID_NodeType_Method, &ID_ModellingRuleOptionalNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &SetSubscriptionDurableId, ID_NodeType_Method, &ID_HasComponentNode);
    
    OpcUa_NodeId RequestServerStateChangeId = ID_OPCNodeId(OpcUaId_ServerType_RequestServerStateChange);
    OpcUa_NodeId RequestServerStateChangeInputId = ID_OPCNodeId(OpcUaId_ServerType_RequestServerStateChange_InputArguments);
    OpcUa_NodeId RequestServerStateChangeTypeId = ID_OPCNodeId(ID_OpcUaId_RequestServerStateChangeMethodType);
    ID_MethodNode * RequestServerStateChange = ID_AddressSpace_CloneMethod(Handle, Indexer, &RequestServerStateChangeTypeId, &RequestServerStateChangeId, &RequestServerStateChangeInputId, NULL);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(RequestServerStateChange, 0, "RequestServerStateChange");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(RequestServerStateChange, "RequestServerStateChange");
    ID_AddressSpace_AddUniReference(Handle, &RequestServerStateChangeId, ID_NodeType_Method, &ID_ModellingRuleOptionalNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &RequestServerStateChangeId, ID_NodeType_Method, &ID_HasComponentNode);
    
    OpcUa_NodeId NamespacesId = ID_OPCNodeId(OpcUaId_ServerType_Namespaces);
    ID_ObjectNode * NamespacesChild = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, NamespacesTypeNode, &NamespacesId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(NamespacesChild, 0, "Namespaces");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(NamespacesChild, "Namespaces");
    ID_AddressSpace_AddUniReference(Handle, &NamespacesId, ID_NodeType_Method, &ID_ModellingRuleOptionalNode, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, &ServerType, ID_NodeType_ObjectType, &NamespacesId, ID_NodeType_Object, &ID_HasComponentNode);
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4221 4204)
void ID_Initialize_StandardEventTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;
    OpcUa_NodeId ID_ModellingRuleOptionalNode = ID_ModellingRuleOptional;

    OpcUa_NodeId BaseObjectType = ID_OPCNodeId(OpcUaId_BaseObjectType);
    ID_ObjectTypeNode * BaseObjectTypeNode = ID_AddressSpace_GetObjectTypeNode(Handle, &BaseObjectType);
    
    ID_ObjectTypeNode * BaseEventTypeNode;
    OpcUa_NodeId BaseEventType = ID_OPCNodeId(OpcUaId_BaseEventType);
    OpcUa_ExpandedNodeId TimeZoneDataType = ID_OPCExpandedNodeId(OpcUaId_TimeZoneDataType_Encoding_DefaultBinary);
    ID_PropertyAddition BaseEventTypeAddition[9] = {{0, "EventId",OpcUaId_BaseEventType_EventId,0,0,OpcUaId_ByteString,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ByteString,0,NULL,NULL},
                                                    {0, "EventType",OpcUaId_BaseEventType_EventType,0,0,OpcUaId_NodeId,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_NodeId,0,NULL,NULL},
                                                    {0, "SourceNode",OpcUaId_BaseEventType_SourceNode,0,0,OpcUaId_NodeId,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_NodeId,0,NULL,NULL},
                                                    {0, "SourceName",OpcUaId_BaseEventType_SourceName,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                    {0, "Time",OpcUaId_BaseEventType_Time,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_DateTime,0,NULL,NULL},
                                                    {0, "ReceiveTime",OpcUaId_BaseEventType_ReceiveTime,0,0,OpcUaId_UtcTime,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_DateTime,0,NULL,NULL},
                                                    {0, "LocalTime",OpcUaId_BaseEventType_LocalTime,0,0,OpcUaId_TimeZoneDataType,OpcUa_True,&ID_ModellingRuleOptionalNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&TimeZoneDataType,&OpcUa_TimeZoneDataType_EncodeableType},
                                                    {0, "Message",OpcUaId_BaseEventType_Message,0,0,OpcUaId_LocalizedText,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ByteString,0,NULL,NULL},
                                                    {0, "Severity",OpcUaId_BaseEventType_Severity,0,0,OpcUaId_UInt16,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ByteString,0,NULL,NULL},};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &BaseEventTypeNode, 0, "BaseEventType", OpcUa_True, BaseEventType, 9, BaseEventTypeAddition);
    
    ID_ObjectTypeNode * AuditEventTypeNode;
    OpcUa_NodeId AuditEventType = ID_OPCNodeId(OpcUaId_AuditEventType);
    ID_PropertyAddition AuditEventTypeAddition[5] = {{0, "ActionTimeStamp",OpcUaId_AuditEventType_ActionTimeStamp,0,0,OpcUaId_UtcTime,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_DateTime,0,NULL,NULL},
                                                     {0, "Status",OpcUaId_AuditEventType_Status,0,0,OpcUaId_Boolean,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Boolean,0,NULL,NULL},
                                                     {0, "ServerId",OpcUaId_AuditEventType_ServerId,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                     {0, "ClientAuditEntryId",OpcUaId_AuditEventType_ClientAuditEntryId,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                                     {0, "ClientUserId",OpcUaId_AuditEventType_ClientUserId,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseEventTypeNode, &AuditEventTypeNode, 0, "AuditEventType", OpcUa_True, AuditEventType, 5, AuditEventTypeAddition);
    
    ID_ObjectTypeNode * AuditSecurityEventTypeNode;
    OpcUa_NodeId AuditSecurityEventType = ID_OPCNodeId(OpcUaId_AuditSecurityEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditEventTypeNode, &AuditSecurityEventTypeNode, 0, "AuditSecurityEventType", OpcUa_True, AuditSecurityEventType, 0, NULL);
    
    ID_ObjectTypeNode * AuditChannelEventTypeNode;
    OpcUa_NodeId AuditChannelEventType = ID_OPCNodeId(OpcUaId_AuditChannelEventType);
    ID_PropertyAddition AuditChannelEventTypeAddition[1] = {{0, "SecureChannelId",OpcUaId_AuditChannelEventType_SecureChannelId,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditSecurityEventTypeNode, &AuditChannelEventTypeNode, 0, "AuditChannelEventType", OpcUa_True, AuditChannelEventType, 1, AuditChannelEventTypeAddition);
    
    OpcUa_NodeId AuditOpenSecureChannelEventType = ID_OPCNodeId(OpcUaId_AuditOpenSecureChannelEventType);
    ID_PropertyAddition AuditOSCAddition[6] = {{0, "ClientCertificate",OpcUaId_AuditOpenSecureChannelEventType_ClientCertificate,0,0,OpcUaId_ByteString,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ByteString,0,NULL,NULL},
                                               {0, "ClientCertificateThumbprint",OpcUaId_AuditOpenSecureChannelEventType_ClientCertificateThumbprint,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                               {0, "RequestType",OpcUaId_AuditOpenSecureChannelEventType_RequestType,0,0,OpcUaId_SecurityTokenRequestType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Int32,0,NULL,NULL},
                                               {0, "SecurityPolicyUri",OpcUaId_AuditOpenSecureChannelEventType_SecurityPolicyUri,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                               {0, "SecurityMode",OpcUaId_AuditOpenSecureChannelEventType_SecurityMode,0,0,OpcUaId_MessageSecurityMode,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Int32,0,NULL,NULL},
                                               {0, "RequestedLifetime",OpcUaId_AuditOpenSecureChannelEventType_RequestedLifetime,0,0,OpcUaId_Duration,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Double,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditChannelEventTypeNode, NULL, 0, "AuditOpenSecureChannelEventType", OpcUa_True, AuditOpenSecureChannelEventType, 6, AuditOSCAddition);
    
    ID_ObjectTypeNode * AuditSessionEventTypeNode = NULL;
    OpcUa_NodeId AuditSessionEventType = ID_OPCNodeId(OpcUaId_AuditSessionEventType);
    ID_PropertyAddition AuditSETAddition[1] = {{0, "SessionId",OpcUaId_AuditSessionEventType_SessionId,0,0,OpcUaId_NodeId,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_NodeId,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditSecurityEventTypeNode, &AuditSessionEventTypeNode, 0, "AuditSessionEventType", OpcUa_True, AuditSessionEventType, 1, AuditSETAddition);
    
    ID_ObjectTypeNode * AuditCreateSessionEventTypeNode = NULL;
    OpcUa_NodeId AuditCreateSessionEventType = ID_OPCNodeId(OpcUaId_AuditCreateSessionEventType);
    ID_PropertyAddition ACSAddition[4] = {{0, "SecureChannelId",OpcUaId_AuditCreateSessionEventType_SecureChannelId,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                          {0, "ClientCertificate",OpcUaId_AuditCreateSessionEventType_ClientCertificate,0,0,OpcUaId_ByteString,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ByteString,0,NULL,NULL},
                                          {0, "ClientCertificateThumbprint",OpcUaId_AuditCreateSessionEventType_ClientCertificateThumbprint,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                          {0, "RevisedSessionTimeout",OpcUaId_AuditCreateSessionEventType_RevisedSessionTimeout,0,0,OpcUaId_Double,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Double,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditSessionEventTypeNode, &AuditCreateSessionEventTypeNode, 0, "AuditCreateSessionEventType", OpcUa_True, AuditCreateSessionEventType, 4, ACSAddition);
    
    OpcUa_NodeId AuditUrlMismatchEventType = ID_OPCNodeId(OpcUaId_AuditUrlMismatchEventType);
    ID_PropertyAddition AuditUMEAddition[1] = {{0, "EndpointUrl",OpcUaId_AuditUrlMismatchEventType_EndpointUrl,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditCreateSessionEventTypeNode, NULL, 0, "AuditUrlMismatchEventType", OpcUa_True, AuditUrlMismatchEventType, 1, AuditUMEAddition);
    
    OpcUa_NodeId AuditActivateSessionEventType = ID_OPCNodeId(OpcUaId_AuditActivateSessionEventType);
    OpcUa_ExpandedNodeId AASClientSoftwareCertificatesId = ID_OPCExpandedNodeId(OpcUaId_SignedSoftwareCertificate_Encoding_DefaultBinary);
    OpcUa_ExpandedNodeId AASUserIentityTokenId = ID_OPCExpandedNodeId(OpcUaId_UserIdentityToken_Encoding_DefaultBinary);
    ID_PropertyAddition AASAddition[3] = {{0, "ClientSoftwareCertificates",OpcUaId_AuditActivateSessionEventType_ClientSoftwareCertificates,0,0,OpcUaId_SignedSoftwareCertificate,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&AASClientSoftwareCertificatesId,&OpcUa_SignedSoftwareCertificate_EncodeableType},
                                          {0, "UserIdentityToken",OpcUaId_AuditActivateSessionEventType_UserIdentityToken,0,0,OpcUaId_UserIdentityToken,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&AASUserIentityTokenId,&OpcUa_UserIdentityToken_EncodeableType},
                                          {0, "SecureChannelId",OpcUaId_AuditActivateSessionEventType_SecureChannelId,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditSessionEventTypeNode, NULL, 0, "AuditActivateSessionEventType", OpcUa_True, AuditActivateSessionEventType, 3, AASAddition);
    
    OpcUa_NodeId AuditCancelEventType = ID_OPCNodeId(OpcUaId_AuditCancelEventType);
    ID_PropertyAddition AuditCETAddition[1] = {{0, "RequestHandle",OpcUaId_AuditCancelEventType_RequestHandle,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditSessionEventTypeNode, NULL, 0, "AuditCancelEventType", OpcUa_True, AuditCancelEventType, 1, AuditCETAddition);
    
    ID_ObjectTypeNode * AuditCertificateEventTypeNode;
    OpcUa_NodeId AuditCertificateEventType = ID_OPCNodeId(OpcUaId_AuditCertificateEventType);
    ID_PropertyAddition AuditCETypeAddition[1] = {{0, "Certificate",OpcUaId_AuditCertificateEventType_Certificate,0,0,OpcUaId_ByteString,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ByteString,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditSecurityEventTypeNode, &AuditCertificateEventTypeNode, 0, "AuditCertificateEventType", OpcUa_True, AuditCertificateEventType, 1, AuditCETypeAddition);
    
    OpcUa_NodeId AuditCertificateDataMismatchEventType = ID_OPCNodeId(OpcUaId_AuditCertificateDataMismatchEventType);
    ID_PropertyAddition ACDMAddition[2] = {{0, "InvalidHostname",OpcUaId_AuditCertificateDataMismatchEventType_InvalidHostname,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                           {0, "InvalidUri",OpcUaId_AuditCertificateDataMismatchEventType_InvalidUri,0,0,OpcUaId_String,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditCertificateEventTypeNode, NULL, 0, "AuditCertificateDataMismatchEventType", OpcUa_True, AuditCertificateDataMismatchEventType, 2, ACDMAddition);
    
    OpcUa_NodeId AuditCertificateExpiredEventType = ID_OPCNodeId(OpcUaId_AuditCertificateExpiredEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditCertificateEventTypeNode, NULL, 0, "AuditCertificateExpiredEventType", OpcUa_True, AuditCertificateExpiredEventType, 0, NULL);
    
    OpcUa_NodeId AuditCertificateInvalidEventType = ID_OPCNodeId(OpcUaId_AuditCertificateInvalidEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditCertificateEventTypeNode, NULL, 0, "AuditCertificateInvalidEventType", OpcUa_True, AuditCertificateInvalidEventType, 0, NULL);
    
    OpcUa_NodeId AuditCertificateUntrustedEventType = ID_OPCNodeId(OpcUaId_AuditCertificateUntrustedEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditCertificateEventTypeNode, NULL, 0, "AuditCertificateUntrustedEventType", OpcUa_True, AuditCertificateUntrustedEventType, 0, NULL);
    
    OpcUa_NodeId AuditCertificateRevokedEventType = ID_OPCNodeId(OpcUaId_AuditCertificateRevokedEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditCertificateEventTypeNode, NULL, 0, "AuditCertificateRevokedEventType", OpcUa_True, AuditCertificateRevokedEventType, 0, NULL);
    
    OpcUa_NodeId AuditCertificateMismatchEventType = ID_OPCNodeId(OpcUaId_AuditCertificateMismatchEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditCertificateEventTypeNode, NULL, 0, "AuditCertificateMismatchEventType", OpcUa_True, AuditCertificateMismatchEventType, 0, NULL);
    
    ID_ObjectTypeNode * AuditNodeManagementEventTypeNode;
    OpcUa_NodeId AuditNodeManagementEventType = ID_OPCNodeId(OpcUaId_AuditNodeManagementEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditEventTypeNode, &AuditNodeManagementEventTypeNode, 0, "AuditNodeManagementEventType", OpcUa_True, AuditNodeManagementEventType, 0, NULL);
    
    OpcUa_NodeId AuditAddNodesEventType = ID_OPCNodeId(OpcUaId_AuditAddNodesEventType);
    OpcUa_ExpandedNodeId AuditANNode = ID_OPCExpandedNodeId(OpcUaId_AddNodesItem_Encoding_DefaultBinary);
    ID_PropertyAddition AuditANAddition[1] = {{0, "NodesToAdd",OpcUaId_AuditAddNodesEventType_NodesToAdd,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_AddNodesItem,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&AuditANNode,&OpcUa_AddNodesItem_EncodeableType}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditNodeManagementEventTypeNode, NULL, 0, "AuditAddNodesEventType", OpcUa_True, AuditAddNodesEventType, 1, AuditANAddition);
    
    OpcUa_NodeId AuditDeleteNodesEventType = ID_OPCNodeId(OpcUaId_AuditDeleteNodesEventType);
    OpcUa_ExpandedNodeId AuditDNNode = ID_OPCExpandedNodeId(OpcUaId_DeleteNodesItem_Encoding_DefaultBinary);
    ID_PropertyAddition AuditDNAddition[1] = {{0, "NodesToDelete",OpcUaId_AuditDeleteNodesEventType_NodesToDelete,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_DeleteNodesItem,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&AuditDNNode,&OpcUa_DeleteNodesItem_EncodeableType}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditNodeManagementEventTypeNode, NULL, 0, "AuditDeleteNodesEventType", OpcUa_True, AuditDeleteNodesEventType, 1, AuditDNAddition);
    
    OpcUa_NodeId AuditAddReferencesEventType = ID_OPCNodeId(OpcUaId_AuditAddReferencesEventType);
    OpcUa_ExpandedNodeId AuditARNode = ID_OPCExpandedNodeId(OpcUaId_AddReferencesItem_Encoding_DefaultBinary);
    ID_PropertyAddition AuditARAddition[1] = {{0, "ReferencesToAdd",OpcUaId_AuditAddReferencesEventType_ReferencesToAdd,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_AddReferencesItem,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&AuditARNode,&OpcUa_AddReferencesItem_EncodeableType}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditNodeManagementEventTypeNode, NULL, 0, "AuditAddReferencesEventType", OpcUa_True, AuditAddReferencesEventType, 1, AuditARAddition);
    
    OpcUa_NodeId AuditDeleteReferencesEventType = ID_OPCNodeId(OpcUaId_AuditDeleteReferencesEventType);
    OpcUa_ExpandedNodeId AuditDRNode = ID_OPCExpandedNodeId(OpcUaId_DeleteReferencesItem_Encoding_DefaultBinary);
    ID_PropertyAddition AuditDRAddition[1] = {{0, "ReferencesToDelete",OpcUaId_AuditDeleteReferencesEventType_ReferencesToDelete,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_DeleteReferencesItem,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&AuditDRNode,&OpcUa_DeleteReferencesItem_EncodeableType}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditNodeManagementEventTypeNode, NULL, 0, "AuditDeleteReferencesEventType", OpcUa_True, AuditDeleteReferencesEventType, 1, AuditDRAddition);
    
    ID_ObjectTypeNode * AuditUpdateEventTypeNode;
    OpcUa_NodeId AuditUpdateEventType = ID_OPCNodeId(OpcUaId_AuditUpdateEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditEventTypeNode, &AuditUpdateEventTypeNode, 0, "AuditUpdateEventType", OpcUa_True, AuditUpdateEventType, 0, NULL);
    
    OpcUa_NodeId AuditWriteUpdateEventType = ID_OPCNodeId(OpcUaId_AuditWriteUpdateEventType);
    ID_PropertyAddition AWUAddition[4] = {{0, "AttributeId",OpcUaId_AuditWriteUpdateEventType_AttributeId,0,0,OpcUaId_UInt32,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt32,0,NULL,NULL},
                                          {0, "IndexRange",OpcUaId_AuditWriteUpdateEventType_IndexRange,0,0,OpcUaId_NumericRange,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_String,0,NULL,NULL},
                                          {0, "NewValue",OpcUaId_AuditWriteUpdateEventType_NewValue,0,0,OpcUaId_BaseDataType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Variant,0,NULL,NULL},
                                          {0, "OldValue",OpcUaId_AuditWriteUpdateEventType_OldValue,0,0,OpcUaId_BaseDataType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Variant,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditUpdateEventTypeNode, NULL, 0, "AuditWriteUpdateEventType", OpcUa_True, AuditWriteUpdateEventType, 4, AWUAddition);
    
    OpcUa_NodeId AuditHistoryUpdateEventType = ID_OPCNodeId(OpcUaId_AuditHistoryUpdateEventType);
    ID_PropertyAddition AuditHUAddition[1] = {{0, "ParameterDataTypeId",OpcUaId_AuditHistoryUpdateEventType_ParameterDataTypeId,0,0,OpcUaId_NodeId,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_NodeId,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditUpdateEventTypeNode, NULL, 0, "AuditHistoryUpdateEventType", OpcUa_True, AuditHistoryUpdateEventType, 1, AuditHUAddition);
    
    OpcUa_NodeId AuditUpdateMethodEventType = ID_OPCNodeId(OpcUaId_AuditUpdateMethodEventType);
    ID_PropertyAddition AUMAddition[2] = {{0, "MethodId",OpcUaId_AuditUpdateMethodEventType_MethodId,0,0,OpcUaId_NodeId,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_NodeId,0,NULL,NULL},
                                          {0, "InputArguments",OpcUaId_AuditUpdateMethodEventType_InputArguments,0,ID_LENGTH_UNKNOWN_INDICATOR,OpcUaId_BaseDataType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Variant,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, AuditEventTypeNode, NULL, 0, "AuditUpdateMethodEventType", OpcUa_True, AuditUpdateMethodEventType, 2, AUMAddition);
    
    ID_ObjectTypeNode * SystemEventTypeNode;
    OpcUa_NodeId SystemEventType = ID_OPCNodeId(OpcUaId_SystemEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseEventTypeNode, &SystemEventTypeNode, 0, "SystemEventType", OpcUa_True, SystemEventType, 0, NULL);
    
    OpcUa_NodeId DeviceFailureEventType = ID_OPCNodeId(OpcUaId_DeviceFailureEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, SystemEventTypeNode, NULL, 0, "DeviceFailureEventType", OpcUa_True, DeviceFailureEventType, 0, NULL);
    
    OpcUa_NodeId SystemStatusChangeEventType = ID_OPCNodeId(OpcUaId_SystemStatusChangeEventType);
    ID_PropertyAddition SSCAddition[1] = {{0, "SystemState",OpcUaId_SystemStatusChangeEventType_SystemState,0,0,OpcUaId_ServerState,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Int32,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, SystemEventTypeNode, NULL, 0, "SystemStatusChangeEventType", OpcUa_True, SystemStatusChangeEventType, 1, SSCAddition);
    
    ID_ObjectTypeNode * BaseModelChangeEventTypeNode;
    OpcUa_NodeId BaseModelChangeEventType = ID_OPCNodeId(OpcUaId_BaseModelChangeEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseEventTypeNode, &BaseModelChangeEventTypeNode, 0, "BaseModelChangeEventType", OpcUa_True, BaseModelChangeEventType, 0, NULL);
    
    OpcUa_NodeId GeneralModelChangeEventType = ID_OPCNodeId(OpcUaId_GeneralModelChangeEventType);
    OpcUa_ExpandedNodeId GMCNode = ID_OPCExpandedNodeId(OpcUaId_ModelChangeStructureDataType_Encoding_DefaultBinary);
    ID_PropertyAddition GMCAddition[1] = {{0, "Changes",OpcUaId_GeneralModelChangeEventType_Changes,0,0,OpcUaId_ModelChangeStructureDataType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&GMCNode,&OpcUa_ModelChangeStructureDataType_EncodeableType}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseModelChangeEventTypeNode, NULL, 0, "GeneralModelChangeEventType", OpcUa_True, GeneralModelChangeEventType, 1, GMCAddition);
    
    OpcUa_NodeId SemanticChangeEventType = ID_OPCNodeId(OpcUaId_SemanticChangeEventType);
    OpcUa_ExpandedNodeId SCNode = ID_OPCExpandedNodeId(OpcUaId_SemanticChangeStructureDataType_Encoding_DefaultBinary);
    ID_PropertyAddition SCAddition[1] = {{0, "Changes",OpcUaId_SemanticChangeEventType_Changes,0,0,OpcUaId_SemanticChangeStructureDataType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_ExtensionObject,OpcUa_ExtensionObjectEncoding_EncodeableObject,&SCNode,&OpcUa_SemanticChangeStructureDataType_EncodeableType}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseEventTypeNode, NULL, 0, "SemanticChangeEventType", OpcUa_True, SemanticChangeEventType, 1, SCAddition);
    
    OpcUa_NodeId EventQueueOverflowEventType = ID_OPCNodeId(OpcUaId_EventQueueOverflowEventType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseEventTypeNode, NULL, 0, "EventQueueOverflowEventType", OpcUa_True, EventQueueOverflowEventType, 0, NULL);
    
    OpcUa_NodeId ProgressEventType = ID_OPCNodeId(OpcUaId_ProgressEventType);
    ID_PropertyAddition PEAddition[2] = {{0, "Context",OpcUaId_ProgressEventType_Context,0,0,OpcUaId_BaseDataType,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_Variant,0,NULL,NULL},
                                         {0, "Progress",OpcUaId_ProgressEventType_Progress,0,0,OpcUaId_UInt16,OpcUa_True,&ID_ModellingRuleMandatoryNode,NULL,OpcUaType_UInt16,0,NULL,NULL}};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseEventTypeNode, NULL, 0, "ProgressEventType", OpcUa_True, ProgressEventType, 2, PEAddition);
}
#pragma warning(pop)