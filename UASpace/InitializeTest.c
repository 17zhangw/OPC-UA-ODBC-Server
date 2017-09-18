/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "Initialize.h"

void ID_Initialize_TestObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
	OpcUa_ReferenceParameter(Indexer);

	ID_VariableNode * TestArrayNode1;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode1, NULL);
    TestArrayNode1->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode1->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode1->BaseAttribute.NodeId.Identifier.Numeric = 1;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode1, 3, "TestArrayNode1");
    TestArrayNode1->ValueRank = OpcUa_ValueRanks_OneDimension;
    TestArrayNode1->NoOfArrayDimensions = 1;
    TestArrayNode1->ArrayDimensions = malloc(sizeof(OpcUa_UInt32));
    TestArrayNode1->ArrayDimensions[0] = 5;
    TestArrayNode1->Value.ArrayType = OpcUa_VariantArrayType_Array;
    TestArrayNode1->Value.Datatype = OpcUaType_Double;
    TestArrayNode1->Value.Value.Array.Length = 5;
    TestArrayNode1->Value.Value.Array.Value.DoubleArray = malloc(sizeof(OpcUa_Double) * 5);
    memset(TestArrayNode1->Value.Value.Array.Value.DoubleArray, 0x00, sizeof(OpcUa_Double) * 5);
    TestArrayNode1->AccessLevel = 3;
    TestArrayNode1->UserAccessLevel = 3;
    TestArrayNode1->DataType.Identifier.Numeric = OpcUaId_Double;
    
	ID_VariableNode * TestArrayNode2;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode2, NULL);
    TestArrayNode2->AccessLevel = 2;
    TestArrayNode2->UserAccessLevel = 2;
    TestArrayNode2->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode2->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode2->BaseAttribute.NodeId.Identifier.Numeric = 2;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode2, 3, "TestArrayNode2");
    
	ID_VariableNode * TestArrayNode3;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode3, NULL);
    TestArrayNode3->AccessLevel = 1;
    TestArrayNode3->UserAccessLevel = 1;
    TestArrayNode3->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode3->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode3->BaseAttribute.NodeId.Identifier.Numeric = 3;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode3, 3, "TestArrayNode3");
    
	ID_VariableNode * TestArrayNode4;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode4, NULL);
    TestArrayNode4->BaseAttribute.WriteMask = 68;
    TestArrayNode4->AccessLevel = 3;
    TestArrayNode4->UserAccessLevel = 3;
    TestArrayNode4->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode4->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode4->BaseAttribute.NodeId.Identifier.Numeric = 4;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode4, 3, "TestArrayNode4");
    TestArrayNode4->ValueRank = -1;
    TestArrayNode4->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
    TestArrayNode4->Value.Datatype = OpcUaType_Boolean;
    TestArrayNode4->Value.Value.Boolean = OpcUa_False;
    TestArrayNode4->DataType.Identifier.Numeric = OpcUaId_Boolean;
    
	ID_VariableNode * TestArrayNode5;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode5, NULL);
    TestArrayNode5->AccessLevel = 3;
    TestArrayNode5->UserAccessLevel = 3;
    TestArrayNode5->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode5->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode5->BaseAttribute.NodeId.Identifier.Numeric = 5;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode5, 3, "TestArrayNode5");
    TestArrayNode5->ValueRank = -1;
    TestArrayNode5->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
    TestArrayNode5->Value.Datatype = OpcUaType_ByteString;
    TestArrayNode5->DataType.Identifier.Numeric = OpcUaId_ByteString;
    OpcUa_ByteString_Initialize(&TestArrayNode5->Value.Value.ByteString);
    
	ID_VariableNode * TestArrayNode6;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode6, NULL);
    TestArrayNode6->AccessLevel = 3;
    TestArrayNode6->UserAccessLevel = 3;
    TestArrayNode6->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode6->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode6->BaseAttribute.NodeId.Identifier.Numeric = 6;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode6, 3, "TestArrayNode6");
    TestArrayNode6->ValueRank = -1;
    TestArrayNode6->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
    TestArrayNode6->Value.Datatype = OpcUaType_Int16;
    TestArrayNode6->DataType.Identifier.Numeric = OpcUaId_Int16;
    TestArrayNode6->Value.Value.Int16 = 8;
    
	ID_VariableNode * TestArrayNode7;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode7, NULL);
    TestArrayNode7->AccessLevel = 3;
    TestArrayNode7->UserAccessLevel = 3;
    TestArrayNode7->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode7->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode7->BaseAttribute.NodeId.Identifier.Numeric = 7;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode7, 3, "TestArrayNode7");
    TestArrayNode7->ValueRank = -1;
    TestArrayNode7->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
    TestArrayNode7->Value.Datatype = OpcUaType_Float;
    TestArrayNode7->DataType.Identifier.Numeric = OpcUaId_Float;
    TestArrayNode7->Value.Value.Float = 0.0;
    
	ID_VariableNode * TestArrayNode8;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode8, NULL);
    TestArrayNode8->AccessLevel = 3;
    TestArrayNode8->UserAccessLevel = 3;
    TestArrayNode8->BaseAttribute.WriteMask = 68;
    TestArrayNode8->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode8->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode8->BaseAttribute.NodeId.Identifier.Numeric = 8;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode8, 3, "TestArrayNode8");
    TestArrayNode8->ValueRank = -1;
    TestArrayNode8->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
    TestArrayNode8->Value.Datatype = OpcUaType_Double;
    TestArrayNode8->DataType.Identifier.Numeric = OpcUaId_Double;
    TestArrayNode8->Value.Value.Double = 0.0;
    
	ID_VariableNode * TestArrayNode9;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode9, NULL);
    TestArrayNode9->AccessLevel = 3;
    TestArrayNode9->UserAccessLevel = 3;
    TestArrayNode9->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode9->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode9->BaseAttribute.NodeId.Identifier.Numeric = 9;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode9, 3, "TestArrayNode9");
    TestArrayNode9->ValueRank = -1;
    TestArrayNode9->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
    TestArrayNode9->Value.Datatype = OpcUaType_LocalizedText;
    TestArrayNode9->DataType.Identifier.Numeric = OpcUaId_LocalizedText;
    TestArrayNode9->Value.Value.LocalizedText = malloc(sizeof(OpcUa_LocalizedText));
    OpcUa_LocalizedText_Initialize(TestArrayNode9->Value.Value.LocalizedText);
    OpcUa_String_AttachCopy(&TestArrayNode9->Value.Value.LocalizedText->Locale, ID_APPLICATION_LOCALE);
    
	ID_VariableNode * TestArrayNode10;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode10, NULL);
    TestArrayNode10->AccessLevel = 3;
    TestArrayNode10->UserAccessLevel = 3;
    TestArrayNode10->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode10->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode10->BaseAttribute.NodeId.Identifier.Numeric = 10;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode10, 3, "TestArrayNode10");
    TestArrayNode10->ValueRank = OpcUa_ValueRanks_OneDimension;
    TestArrayNode10->NoOfArrayDimensions = 1;
    TestArrayNode10->ArrayDimensions = malloc(sizeof(OpcUa_UInt32));
    TestArrayNode10->ArrayDimensions[0] = 10;
    TestArrayNode10->Value.ArrayType = OpcUa_VariantArrayType_Array;
    TestArrayNode10->Value.Datatype = OpcUaType_Byte;
    TestArrayNode10->DataType.Identifier.Numeric = OpcUaId_Byte;
    TestArrayNode10->Value.Value.Array.Length = 10;
    TestArrayNode10->Value.Value.Array.Value.ByteArray = malloc(sizeof(OpcUa_Byte) * 10);
    memset(TestArrayNode10->Value.Value.Array.Value.ByteArray, 0x00, sizeof(OpcUa_Byte) * 10);
    
	ID_VariableNode * TestArrayNode11;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode11, NULL);
    TestArrayNode11->AccessLevel = 3;
    TestArrayNode11->UserAccessLevel = 3;
    TestArrayNode11->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode11->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode11->BaseAttribute.NodeId.Identifier.Numeric = 11;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode11, 3, "TestArrayNode11");
    TestArrayNode11->ValueRank = -1;
    TestArrayNode11->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
    TestArrayNode11->Value.Datatype = OpcUaType_Int16;
    TestArrayNode11->DataType.Identifier.Numeric = OpcUaId_Integer;
    
	ID_VariableNode * TestArrayNode12;
	ID_AddressSpace_CreateVariableNode(Handle, &TestArrayNode12, NULL);
    TestArrayNode12->AccessLevel = 3;
    TestArrayNode12->UserAccessLevel = 3;
    TestArrayNode12->BaseAttribute.NodeId.NamespaceIndex = 3;
    TestArrayNode12->BaseAttribute.NodeId.IdentifierType = OpcUa_IdType_Numeric;
    TestArrayNode12->BaseAttribute.NodeId.Identifier.Numeric = 12;
	ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(TestArrayNode12, 3, "TestArrayNode12");
    TestArrayNode12->ValueRank = -1;
    TestArrayNode12->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
    TestArrayNode12->Value.Datatype = OpcUaType_UInt16;
    TestArrayNode12->DataType.Identifier.Numeric = OpcUaId_UInteger;
}