/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "Initialize.h"
#include "../SessionManager/SecurityManager.h"

extern ID_SecurityHandle GlobalSecurityHandle;

void ID_Initialize_AllStandardObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    ID_Initialize_ModellingRuleObjects(Handle, Indexer);
    ID_Initialize_ServerObjects(Handle, Indexer);
    ID_Initialize_OrganizationalObjects(Handle, Indexer);
}

void ID_Initialize_OrganizationalObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_NodeId ID_OrganizesNode = ID_Organizes;
    OpcUa_NodeId FolderType = ID_OPCNodeId(OpcUaId_FolderType);
    ID_ObjectTypeNode * FolderTypeNode = ID_AddressSpace_GetObjectTypeNode(Handle, &FolderType);
    
    OpcUa_NodeId RootId = ID_OPCNodeId(OpcUaId_RootFolder);
    OpcUa_NodeId ViewsId = ID_OPCNodeId(OpcUaId_ViewsFolder);
    OpcUa_NodeId ObjectsId = ID_OPCNodeId(OpcUaId_ObjectsFolder);
    OpcUa_NodeId TypesId = ID_OPCNodeId(OpcUaId_TypesFolder);
    
    ID_ObjectNode * Root = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &RootId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Root, 0, "Root");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Root, "Root");
    
    ID_ObjectNode * Views = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &ViewsId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Views, 0, "Views");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Views, "Views");
    ID_AddressSpace_AddBiReference(Handle, &RootId, ID_NodeType_Object, &ViewsId, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_BuildReferencesOfTypeToNode(Handle, ID_NodeType_View, &ViewsId, &ID_OrganizesNode);
    
    ID_ObjectNode * Objects = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &ObjectsId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Objects, 0, "Objects");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Objects, "Objects");
    ID_AddressSpace_AddBiReference(Handle, &RootId, ID_NodeType_Object, &ObjectsId, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_BuildReferencesOfTypeToNode(Handle, ID_NodeType_Object, &ObjectsId, &ID_OrganizesNode);
    
    ID_ObjectNode * Types = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &TypesId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Types, 0, "Types");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Types, "Types");
    ID_AddressSpace_AddBiReference(Handle, &RootId, ID_NodeType_Object, &TypesId, ID_NodeType_Object, &ID_OrganizesNode);
    
    OpcUa_NodeId ObjectTypesId = ID_OPCNodeId(OpcUaId_ObjectTypesFolder);
    OpcUa_NodeId VariableTypesId = ID_OPCNodeId(OpcUaId_VariableTypesFolder);
    OpcUa_NodeId ReferenceTypesId = ID_OPCNodeId(OpcUaId_ReferenceTypesFolder);
    OpcUa_NodeId DataTypesId = ID_OPCNodeId(OpcUaId_DataTypesFolder);
    OpcUa_NodeId EventTypesId = ID_OPCNodeId(OpcUaId_EventTypesFolder);
    
    ID_ObjectNode * VariableTypes = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &VariableTypesId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(VariableTypes, 0, "VariableTypes");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(VariableTypes, "VariableTypes");
    ID_AddressSpace_AddBiReference(Handle, &TypesId, ID_NodeType_ObjectType, &VariableTypesId, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_BuildReferencesOfTypeToNode(Handle, ID_NodeType_VariableType, &VariableTypesId, &ID_OrganizesNode);
    
    ID_ObjectNode * ReferenceTypes = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &ReferenceTypesId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ReferenceTypes, 0, "ReferenceTypes");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ReferenceTypes, "ReferenceTypes");
    ID_AddressSpace_AddBiReference(Handle, &TypesId, ID_NodeType_ObjectType, &ReferenceTypesId, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_BuildReferencesOfTypeToNode(Handle, ID_NodeType_ReferenceType, &ReferenceTypesId, &ID_OrganizesNode);
    
    ID_ObjectNode * DataTypes = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &DataTypesId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(DataTypes, 0, "DataTypes");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(DataTypes, "DataTypes");
    ID_AddressSpace_AddBiReference(Handle, &TypesId, ID_NodeType_ObjectType, &DataTypesId, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_BuildReferencesOfTypeToNode(Handle, ID_NodeType_DataType, &DataTypesId, &ID_OrganizesNode);
    
    ID_ObjectNode * EventTypes = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &EventTypesId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(EventTypes, 0, "EventTypes");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(EventTypes, "EventTypes");
    ID_AddressSpace_AddBiReference(Handle, &TypesId, ID_NodeType_Object, &EventTypesId, ID_NodeType_Object, &ID_OrganizesNode);
    OpcUa_NodeId BaseEventTypeId = ID_OPCNodeId(OpcUaId_BaseEventType);
    ID_AddressSpace_AddBiReference(Handle, &EventTypesId, ID_NodeType_Object, &BaseEventTypeId, ID_NodeType_ObjectType, &ID_OrganizesNode);
    
    ID_ObjectNode * ObjectTypes = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, FolderTypeNode, &ObjectTypesId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ObjectTypes, 0, "ObjectTypes");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ObjectTypes, "ObjectTypes");
    ID_AddressSpace_AddBiReference(Handle, &TypesId, ID_NodeType_ObjectType, &ObjectTypesId, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_BuildReferencesOfTypeToNode(Handle, ID_NodeType_ObjectType, &ObjectTypesId, &ID_OrganizesNode);
}

void ID_SetupServerCapabilitiesObject(ID_AddressSpaceHandle Handle, ID_ObjectNode * Server) {
    OpcUa_NodeId ID_ModellingRuleOptionalNode = ID_ModellingRuleOptional;
    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;
    OpcUa_NodeId ID_ModellingRuleOptionalPlaceholderNode = ID_ModellingRuleOptionalPlaceholder;
    OpcUa_NodeId ID_ModellingRuleMandatoryPlaceholderNode = ID_ModellingRuleMandatoryPlaceholder;
    OpcUa_NodeId ID_ModellingRuleExposesItsArrayNode = ID_ModellingRuleExposesItsArray;
    OpcUa_NodeId ID_OrganizesNode = ID_Organizes;
    const int MAX_LIMIT = OpcUa_Int32_Max - 1024;

    OpcUa_QualifiedName SearchName;
    OpcUa_QualifiedName_Initialize(&SearchName);

    // setup servercapabilities object
    OpcUa_String_AttachCopy(&SearchName.Name, "ServerCapabilities");
    ID_ObjectNode * ServerCapabilities = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Server, SearchName);
    ID_MAP_NODE_TO_NEW_ID(ServerCapabilities, OpcUaId_Server_ServerCapabilities);

    // serverprofilearray
    OpcUa_String_AttachCopy(&SearchName.Name, "ServerProfileArray");
    ID_VariableNode * ServerProfileArray = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    ServerProfileArray->AccessLevel = 1;
    ServerProfileArray->UserAccessLevel = 1;
    ServerProfileArray->Value.Value.Array.Length = ID_NUM_SUPPORTED_PROFILES;
    ServerProfileArray->Value.Value.Array.Value.StringArray = malloc(sizeof(OpcUa_String) * ID_NUM_SUPPORTED_PROFILES);
    char * ServerProfiles[ID_NUM_SUPPORTED_PROFILES] = ID_SERVER_PROFILES;
    for (int i = 0; i < ID_NUM_SUPPORTED_PROFILES; i++) { 
        OpcUa_String_Initialize(&ServerProfileArray->Value.Value.Array.Value.StringArray[i]); 
        OpcUa_String_AttachCopy(&ServerProfileArray->Value.Value.Array.Value.StringArray[i], ServerProfiles[i]);
    }
    ID_MAP_NODE_TO_NEW_ID(ServerProfileArray, OpcUaId_Server_ServerCapabilities_ServerProfileArray);

    // localeidarray
    OpcUa_String_AttachCopy(&SearchName.Name, "LocaleIdArray");
    ID_VariableNode * LocaleIdArray = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    LocaleIdArray->AccessLevel = 1;
    LocaleIdArray->UserAccessLevel = 1;
    LocaleIdArray->Value.Value.Array.Length = 1;
    LocaleIdArray->Value.Value.Array.Value.StringArray = malloc(sizeof(OpcUa_String) * 1);
    for (int i = 0; i < 1; i++) { OpcUa_String_Initialize(&LocaleIdArray->Value.Value.Array.Value.StringArray[i]); }
    OpcUa_String_AttachCopy(&LocaleIdArray->Value.Value.Array.Value.StringArray[0], ID_APPLICATION_LOCALE);
    ID_MAP_NODE_TO_NEW_ID(LocaleIdArray, OpcUaId_Server_ServerCapabilities_LocaleIdArray);

    // minsupportedsamplerate
    OpcUa_String_AttachCopy(&SearchName.Name, "MinSupportedSampleRate");
    ID_VariableNode * MinSupportedSampleRate = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    MinSupportedSampleRate->AccessLevel = 1;
    MinSupportedSampleRate->UserAccessLevel = 1;
    MinSupportedSampleRate->Value.Value.Double = -1;
    ID_MAP_NODE_TO_NEW_ID(MinSupportedSampleRate, OpcUaId_Server_ServerCapabilities_MinSupportedSampleRate);

    // maxbrowsecontinuationpoints
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxBrowseContinuationPoints");
    ID_VariableNode * MaxBrowseContinuationPoints = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    MaxBrowseContinuationPoints->AccessLevel = 1;
    MaxBrowseContinuationPoints->UserAccessLevel = 1;
    MaxBrowseContinuationPoints->Value.Value.UInt16 = 0;
    ID_MAP_NODE_TO_NEW_ID(MaxBrowseContinuationPoints, OpcUaId_Server_ServerCapabilities_MaxBrowseContinuationPoints);

    // maxquerycontinuationpoints
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxQueryContinuationPoints");
    ID_VariableNode * MaxQueryContinuationPoints = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    MaxQueryContinuationPoints->AccessLevel = 3;
    MaxQueryContinuationPoints->UserAccessLevel = 3;
    MaxQueryContinuationPoints->Value.Value.UInt16 = 0;
    MaxQueryContinuationPoints->BaseAttribute.WriteMask = 65;
    ID_MAP_NODE_TO_NEW_ID(MaxQueryContinuationPoints, OpcUaId_Server_ServerCapabilities_MaxQueryContinuationPoints);

    // maxhistorycontinuationpoints
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxHistoryContinuationPoints");
    ID_VariableNode * MaxHistoryContinuationPoints = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    MaxHistoryContinuationPoints->AccessLevel = 1;
    MaxHistoryContinuationPoints->UserAccessLevel = 1;
    MaxHistoryContinuationPoints->Value.Value.UInt16 = 0;
    ID_MAP_NODE_TO_NEW_ID(MaxHistoryContinuationPoints, OpcUaId_Server_ServerCapabilities_MaxHistoryContinuationPoints);

    // softwarecertificates
    OpcUa_String_AttachCopy(&SearchName.Name, "SoftwareCertificates");
    ID_VariableNode * SoftwareCertificates = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    SoftwareCertificates->AccessLevel = 1;
    SoftwareCertificates->UserAccessLevel = 1;
    SoftwareCertificates->Value.Value.Array.Length = 1;
    SoftwareCertificates->Value.Value.Array.Value.ExtensionObjectArray = malloc(sizeof(OpcUa_ExtensionObject) * 1);
    memset(SoftwareCertificates->Value.Value.Array.Value.ExtensionObjectArray, 0x00, sizeof(OpcUa_ExtensionObject) * 1);
    ID_MAP_NODE_TO_NEW_ID(SoftwareCertificates, OpcUaId_Server_ServerCapabilities_SoftwareCertificates);
    for (int i = 0; i < 1; i++) {
        OpcUa_ExtensionObject * ExtensionObject = &SoftwareCertificates->Value.Value.Array.Value.ExtensionObjectArray[i];
        OpcUa_ExtensionObject_Initialize(ExtensionObject);
        ExtensionObject->Encoding = OpcUa_ExtensionObjectEncoding_EncodeableObject;
        ExtensionObject->TypeId.ServerIndex = 0;
        ExtensionObject->TypeId.NodeId.Identifier.Numeric = OpcUaId_SignedSoftwareCertificate;
        ExtensionObject->Body.EncodeableObject.Type = &OpcUa_SignedSoftwareCertificate_EncodeableType;
        ExtensionObject->Body.EncodeableObject.Object = malloc(sizeof(OpcUa_SignedSoftwareCertificate));
        OpcUa_SignedSoftwareCertificate * SignedCertificate = ExtensionObject->Body.EncodeableObject.Object;
        OpcUa_SignedSoftwareCertificate_Initialize(SignedCertificate);
        ID_OpcUa_ByteString_CopyTo(&GlobalSecurityHandle->ServerCertificate, &SignedCertificate->CertificateData);
    }

    // MaxArrayLength
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxArrayLength");
    ID_VariableNode * MaxArrayLength = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    MaxArrayLength->Value.Value.UInt32 = MAX_LIMIT;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxArrayLength);
    ID_MAP_NODE_TO_NEW_ID(MaxArrayLength, OpcUaId_Server_ServerCapabilities_MaxArrayLength);

    // maxstringlength
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxStringLength");
    ID_VariableNode * MaxStringLength = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    MaxStringLength->Value.Value.UInt32 = MAX_LIMIT;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxStringLength);
    ID_MAP_NODE_TO_NEW_ID(MaxStringLength, OpcUaId_Server_ServerCapabilities_MaxStringLength);

    // MaxByteStringLength
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxByteStringLength");
    ID_VariableNode * MaxByteStringLength = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    MaxByteStringLength->Value.Value.UInt32 = MAX_LIMIT;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxByteStringLength);
    ID_MAP_NODE_TO_NEW_ID(MaxByteStringLength, OpcUaId_Server_ServerCapabilities_MaxByteStringLength);

    // modellingrules
    OpcUa_String_AttachCopy(&SearchName.Name, "ModellingRules");
    ID_ObjectNode * ModellingRules = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    OpcUa_NodeId * MRNodeId = &ModellingRules->BaseAttribute.NodeId;
    ID_AddressSpace_AddBiReference(Handle, MRNodeId, ID_NodeType_Object, &ID_ModellingRuleMandatoryNode, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_AddBiReference(Handle, MRNodeId, ID_NodeType_Object, &ID_ModellingRuleExposesItsArrayNode, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_AddBiReference(Handle, MRNodeId, ID_NodeType_Object, &ID_ModellingRuleOptionalNode, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_AddBiReference(Handle, MRNodeId, ID_NodeType_Object, &ID_ModellingRuleMandatoryPlaceholderNode, ID_NodeType_Object, &ID_OrganizesNode);
    ID_AddressSpace_AddBiReference(Handle, MRNodeId, ID_NodeType_Object, &ID_ModellingRuleOptionalPlaceholderNode, ID_NodeType_Object, &ID_OrganizesNode);
    ID_MAP_NODE_TO_NEW_ID(ModellingRules, OpcUaId_Server_ServerCapabilities_ModellingRules);

    // aggregatefunctions
    OpcUa_String_AttachCopy(&SearchName.Name, "AggregateFunctions");
    ID_ObjectNode * AggregateFunctions = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    ID_MAP_NODE_TO_NEW_ID(AggregateFunctions, OpcUaId_Server_ServerCapabilities_AggregateFunctions);

    // operation limits & child mapping
    OpcUa_String_AttachCopy(&SearchName.Name, "OperationLimits");
    ID_ObjectNode * OperationLimits = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerCapabilities, SearchName);
    ID_MAP_NODE_TO_NEW_ID(OperationLimits, OpcUaId_Server_ServerCapabilities_OperationLimits);

    // maxnodesperread
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerRead");
    ID_VariableNode * MaxNodesPerRead = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerRead->Value.Value.UInt32 = MAX_LIMIT;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerRead);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerRead, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerRead);

    // maxnodesperhistoryreaddata
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerHistoryReadData");
    ID_VariableNode * MaxNodesPerHistoryReadData = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerHistoryReadData->Value.Value.UInt32 = 0;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerHistoryReadData);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerHistoryReadData, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerHistoryReadData);

    // maxnodesperhistoryreadevents
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerHistoryReadEvents");
    ID_VariableNode * MaxNodesPerHistoryReadEvents = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerHistoryReadEvents->Value.Value.UInt32 = 0;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerHistoryReadEvents);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerHistoryReadEvents, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerHistoryReadEvents);

    // maxnodesperwrite
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerWrite");
    ID_VariableNode * MaxNodesPerWrite = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerWrite->Value.Value.UInt32 = MAX_LIMIT;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerWrite);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerWrite, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerWrite);

    // maxnodesperhistoryupdatedata
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerHistoryUpdateData");
    ID_VariableNode * MaxNodesPerHistoryUpdateData = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerHistoryUpdateData->Value.Value.UInt32 = 0;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerHistoryUpdateData);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerHistoryUpdateData, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerHistoryUpdateData);

    // maxnodesperhistoryupdateevents
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerHistoryUpdateEvents");
    ID_VariableNode * MaxNodesPerHistoryUpdateEvents = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerHistoryUpdateEvents->Value.Value.UInt32 = 0;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerHistoryReadEvents);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerHistoryUpdateEvents, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerHistoryUpdateEvents);

    // maxnodespermethodcall
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerMethodCall");
    ID_VariableNode * MaxNodesPerMethodCall = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerMethodCall->Value.Value.UInt32 = MAX_LIMIT;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerMethodCall);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerMethodCall, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerMethodCall);

    // maxnodesperbrowse
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerBrowse");
    ID_VariableNode * MaxNodesPerBrowse = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerBrowse->Value.Value.UInt32 = MAX_LIMIT;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerBrowse);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerBrowse, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerBrowse);

    // maxnodesperregisternodes
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerRegisterNodes");
    ID_VariableNode * MaxNodesPerRegisterNodes = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerRegisterNodes->Value.Value.UInt32 = MAX_LIMIT;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerRegisterNodes);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerRegisterNodes, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerRegisterNodes);

    // maxnodespertranslatebrowsepathstonodeids
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerTranslateBrowsePathsToNodeIds");
    ID_VariableNode * MaxNodesPerTranslateBrowsePathsToNodeIds = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerTranslateBrowsePathsToNodeIds->Value.Value.UInt32 = MAX_LIMIT;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerTranslateBrowsePathsToNodeIds);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerTranslateBrowsePathsToNodeIds, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerTranslateBrowsePathsToNodeIds);

    // maxnodespernodemanagement
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxNodesPerNodeManagement");
    ID_VariableNode * MaxNodesPerNodeManagement = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxNodesPerNodeManagement->Value.Value.UInt32 = 0;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxNodesPerNodeManagement);
    ID_MAP_NODE_TO_NEW_ID(MaxNodesPerNodeManagement, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxNodesPerNodeManagement);

    // maxmonitoreditemspercall
    OpcUa_String_AttachCopy(&SearchName.Name, "MaxMonitoredItemsPerCall");
    ID_VariableNode * MaxMonitoredItemsPerCall = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OperationLimits, SearchName);
    MaxMonitoredItemsPerCall->Value.Value.UInt32 = 0;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(MaxMonitoredItemsPerCall);
    ID_MAP_NODE_TO_NEW_ID(MaxMonitoredItemsPerCall, OpcUaId_Server_ServerCapabilities_OperationLimits_MaxMonitoredItemsPerCall);
    
    OpcUa_QualifiedName_Clear(&SearchName);
}

void ID_SetupServerRedundancyObject(ID_AddressSpaceHandle Handle, ID_ObjectNode * Server) {
    OpcUa_QualifiedName SearchName;
    OpcUa_QualifiedName_Initialize(&SearchName);
    
    // server redundancy
    OpcUa_String_AttachCopy(&SearchName.Name, "ServerRedundancy");
    ID_ObjectNode * ServerRedundancy = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Server, SearchName);
    ID_MAP_NODE_TO_NEW_ID(ServerRedundancy, OpcUaId_Server_ServerRedundancy);

    // redundancysupport
    OpcUa_String_AttachCopy(&SearchName.Name, "RedundancySupport");
    ID_VariableNode * RedundancySupport = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerRedundancy, SearchName);
    RedundancySupport->Value.Value.Int32 = OpcUa_RedundancySupport_None;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(RedundancySupport);
    ID_MAP_NODE_TO_NEW_ID(RedundancySupport, OpcUaId_Server_ServerRedundancy_RedundancySupport);

    OpcUa_QualifiedName_Clear(&SearchName);
}

void ID_SetupServerStatusObject(ID_AddressSpaceHandle Handle, ID_ObjectNode * Server) {
    OpcUa_QualifiedName SearchName;
    OpcUa_QualifiedName_Initialize(&SearchName);

    // serverstatus and component types
    OpcUa_String_AttachCopy(&SearchName.Name, "ServerStatus");
    ID_VariableNode * ServerStatus = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Server, SearchName);
    ID_MAP_NODE_TO_NEW_ID(ServerStatus, OpcUaId_Server_ServerStatus);

    // starttime
    OpcUa_String_AttachCopy(&SearchName.Name, "StartTime");
    ID_VariableNode * StartTime = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerStatus, SearchName);
    StartTime->Value.Value.DateTime = OpcUa_DateTime_UtcNow();
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(StartTime);
    ID_MAP_NODE_TO_NEW_ID(StartTime, OpcUaId_Server_ServerStatus_StartTime);

    // currenttime
    OpcUa_String_AttachCopy(&SearchName.Name, "CurrentTime");
    ID_VariableNode * CurrentTime = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerStatus, SearchName);
    CurrentTime->Value.Value.DateTime = OpcUa_DateTime_UtcNow();
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(CurrentTime);
    ID_MAP_NODE_TO_NEW_ID(CurrentTime, OpcUaId_Server_ServerStatus_CurrentTime);

    // state
    OpcUa_String_AttachCopy(&SearchName.Name, "State");
    ID_VariableNode * State = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerStatus, SearchName);
    State->Value.Value.Int32 = OpcUa_ServerState_Running;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(State);
    ID_MAP_NODE_TO_NEW_ID(State, OpcUaId_Server_ServerStatus_State);

    // secondstillshutdown
    OpcUa_String_AttachCopy(&SearchName.Name, "SecondsTillShutdown");
    ID_VariableNode * SecondsTillShutdown = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerStatus, SearchName);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(SecondsTillShutdown);
    ID_MAP_NODE_TO_NEW_ID(SecondsTillShutdown, OpcUaId_Server_ServerStatus_SecondsTillShutdown);

    // shutdown reason
    OpcUa_String_AttachCopy(&SearchName.Name, "ShutdownReason");
    ID_VariableNode * ShutdownReason = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerStatus, SearchName);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(ShutdownReason);
    ID_MAP_NODE_TO_NEW_ID(ShutdownReason, OpcUaId_Server_ServerStatus_ShutdownReason);

    // serverstatus buildinfo
    OpcUa_String_AttachCopy(&SearchName.Name, "BuildInfo");
    ID_VariableNode * BuildInfo = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerStatus, SearchName);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(BuildInfo);
    ID_MAP_NODE_TO_NEW_ID(BuildInfo, OpcUaId_Server_ServerStatus_BuildInfo);
    
    // producturi
    OpcUa_String_AttachCopy(&SearchName.Name, "ProductUri");
    ID_VariableNode * ProductUri = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)BuildInfo, SearchName);
    OpcUa_String_AttachCopy(&ProductUri->Value.Value.String, ID_PRODUCT_URI);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(ProductUri);
    ID_MAP_NODE_TO_NEW_ID(ProductUri, OpcUaId_Server_ServerStatus_BuildInfo_ProductUri);

    // manufacturername
    OpcUa_String_AttachCopy(&SearchName.Name, "ManufacturerName");
    ID_VariableNode * ManufacturerName = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)BuildInfo, SearchName);
    OpcUa_String_AttachCopy(&ManufacturerName->Value.Value.String, ID_MANUFACTURER_NAME);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(ManufacturerName);
    ID_MAP_NODE_TO_NEW_ID(ManufacturerName, OpcUaId_Server_ServerStatus_BuildInfo_ManufacturerName);

    // productname
    OpcUa_String_AttachCopy(&SearchName.Name, "ProductName");
    ID_VariableNode * ProductName = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)BuildInfo, SearchName);
    OpcUa_String_AttachCopy(&ProductName->Value.Value.String, ID_PRODUCT_NAME);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(ProductName);
    ID_MAP_NODE_TO_NEW_ID(ProductName, OpcUaId_Server_ServerStatus_BuildInfo_ProductName);

    // softwareversion
    OpcUa_String_AttachCopy(&SearchName.Name, "SoftwareVersion");
    ID_VariableNode * SoftwareVersion = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)BuildInfo, SearchName);
    OpcUa_String_AttachCopy(&SoftwareVersion->Value.Value.String, ID_SOFTWARE_VERSION);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(SoftwareVersion);
    ID_MAP_NODE_TO_NEW_ID(SoftwareVersion, OpcUaId_Server_ServerStatus_BuildInfo_SoftwareVersion);

    // buildnumber
    OpcUa_String_AttachCopy(&SearchName.Name, "BuildNumber");
    ID_VariableNode * BuildNumber = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)BuildInfo, SearchName);
    OpcUa_String_AttachCopy(&ProductName->Value.Value.String, ID_BUILD_NUMBER);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(BuildNumber);
    ID_MAP_NODE_TO_NEW_ID(BuildNumber, OpcUaId_Server_ServerStatus_BuildInfo_BuildNumber);

    // builddate
    OpcUa_String_AttachCopy(&SearchName.Name, "BuildDate");
    ID_VariableNode * BuildDate = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)BuildInfo, SearchName);
    BuildDate->Value.Value.DateTime = ID_BUILD_DATE;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(BuildDate);
    ID_MAP_NODE_TO_NEW_ID(BuildDate, OpcUaId_Server_ServerStatus_BuildInfo_BuildDate);

    // copy information into buildinfo structure
    OpcUa_BuildInfo * BuildInfoStructure = BuildInfo->Value.Value.ExtensionObject->Body.EncodeableObject.Object;
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&BuildInfoStructure->ProductUri, &ProductUri->Value.Value.String);
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&BuildInfoStructure->ManufacturerName, &ManufacturerName->Value.Value.String);
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&BuildInfoStructure->ProductName, &ProductName->Value.Value.String);
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&BuildInfoStructure->SoftwareVersion, &SoftwareVersion->Value.Value.String);
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&BuildInfoStructure->BuildNumber, &BuildNumber->Value.Value.String);
    BuildInfoStructure->BuildDate = BuildDate->Value.Value.DateTime;

    // copy info into serverstatus structure
    ServerStatus->AccessLevel = 1;
    ServerStatus->UserAccessLevel = 1;
    OpcUa_ServerStatusDataType * ServerStatusStructure = ServerStatus->Value.Value.ExtensionObject->Body.EncodeableObject.Object;
    ServerStatusStructure->StartTime = StartTime->Value.Value.DateTime;
    ServerStatusStructure->CurrentTime = CurrentTime->Value.Value.DateTime;
    ServerStatusStructure->State = State->Value.Value.Int32;
    ServerStatusStructure->BuildInfo.BuildDate = BuildDate->Value.Value.DateTime;
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&ServerStatusStructure->BuildInfo.BuildNumber, &BuildNumber->Value.Value.String);
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&ServerStatusStructure->BuildInfo.ManufacturerName, &ManufacturerName->Value.Value.String);
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&ServerStatusStructure->BuildInfo.ProductName, &ProductName->Value.Value.String);
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&ServerStatusStructure->BuildInfo.ProductUri, &ProductUri->Value.Value.String);
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&ServerStatusStructure->BuildInfo.SoftwareVersion, &SoftwareVersion->Value.Value.String);

    OpcUa_QualifiedName_Clear(&SearchName);
}

#pragma warning(push)
#pragma warning(disable : 4204)
void ID_SetupServerDiagnosticsSummary(ID_AddressSpaceHandle Handle, ID_VariableNode * ServerDiagnosticsSummary) {
    OpcUa_QualifiedName SearchName;
    OpcUa_QualifiedName_Initialize(&SearchName);

    OpcUa_String_AttachCopy(&SearchName.Name, "ServerViewCount");
    ID_VariableNode * ServerViewCount = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerDiagnosticsSummary, SearchName);
    ID_ASSERT(ServerViewCount->Value.Datatype == OpcUaType_UInt32);
    ServerViewCount->Value.Value.UInt32 = 1;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(ServerViewCount);
    ID_MAP_NODE_TO_NEW_ID(ServerViewCount, OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_ServerViewCount);

    OpcUa_ServerDiagnosticsSummaryDataType * SDSObj = ServerDiagnosticsSummary->Value.Value.ExtensionObject->Body.EncodeableObject.Object;
    SDSObj->ServerViewCount = 1;

    char * children[11] = {"CurrentSessionCount","CumulatedSessionCount","SecurityRejectedSessionCount","RejectedSessionCount",
                           "SessionTimeoutCount","SessionAbortCount","PublishingIntervalCount","CurrentSubscriptionCount",
                           "CumulatedSubscriptionCount","SecurityRejectedRequestsCount","RejectedRequestsCount"};
    int ids[11] = { OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_CurrentSessionCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_CumulatedSessionCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_SecurityRejectedSessionCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_RejectedSessionCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_SessionTimeoutCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_SessionAbortCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_PublishingIntervalCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_CurrentSubscriptionCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_CumulatedSubscriptionCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_SecurityRejectedRequestsCount,
                    OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary_RejectedRequestsCount};
    for (int i = 0; i < 11; i++) {
        OpcUa_String_AttachCopy(&SearchName.Name, children[i]);
        ID_VariableNode * Node = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerDiagnosticsSummary, SearchName);
        ID_ASSERT(Node->Value.Datatype == OpcUaType_UInt32);
        ID_CONFIGURE_VARIABLE_AS_READ_ONLY(Node);
        ID_MAP_NODE_TO_NEW_ID(Node, ids[i]);
    }

    OpcUa_QualifiedName_Clear(&SearchName);
}
#pragma warning(pop)

void ID_SetupServerDiagnosticsObject(ID_AddressSpaceHandle Handle, ID_ObjectNode * Server) {
    OpcUa_QualifiedName SearchName;
    OpcUa_QualifiedName_Initialize(&SearchName);
    
    // serverdiagnosticstype
    OpcUa_String_AttachCopy(&SearchName.Name, "ServerDiagnostics");
    ID_ObjectNode * ServerDiagnostics = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Server, SearchName);
    ID_MAP_NODE_TO_NEW_ID(ServerDiagnostics, OpcUaId_Server_ServerDiagnostics);

    // enabledflag
    OpcUa_String_AttachCopy(&SearchName.Name, "EnabledFlag");
    ID_VariableNode * EnabledFlag = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerDiagnostics, SearchName);
    EnabledFlag->Value.Value.Boolean = OpcUa_False;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(EnabledFlag);
    ID_MAP_NODE_TO_NEW_ID(EnabledFlag, OpcUaId_Server_ServerDiagnostics_EnabledFlag);

    // serverdiagnosticssummary
    OpcUa_String_AttachCopy(&SearchName.Name, "ServerDiagnosticsSummary");
    ID_VariableNode * ServerDiagnosticsSummary = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerDiagnostics, SearchName);
    ID_ASSERT(ServerDiagnosticsSummary->Value.Datatype == OpcUaType_ExtensionObject);
    ID_ASSERT(ServerDiagnosticsSummary->Value.Value.ExtensionObject->Encoding == OpcUa_ExtensionObjectEncoding_EncodeableObject);
    ID_ASSERT(ServerDiagnosticsSummary->Value.Value.ExtensionObject->Body.EncodeableObject.Type->TypeId == OpcUaId_ServerDiagnosticsSummaryDataType);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(ServerDiagnosticsSummary);
    ID_MAP_NODE_TO_NEW_ID(ServerDiagnosticsSummary, OpcUaId_Server_ServerDiagnostics_ServerDiagnosticsSummary);
    ID_SetupServerDiagnosticsSummary(Handle, ServerDiagnosticsSummary);

    // SamplingIntervalDiagnosticsArray
    OpcUa_String_AttachCopy(&SearchName.Name, "SamplingIntervalDiagnosticsArray");
    ID_VariableNode * SamplingIntervalDiagnosticsArray = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerDiagnostics, SearchName);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(SamplingIntervalDiagnosticsArray);
    ID_MAP_NODE_TO_NEW_ID(SamplingIntervalDiagnosticsArray, OpcUaId_Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray);

    // SubscriptionDiagnosticsArray
    OpcUa_String_AttachCopy(&SearchName.Name, "SubscriptionDiagnosticsArray");
    ID_VariableNode * SubscriptionDiagnosticsArray = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerDiagnostics, SearchName);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(SubscriptionDiagnosticsArray);
    ID_MAP_NODE_TO_NEW_ID(SubscriptionDiagnosticsArray, OpcUaId_Server_ServerDiagnostics_SubscriptionDiagnosticsArray);

    // SessionsDiagnosticsSummary
    OpcUa_String_AttachCopy(&SearchName.Name, "SessionsDiagnosticsSummary");
    ID_ObjectNode * SessionsDiagnosticsSummary = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ServerDiagnostics, SearchName);
    ID_MAP_NODE_TO_NEW_ID(SessionsDiagnosticsSummary, OpcUaId_Server_ServerDiagnostics_SessionsDiagnosticsSummary);

    OpcUa_String_AttachCopy(&SearchName.Name, "SessionDiagnosticsArray");
    ID_VariableNode * SessionDiagnosticsArray = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)SessionsDiagnosticsSummary, SearchName);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(SessionDiagnosticsArray);
    ID_MAP_NODE_TO_NEW_ID(SessionDiagnosticsArray, OpcUaId_Server_ServerDiagnostics_SessionsDiagnosticsSummary_SessionDiagnosticsArray);

    OpcUa_String_AttachCopy(&SearchName.Name, "SessionSecurityDiagnosticsArray");
    ID_VariableNode * SessionSecurityDiagnosticsArray = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)SessionsDiagnosticsSummary, SearchName);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(SessionSecurityDiagnosticsArray);
    ID_MAP_NODE_TO_NEW_ID(SessionSecurityDiagnosticsArray, OpcUaId_Server_ServerDiagnostics_SessionsDiagnosticsSummary_SessionSecurityDiagnosticsArray);

    OpcUa_QualifiedName_Clear(&SearchName);
}

void ID_Initialize_ServerObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_QualifiedName SearchName;
    OpcUa_String_Initialize(&SearchName.Name);
    SearchName.NamespaceIndex = 0;
    
    OpcUa_NodeId ServerType = ID_OPCNodeId(OpcUaId_ServerType);
    ID_ObjectTypeNode * ServerTypeNode = ID_AddressSpace_GetObjectTypeNode(Handle, &ServerType);
    OpcUa_NodeId ServerId = ID_OPCNodeId(OpcUaId_Server);
    ID_ObjectNode * Server = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, ServerTypeNode, &ServerId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Server, 0, "Server");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Server, "Server");
    
    // ServerArray
    OpcUa_String_AttachCopy(&SearchName.Name, "ServerArray");
    ID_VariableNode * ServerArray = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Server, SearchName);
    ServerArray->Value.Value.Array.Length = 1;
    ServerArray->Value.Value.Array.Value.StringArray = malloc(sizeof(OpcUa_String));
    OpcUa_String_Initialize(ServerArray->Value.Value.Array.Value.StringArray);
    OpcUa_String_AttachCopy(ServerArray->Value.Value.Array.Value.StringArray, ID_SERVER_URI);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(ServerArray);
    ID_MAP_NODE_TO_NEW_ID(ServerArray, OpcUaId_Server_ServerArray);
    
    // NamespaceArray
    OpcUa_String_AttachCopy(&SearchName.Name, "NamespaceArray");
    ID_VariableNode * NamespaceArray = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Server, SearchName);
    NamespaceArray->Value.Value.Array.Length = 3;
    NamespaceArray->Value.Value.Array.Value.StringArray = malloc(sizeof(OpcUa_String) * 3);
    OpcUa_String_Initialize(&NamespaceArray->Value.Value.Array.Value.StringArray[0]);
    OpcUa_String_Initialize(&NamespaceArray->Value.Value.Array.Value.StringArray[1]);
	OpcUa_String_Initialize(&NamespaceArray->Value.Value.Array.Value.StringArray[2]);
    OpcUa_String_AttachCopy(&NamespaceArray->Value.Value.Array.Value.StringArray[0], ID_NAMESPACE_OPC_URI);
    OpcUa_String_AttachCopy(&NamespaceArray->Value.Value.Array.Value.StringArray[1], ID_NAMESPACE_ID_URI);
	OpcUa_String_AttachCopy(&NamespaceArray->Value.Value.Array.Value.StringArray[2], ID_NAMESPACE_ALIAS_URI);
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(NamespaceArray);
    ID_MAP_NODE_TO_NEW_ID(NamespaceArray, OpcUaId_Server_NamespaceArray);
    
    // ServiceLevel
    OpcUa_String_AttachCopy(&SearchName.Name, "ServiceLevel");
    ID_VariableNode * ServiceLevel = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Server, SearchName);
    ServiceLevel->Value.Value.Byte = 255;
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(ServiceLevel);
    ID_MAP_NODE_TO_NEW_ID(ServiceLevel, OpcUaId_Server_ServiceLevel);
    
    // Auditing
    OpcUa_String_AttachCopy(&SearchName.Name, "Auditing");
    ID_VariableNode * Auditing = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Server, SearchName);
    Auditing->Value.Value.Boolean = OpcUa_False;
    ID_MAP_NODE_TO_NEW_ID(Auditing, OpcUaId_Server_Auditing);

    // vendorserverinfo
    OpcUa_String_AttachCopy(&SearchName.Name, "VendorServerInfo");
    ID_ObjectNode * VendorServerInfo = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Server, SearchName);
    ID_MAP_NODE_TO_NEW_ID(VendorServerInfo, OpcUaId_Server_VendorServerInfo);

    ID_SetupServerCapabilitiesObject(Handle, Server);
    ID_SetupServerRedundancyObject(Handle, Server);
    ID_SetupServerStatusObject(Handle, Server);
    ID_SetupServerDiagnosticsObject(Handle, Server);

    OpcUa_String_Clear(&SearchName.Name);
}

void ID_Initialize_ModellingRuleObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_NodeId ID_ModellingRuleExposesItsArrayNode = ID_ModellingRuleExposesItsArray;
    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;
    OpcUa_NodeId ID_ModellingRuleOptionalNode = ID_ModellingRuleOptional;
    OpcUa_NodeId ID_ModellingRuleMandatoryPlaceholderNode = ID_ModellingRuleMandatoryPlaceholder;
    OpcUa_NodeId ID_ModellingRuleOptionalPlaceholderNode = ID_ModellingRuleOptionalPlaceholder;

    OpcUa_QualifiedName SearchName;
    OpcUa_String_Initialize(&SearchName.Name);
    SearchName.NamespaceIndex = 0;
    OpcUa_String_AttachCopy(&SearchName.Name, "NamingRule");

    OpcUa_NodeId ModellingRuleType = ID_OPCNodeId(OpcUaId_ModellingRuleType);
    ID_ObjectTypeNode * ModellingRuleTypeNode = ID_AddressSpace_GetObjectTypeNode(Handle, &ModellingRuleType);
    
    ID_ObjectNode * ExposesItsArray = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, ModellingRuleTypeNode, &ID_ModellingRuleExposesItsArrayNode);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(ExposesItsArray, 0, "ExposesItsArray");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(ExposesItsArray, "ExposesItsArray");
    ID_VariableNode * ExposesItsArrayProperty = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)ExposesItsArray, SearchName);
    ExposesItsArrayProperty->Value.Value.Int32 = ID_NAMING_RULE_TYPE_CONSTRAINT;

    ID_ObjectNode * Mandatory = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, ModellingRuleTypeNode, &ID_ModellingRuleMandatoryNode);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Mandatory, 0, "Mandatory");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Mandatory, "Mandatory");
    ID_VariableNode * MandatoryProperty = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Mandatory, SearchName);
    MandatoryProperty->Value.Value.Int32 = ID_NAMING_RULE_TYPE_MANDATORY;
    
    ID_ObjectNode * Optional = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, ModellingRuleTypeNode, &ID_ModellingRuleOptionalNode);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Optional, 0, "Optional");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Optional, "Optional");
    ID_VariableNode * OptionalProperty = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)Optional, SearchName);
    OptionalProperty->Value.Value.Int32 = ID_NAMING_RULE_TYPE_OPTIONAL;
    
    ID_ObjectNode * MandatoryPlaceholder = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, ModellingRuleTypeNode, &ID_ModellingRuleMandatoryPlaceholderNode);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(MandatoryPlaceholder, 0, "MandatoryPlaceholder");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(MandatoryPlaceholder, "MandatoryPlaceholder");
    ID_VariableNode * MandatoryPlaceholderProperty = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)MandatoryPlaceholder, SearchName);
    MandatoryPlaceholderProperty->Value.Value.Int32 = ID_NAMING_RULE_TYPE_CONSTRAINT;
    
    ID_ObjectNode * OptionalPlaceholder = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, ModellingRuleTypeNode, &ID_ModellingRuleOptionalPlaceholderNode);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(OptionalPlaceholder, 0, "OptionalPlaceholder");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(OptionalPlaceholder, "OptionalPlaceholder");
    ID_VariableNode * OptionalPlaceholderProperty = ID_AddressSpace_HuntForReferenceWithNode(Handle, (ID_BaseAttributes*)OptionalPlaceholder, SearchName);
    OptionalPlaceholderProperty->Value.Value.Int32 = ID_NAMING_RULE_TYPE_CONSTRAINT;
    
    OpcUa_String_Clear(&SearchName.Name);
}