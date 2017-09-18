/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ADDRESSSPACEHELPER_H
#define ADDRESSSPACEHELPER_H

#include "AddressSpace.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // Checks if two Ids are equal
    OpcUa_Boolean ID_AddressSpace_AreIdsEqual(OpcUa_NodeId * pValue1, OpcUa_NodeId * pValue2);

    // Checks if a NodeId represents a numeric datatype
    OpcUa_Boolean ID_AddressSpace_IsNumericDataType(OpcUa_NodeId * DataType);
    
    // Creates the NodeVersion property on a node
    void ID_AddressSpace_CreateNodeVersionProperty(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, OpcUa_NodeId * ParentNodeId, ID_NodeType ParentType, ID_VariableNode ** VariableNode);

    // CreateProperty can only be called for creating variables of PropertyType
    // ParentNodeId=>Parent Identifier
    // DataTypeId=>Data Type of the Value not Variant
    // Length=>kAddress_valueRanks_Any, kAddressSpace_Length_Unknown or 0+ value
    void ID_AddressSpace_CreateProperty(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId * ParentNodeId, ID_NodeType ParentType,
                                        ID_VariableNode ** Node, OpcUa_Int BrowseNamespace, OpcUa_StringA BrowseName, OpcUa_NodeId * NodeId,
                                        OpcUa_NodeId * DataTypeId, OpcUa_Int32 Length, OpcUa_NodeId * ModellingRuleId,
                                        OpcUa_BuiltInType BuiltInType, OpcUa_ExtensionObjectEncoding Encoding, 
                                        OpcUa_ExpandedNodeId * EncodeableNodeId, OpcUa_EncodeableType * Type);
    
    // CreateComponent that is an instance of the Type pointed to by ComponentType
    // The ComponentType must be of a valid VariableType
    void ID_AddressSpace_CreateComponent(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer,
                                         OpcUa_NodeId * ParentNodeId, ID_NodeType ParentType, ID_VariableNode ** NodePtr,
                                         OpcUa_Int BrowseNamespace, OpcUa_StringA BrowseName, OpcUa_NodeId * NodeId, OpcUa_NodeId * DataTypeId, 
                                         OpcUa_Int32 Length, OpcUa_NodeId * ComponentType, OpcUa_NodeId * ModellingRule,
                                         OpcUa_BuiltInType BuiltInType, OpcUa_ExtensionObjectEncoding Encoding, OpcUa_ExpandedNodeId * EncodeableNodeId, OpcUa_EncodeableType * Type);
    
    // Initializes a Variant
    void ID_AddressSpace_InitializeVariant(OpcUa_Variant * var);

    // Copy source[sourceIndex] to target[targetIndex]
    OpcUa_StatusCode ID_AddressSpace_CopyVariantArray(OpcUa_Variant * source, int sourceIndex, OpcUa_Variant * target, int targetIndex);
    
    // Expands the variant array by expansion elements
    void ID_AddressSpace_ExpandVariantArray(OpcUa_Variant * var, int expansion);
    
    // Shrinks the variant array by shrink elements
    void ID_AddressSpace_ShrinkVariantArray(OpcUa_Variant * var, int shrink);

    // Removes the element from a variant array at index element
    void ID_AddressSpace_EmptyVariantArray(OpcUa_Variant * var, int element);
    OpcUa_StatusCode ID_AddressSpace_CopyVariant(OpcUa_Variant * source, OpcUa_Variant * target);
    
    // Setup variant on a variable node with parameters of interest
    // Length=>length of array, 0 for indeterminate, kAddress_valueRanks_Any to indicate unknown
    // BuiltInType=>Variant Type
    // Encoding=>Encoding of extension object
    // EncodeableNodeId=>NodeId pointing to the encoding id of the type
    // Type=>Pointer to the struct type stored in extension object
    void ID_AddressSpace_SetupVariantOnVariable(void * Node, OpcUa_Int32 Length, OpcUa_BuiltInType BuiltInType, OpcUa_ExtensionObjectEncoding Encoding, OpcUa_ExpandedNodeId * EncodeableNodeId, OpcUa_EncodeableType * Type);
    
    // Retreives target node that has a BrowseName from Node
    void * ID_AddressSpace_HuntForReferenceWithNode(ID_AddressSpaceHandle Handle, ID_BaseAttributes * Source, OpcUa_QualifiedName BrowseName);

    // Retrieves target node that has a BrowseName from node with id Source
    void * ID_AddressSpace_HuntForReferenceWithNodeId(ID_AddressSpaceHandle Handle, OpcUa_NodeId * Source, OpcUa_QualifiedName BrowseName);

    // Initialize Extension object with parameters
    // Encoding=>Encoding of extension object
    // EncodeableNodeId=>NodeId pointing to encoding id of the type (binary/xml)
    // Type=>Pointer to the struct type stored in extension object
    void ID_AddressSpace_InitializeExtensionObject(OpcUa_ExtensionObject * Extension, OpcUa_ExtensionObjectEncoding Encoding, OpcUa_ExpandedNodeId * EncodeableNodeId, OpcUa_EncodeableType * Type);
    
    // Checks if Type pointed to by Sub is a subtype of Type pointed to by Parent
    OpcUa_Boolean ID_AddressSpace_IsTypeASubtypeofB(ID_AddressSpaceHandle Handle, OpcUa_NodeId * Sub, OpcUa_NodeId * Parent);

    void ID_AddressSpaceHelper_CreateMethodNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_MethodNode ** Node, OpcUa_NodeId Id,
                                                OpcUa_Int BrowseNamespace, char * BrowseName, OpcUa_Int32 InputNum, OpcUa_NodeId InputArgId,
                                                ID_InputArgsInfo * InputArgs, OpcUa_Int32 OutputNum, OpcUa_NodeId OutputArgId, ID_InputArgsInfo * OutputArgs);

    void ID_AddressSpaceHelper_CreateObjectType(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, ID_ObjectTypeNode * Parent, ID_ObjectTypeNode ** NodePtr,
                                                OpcUa_Int BrowseNamespace, OpcUa_StringA BrowseName, OpcUa_Boolean IsAbstract, OpcUa_NodeId NodeId,
                                                OpcUa_UInt32 PropertyCount, ID_PropertyAddition * Properties);

    void ID_AddressSpaceHelper_CreateVariable(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, OpcUa_NodeId * Parent, ID_NodeType ParentType, OpcUa_NodeId * VariableId, OpcUa_Int BrowseNamespace, char * BrowseName, OpcUa_NodeId * VariableType, OpcUa_NodeId * ModellingRule);

    void ID_AddressSpaceHelper_CreateVariableTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_VariableTypeNode ** NodePtr,
                                                      OpcUa_NodeId NodeId, OpcUa_Int BrowseNamespace, OpcUa_StringA BrowseName, OpcUa_Boolean IsAbstract,
                                                      OpcUa_Int32 Length, OpcUa_NodeId DataTypeId, OpcUa_NodeId * ParentNodeId, ID_VariantStructure * MainStructure,
                                                      int ChildrenCount, int * ChildNameNamespaces, char ** ChildNames, int * ChildNodeId, int * ChildDataTypeId,
                                                      int * ChildTypeDataTypeId, int * ChildLengths, ID_VariantStructure * Structures);

    void ID_AddressSpaceHelper_CreateReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer,
                                                       OpcUa_NodeId * ParentNode, ID_ReferenceTypeNode ** Node,
                                                       OpcUa_Boolean Symmetric, OpcUa_Boolean IsAbstract,
                                                       OpcUa_Int NameNamespace, OpcUa_StringA Name, OpcUa_StringA InverseName, OpcUa_NodeId Identifier);

    void ID_AddressSpaceHelper_CreateDataTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, OpcUa_NodeId * ParentNode,
                                                  ID_DataTypeNode ** Node, OpcUa_Boolean IsAbstract, OpcUa_Int NameNamespace, OpcUa_StringA Name, OpcUa_NodeId Identifier,
                                                  OpcUa_UInt32 NumEnumValues, char ** Locales, char ** Texts, OpcUa_NodeId EnumId, int * ValueIds, OpcUa_NodeId * StructureEncodingId);

#ifdef __cplusplus
}
#endif

#endif /* ADDRESSSPACEHELPER_H */
