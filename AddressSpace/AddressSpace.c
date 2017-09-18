/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "AddressSpace.h"
#include "AddressSpaceHelper.h"
#include "AddressSpaceCopier.h"

#define ID_ADDRESSSPACE_INITIAL_CAPACITY    128
#define ID_ADDRESSSPACE_INCREMENT_BY        30
#define ID_FASTLIST_INCREMENT_BY            16

// fastlist implemented as a dynamic array not as a linked list
// because the array is small enough that any additional overhead
// is insignificant
typedef struct _ID_FastList_ {
    OpcUa_UInt32 count;
    OpcUa_UInt32 capacity;
    ID_BaseAttributes ** references;
} ID_FastList;

// Address Space Struct
typedef struct _ID_AddressSpace_ {
    // number of current elements
    OpcUa_UInt32 count;

    // number of elements can be held
    OpcUa_UInt32 capacity;

    // space to increase per enlargment
    OpcUa_UInt32 incrementBy;

    // array of pointers to addressspacenode
    ID_AddressSpaceNode ** allNodes;

    // mutex to synchronize node access
    OpcUa_Mutex NodeMutex;

    ID_FastList * FastAccess;
} ID_AddressSpaceInternal;

// Ensure that FastList has space to add an additional element
void ID_FastList_EnsureAdditionSize(ID_FastList * Handle) {
    if (Handle->count + 1 > Handle->capacity) {
        Handle->capacity += ID_FASTLIST_INCREMENT_BY;
        Handle->references = realloc(Handle->references, sizeof(ID_BaseAttributes*) * Handle->capacity);
    }
}

// Insert pointer to BaseAttributes into the FastList
void ID_FastList_InsertItem(ID_FastList * Handle, ID_BaseAttributes * Node) {
    ID_ASSERT(Node != NULL);
    ID_FastList_EnsureAdditionSize(Handle);
    Handle->references[Handle->count] = Node;
    Handle->count++;
}

// Insert item into FastList by finding a node relative to another node
ID_BaseAttributes * ID_FastList_InsertItemRelative(ID_AddressSpaceHandle AddressSpace, ID_FastList * Handle, ID_BaseAttributes * Source, char * Browse) {
    OpcUa_QualifiedName QName;
    OpcUa_QualifiedName_Initialize(&QName);

    OpcUa_String_AttachCopy(&QName.Name, Browse);
    ID_BaseAttributes * Attrs = ID_AddressSpace_HuntForReferenceWithNode(AddressSpace, Source, QName);
    ID_FastList_InsertItem(Handle, Attrs);

    OpcUa_QualifiedName_Clear(&QName);
    return Attrs;
}

// Fast List
#pragma warning(push)
#pragma warning(disable : 4204)
// builds fast list by accesssing elements of interest
// and then by tracing references outbound
void ID_AddressSpace_ConstructFastList(ID_AddressSpaceHandle AddressSpace) {
    OpcUa_NodeId ID_HasComponentNode = ID_HasComponent;
    OpcUa_NodeId ID_OrganizesNode = ID_Organizes;

    ID_AddressSpaceInternal * Internal = AddressSpace;
    if (Internal->FastAccess == NULL) {
        Internal->FastAccess = malloc(sizeof(ID_FastList));
        memset(Internal->FastAccess, 0x00, sizeof(ID_FastList));
    }

    /* use databaseview, merge using forward browsename */
    OpcUa_NodeId DatabaseView = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_VIEW);
    ID_BaseAttributes * DatabaseViewNode = ID_AddressSpace_GetNode(AddressSpace, &DatabaseView);
    ID_FastList_InsertItem(Internal->FastAccess, DatabaseViewNode);

    /* merge all forward organizes references */
    for (OpcUa_UInt32 i = 0; i < DatabaseViewNode->NoOfReferences; i++) {
        ID_ReferenceNode * Ref = DatabaseViewNode->References[i];
        if (ID_AddressSpace_AreIdsEqual(&Ref->ReferenceTypeId, &ID_OrganizesNode) == OpcUa_True && Ref->IsInverse == OpcUa_False) {
            ID_BaseAttributes * Attrs = ID_AddressSpace_GetNode(AddressSpace, &Ref->TargetNodeId);
            ID_FastList_InsertItem(Internal->FastAccess, Attrs);

            char * BrowseName = OpcUa_String_GetRawString(&Attrs->BrowseName.Name);
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Fast Access constructed to BrowseName: %s\n", BrowseName);
        }
    }

    /* insert server relevant objects */
    OpcUa_NodeId ServerId = ID_OPCNodeId(OpcUaId_Server);
    ID_BaseAttributes * ServerNode = ID_AddressSpace_GetNode(AddressSpace, &ServerId);
    ID_FastList_InsertItem(Internal->FastAccess, ServerNode);
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerNode, "ServerArray");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerNode, "NamespaceArray");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerNode, "ServiceLevel");

    ID_BaseAttributes * ServerDiagnosticsNode = ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerNode, "ServerDiagnostics");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerDiagnosticsNode, "EnabledFlag");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerDiagnosticsNode, "SamplingIntervalDiagnosticsArray");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerDiagnosticsNode, "SubscriptionDiagnosticsArray");

    ID_BaseAttributes * SessionsDiagnosticsSummary = ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerDiagnosticsNode, "SessionsDiagnosticsSummary");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, SessionsDiagnosticsSummary, "SessionDiagnosticsArray");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, SessionsDiagnosticsSummary, "SessionSecurityDiagnosticsArray");
    
    ID_BaseAttributes * ServerDiagnosticsSummaryNode = ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerDiagnosticsNode, "ServerDiagnosticsSummary");
    OpcUa_UInt32 InsertCount = 0;
    for (OpcUa_UInt32 i = 0; i < ServerDiagnosticsSummaryNode->NoOfReferences; i++) {
        ID_ReferenceNode * Ref = ServerDiagnosticsSummaryNode->References[i];
        if (ID_AddressSpace_AreIdsEqual(&Ref->ReferenceTypeId, &ID_HasComponentNode) == OpcUa_True && Ref->IsInverse == OpcUa_False) {
            ID_BaseAttributes * Attrs = ID_AddressSpace_GetNode(AddressSpace, &Ref->TargetNodeId);
            ID_FastList_InsertItem(Internal->FastAccess, Attrs);

            char * BrowseName = OpcUa_String_GetRawString(&Attrs->BrowseName.Name);
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Fast Access constructed to BrowseName: %s\n", BrowseName);

            InsertCount++;
        }
    }
    ID_ASSERT(InsertCount == 12);

    ID_BaseAttributes * ServerStatusNode = ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerNode, "ServerStatus");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerStatusNode, "StartTime");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerStatusNode, "CurrentTime");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerStatusNode, "State");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerStatusNode, "BuildInfo");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerStatusNode, "SecondsTillShutdown");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerStatusNode, "ShutdownReason");
    
    ID_BaseAttributes * ServerCapabilitiesNode = ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerNode, "ServerCapabilities");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "ServerProfileArray");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "LocaleIdArray");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "MinSupportedSampleRate");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "MaxBrowseContinuationPoints");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "MaxQueryContinuationPoints");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "MaxHistoryContinuationPoints");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "SoftwareCertificates");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "MaxArrayLength");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "MaxStringLength");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "MaxByteStringLength");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "ModellingRules");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "AggregateFunctions");
    
    ID_BaseAttributes * OperationLimitsNode = ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerCapabilitiesNode, "OperationLimits");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerRead");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerHistoryReadData");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerHistoryReadEvents");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerWrite");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerHistoryUpdateData");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerHistoryUpdateEvents");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerMethodCall");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerBrowse");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerRegisterNodes");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerTranslateBrowsePathsToNodeIds");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxNodesPerNodeManagement");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, OperationLimitsNode, "MaxMonitoredItemsPerCall");

    ID_BaseAttributes * ServerRedundancy = ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerNode, "ServerRedundancy");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerRedundancy, "RedundancySupport");
    ID_FastList_InsertItemRelative(AddressSpace, Internal->FastAccess, ServerNode, "VendorServerInfo");
}
#pragma warning(pop)

// Get a pointer to the inner body of the AddressSpaceNode
void * ID_AddressSpace_AccessNodeBody(ID_AddressSpaceNode * node, ID_NodeType Type) {
    switch (Type) {
        case ID_NodeType_ReferenceType:
            return &node->Node.RefTypeNode;
            break;
        case ID_NodeType_View:
            return &node->Node.VNode;
            break;
        case ID_NodeType_Object:
            return &node->Node.ObjNode;
            break;
        case ID_NodeType_ObjectType:
            return &node->Node.ObjTypeNode;
            break;
        case ID_NodeType_Variable:
            return &node->Node.VarNode;
            break;
        case ID_NodeType_VariableType:
            return &node->Node.VarTypeNode;
            break;
        case ID_NodeType_Method:
            return &node->Node.MNode;
            break;
        case ID_NodeType_DataType:
            return &node->Node.DTypeNode;
            break;
        default: 
            break;
    }

    return NULL;
}

// Look for a Node with specified NodeId in the FastList Array
void * ID_AddressSpace_FastAccessNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId * NodeId) {
    ID_AddressSpaceInternal * AS = AddressSpace;
    if (AS->FastAccess == NULL) {
        return NULL;
    }

    for (OpcUa_UInt32 i = 0; i < AS->FastAccess->count; i++) {
        ID_BaseAttributes * Node = AS->FastAccess->references[i];
        if (ID_AddressSpace_AreIdsEqual(&Node->NodeId, NodeId) == OpcUa_True) {
            return Node;
        }
    }

    return NULL;
}

// Address Space Core

// Get number of elements in AddressSpace
OpcUa_UInt32 ID_AddressSpace_GetSize(ID_AddressSpaceHandle AddressSpace) {
    ID_AddressSpaceInternal * Internal = AddressSpace;
    return Internal->count;
}

// Copy the Variable stored in the VarTypeNode to the VarNode
// Can also be used to copy values between variables due to similar structs
// Function only copies value if the ValueRanks is One Dimensional or Scalar
void ID_AddressSpace_CopyVariableFromType(ID_VariableTypeNode * VarTypeNode, ID_VariableNode * VarNode) {
    /* configure length attribute based on valuerank */
    OpcUa_Int32 Length = 0;
    if (VarTypeNode->ValueRank == OpcUa_ValueRanks_Any) Length = ID_VALUERANK_ANY_INDICATOR;
    else if (VarTypeNode->ValueRank == OpcUa_ValueRanks_OneDimension) Length = VarTypeNode->ArrayDimensions[0];    
    OpcUa_BuiltInType BIType = VarTypeNode->Value.Datatype;
    
    OpcUa_ExpandedNodeId * ExpandedNodeId = NULL;
    OpcUa_ExtensionObjectEncoding Encoding = 0;
    OpcUa_EncodeableType * EncodeableType = NULL;
    if (BIType == OpcUaType_ExtensionObject) {
        if (Length > 0 && VarTypeNode->ValueRank == OpcUa_ValueRanks_OneDimension) {
            EncodeableType = VarTypeNode->Value.Value.Array.Value.ExtensionObjectArray[0].Body.EncodeableObject.Type;
            Encoding = VarTypeNode->Value.Value.Array.Value.ExtensionObjectArray[0].Encoding;
            ExpandedNodeId = &VarTypeNode->Value.Value.Array.Value.ExtensionObjectArray[0].TypeId;
        } else if (Length == 0 && VarTypeNode->ValueRank == OpcUa_ValueRanks_OneDimension) {
            // mark array of unknown length as uninitializale
            // this also makes it impossible for extensionobject info to be copied in
            Length = ID_LENGTH_UNKNOWN_INDICATOR;
        }
    } else if (VarTypeNode->ValueRank == OpcUa_ValueRanks_OneDimension && Length == 0) { 
        // mark array of unknown length as uninitializable
        Length = ID_LENGTH_UNKNOWN_INDICATOR;
    }
    
    // Setup the variable's variant attribute based on information gathered
    ID_AddressSpace_SetupVariantOnVariable(VarNode, Length, BIType, Encoding, ExpandedNodeId, EncodeableType);

    // abort the copy unless for sure ValueRanks and Length are 
    // valid and usable...removes potential for UNKNOWN copies and multidimensional copies
    // the copy will only succeed if ValueRank is scalar or one dimension
    if ((VarTypeNode->ValueRank == OpcUa_ValueRanks_OneDimension && Length > 0) || (VarTypeNode->ValueRank == OpcUa_ValueRanks_Scalar)) {
        ID_AddressSpace_CopyVariant(&VarTypeNode->Value, &VarNode->Value);
    }
}

// Ensure that no two nodes in the AddressSpace share NodeIds
// Check will not return an assertion error but will only log the error
void ID_AddressSpace_AnalyzeConflicts(ID_AddressSpaceHandle AddressSpace) {
    ID_AddressSpaceInternal * Internal = (ID_AddressSpaceInternal*)AddressSpace;
    for (OpcUa_UInt32 i = 0; i < Internal->count; i++) {        
        ID_AddressSpaceNode * p1 = Internal->allNodes[i];
        ID_BaseAttributes * p1a = ID_AddressSpace_AccessNodeBody(p1, p1->Type);
        if (p1a->NodeId.NamespaceIndex == 0 && p1a->NodeId.IdentifierType == OpcUa_IdType_Numeric &&
            p1a->NodeId.Identifier.Numeric == 0) {
            printf("OH NO! BAD NODE ID FOUND with BrowseName %s at Index %d\n", OpcUa_String_GetRawString(&p1a->BrowseName.Name), i);
        }
        
        for (OpcUa_UInt32 j = i + 1; j < Internal->count; j++) {
            ID_AddressSpaceNode * p2 = Internal->allNodes[j];
            ID_BaseAttributes * p2a = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(p2, p2->Type);
            
            if (ID_AddressSpace_AreIdsEqual(&p1a->NodeId, &p2a->NodeId) == OpcUa_True) {
                printf("Oh no! Match found\n");
                printf("Node A BrowseName %s at index %d\n", OpcUa_String_GetRawString(&p1a->BrowseName.Name), i);
                printf("Node B BrowseName %s at index %d\n", OpcUa_String_GetRawString(&p2a->BrowseName.Name), j);
                printf("\n");
            }
        }
    }
}

// Create AddressSpace
void ID_AddressSpace_Create(ID_AddressSpaceHandle * AddressSpace) {
    ID_AddressSpace_CreateWithInitialCapacity(AddressSpace, ID_ADDRESSSPACE_INITIAL_CAPACITY);
}

// Create AddressSpace with initial capacity
void ID_AddressSpace_CreateWithInitialCapacity(ID_AddressSpaceHandle * AddressSpace, OpcUa_UInt32 capacity) {
    if (AddressSpace == NULL) {
        return;
    }
    
    ID_AddressSpaceInternal * aSpace = malloc(sizeof(ID_AddressSpaceInternal));
    memset(aSpace, 0x00, sizeof(ID_AddressSpaceInternal));

    aSpace->capacity = capacity;
    aSpace->incrementBy = ID_ADDRESSSPACE_INCREMENT_BY;
    aSpace->count = 0;
    
    aSpace->allNodes = malloc(sizeof(ID_AddressSpaceNode*) * capacity);
    memset(aSpace->allNodes, 0, sizeof(ID_AddressSpaceNode*) * capacity);

    OPCUA_P_MUTEX_CREATE(&aSpace->NodeMutex);
    *AddressSpace = (ID_AddressSpaceHandle*)aSpace;
}

// Find node with NodeId in the Address Space or NULL
ID_AddressSpaceNode * ID_AddressSpace_FindNodeWithID(ID_AddressSpaceHandle Handle, OpcUa_NodeId * nodeId) {
    ID_AddressSpaceInternal * Internal = (ID_AddressSpaceInternal*)Handle;
    for (OpcUa_Int i = Internal->count - 1; i >= 0; i--) {
        ID_AddressSpaceNode * node = Internal->allNodes[i];
        ID_BaseAttributes * attrs = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(node, node->Type);
        if (ID_AddressSpace_AreIdsEqual(&attrs->NodeId, nodeId) == OpcUa_True) {
            return node;
        }
    }
    
    return NULL;
}

// Destroys an AddressSpaceNode; this function is not mutexed
// Caller's responsibility to ensure that synchronous access is used or maintained
void ID_AddressSpace_DestroyNode(ID_AddressSpaceInternal * Internal, ID_AddressSpaceNode * node) {
    OpcUa_ReferenceParameter(Internal);
    ID_BaseAttributes * attrs = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(node, node->Type);

    // Remove all references after ensuring synchronous access
    OPCUA_P_MUTEX_LOCK(attrs->NodeMutex);
    OPCUA_P_MUTEX_LOCK(attrs->RefMutex);
    OpcUa_UInt32 j = 0;
    for (j = 0; j < attrs->NoOfReferences; j++) {
        OpcUa_NodeId_Clear(&attrs->References[j]->ReferenceTypeId);
        OpcUa_NodeId_Clear(&attrs->References[j]->TargetNodeId);
        free(attrs->References[j]);
        attrs->References[j] = NULL;
    }
        
    free(attrs->References);
    attrs->References = NULL;

    OpcUa_QualifiedName_Clear(&attrs->BrowseName);
    OpcUa_LocalizedText_Clear(&attrs->DisplayName);
        
    if (node->Type == ID_NodeType_VariableType) {
        OpcUa_Variant_Clear(&node->Node.VarTypeNode.Value);
        if (node->Node.VarTypeNode.ArrayDimensions != NULL) {
            free(node->Node.VarTypeNode.ArrayDimensions);
            node->Node.VarTypeNode.ArrayDimensions = NULL;
        }
    } else if (node->Type == ID_NodeType_Variable) {
        OpcUa_Variant_Clear(&node->Node.VarNode.Value);
        if (node->Node.VarNode.ArrayDimensions != NULL) {
            free(node->Node.VarNode.ArrayDimensions);
            node->Node.VarNode.ArrayDimensions = NULL;
        }
    }
        
    if (node->Type == ID_NodeType_ReferenceType) {
        OpcUa_LocalizedText_Clear(&node->Node.RefTypeNode.InverseName);
    }
        
    if (node->FreeFunction != NULL) {
		void * NodeBody = ID_AddressSpace_AccessNodeBody(node, node->Type);
		node->FreeFunction(NodeBody);
    }
        
    OpcUa_NodeId_Clear(&attrs->NodeId);    
    OPCUA_P_MUTEX_UNLOCK(attrs->RefMutex);
    OPCUA_P_MUTEX_UNLOCK(attrs->NodeMutex);
    OPCUA_P_MUTEX_DELETE(&attrs->RefMutex);
    OPCUA_P_MUTEX_DELETE(&attrs->NodeMutex);
    free(node);
}

// Remove all AddressSpaceNodes and FastList in the AddressSpace
void ID_AddressSpace_Clear(ID_AddressSpaceHandle AddressSpace) {
    ID_AddressSpaceInternal * Internal = (ID_AddressSpaceInternal*)AddressSpace;
    if (Internal->FastAccess != NULL) {
        free(Internal->FastAccess->references);
        free(Internal->FastAccess);
        Internal->FastAccess = NULL;
    }

    int i;
    for (i = Internal->count - 1; i >= 0; i--) {
        ID_AddressSpaceNode * node = Internal->allNodes[i];
        ID_AddressSpace_DestroyNode(Internal, node);
        Internal->allNodes[i] = NULL;
    }
    
    Internal->count = 0;
}

// Destroy the Address Space
void ID_AddressSpace_Destroy(ID_AddressSpaceHandle * AddressSpace) {
    ID_AddressSpaceInternal * aSpace = (ID_AddressSpaceInternal*)(*AddressSpace);
    ID_AddressSpace_Clear(*AddressSpace);
    OPCUA_P_MUTEX_DELETE(&aSpace->NodeMutex);
    free(aSpace->allNodes);
    free(aSpace);
    *AddressSpace = NULL;
}

// Checks if the AddressSpace is large enough
void ID_AddressSpace_CheckForRealloc(ID_AddressSpaceInternal * AddressSpaceInternal) {
    if (AddressSpaceInternal->count >= AddressSpaceInternal->capacity) {
        AddressSpaceInternal->capacity += AddressSpaceInternal->incrementBy;
        AddressSpaceInternal->allNodes = realloc(AddressSpaceInternal->allNodes, sizeof(ID_AddressSpaceNode*) * AddressSpaceInternal->capacity);
    }
}

// Allocates AddressSpaceNode
void ID_AddressSpace_CreateAddressSpaceNode(ID_AddressSpaceInternal * AddressSpaceInternal, ID_AddressSpaceNode ** NodePtr) {
    OPCUA_P_MUTEX_LOCK(AddressSpaceInternal->NodeMutex); // lock nodes

    ID_AddressSpace_CheckForRealloc(AddressSpaceInternal);
    ID_AddressSpaceNode * node = malloc(sizeof(ID_AddressSpaceNode));
    memset(node, 0, sizeof(ID_AddressSpaceNode));
    AddressSpaceInternal->allNodes[AddressSpaceInternal->count] = node;
    AddressSpaceInternal->count++;
    *NodePtr = node;

    OPCUA_P_MUTEX_UNLOCK(AddressSpaceInternal->NodeMutex);
}

// Creates Node
void ID_AddressSpace_CreateNode(ID_AddressSpaceHandle AddressSpace, void ** nodePtr, ID_NodeType nType, ID_AddressSpace_CustomFreeFunction * FreeFunction) {
    ID_AddressSpaceInternal * AddressSpaceInternalPtr = (ID_AddressSpaceInternal*)AddressSpace;

    // Allocates Node
    ID_AddressSpaceNode * Node = NULL;
    ID_AddressSpace_CreateAddressSpaceNode(AddressSpaceInternalPtr, &Node);

    // Setup default parameters
    Node->Type = nType;
    Node->FreeFunction = FreeFunction;
    ID_BaseAttributes * BaseAttrs = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(Node, Node->Type);
    OpcUa_QualifiedName_Initialize(&BaseAttrs->BrowseName);
    OpcUa_LocalizedText_Initialize(&BaseAttrs->DisplayName);
    BaseAttrs->NoOfReferences = 0;
    BaseAttrs->NodeClass = OpcUa_NodeClass_Unspecified;
    OpcUa_NodeId_Initialize(&BaseAttrs->NodeId);
    BaseAttrs->WriteMask = 0;
    BaseAttrs->References = NULL;
    OPCUA_P_MUTEX_CREATE(&BaseAttrs->RefMutex);
    OPCUA_P_MUTEX_CREATE(&BaseAttrs->NodeMutex);
    
    *nodePtr = ID_AddressSpace_AccessNodeBody(Node, nType);
}

// Creates DataTypeNode
void ID_AddressSpace_CreateDataTypeNode(ID_AddressSpaceHandle AddressSpace, ID_DataTypeNode ** DataTypeNode, ID_AddressSpace_CustomFreeFunction * FreeFunction) {
    ID_AddressSpace_CreateNode(AddressSpace, (void**)DataTypeNode, ID_NodeType_DataType, FreeFunction);
    (*DataTypeNode)->BaseAttribute.NodeClass = OpcUa_NodeClass_DataType;
    (*DataTypeNode)->IsAbstract = OpcUa_True;
}

// Creates MethodNode
void ID_AddressSpace_CreateMethodNode(ID_AddressSpaceHandle AddressSpace, ID_MethodNode ** MethodNode, ID_AddressSpace_CustomFreeFunction * FreeFunction) {
    ID_AddressSpace_CreateNode(AddressSpace, (void**)MethodNode, ID_NodeType_Method, FreeFunction);
    (*MethodNode)->BaseAttribute.NodeClass = OpcUa_NodeClass_Method;
    (*MethodNode)->Executable = OpcUa_False;
    (*MethodNode)->Function = NULL;
    (*MethodNode)->UserExecutable = OpcUa_False;
}

// Creates ObjectNode
void ID_AddressSpace_CreateObjectNode(ID_AddressSpaceHandle AddressSpace, ID_ObjectNode ** ObjectNode, ID_AddressSpace_CustomFreeFunction * FreeFunction) {
    ID_AddressSpace_CreateNode(AddressSpace, (void**)ObjectNode, ID_NodeType_Object, FreeFunction);
    (*ObjectNode)->BaseAttribute.NodeClass = OpcUa_NodeClass_Object;
    (*ObjectNode)->EventNotifier = (OpcUa_Byte)0;
}

// Creates ObjectTypeNode
void ID_AddressSpace_CreateObjectTypeNode(ID_AddressSpaceHandle AddressSpace, ID_ObjectTypeNode ** ObjectTypeNode, ID_AddressSpace_CustomFreeFunction * FreeFunction) {
    ID_AddressSpace_CreateNode(AddressSpace, (void**)ObjectTypeNode, ID_NodeType_ObjectType, FreeFunction);
    (*ObjectTypeNode)->BaseAttribute.NodeClass = OpcUa_NodeClass_ObjectType;
    (*ObjectTypeNode)->IsAbstract = OpcUa_True;
}

// Creates ReferenceTypeNode
void ID_AddressSpace_CreateReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, ID_ReferenceTypeNode ** ReferenceTypeNode, ID_AddressSpace_CustomFreeFunction * FreeFunction) {
    ID_AddressSpace_CreateNode(AddressSpace, (void**)ReferenceTypeNode, ID_NodeType_ReferenceType, FreeFunction);
    OpcUa_LocalizedText_Initialize(&(*ReferenceTypeNode)->InverseName);
    (*ReferenceTypeNode)->BaseAttribute.NodeClass = OpcUa_NodeClass_ReferenceType;
    (*ReferenceTypeNode)->IsAbstract = OpcUa_True;
    (*ReferenceTypeNode)->Symmetric = OpcUa_False;
}

// Creates VariableNode
void ID_AddressSpace_CreateVariableNode(ID_AddressSpaceHandle AddressSpace, ID_VariableNode ** VariableNode, ID_AddressSpace_CustomFreeFunction * FreeFunction) {
    ID_AddressSpace_CreateNode(AddressSpace, (void**)VariableNode, ID_NodeType_Variable, FreeFunction);
    (*VariableNode)->BaseAttribute.NodeClass = OpcUa_NodeClass_Variable;
    (*VariableNode)->ValueRank = OpcUa_ValueRanks_Any;
    (*VariableNode)->NoOfArrayDimensions = 0;
    (*VariableNode)->ArrayDimensions = NULL;
    (*VariableNode)->AccessLevel = 1;
    (*VariableNode)->UserAccessLevel = 1;
    (*VariableNode)->MinimumSamplingInterval = -1;
    (*VariableNode)->Historizing = OpcUa_False;
    OpcUa_NodeId_Initialize(&(*VariableNode)->DataType);
    OpcUa_Variant_Initialize(&(*VariableNode)->Value);
}

// Creates VariableTypeNode
void ID_AddressSpace_CreateVariableTypeNode(ID_AddressSpaceHandle AddressSpace, ID_VariableTypeNode ** VariableTypeNode, ID_AddressSpace_CustomFreeFunction * FreeFunction) {
    ID_AddressSpace_CreateNode(AddressSpace, (void**)VariableTypeNode, ID_NodeType_VariableType, FreeFunction);
    (*VariableTypeNode)->BaseAttribute.NodeClass = OpcUa_NodeClass_VariableType;
    (*VariableTypeNode)->IsAbstract = OpcUa_True;
    (*VariableTypeNode)->NoOfArrayDimensions = 0;
    (*VariableTypeNode)->ArrayDimensions = NULL;
    (*VariableTypeNode)->ValueRank = OpcUa_ValueRanks_Any;
    OpcUa_NodeId_Initialize(&(*VariableTypeNode)->DataType);
    OpcUa_Variant_Initialize(&(*VariableTypeNode)->Value);
}

// Creates ViewNode
void ID_AddressSpace_CreateViewNode(ID_AddressSpaceHandle AddressSpace, ID_ViewNode ** ViewNode, ID_AddressSpace_CustomFreeFunction * FreeFunction) {
    ID_AddressSpace_CreateNode(AddressSpace, (void**)ViewNode, ID_NodeType_View, FreeFunction);
    (*ViewNode)->BaseAttribute.NodeClass = OpcUa_NodeClass_View;
    (*ViewNode)->ContainsNoLoops = OpcUa_False;
    (*ViewNode)->EventNotifier = 0;
}

// Remove DataTypeNode
void ID_AddressSpace_RemoveDataTypeNode(ID_AddressSpaceHandle AddressSpace, ID_DataTypeNode * DataTypeNode) {
    ID_AddressSpace_RemoveNode(AddressSpace, &DataTypeNode->BaseAttribute.NodeId, ID_NodeType_DataType);
}

// Remove MethodNode
void ID_AddressSpace_RemoveMethodNode(ID_AddressSpaceHandle AddressSpace, ID_MethodNode * MethodNode) {
    ID_AddressSpace_RemoveNode(AddressSpace, &MethodNode->BaseAttribute.NodeId, ID_NodeType_Method);
}

// Remove ObjectNode
void ID_AddressSpace_RemoveObjectNode(ID_AddressSpaceHandle AddressSpace, ID_ObjectNode * ObjectNode) {
    ID_AddressSpace_RemoveNode(AddressSpace, &ObjectNode->BaseAttribute.NodeId, ID_NodeType_Object);
}

// Remove ObjectTypeNode
void ID_AddressSpace_RemoveObjectTypeNode(ID_AddressSpaceHandle AddressSpace, ID_ObjectTypeNode * ObjectTypeNode) {
    ID_AddressSpace_RemoveNode(AddressSpace, &ObjectTypeNode->BaseAttribute.NodeId, ID_NodeType_ObjectType);
}

// Remove ReferenceTypeNode
void ID_AddressSpace_RemoveReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, ID_ReferenceTypeNode * ReferenceTypeNode) {
    ID_AddressSpace_RemoveNode(AddressSpace, &ReferenceTypeNode->BaseAttribute.NodeId, ID_NodeType_ReferenceType);
}

// Remove VariableNode
void ID_AddressSpace_RemoveVariableNode(ID_AddressSpaceHandle AddressSpace, ID_VariableNode * VariableNode) {
    ID_AddressSpace_RemoveNode(AddressSpace, &VariableNode->BaseAttribute.NodeId, ID_NodeType_Variable);
}

// RemoveVariableTypeNode
void ID_AddressSpace_RemoveVariableTypeNode(ID_AddressSpaceHandle AddressSpace, ID_VariableTypeNode * VariableTypeNode) {
    ID_AddressSpace_RemoveNode(AddressSpace, &VariableTypeNode->BaseAttribute.NodeId, ID_NodeType_VariableType);
}

// Remove ViewNode
void ID_AddressSpace_RemoveViewNode(ID_AddressSpaceHandle AddressSpace, ID_ViewNode * ViewNode) {
    ID_AddressSpace_RemoveNode(AddressSpace, &ViewNode->BaseAttribute.NodeId, ID_NodeType_View);
}

// Remove Reference at index from Node
void ID_AddressSpace_RemoveReferenceFromArray(ID_BaseAttributes * Node, int Index) {
    OPCUA_P_MUTEX_LOCK(Node->RefMutex); // lock references

    // destroy reference
    ID_ReferenceNode * refNode = Node->References[Index];
    OpcUa_NodeId_Clear(&refNode->ReferenceTypeId);
    OpcUa_NodeId_Clear(&refNode->TargetNodeId);
    free(Node->References[Index]);
    
    // adjusts the pointers
    for (OpcUa_UInt32 j = Index + 1; j < Node->NoOfReferences; j++) {
        Node->References[j - 1] = Node->References[j];
    }
    
    Node->References[Node->NoOfReferences - 1] = NULL;
    Node->NoOfReferences--;

    OPCUA_P_MUTEX_UNLOCK(Node->RefMutex); // unlock references
}

// Remove all references with a target equivalent to NodeId
void ID_AddressSpace_RemoveReferencesToNodeId(ID_BaseAttributes * node, OpcUa_NodeId* NodeId) {
    for (OpcUa_UInt32 i = 0; i < node->NoOfReferences; i++) {
        ID_ReferenceNode * refNode = node->References[i];
        if (ID_AddressSpace_AreIdsEqual(&refNode->TargetNodeId, NodeId) == OpcUa_True) {
            ID_AddressSpace_RemoveReferenceFromArray(node, i);
            i--;
        }
    }
}

// Removes a Node from the Address Space
void ID_AddressSpace_RemoveNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId, ID_NodeType nType) {
    ID_AddressSpaceInternal * AddressSpaceInternalPtr = (ID_AddressSpaceInternal*)AddressSpace;
    OPCUA_P_MUTEX_LOCK(AddressSpaceInternalPtr->NodeMutex); // lock

    // This iterative function is not extremely fast because of the possibility that
    // one directional relationships might exist, which means that evaluating each
    // node is required to ensure that the address space remains valid in terms of
    // relationships between nodes
    OpcUa_Boolean IsDeleted = OpcUa_False;
    for (OpcUa_UInt32 i = 0; i < AddressSpaceInternalPtr->count; i++) {
        ID_AddressSpaceNode * node = AddressSpaceInternalPtr->allNodes[i];
        ID_BaseAttributes * nodeAttributes = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(node, nType);
        OpcUa_NodeId TargetNodeId = nodeAttributes->NodeId;
        if (OpcUa_NodeId_IsNull(&TargetNodeId)) {
            continue;
        }
        
        // bypass the comparison if target NodeId has been deleted (NodeId is unique)
        if (!IsDeleted && ID_AddressSpace_AreIdsEqual(NodeId, &TargetNodeId) == OpcUa_True) {
            ID_AddressSpace_DestroyNode(AddressSpaceInternalPtr, node);

            // fix address space pointers
            for (OpcUa_UInt32 j = i + 1; j < AddressSpaceInternalPtr->count; j++) {
                AddressSpaceInternalPtr->allNodes[j - 1] = AddressSpaceInternalPtr->allNodes[j];
            }
            
            // does not automatically contract
            AddressSpaceInternalPtr->allNodes[AddressSpaceInternalPtr->count - 1] = NULL;
            AddressSpaceInternalPtr->count--;
            i--;
            IsDeleted = OpcUa_True;
        } else {
            // Remove references from other nodes that might point to NodeId
            ID_AddressSpace_RemoveReferencesToNodeId(nodeAttributes, NodeId);
        }
    }

    OPCUA_P_MUTEX_UNLOCK(AddressSpaceInternalPtr->NodeMutex);
}

// Adds one directional reference pointing from Source to Target with type ReferenceTypeId and inverse IsInverse
void ID_AddressSpace_AddUniReference(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* Source, ID_NodeType Type, OpcUa_NodeId* Target, OpcUa_NodeId* ReferenceTypeId, OpcUa_Boolean IsInverse) {
    OpcUa_ReferenceParameter(Type);

    // Create Reference Node first
    OpcUa_UInt32 ReferenceNodeSize = sizeof(ID_ReferenceNode);
    ID_ReferenceNode * refNode = malloc(ReferenceNodeSize);
    memset(refNode, 0, sizeof(ID_ReferenceNode));
    ID_OpcUa_NodeId_CopyTo(ReferenceTypeId, &refNode->ReferenceTypeId);
    ID_OpcUa_NodeId_CopyTo(Target, &refNode->TargetNodeId);
    refNode->IsInverse = IsInverse;
    
    ID_BaseAttributes * attrs = ID_AddressSpace_GetNode(AddressSpace, Source);
    OPCUA_P_MUTEX_LOCK(attrs->RefMutex); // lock late to reduce time held

    // insert the reference node pointer
    attrs->NoOfReferences++;
    attrs->References = realloc(attrs->References, sizeof(ID_ReferenceNode*) * attrs->NoOfReferences);
    attrs->References[attrs->NoOfReferences - 1] = refNode;

    OPCUA_P_MUTEX_UNLOCK(attrs->RefMutex);
}

// Adds bidirectional reference which is equivalent to two unidirectional with Source/Target inverted
void ID_AddressSpace_AddBiReference(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* Source, ID_NodeType SourceType, OpcUa_NodeId* Target, ID_NodeType TargetType, OpcUa_NodeId* ReferenceId) {
    ID_AddressSpace_AddUniReference(AddressSpace, Source, SourceType, Target, ReferenceId, OpcUa_False);
    ID_AddressSpace_AddUniReference(AddressSpace, Target, TargetType, Source, ReferenceId, OpcUa_True);
}
 
 // Remove one directional reference from Source->Target with ReferneceTypeId and IsInverse   
void ID_AddressSpace_RemoveUniReference(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* Source, ID_NodeType SourceType, OpcUa_NodeId* Target, OpcUa_NodeId* ReferenceTypeId, OpcUa_Boolean IsInverse) {
	OpcUa_ReferenceParameter(SourceType);
    ID_BaseAttributes * attrs = ID_AddressSpace_GetNode(AddressSpace, Source);
    for (OpcUa_UInt32 i = 0; i < attrs->NoOfReferences; i++) {
        ID_ReferenceNode * RefNode = attrs->References[i];
        
        // check Target first because same ReferenceTypeId is more common
        if (ID_AddressSpace_AreIdsEqual(&RefNode->TargetNodeId, Target) == OpcUa_True &&
            ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, ReferenceTypeId) == OpcUa_True &&
            RefNode->IsInverse == IsInverse) {
            
            ID_AddressSpace_RemoveReferenceFromArray(attrs, i);
            return;
        }
    }
}

// Removes bireference between Source and Target
void ID_AddressSpace_RemoveBiReference(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* Source, ID_NodeType SourceType, OpcUa_NodeId* Target, ID_NodeType TargetType, OpcUa_NodeId* ReferenceId) {
    ID_AddressSpace_RemoveUniReference(AddressSpace, Source, SourceType, Target, ReferenceId, OpcUa_False);
    ID_AddressSpace_RemoveUniReference(AddressSpace, Target, TargetType, Source, ReferenceId, OpcUa_True);
}

// Get a node with NodeId
void * ID_AddressSpace_GetNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId * NodeId) {
    // trick is to use the fast cache in case NodeId is of HOT interest
    void * targetPtr = ID_AddressSpace_FastAccessNode(AddressSpace, NodeId);
    if (targetPtr != NULL) { return targetPtr; }

    // Find the NodeId in a traditional manner
    // attach a mutex to prevent a deletion operation from messing with this
    OPCUA_P_MUTEX_LOCK(((ID_AddressSpaceInternal*)AddressSpace)->NodeMutex);
    ID_AddressSpaceNode * Node = ID_AddressSpace_FindNodeWithID(AddressSpace, NodeId);
    void * body = NULL;
    if (Node != NULL) {
        body = ID_AddressSpace_AccessNodeBody(Node, Node->Type);
    }

    OPCUA_P_MUTEX_UNLOCK(((ID_AddressSpaceInternal*)AddressSpace)->NodeMutex);
    return body;
}

// Get DataTypeNode with NodeId
ID_DataTypeNode * ID_AddressSpace_GetDataTypeNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId) {
    return ID_AddressSpace_GetNode(AddressSpace, NodeId);
}

// Get MethodNode with NodeId
ID_MethodNode * ID_AddressSpace_GetMethodNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId) {
    return ID_AddressSpace_GetNode(AddressSpace, NodeId);
}

// Get ObjectNode with NodeId
ID_ObjectNode * ID_AddressSpace_GetObjectNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId) {
    return ID_AddressSpace_GetNode(AddressSpace, NodeId);
}

// Get ObjectTypeNode with NodeId
ID_ObjectTypeNode * ID_AddressSpace_GetObjectTypeNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId) {
    return ID_AddressSpace_GetNode(AddressSpace, NodeId);
}

// Get ReferenceTypeNode with NodeId
ID_ReferenceTypeNode * ID_AddressSpace_GetReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId) {
    return ID_AddressSpace_GetNode(AddressSpace, NodeId);
}

// Get VariableNode with NodeId
ID_VariableNode * ID_AddressSpace_GetVariableNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId) {
    return ID_AddressSpace_GetNode(AddressSpace, NodeId);
}

// Get VariableTypeNode with NodeId
ID_VariableTypeNode * ID_AddressSpace_GetVariableTypeNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId) {
    return ID_AddressSpace_GetNode(AddressSpace, NodeId);
}

// Get ViewNode with NodeId
ID_ViewNode * ID_AddressSpace_GetViewNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId) {
    return ID_AddressSpace_GetNode(AddressSpace, NodeId);
}

#pragma warning(push)
#pragma warning(disable : 4204)
// Inherits references held by Source node into the Target node
// Property and Component references are only inherited if in the FORWARD direction
// HasSubtype references always skipped; Optionals are always skipped
void ID_AddressSpace_InheritReferences(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, OpcUa_NodeId * Source, OpcUa_NodeId * Target) {
    OpcUa_NodeId ID_HasModellingRuleNode = ID_HasModellingRule;
    OpcUa_NodeId ID_HasPropertyNode = ID_HasProperty;
    OpcUa_NodeId ID_HasComponentNode = ID_HasComponent;
    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;
    OpcUa_NodeId ID_ModellingRuleExposesItsArrayNode = ID_ModellingRuleExposesItsArray;
    OpcUa_NodeId ID_ModellingRuleMandatoryPlaceholderNode = ID_ModellingRuleMandatoryPlaceholder;
    OpcUa_NodeId ID_ModellingRuleOptionalPlaceholderNode = ID_ModellingRuleOptionalPlaceholder;
    OpcUa_NodeId ID_HasSubtypeNode = ID_HasSubtype;
    OpcUa_NodeId ID_HasTypeDefinitionNode = ID_HasTypeDefinition;
    OpcUa_NodeId ID_OrganizesNode = ID_Organizes;
    
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Inheriting references from Source NS:%d, ID:%d\n", Source->NamespaceIndex, Source->Identifier.Numeric);
    ID_AddressSpaceNode * SourceASN = ID_AddressSpace_FindNodeWithID(AddressSpace, Source);
    ID_AddressSpaceNode * TargetASN = ID_AddressSpace_FindNodeWithID(AddressSpace, Target);
    ID_BaseAttributes * SourceBaseAttributes = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(SourceASN, SourceASN->Type);
    ID_BaseAttributes * TargetBaseAttributes = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(TargetASN, TargetASN->Type);
    
    for (OpcUa_UInt32 i = 0; i < SourceBaseAttributes->NoOfReferences; i++) {
        ID_ReferenceNode * RefNode = SourceBaseAttributes->References[i];
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Reference ID %u for %s with %s\n", RefNode->ReferenceTypeId.Identifier.Numeric, OpcUa_String_GetRawString(&SourceBaseAttributes->BrowseName.Name), RefNode->IsInverse == OpcUa_True ? "Backwards" : "Forwards");
        if (ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasModellingRuleNode) == OpcUa_True) {
            // preserve the modelling rule upon inheriting references of interest
            // if the original is mandatory, then clone is also mandatory
            ID_AddressSpace_AddUniReference(AddressSpace, &TargetBaseAttributes->NodeId, TargetASN->Type, &RefNode->TargetNodeId, &RefNode->ReferenceTypeId, RefNode->IsInverse);
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Inserted Modelling Rule");
        } else if ((ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasPropertyNode) == OpcUa_True || ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasComponentNode) == OpcUa_True)) {
            // only inherit property and component references if held in the forward direction
            if (RefNode->IsInverse == OpcUa_True) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Skipping backwards reference\n");
                continue;
            }
           
            // the pointer may not necessarily be a variable node, hence work in terms of the BaseAttributes()
            ID_AddressSpaceNode * RefASNNode = ID_AddressSpace_FindNodeWithID(AddressSpace, &RefNode->TargetNodeId);
            ID_BaseAttributes * RefTargetNode = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(RefASNNode, RefASNNode->Type);

            // verify whether inheritance rule specifies allocation
            // modellingRule optionals are always skipped in current implementation
            // regardless of whether cloning subtypes or actual objects as Optional
            // mandatory rules are never obeyed by the implementation
            int shouldInherit = 0;
            for (OpcUa_UInt32 ref = 0; ref < RefTargetNode->NoOfReferences; ref++) {
                ID_ReferenceNode * VarNodeReference = RefTargetNode->References[ref];
                if (ID_AddressSpace_AreIdsEqual(&VarNodeReference->ReferenceTypeId, &ID_HasModellingRuleNode) == OpcUa_True &&
                    (ID_AddressSpace_AreIdsEqual(&VarNodeReference->TargetNodeId, &ID_ModellingRuleMandatoryNode) == OpcUa_True ||
                     (
                      // if the target is of objecttype/variabletype then inherit if the variable node
                      // possesses any placehodler or exposesitsarray references (treated as mandatory in this case only)
                      (TargetBaseAttributes->NodeClass == OpcUa_NodeClass_ObjectType || TargetBaseAttributes->NodeClass == OpcUa_NodeClass_VariableType) &&
                      (ID_AddressSpace_AreIdsEqual(&VarNodeReference->TargetNodeId, &ID_ModellingRuleOptionalPlaceholderNode) == OpcUa_True ||
                       ID_AddressSpace_AreIdsEqual(&VarNodeReference->TargetNodeId, &ID_ModellingRuleMandatoryPlaceholderNode) == OpcUa_True ||
                       ID_AddressSpace_AreIdsEqual(&VarNodeReference->TargetNodeId, &ID_ModellingRuleExposesItsArrayNode) == OpcUa_True
                      )
                     )
                    )) {

                    shouldInherit = 1;
                    break;
                }
            }
            
            if (shouldInherit == 0) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Skipping non-mandatory rule\n");
                continue;
            }

            // check to make sure that the BrowseName being inherited is unique within context of the node
            int isFound = 0;
            for (OpcUa_UInt32 ref = 0; ref < TargetBaseAttributes->NoOfReferences; ref++) {
                ID_BaseAttributes * ExistingNode = ID_AddressSpace_GetNode(AddressSpace, &TargetBaseAttributes->References[ref]->TargetNodeId);
                if (ExistingNode == NULL) {
                    continue;
                }
                
                if (RefTargetNode->BrowseName.NamespaceIndex == ExistingNode->BrowseName.NamespaceIndex) {
                    char * RefTargetBrowse = OpcUa_String_GetRawString(&RefTargetNode->BrowseName.Name);
                    char * RefExistingBrowse = OpcUa_String_GetRawString(&ExistingNode->BrowseName.Name);
                    if (RefTargetBrowse != NULL && RefExistingBrowse != NULL && strcmp(RefTargetBrowse, RefExistingBrowse) == 0) {
                        isFound = 1;
                        break;
                    }
                }
            }
            
            if (isFound != 0) {
                continue;
            }
            
            // Get a new Nodeid, clone the node, and then add the reciprocal relationship
            OpcUa_NodeId NewNodeId = { OpcUa_IdType_Numeric, ID_NAMESPACE_INDEX, ID_Indexer_GetNextIndex(Indexer) };
            ID_AddressSpace_CloneNode(AddressSpace, Indexer, RefNode->TargetNodeId, &NewNodeId, NULL);
            ID_AddressSpace_AddBiReference(AddressSpace, &TargetBaseAttributes->NodeId, TargetASN->Type, &NewNodeId, RefASNNode->Type, &RefNode->ReferenceTypeId);
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Created Property in the forward direction");
        } else if (ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasSubtypeNode) == OpcUa_True) {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Skipping HasSubtype reference\n");
        } else if (ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasTypeDefinitionNode) == OpcUa_True) {
            // Inherit HasTypeDefiniton reference if its in the forward direction (object->type, or variable->type)
            if (RefNode->IsInverse == OpcUa_False) {
                ID_AddressSpaceNode * TargetNode = ID_AddressSpace_FindNodeWithID(AddressSpace, &RefNode->TargetNodeId);
                ID_AddressSpace_AddBiReference(AddressSpace, &TargetBaseAttributes->NodeId, TargetASN->Type, &RefNode->TargetNodeId, TargetNode->Type, &RefNode->ReferenceTypeId);
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Inserted HasTypeDefinition reference\n");
            } else {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Skipping HasTypeDefinition reference reverse\n");
            }
        } else if (ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_OrganizesNode) == OpcUa_False || SourceASN->Type == TargetASN->Type) {
            // if the reference is not of type Organizes OR if it is and the types are equivalent
            // then inherit the reference of interest (this is in a way a catch-all to all references not directly handled above)
            // this may or may not be a problem
            if (RefNode->IsInverse == OpcUa_False) {
                ID_AddressSpaceNode * TargetNode = ID_AddressSpace_FindNodeWithID(AddressSpace, &RefNode->TargetNodeId);
                ID_AddressSpace_AddBiReference(AddressSpace, &TargetBaseAttributes->NodeId, TargetASN->Type, &RefNode->TargetNodeId, TargetNode->Type, &RefNode->ReferenceTypeId);
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Inserted forward reference\n");
            } else if (RefNode->IsInverse == OpcUa_True) {
                ID_AddressSpaceNode * SourceNode = ID_AddressSpace_FindNodeWithID(AddressSpace, &RefNode->TargetNodeId);
                ID_AddressSpace_AddBiReference(AddressSpace, &RefNode->TargetNodeId, SourceNode->Type, &TargetBaseAttributes->NodeId, TargetASN->Type, &RefNode->ReferenceTypeId);
                OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Inserted backward reference relation\n");
            }
        } else {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Found unhandled reference!");
        }
    }
}
#pragma warning(pop)

// Clone Node with Id
void ID_AddressSpace_CloneNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, OpcUa_NodeId Node, OpcUa_NodeId * CloneNodeId, void ** ClonedNode) {
    ID_AddressSpaceInternal * Internal = AddressSpace;
    ID_AddressSpaceNode * ASNode = ID_AddressSpace_FindNodeWithID(Internal, &Node);
    
    // Create the new node
    void * LocalNode;
    ID_AddressSpace_CreateNode(AddressSpace, &LocalNode, ASNode->Type, ASNode->FreeFunction);
    
    // manual copy attributes of interest
    ID_BaseAttributes * OriginalBaseAttributes = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(ASNode, ASNode->Type);
    ID_BaseAttributes * NewBaseAttributes = LocalNode; // force recast to start of pointer
    NewBaseAttributes->BrowseName.NamespaceIndex = OriginalBaseAttributes->BrowseName.NamespaceIndex;
    NewBaseAttributes->NodeClass = OriginalBaseAttributes->NodeClass;
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&NewBaseAttributes->BrowseName.Name, &OriginalBaseAttributes->BrowseName.Name);
    ID_COPY_DISPLAY_NAME_FROM_NODE_TO_NODE(OriginalBaseAttributes, NewBaseAttributes);
    ID_OpcUa_NodeId_CopyTo(CloneNodeId, &NewBaseAttributes->NodeId);
    
    // Inherit References
    ID_AddressSpace_InheritReferences(AddressSpace, Indexer, &OriginalBaseAttributes->NodeId, &NewBaseAttributes->NodeId);

    // Copy items specific to interest
    switch (ASNode->Type) {
        case ID_NodeType_DataType: ((ID_DataTypeNode*)LocalNode)->IsAbstract = ASNode->Node.DTypeNode.IsAbstract; break;
        case ID_NodeType_Object: ((ID_ObjectNode*)LocalNode)->EventNotifier = ASNode->Node.ObjNode.EventNotifier; break;
        case ID_NodeType_ObjectType: ((ID_ObjectTypeNode*)LocalNode)->IsAbstract = ASNode->Node.ObjTypeNode.IsAbstract; break;
        case ID_NodeType_Method:
            ((ID_MethodNode*)LocalNode)->Executable = ASNode->Node.MNode.Executable;
            ((ID_MethodNode*)LocalNode)->Function = ASNode->Node.MNode.Function;
            ((ID_MethodNode*)LocalNode)->UserExecutable = ASNode->Node.MNode.UserExecutable;
            break;
        case ID_NodeType_ReferenceType:
            ((ID_ReferenceTypeNode*)LocalNode)->IsAbstract = ASNode->Node.RefTypeNode.IsAbstract;
            ((ID_ReferenceTypeNode*)LocalNode)->Symmetric = ASNode->Node.RefTypeNode.Symmetric;
            ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&((ID_ReferenceTypeNode*)LocalNode)->InverseName.Locale, &ASNode->Node.RefTypeNode.InverseName.Locale);
            ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&((ID_ReferenceTypeNode*)LocalNode)->InverseName.Text, &ASNode->Node.RefTypeNode.InverseName.Text);
            break;
        case ID_NodeType_Variable:{
            ((ID_VariableNode*)LocalNode)->AccessLevel = ASNode->Node.VarNode.AccessLevel;
            ((ID_VariableNode*)LocalNode)->Historizing = ASNode->Node.VarNode.Historizing;
            ((ID_VariableNode*)LocalNode)->MinimumSamplingInterval = ASNode->Node.VarNode.MinimumSamplingInterval;
            ((ID_VariableNode*)LocalNode)->UserAccessLevel = ASNode->Node.VarNode.UserAccessLevel;
            ((ID_VariableNode*)LocalNode)->ValueRank = ASNode->Node.VarNode.ValueRank;
            ID_OpcUa_NodeId_CopyTo(&ASNode->Node.VarNode.DataType, &((ID_VariableNode*)LocalNode)->DataType);
            ID_AddressSpace_CopyVariableFromType((ID_VariableTypeNode*)&ASNode->Node.VarNode, (ID_VariableNode*)LocalNode);
            break;}
        case ID_NodeType_VariableType:{
            ((ID_VariableTypeNode*)LocalNode)->IsAbstract = ASNode->Node.VarTypeNode.IsAbstract;
            ((ID_VariableTypeNode*)LocalNode)->ValueRank = ASNode->Node.VarTypeNode.ValueRank;
            ID_OpcUa_NodeId_CopyTo(&ASNode->Node.VarTypeNode.DataType, &((ID_VariableTypeNode*)LocalNode)->DataType);
            ID_AddressSpace_CopyVariableFromType(&ASNode->Node.VarTypeNode, (ID_VariableNode*)LocalNode);
            break;}
        case ID_NodeType_View:
            ((ID_ViewNode*)LocalNode)->ContainsNoLoops = ASNode->Node.VNode.ContainsNoLoops;
            ((ID_ViewNode*)LocalNode)->EventNotifier = ASNode->Node.VNode.EventNotifier;
            break;
        default:
            break;
    }
    
    if (ClonedNode != NULL) {
        *ClonedNode = LocalNode;
    }
}

// Clone DataTypeNode
void ID_AddressSpace_CloneDataTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_DataTypeNode * Orig, OpcUa_NodeId * CloneNodeId, ID_DataTypeNode ** Cloned) {
    ID_AddressSpace_CloneNode(AddressSpace, Indexer, Orig->BaseAttribute.NodeId, CloneNodeId, (void**)Cloned);
}

// Clone MethodNode
void ID_AddressSpace_CloneMethodNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_MethodNode * Orig, OpcUa_NodeId * CloneNodeId, ID_MethodNode ** Cloned) {
    ID_AddressSpace_CloneNode(AddressSpace, Indexer, Orig->BaseAttribute.NodeId, CloneNodeId, (void**)Cloned);
}

// Clone ObjectNode
void ID_AddressSpace_CloneObjectNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_ObjectNode * Orig, OpcUa_NodeId * CloneNodeId, ID_ObjectNode ** Cloned) {
    ID_AddressSpace_CloneNode(AddressSpace, Indexer, Orig->BaseAttribute.NodeId, CloneNodeId, (void**)Cloned);
}

// Clone ObjectTypeNode
void ID_AddressSpace_CloneObjectTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_ObjectTypeNode * Orig, OpcUa_NodeId * CloneNodeId, ID_ObjectTypeNode ** Cloned) {
    ID_AddressSpace_CloneNode(AddressSpace, Indexer, Orig->BaseAttribute.NodeId, CloneNodeId, (void**)Cloned);
}

// Clone ReferenceTypeNode
void ID_AddressSpace_CloneReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_ReferenceTypeNode * Orig, OpcUa_NodeId * CloneNodeId, ID_ReferenceTypeNode ** Cloned) {
    ID_AddressSpace_CloneNode(AddressSpace, Indexer, Orig->BaseAttribute.NodeId, CloneNodeId, (void**)Cloned);
}

// Clone VariableNode
void ID_AddressSpace_CloneVariableNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_VariableNode * Orig, OpcUa_NodeId * CloneNodeId, ID_VariableTypeNode ** Cloned) {
    ID_AddressSpace_CloneNode(AddressSpace, Indexer, Orig->BaseAttribute.NodeId, CloneNodeId, (void**)Cloned);
}

// Clone VariableTypeNode
void ID_AddressSpace_CloneVariableTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_VariableTypeNode * Orig, OpcUa_NodeId * CloneNodeId, ID_VariableTypeNode ** Cloned) {
    ID_AddressSpace_CloneNode(AddressSpace, Indexer, Orig->BaseAttribute.NodeId, CloneNodeId, (void**)Cloned);
}

// Clone ViewNode
void ID_AddressSpace_CloneViewNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_ViewNode * Orig, OpcUa_NodeId * CloneNodeId, ID_ViewNode ** Cloned) {
    ID_AddressSpace_CloneNode(AddressSpace, Indexer, Orig->BaseAttribute.NodeId, CloneNodeId, (void**)Cloned);
}

// Replace a NodeId of an existing address space node
void ID_AddressSpace_MapNodeToId(ID_AddressSpaceHandle Handle, OpcUa_NodeId * Original, OpcUa_NodeId * New) {
    OpcUa_NodeId ID_HasModellingRuleNode = ID_HasModellingRule;

    ID_AddressSpaceNode * Node = ID_AddressSpace_FindNodeWithID(Handle, Original);
    ID_BaseAttributes * Attrs = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(Node, Node->Type);
    for (OpcUa_UInt32 i = 0; i < Attrs->NoOfReferences; i++) {
        ID_ReferenceNode * RefNode = Attrs->References[i];

        // ignore modelling rule as they are unidirectional references
        if (ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasModellingRuleNode) != OpcUa_True) {
            ID_AddressSpaceNode * RefTargetNode = ID_AddressSpace_FindNodeWithID(Handle, &RefNode->TargetNodeId);
            if (RefTargetNode != NULL) {
                // Get the Node being pointed to as the destination
                ID_BaseAttributes * RefTargetAttrs = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(RefTargetNode, RefTargetNode->Type);
                for (OpcUa_UInt32 j = 0; j < RefTargetAttrs->NoOfReferences; j++) {
                    ID_ReferenceNode * TargetRefNode = RefTargetAttrs->References[j];

                    // check to make sure that referencetype is same as original pointed to
                    // and is the reciprocal Of the 
                    if (ID_AddressSpace_AreIdsEqual(&TargetRefNode->TargetNodeId, Original) == OpcUa_True &&
                        ID_AddressSpace_AreIdsEqual(&TargetRefNode->ReferenceTypeId, &RefNode->ReferenceTypeId) == OpcUa_True &&
                        ((RefNode->IsInverse == OpcUa_True && TargetRefNode->IsInverse == OpcUa_False) || 
                         (RefNode->IsInverse == OpcUa_False && TargetRefNode->IsInverse == OpcUa_True) ) ) {

                        // replace the target of the reference
                        ID_OpcUa_NodeId_CopyTo(New, &TargetRefNode->TargetNodeId);
                    }
                }
            }
        }
    }
    
    // Replace the identifier at the end
    ID_OpcUa_NodeId_CopyTo(New, &Attrs->NodeId);
}

// Creates a variable from a given variable type
ID_VariableNode * ID_AddressSpace_CreateVariableFromVariableType(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, ID_VariableTypeNode * VarTypeNode, OpcUa_NodeId * NodeId) {
    OpcUa_NodeId ID_HasTypeDefinitionNode = ID_HasTypeDefinition;
    ID_VariableNode * VarNode = NULL;
    ID_AddressSpace_CreateVariableNode(Handle, &VarNode, NULL);
    ID_OpcUa_NodeId_CopyTo(&VarTypeNode->DataType, &VarNode->DataType);
    ID_OpcUa_NodeId_CopyTo(NodeId, &VarNode->BaseAttribute.NodeId);

    // inherits common references of interest including sub nodes
    ID_AddressSpace_InheritReferences(Handle, Indexer, &VarTypeNode->BaseAttribute.NodeId, NodeId);

    // copies the value
    ID_AddressSpace_CopyVariableFromType(VarTypeNode, VarNode);

    // add the type definition reference from Variable back to VariableType
    ID_AddressSpace_AddBiReference(Handle, &VarNode->BaseAttribute.NodeId, ID_NodeType_Variable, &VarTypeNode->BaseAttribute.NodeId, ID_NodeType_VariableType, &ID_HasTypeDefinitionNode);
    return VarNode;
}

// Creates an object from a given object type
ID_ObjectNode * ID_AddressSpace_CreateObjectFromObjectType(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, ID_ObjectTypeNode * ObjTypeNode, OpcUa_NodeId * NodeId) {
    OpcUa_NodeId ID_HasTypeDefinitionNode = ID_HasTypeDefinition;
    
    ID_ObjectNode * ObjNode = NULL;
    ID_AddressSpace_CreateObjectNode(Handle, &ObjNode, NULL);
    ID_OpcUa_NodeId_CopyTo(NodeId, &ObjNode->BaseAttribute.NodeId);

    // inherits common references of interest
    ID_AddressSpace_InheritReferences(Handle, Indexer, &ObjTypeNode->BaseAttribute.NodeId, NodeId);

    // add the type definition reference from Object back to ObjectType
    ID_AddressSpace_AddBiReference(Handle, NodeId, ID_NodeType_Object, &ObjTypeNode->BaseAttribute.NodeId, ID_NodeType_ObjectType, &ID_HasTypeDefinitionNode);
    return ObjNode;
}

// Ensure that Argument on a rudimentary basis inherited correctly;
void ID_assertArgumentConfigurationInherited(ID_VariableNode * VarNode) {
    ID_ASSERT(VarNode->Value.Datatype == OpcUaType_ExtensionObject);
    ID_ASSERT(VarNode->Value.Value.Array.Length != 0);
    ID_ASSERT(VarNode->Value.Value.Array.Value.ExtensionObjectArray != NULL);
    ID_ASSERT(VarNode->Value.Value.Array.Value.ExtensionObjectArray[0].Body.EncodeableObject.Type->TypeId == OpcUaId_Argument);
    OpcUa_Argument * Arg = VarNode->Value.Value.Array.Value.ExtensionObjectArray[0].Body.EncodeableObject.Object;
    char * Name = OpcUa_String_GetRawString(&Arg->Name);
    ID_ASSERT(Name != NULL);
    ID_ASSERT(strlen(Name) != 0);
}

// Clones a method
ID_MethodNode * ID_AddressSpace_CloneMethod(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, OpcUa_NodeId * MethodId, OpcUa_NodeId * NewNodeId, OpcUa_NodeId * InputId, OpcUa_NodeId * OutputId) {
    OpcUa_NodeId ID_HasPropertyNode = ID_HasProperty; 
    ID_MethodNode * Method1 = ID_AddressSpace_GetMethodNode(Handle, MethodId);
    
    // Copy common parameters
    ID_MethodNode * MethodNode = NULL;
    ID_AddressSpace_CreateMethodNode(Handle, &MethodNode, NULL);
    MethodNode->Executable = Method1->Executable;
    MethodNode->Function = Method1->Function;
    MethodNode->UserExecutable = Method1->UserExecutable;
    ID_OpcUa_NodeId_CopyTo(NewNodeId, &MethodNode->BaseAttribute.NodeId);

    // inherit the references of interest
    ID_AddressSpace_InheritReferences(Handle, Indexer, &Method1->BaseAttribute.NodeId, NewNodeId);
    
    // remap the input and output arguments as necessary
    for (OpcUa_UInt32 i = 0; i < MethodNode->BaseAttribute.NoOfReferences; i++) {
        ID_ReferenceNode * RefNode = MethodNode->BaseAttribute.References[i];
        if (RefNode->IsInverse == OpcUa_False && ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasPropertyNode) == OpcUa_True) {
            ID_VariableNode * VarNode = ID_AddressSpace_GetVariableNode(Handle, &RefNode->TargetNodeId);
            if (VarNode != NULL && InputId != NULL && VarNode->BaseAttribute.BrowseName.NamespaceIndex == 0 &&
                strcmp(OpcUa_String_GetRawString(&VarNode->BaseAttribute.BrowseName.Name), "InputArguments") == 0) {
                // check to make sure that variable is inherited correctly
                ID_assertArgumentConfigurationInherited(VarNode);
                ID_AddressSpace_MapNodeToId(Handle, &VarNode->BaseAttribute.NodeId, InputId);
            } else if (VarNode != NULL && OutputId != NULL && VarNode->BaseAttribute.BrowseName.NamespaceIndex == 0 &&
                       strcmp(OpcUa_String_GetRawString(&VarNode->BaseAttribute.BrowseName.Name), "OutputArguments") == 0) {
                ID_assertArgumentConfigurationInherited(VarNode);
                ID_AddressSpace_MapNodeToId(Handle, &VarNode->BaseAttribute.NodeId, OutputId);
            }
        }
    }
    
    return MethodNode;
}

// Build references from TargetNode with Reference to nodes of Type
void ID_AddressSpace_BuildReferencesOfTypeToNode(ID_AddressSpaceHandle Handle, ID_NodeType Type, OpcUa_NodeId * TargetNode, OpcUa_NodeId * Reference) {
    ID_AddressSpaceInternal * Internal = Handle;
    ID_AddressSpaceNode * TargetASNode = ID_AddressSpace_FindNodeWithID(Internal, TargetNode);
    ID_BaseAttributes * TargetAttrs = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(TargetASNode, TargetASNode->Type);
    
    for (int i = Internal->count - 1; i >= 0; i--) {
        ID_AddressSpaceNode * Node = Internal->allNodes[i];
        if (Node->Type == Type) {
            ID_BaseAttributes * Attrs = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(Node, Node->Type);
            if (ID_AddressSpace_AreIdsEqual(&Attrs->NodeId, TargetNode) != OpcUa_True) {
                OpcUa_Boolean isFound = OpcUa_False;
                for (OpcUa_UInt32 r = 0; r < Attrs->NoOfReferences; r++) {
                    ID_ReferenceNode * Ref = Attrs->References[r];

                    // ensure that the reference does not already exist
                    if (ID_AddressSpace_AreIdsEqual(&Ref->ReferenceTypeId, Reference) == OpcUa_True &&
                        ID_AddressSpace_AreIdsEqual(&Ref->TargetNodeId, TargetNode) == OpcUa_True) {
                        isFound = OpcUa_True;
                        break;
                    }
                }
                
                if (!isFound) {
                    // add bireference
                    ID_AddressSpace_AddBiReference(Handle, &TargetAttrs->NodeId, TargetASNode->Type, &Attrs->NodeId, Node->Type, Reference);
                }
            }
        }
    }
}