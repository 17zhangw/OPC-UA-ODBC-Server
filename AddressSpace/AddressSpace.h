/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ADDRESSSPACE_H
#define ADDRESSSPACE_H

#include "../Definitions/id_opcua.h"
#include "../Indexer/Indexer.h"
#include "OPCDerivations.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // Handle definition
    typedef void * ID_AddressSpaceHandle;
    OpcUa_NodeId ID_ExportNamespaceTypeNode;
    
    // Enum to identify Node Type
    typedef enum {
        ID_NodeType_ReferenceType,
        ID_NodeType_View,
        ID_NodeType_Object,
        ID_NodeType_ObjectType,
        ID_NodeType_Variable,
        ID_NodeType_VariableType,
        ID_NodeType_Method,
        ID_NodeType_DataType
    } ID_NodeType;
    
    // Struct to represent a relationship from one node to another
    typedef struct {
        OpcUa_NodeId            ReferenceTypeId;
        OpcUa_Boolean           IsInverse;
        OpcUa_NodeId            TargetNodeId;
    } ID_ReferenceNode;
    
    // Base Attributes for all nodes
    // RefMutex used to synchronize mutation of References
    typedef struct {
        OpcUa_NodeId            NodeId;
        OpcUa_NodeClass         NodeClass;
        OpcUa_QualifiedName     BrowseName;
        OpcUa_LocalizedText     DisplayName;
        OpcUa_UInt32            WriteMask;
        OpcUa_UInt32            NoOfReferences;
        ID_ReferenceNode **     References;
        OpcUa_Mutex             RefMutex;
        OpcUa_Mutex             NodeMutex;
    } ID_BaseAttributes;

    // Definition of all node types in accordance with Part 3
    typedef struct {
        ID_BaseAttributes       BaseAttribute;
        OpcUa_Boolean           IsAbstract;
        OpcUa_Boolean           Symmetric;
        OpcUa_LocalizedText     InverseName;
    } ID_ReferenceTypeNode;
    
    typedef struct {
        ID_BaseAttributes       BaseAttribute;
        OpcUa_Byte              EventNotifier;
        OpcUa_Boolean           ContainsNoLoops;
    } ID_ViewNode;
    
    typedef struct {
        ID_BaseAttributes       BaseAttribute;
        OpcUa_Byte              EventNotifier;
    } ID_ObjectNode;
    
    typedef struct {
        ID_BaseAttributes       BaseAttribute;
        OpcUa_Boolean           IsAbstract;
    } ID_ObjectTypeNode;
    
    typedef struct {
        ID_BaseAttributes       BaseAttribute;
        OpcUa_Variant           Value;
        OpcUa_NodeId            DataType;
        OpcUa_Int32             ValueRank;
        OpcUa_UInt32            NoOfArrayDimensions;
        OpcUa_UInt32 *          ArrayDimensions;
        OpcUa_Byte              AccessLevel;
        OpcUa_Byte              UserAccessLevel;
        OpcUa_Double            MinimumSamplingInterval; // Duration datatype - double
        OpcUa_Boolean           Historizing;
    } ID_VariableNode;
    
    typedef struct {
        ID_BaseAttributes       BaseAttribute;
        OpcUa_Variant           Value;
        OpcUa_NodeId            DataType;
        OpcUa_Int32             ValueRank;
        OpcUa_UInt32            NoOfArrayDimensions;
        OpcUa_UInt32 *          ArrayDimensions;
        OpcUa_Boolean           IsAbstract;
    } ID_VariableTypeNode;
    
    // Function pointer used to define function target of Method node
    typedef OpcUa_StatusCode (ID_Method_FuncPtr)(OpcUa_NodeId * AuthToken, int NoOfInputArguments, OpcUa_Variant * InputArguments, int * NoOfOutputArguments, OpcUa_Variant ** OutputArguments, OpcUa_StatusCode * InputCodes);
    typedef struct {
        ID_BaseAttributes       BaseAttribute;
        OpcUa_Boolean           Executable;
        OpcUa_Boolean           UserExecutable;
        ID_Method_FuncPtr *     Function;
    } ID_MethodNode;
    
    typedef struct {
        ID_BaseAttributes       BaseAttribute;
        OpcUa_Boolean           IsAbstract;
    } ID_DataTypeNode;
    
    // Definition of a Node stored in an AddressSpace utilizing the union mechanic of static structs
    // Struct works inline and does not rely on pointer (slightly higher memory consumption)
    typedef void (ID_AddressSpace_CustomFreeFunction)(void * Node);
    typedef struct {
        ID_NodeType Type;
        union {
            ID_ReferenceTypeNode RefTypeNode;
            ID_ViewNode VNode;
            ID_ObjectNode ObjNode;
            ID_ObjectTypeNode ObjTypeNode;
            ID_VariableNode VarNode;
            ID_VariableTypeNode VarTypeNode;
            ID_MethodNode MNode;
            ID_DataTypeNode DTypeNode;
        } Node;
        ID_AddressSpace_CustomFreeFunction * FreeFunction;
    } ID_AddressSpaceNode;
    
    /* AddressSpace control functions */

    // Creates AddressSpace with default capcity
    void ID_AddressSpace_Create(ID_AddressSpaceHandle * AddressSpace);

    // Creates Address Space with initial capacity
    void ID_AddressSpace_CreateWithInitialCapacity(ID_AddressSpaceHandle * AddressSpace, OpcUa_UInt32 capacity);

    // Retrieve number of items in Address Space
    OpcUa_UInt32 ID_AddressSpace_GetSize(ID_AddressSpaceHandle AddressSpace);

    // Access the inner body of an AddressSpaceNode
    void * ID_AddressSpace_AccessNodeBody(ID_AddressSpaceNode * node, ID_NodeType Type);

    // Analyze Address Space for id conflicts
    void ID_AddressSpace_AnalyzeConflicts(ID_AddressSpaceHandle AddressSpace);

    // PUrge all elements in Address Space
    void ID_AddressSpace_Clear(ID_AddressSpaceHandle  AddressSpace);

    // Destroy Address Space
    void ID_AddressSpace_Destroy(ID_AddressSpaceHandle * AddressSpace);
    
    /* Functions to create a raw unconfigured node */

    // Creates a new Node
    void ID_AddressSpace_CreateNode(ID_AddressSpaceHandle AddressSpace, void ** nodePtr, ID_NodeType nType, ID_AddressSpace_CustomFreeFunction * FreeFunction);
    
    // Creates a DataTypeNode
    void ID_AddressSpace_CreateDataTypeNode(ID_AddressSpaceHandle AddressSpace, ID_DataTypeNode ** DataTypeNode, ID_AddressSpace_CustomFreeFunction * FreeFunction);
    
    // Creates MethodNode
    void ID_AddressSpace_CreateMethodNode(ID_AddressSpaceHandle AddressSpace, ID_MethodNode ** MethodNode, ID_AddressSpace_CustomFreeFunction * FreeFunction);
    
    // Creates ObjectNode
    void ID_AddressSpace_CreateObjectNode(ID_AddressSpaceHandle AddressSpace, ID_ObjectNode ** ObjectNode, ID_AddressSpace_CustomFreeFunction * FreeFunction);
    
    // Creates ObjectTypeNode
    void ID_AddressSpace_CreateObjectTypeNode(ID_AddressSpaceHandle AddressSpace, ID_ObjectTypeNode ** ObjectTypeNode, ID_AddressSpace_CustomFreeFunction * FreeFunction);
    
    // Creates ReferenceTypeNode
    void ID_AddressSpace_CreateReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, ID_ReferenceTypeNode ** ReferenceTypeNode, ID_AddressSpace_CustomFreeFunction * FreeFunction);
    
    // Creates VariableNode
    void ID_AddressSpace_CreateVariableNode(ID_AddressSpaceHandle AddressSpace, ID_VariableNode ** VariableNode, ID_AddressSpace_CustomFreeFunction * FreeFunction);
    
    // Creates VariableTypeNode
    void ID_AddressSpace_CreateVariableTypeNode(ID_AddressSpaceHandle AddressSpace, ID_VariableTypeNode ** VariableTypeNode, ID_AddressSpace_CustomFreeFunction * FreeFunction);
    
    // Creates ViewNode
    void ID_AddressSpace_CreateViewNode(ID_AddressSpaceHandle AddressSpace, ID_ViewNode ** ViewNode, ID_AddressSpace_CustomFreeFunction * FreeFunction);
    
    /* Functions to clone a node from a base node */

    // Clones a Node
    void ID_AddressSpace_CloneNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, OpcUa_NodeId Node, OpcUa_NodeId * CloneNodeId, void ** ClonedNode);
    
    // Clones a DataTypeNode
    void ID_AddressSpace_CloneDataTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_DataTypeNode * Orig, OpcUa_NodeId * CloneNodeId, ID_DataTypeNode ** Cloned);
    
    // Clones MethodNode
    void ID_AddressSpace_CloneMethodNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_MethodNode * Orig, OpcUa_NodeId * CloneNodeId, ID_MethodNode ** Cloned);
    
    // Clones ObjectNode
    void ID_AddressSpace_CloneObjectNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_ObjectNode * Orig, OpcUa_NodeId * CloneNodeId, ID_ObjectNode ** Cloned);
    
    // Clones ObjectTypeNode
    void ID_AddressSpace_CloneObjectTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_ObjectTypeNode * Orig, OpcUa_NodeId * CloneNodeId, ID_ObjectTypeNode ** Cloned);
    
    // Clones ReferenceTypeNode
    void ID_AddressSpace_CloneReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_ReferenceTypeNode * Orig, OpcUa_NodeId * CloneNodeId, ID_ReferenceTypeNode ** Cloned);
    
    // Clones VariableNode
    void ID_AddressSpace_CloneVariableNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_VariableNode * Orig, OpcUa_NodeId * CloneNodeId, ID_VariableTypeNode ** Cloned);
    
    // Clones VariableTypeNode
    void ID_AddressSpace_CloneVariableTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_VariableTypeNode * Orig, OpcUa_NodeId * CloneNodeId, ID_VariableTypeNode ** Cloned);
    
    // Clones ViewNode
    void ID_AddressSpace_CloneViewNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_ViewNode * Orig, OpcUa_NodeId * CloneNodeId, ID_ViewNode ** Cloned);
    
    /* Removes a Node from the AddressSpace */

    // Removes DataTypeNode
    void ID_AddressSpace_RemoveDataTypeNode(ID_AddressSpaceHandle AddressSpace, ID_DataTypeNode * DataTypeNode);
    
    // Removes MethodNode
    void ID_AddressSpace_RemoveMethodNode(ID_AddressSpaceHandle AddressSpace, ID_MethodNode * MethodNode);
    
    // Removes ObjectNode
    void ID_AddressSpace_RemoveObjectNode(ID_AddressSpaceHandle AddressSpace, ID_ObjectNode * ObjectNode);
    
    // Removes ObjectTypeNode
    void ID_AddressSpace_RemoveObjectTypeNode(ID_AddressSpaceHandle AddressSpace, ID_ObjectTypeNode * ObjectTypeNode);
    
    // Removes ReferenceTypeNode
    void ID_AddressSpace_RemoveReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, ID_ReferenceTypeNode * ReferenceTypeNode);
    
    // Removes VariableNode
    void ID_AddressSpace_RemoveVariableNode(ID_AddressSpaceHandle AddressSpace, ID_VariableNode * VariableNode);
    
    // Removes VariableTypeNode
    void ID_AddressSpace_RemoveVariableTypeNode(ID_AddressSpaceHandle AddressSpace, ID_VariableTypeNode * VariableTypeNode);
    
    // Removes ViewNode
    void ID_AddressSpace_RemoveViewNode(ID_AddressSpaceHandle AddressSpace, ID_ViewNode * ViewNode);
    
    // Removes Node
    void ID_AddressSpace_RemoveNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId, ID_NodeType nType);
    
    /* Functions to retrieve a Node with a NodeId */
    
    // Get Node with identifier
    void *              ID_AddressSpace_GetNode(ID_AddressSpaceHandle, OpcUa_NodeId * NodeId);
    
    // Get DataTypeNode with identifier
    ID_DataTypeNode *      ID_AddressSpace_GetDataTypeNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId);
    
    // Get MethodNode with identifier
    ID_MethodNode *        ID_AddressSpace_GetMethodNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId);
    
    // Get ObjectNode with identifier
    ID_ObjectNode *        ID_AddressSpace_GetObjectNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId);
    
    // Get ObjectTypeNode with identifier
    ID_ObjectTypeNode *    ID_AddressSpace_GetObjectTypeNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId);
    
    // Get ReferenceTypeNode with identifier
    ID_ReferenceTypeNode * ID_AddressSpace_GetReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId);
    
    // Get VariableNode with identifier
    ID_VariableNode *      ID_AddressSpace_GetVariableNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId);
    
    // Get VariableTypeNode with identifier
    ID_VariableTypeNode *  ID_AddressSpace_GetVariableTypeNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId);
    
    // Get ViewNode with identifier
    ID_ViewNode *          ID_AddressSpace_GetViewNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* NodeId);
    
    /* Functions to control node relationships */

    // Add one directional reference from Source->Target with ReferenceTypeId and IsInverse
    void ID_AddressSpace_AddUniReference(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* Source, ID_NodeType Type, OpcUa_NodeId* Target, OpcUa_NodeId* ReferenceTypeId, OpcUa_Boolean IsInverse);
    
    // Add bidirectional reference from Source<->Target with ReferenceTypeId and IsInverse
    void ID_AddressSpace_AddBiReference(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* Source, ID_NodeType SourceType, OpcUa_NodeId* Target, ID_NodeType TargetType, OpcUa_NodeId* ReferenceId);
    
    // Removes one directional reference from Source->Target with ReferenceTypeId and IsInverse
    void ID_AddressSpace_RemoveUniReference(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* Source, ID_NodeType SourceType, OpcUa_NodeId* Target, OpcUa_NodeId* ReferenceTypeId, OpcUa_Boolean IsInverse);
    
    // Removes bidirectional reference from Source<->Target with ReferenceTypeId and IsInverse
    void ID_AddressSpace_RemoveBiReference(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId* Source, ID_NodeType SourceType, OpcUa_NodeId* Target, ID_NodeType TargetType, OpcUa_NodeId* ReferenceId);
    
    // Inherit source references into target
    void ID_AddressSpace_InheritReferences(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, OpcUa_NodeId * Source, OpcUa_NodeId * Target);
    
    // Build all references of identifier Reference from TargetNode to nodes of Type
    void ID_AddressSpace_BuildReferencesOfTypeToNode(ID_AddressSpaceHandle Handle, ID_NodeType Type, OpcUa_NodeId * TargetNode, OpcUa_NodeId * Reference);

    /* Functions to create from object/variable types or to clone a method for use */

    // Creates a variable from a provided VariableType
    ID_VariableNode *  ID_AddressSpace_CreateVariableFromVariableType(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, ID_VariableTypeNode * VarTypeNode, OpcUa_NodeId * NodeId);
    
    // Copies a value from a variableType to a Variable
    void ID_AddressSpace_CopyVariableFromType(ID_VariableTypeNode * VarTypeNode, ID_VariableNode * VarNode);

    // Creates an object from a provided ObjectType
    ID_ObjectNode *    ID_AddressSpace_CreateObjectFromObjectType(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, ID_ObjectTypeNode * ObjTypeNode, OpcUa_NodeId * NodeId);
    
    // Clones a method and replaces the InputArgumentsId and OutputArgumentsid
    ID_MethodNode *    ID_AddressSpace_CloneMethod(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, OpcUa_NodeId * MethodId, OpcUa_NodeId * NewNodeId, OpcUa_NodeId * InputId, OpcUa_NodeId * OutputId);
    
    /* Map function and searching for a AddressSpaceNode with identifier */

    // Find an AddressSpaceNode within the AddressSpace of identifier nodeId
    ID_AddressSpaceNode* ID_AddressSpace_FindNodeWithID(ID_AddressSpaceHandle Internal, OpcUa_NodeId * nodeId);

    // Replaces a Node with identifier Original WITH New
    void ID_AddressSpace_MapNodeToId(ID_AddressSpaceHandle Handle, OpcUa_NodeId * Original, OpcUa_NodeId * New);

    /* Functions to control fast list */

    // Constructs the FastList Access Queue
    void ID_AddressSpace_ConstructFastList(ID_AddressSpaceHandle AddressSpace);

    // Use the FastList Access to find a node of interest
    void * ID_AddressSpace_FastAccessNode(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId * NodeId);
#ifdef __cplusplus
}
#endif

#endif /* ADDRESSSPACE_H */
