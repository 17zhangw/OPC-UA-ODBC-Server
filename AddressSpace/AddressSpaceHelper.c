/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "AddressSpaceHelper.h"
#include "AddressSpaceCopier.h"

void ID_AddressSpace_CopyExpandedNodeId(OpcUa_ExpandedNodeId * source, OpcUa_ExpandedNodeId * target) {
    target->ServerIndex = source->ServerIndex;
    OpcUa_CharA * strContents = OpcUa_String_GetRawString(&source->NamespaceUri);
    if (strContents != NULL) {
        OpcUa_String_AttachCopy(&target->NamespaceUri, strContents);
    }
    ID_OpcUa_NodeId_CopyTo(&source->NodeId, &target->NodeId);
}

int ID_EvalMemSizeForVariantDataType(OpcUa_Byte Datatype) {
    int Size = 0;
    switch (Datatype) {
        case OpcUaType_Null: break;
        case OpcUaType_Boolean:         Size = sizeof(OpcUa_Boolean); break;
        case OpcUaType_SByte:           Size = sizeof(OpcUa_SByte); break;
        case OpcUaType_Byte:            Size = sizeof(OpcUa_Byte); break;
        case OpcUaType_Int16:           Size = sizeof(OpcUa_Int16); break;
        case OpcUaType_UInt16:          Size = sizeof(OpcUa_UInt16); break;
        case OpcUaType_Int32:           Size = sizeof(OpcUa_Int32); break;
        case OpcUaType_UInt32:          Size = sizeof(OpcUa_UInt32); break;
        case OpcUaType_Int64:           Size = sizeof(OpcUa_Int64); break;
        case OpcUaType_UInt64:          Size = sizeof(OpcUa_UInt64); break;
        case OpcUaType_Float:           Size = sizeof(OpcUa_Float); break;
        case OpcUaType_Double:          Size = sizeof(OpcUa_Double); break;
        case OpcUaType_String:          Size = sizeof(OpcUa_String); break;
        case OpcUaType_DateTime:        Size = sizeof(OpcUa_DateTime); break;
        case OpcUaType_Guid:            Size = sizeof(OpcUa_Guid); break;
        case OpcUaType_ByteString:      Size = sizeof(OpcUa_ByteString); break;
        case OpcUaType_XmlElement:      Size = sizeof(OpcUa_XmlElement); break;
        case OpcUaType_NodeId:          Size = sizeof(OpcUa_NodeId); break;
        case OpcUaType_ExpandedNodeId:  Size = sizeof(OpcUa_ExpandedNodeId); break;
        case OpcUaType_StatusCode:      Size = sizeof(OpcUa_StatusCode); break;
        case OpcUaType_QualifiedName:   Size = sizeof(OpcUa_QualifiedName); break;
        case OpcUaType_LocalizedText:   Size = sizeof(OpcUa_LocalizedText); break;
        case OpcUaType_ExtensionObject: Size = sizeof(OpcUa_ExtensionObject); break;
        case OpcUaType_DataValue:       Size = sizeof(OpcUa_DataValue); break;
        case OpcUaType_Variant:         Size = sizeof(OpcUa_Variant); break;
        default:
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Invalid Variant Datatype supplied to EvalMemSizeForVariantType()\n");
            break;
    }

    return Size;
}

void ID_AddressSpace_ShrinkVariantArray(OpcUa_Variant * var, int shrink) {
    for (int i = var->Value.Array.Length - 1; i >= var->Value.Array.Length - shrink; i--) {
        ID_AddressSpace_EmptyVariantArray(var, i);
    }
    
    int Length = var->Value.Array.Length - shrink;
    if (Length == 0) {
        var->Value.Array.Length = 0;
        free(var->Value.Array.Value.Array);
        var->Value.Array.Value.Array = NULL;
        return;
    }
    
    int Size = ID_EvalMemSizeForVariantDataType(var->Datatype);
    var->Value.Array.Value.Array = realloc(var->Value.Array.Value.Array, Size * Length);
    var->Value.Array.Length -= shrink;
}

void ID_AddressSpace_ExpandVariantArray(OpcUa_Variant * var, int expansion) {
    int Length = var->Value.Array.Length + expansion;
    int Size = ID_EvalMemSizeForVariantDataType(var->Datatype);
    var->Value.Array.Value.Array = realloc(var->Value.Array.Value.Array, Size * Length);
    
    for (int i = var->Value.Array.Length; i < var->Value.Array.Length + expansion; i++) {
        switch (var->Datatype) {
            case OpcUaType_Null: break;
            case OpcUaType_Boolean: var->Value.Array.Value.BooleanArray[i] = 0; break;
            case OpcUaType_SByte: var->Value.Array.Value.SByteArray[i] = 0; break;
            case OpcUaType_Byte: var->Value.Array.Value.ByteArray[i] = 0; break;
            case OpcUaType_Int16: var->Value.Array.Value.Int16Array[i] = 0; break;
            case OpcUaType_UInt16: var->Value.Array.Value.UInt16Array[i] = 0; break;
            case OpcUaType_Int32: var->Value.Array.Value.Int32Array[i] = 0; break;
            case OpcUaType_UInt32: var->Value.Array.Value.UInt32Array[i] = 0; break;
            case OpcUaType_Int64: var->Value.Array.Value.Int64Array[i] = 0; break;
            case OpcUaType_UInt64: var->Value.Array.Value.UInt64Array[i] = 0; break;
            case OpcUaType_Float: var->Value.Array.Value.FloatArray[i] = 0; break;
            case OpcUaType_Double: var->Value.Array.Value.DoubleArray[i] = 0; break;
            case OpcUaType_String: OpcUa_String_Initialize(&var->Value.Array.Value.StringArray[i]); break;
            case OpcUaType_DateTime: OpcUa_DateTime_Initialize(&var->Value.Array.Value.DateTimeArray[i]); break;
            case OpcUaType_Guid: OpcUa_Guid_Initialize(&var->Value.Array.Value.GuidArray[i]); break;
            case OpcUaType_ByteString: OpcUa_ByteString_Initialize(&var->Value.Array.Value.ByteStringArray[i]); break;
            case OpcUaType_XmlElement: OpcUa_ByteString_Initialize(&var->Value.Array.Value.XmlElementArray[i]); break;
            case OpcUaType_NodeId: OpcUa_NodeId_Initialize(&var->Value.Array.Value.NodeIdArray[i]); break;
            case OpcUaType_ExpandedNodeId: OpcUa_ExpandedNodeId_Initialize(&var->Value.Array.Value.ExpandedNodeIdArray[i]); break;
            case OpcUaType_StatusCode: OpcUa_StatusCode_Initialize(&var->Value.Array.Value.StatusCodeArray[i]); break;
            case OpcUaType_QualifiedName: OpcUa_QualifiedName_Initialize(&var->Value.Array.Value.QualifiedNameArray[i]); break;
            case OpcUaType_LocalizedText: OpcUa_LocalizedText_Initialize(&var->Value.Array.Value.LocalizedTextArray[i]); break;
            case OpcUaType_ExtensionObject: OpcUa_ExtensionObject_Initialize(&var->Value.Array.Value.ExtensionObjectArray[i]); break;
            case OpcUaType_DataValue: OpcUa_DataValue_Initialize(&var->Value.Array.Value.DataValueArray[i]); break;
            case OpcUaType_Variant: OpcUa_Variant_Initialize(&var->Value.Array.Value.VariantArray[i]); break;
            default: printf("ERROR - initializing variant of wrong type - \n"); break;
        }
    }
    
    var->Value.Array.Length += expansion;
}

void ID_AddressSpace_EmptyVariantArray(OpcUa_Variant * var, int element) {
    if (var->ArrayType == OpcUa_VariantArrayType_Array) {
        switch (var->Datatype) {
            case OpcUaType_String: OpcUa_String_Clear(&var->Value.Array.Value.StringArray[element]); break;
            case OpcUaType_Guid: OpcUa_Guid_Clear(&var->Value.Array.Value.GuidArray[element]); break;

            case OpcUaType_ByteString:
            case OpcUaType_XmlElement: OpcUa_ByteString_Clear(&var->Value.Array.Value.XmlElementArray[element]); break;
            
            case OpcUaType_NodeId: OpcUa_NodeId_Clear(&var->Value.Array.Value.NodeIdArray[element]); break;
            case OpcUaType_ExpandedNodeId: OpcUa_ExpandedNodeId_Initialize(&var->Value.Array.Value.ExpandedNodeIdArray[element]); break;
            case OpcUaType_LocalizedText: OpcUa_LocalizedText_Clear(&var->Value.Array.Value.LocalizedTextArray[element]); break;
            case OpcUaType_DataValue: OpcUa_DataValue_Clear(&var->Value.Array.Value.DataValueArray[element]); break;
            case OpcUaType_ExtensionObject: OpcUa_ExtensionObject_Clear(&var->Value.Array.Value.ExtensionObjectArray[element]); break;
            case OpcUaType_Variant: OpcUa_Variant_Clear(&var->Value.Array.Value.VariantArray[element]); break;
            default: break;
        }
    }
}

// does not actually remove the variant
// directly overwrites, may result in memory leaks or faulty operations
void ID_AddressSpace_InitializeVariant(OpcUa_Variant * var) {
    if (var->ArrayType == OpcUa_VariantArrayType_Scalar) {
        switch (var->Datatype) {
            case OpcUaType_Null:
            case OpcUaType_Boolean:
            case OpcUaType_SByte:
            case OpcUaType_Byte:
            case OpcUaType_Int16:
            case OpcUaType_UInt16:
            case OpcUaType_Int32:
            case OpcUaType_UInt32:
            case OpcUaType_Int64:
            case OpcUaType_UInt64:
            case OpcUaType_Float:
            case OpcUaType_Double:
                break;
            case OpcUaType_String: OpcUa_String_Clear(&var->Value.String); OpcUa_String_Initialize(&var->Value.String); break;
            case OpcUaType_DateTime: OpcUa_DateTime_Initialize(&var->Value.DateTime); break;
            case OpcUaType_StatusCode: OpcUa_StatusCode_Initialize(&var->Value.StatusCode); break;
            case OpcUaType_ByteString: OpcUa_ByteString_Clear(&var->Value.ByteString); OpcUa_ByteString_Initialize(&var->Value.ByteString); break;
            case OpcUaType_XmlElement: OpcUa_ByteString_Clear(&var->Value.XmlElement); OpcUa_ByteString_Initialize(&var->Value.XmlElement); break;
            case OpcUaType_Guid: var->Value.Guid = malloc(sizeof(OpcUa_Guid)); memset(var->Value.Guid, 0x00, sizeof(OpcUa_Guid)); break;
            case OpcUaType_NodeId: var->Value.NodeId = malloc(sizeof(OpcUa_NodeId)); OpcUa_NodeId_Initialize(var->Value.NodeId); break;
            case OpcUaType_ExpandedNodeId: var->Value.ExpandedNodeId = malloc(sizeof(OpcUa_ExpandedNodeId)); OpcUa_ExpandedNodeId_Initialize(var->Value.ExpandedNodeId); break;
            case OpcUaType_QualifiedName: var->Value.QualifiedName = malloc(sizeof(OpcUa_QualifiedName)); OpcUa_QualifiedName_Initialize(var->Value.QualifiedName); break;
            case OpcUaType_LocalizedText: var->Value.LocalizedText = malloc(sizeof(OpcUa_LocalizedText)); OpcUa_LocalizedText_Initialize(var->Value.LocalizedText); break;
            case OpcUaType_ExtensionObject: var->Value.ExtensionObject = malloc(sizeof(OpcUa_ExtensionObject)); OpcUa_ExtensionObject_Initialize(var->Value.ExtensionObject); break;
            case OpcUaType_DataValue: var->Value.DataValue = malloc(sizeof(OpcUa_DataValue)); OpcUa_DataValue_Initialize(var->Value.DataValue); break;
            case OpcUaType_Variant: break;
            default:
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Invalid variant datatype to InitializeVariant()\n");
                break;
        }
    } else if (var->ArrayType == OpcUa_VariantArrayType_Array) {
        OpcUa_Int32 Length = (OpcUa_Int32)var->Value.Array.Length;
        if (Length == 0) {
            return;
        }

        int AllocSize = ID_EvalMemSizeForVariantDataType(var->Datatype);
        var->Value.Array.Value.Array = malloc(AllocSize * Length);
        memset(var->Value.Array.Value.Array, 0x00, AllocSize * Length);

        switch (var->Datatype) {
            case OpcUaType_String: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, String); break;
            case OpcUaType_DateTime: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, DateTime); break;
            case OpcUaType_Guid: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, Guid); break;
            case OpcUaType_ByteString: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, ByteString); break;
            case OpcUaType_XmlElement: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, XmlElement); break;
            case OpcUaType_NodeId: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, NodeId); break;
            case OpcUaType_ExpandedNodeId: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, ExpandedNodeId); break;
            case OpcUaType_StatusCode: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, StatusCode); break;
            case OpcUaType_QualifiedName: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, QualifiedName); break;
            case OpcUaType_LocalizedText: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, LocalizedText); break;
            case OpcUaType_ExtensionObject: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, ExtensionObject); break;
            case OpcUaType_DataValue: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, DataValue); break;
            case OpcUaType_Variant: ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(var, Length, Variant); break;
            default:
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Invalid variant datatype to InitializeVariant()\n");
                break;
        }
    }
}

OpcUa_StatusCode ID_AddressSpace_CopyLocalizedText(OpcUa_LocalizedText * Target, OpcUa_LocalizedText * Source) {
    OpcUa_CharA * strContents = OpcUa_String_GetRawString(&Source->Locale);
    if (strContents != NULL && strcmp(strContents, ID_APPLICATION_LOCALE) != 0 && strcmp(strContents, ID_APPLICATION_LOCALE_LINGUAL) != 0) {
        return OpcUa_BadWriteNotSupported;
    }

    if (strContents == NULL) { strContents = ""; }
    OpcUa_String_AttachCopy(&Target->Locale, strContents);

    strContents = OpcUa_String_GetRawString(&Source->Text);
    if (strContents == NULL) { strContents = ""; }
    OpcUa_String_AttachCopy(&Target->Text, strContents);
    return OpcUa_Good;
}

OpcUa_StatusCode ID_AddressSpace_CopyExtensionObject(OpcUa_ExtensionObject * Target, OpcUa_ExtensionObject * Source) {
    Target->Encoding = Source->Encoding;
    Target->BodySize = Source->BodySize;
    Target->TypeId.ServerIndex = Source->TypeId.ServerIndex;
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&Target->TypeId.NamespaceUri, &Source->TypeId.NamespaceUri);
    ID_OpcUa_NodeId_CopyTo(&Source->TypeId.NodeId, &Target->TypeId.NodeId);

    if (Target->Encoding == OpcUa_ExtensionObjectEncoding_Binary) { ID_OpcUa_ByteString_CopyTo(&Source->Body.Binary, &Target->Body.Binary); }
    else if (Target->Encoding == OpcUa_ExtensionObjectEncoding_Xml) { ID_OpcUa_ByteString_CopyTo(&Source->Body.Xml, &Target->Body.Xml); }
    else {
        if (Source->Body.EncodeableObject.Type == NULL) {
            return OpcUa_Good;
        }

        if (Target->Body.EncodeableObject.Object != NULL) {
            Target->Body.EncodeableObject.Type->Clear(Target->Body.EncodeableObject.Object);
            free(Target->Body.EncodeableObject.Object);
        }

        OpcUa_EncodeableType * EType = Source->Body.EncodeableObject.Type;
        Target->Body.EncodeableObject.Type = EType;
        Target->Body.EncodeableObject.Object = malloc(EType->AllocationSize);
        EType->Initialize(Target->Body.EncodeableObject.Object);
        return ID_AddressSpace_CopyExtensionObjectBody(Source, Target);
    }

    return OpcUa_Good;
}

OpcUa_StatusCode ID_AddressSpace_CopyDataValue(OpcUa_DataValue * Target, OpcUa_DataValue * Source) {
    Target->ServerPicoseconds = Source->ServerPicoseconds;
    Target->ServerTimestamp = Source->ServerTimestamp;
    Target->SourcePicoseconds = Source->SourcePicoseconds;
    Target->SourceTimestamp = Source->SourceTimestamp;
    Target->StatusCode = Source->StatusCode;
    return ID_AddressSpace_CopyVariant(&Source->Value, &Target->Value);
}

OpcUa_StatusCode ID_AddressSpace_CopyVariantArray(OpcUa_Variant * source, int sourceIndex, OpcUa_Variant * target, int targetIndex) {
    OpcUa_StatusCode uStatus = OpcUa_Good;
    target->Datatype = source->Datatype;
    switch (source->Datatype) {
        case OpcUaType_Boolean: target->Value.Array.Value.BooleanArray[targetIndex] = source->Value.Array.Value.BooleanArray[sourceIndex]; break;
        case OpcUaType_SByte: target->Value.Array.Value.SByteArray[targetIndex] = source->Value.Array.Value.SByteArray[sourceIndex]; break;
        case OpcUaType_Byte: target->Value.Array.Value.ByteArray[targetIndex] = source->Value.Array.Value.ByteArray[sourceIndex]; break;
        case OpcUaType_Int16: target->Value.Array.Value.Int16Array[targetIndex] = source->Value.Array.Value.Int16Array[sourceIndex]; break;
        case OpcUaType_UInt16: target->Value.Array.Value.UInt16Array[targetIndex] = source->Value.Array.Value.UInt16Array[sourceIndex]; break;
        case OpcUaType_Int32: target->Value.Array.Value.Int32Array[targetIndex] = source->Value.Array.Value.Int32Array[sourceIndex]; break;
        case OpcUaType_UInt32: target->Value.Array.Value.UInt32Array[targetIndex] = source->Value.Array.Value.UInt32Array[sourceIndex]; break;
        case OpcUaType_Int64: target->Value.Array.Value.Int64Array[targetIndex] = source->Value.Array.Value.Int64Array[sourceIndex]; break;
        case OpcUaType_UInt64: target->Value.Array.Value.UInt64Array[targetIndex] = source->Value.Array.Value.UInt64Array[sourceIndex]; break;
        case OpcUaType_Float: target->Value.Array.Value.FloatArray[targetIndex] = source->Value.Array.Value.FloatArray[sourceIndex]; break;    
        case OpcUaType_Double: target->Value.Array.Value.DoubleArray[targetIndex] = source->Value.Array.Value.DoubleArray[sourceIndex]; break;
        case OpcUaType_String: ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&target->Value.Array.Value.StringArray[targetIndex], &source->Value.Array.Value.StringArray[sourceIndex]); break;
        case OpcUaType_DateTime: memcpy(&target->Value.Array.Value.DateTimeArray[targetIndex], &source->Value.Array.Value.DateTimeArray[sourceIndex], sizeof(OpcUa_DateTime)); break;
        case OpcUaType_Guid: OpcUa_Guid_Copy(&target->Value.Array.Value.GuidArray[targetIndex], &source->Value.Array.Value.GuidArray[sourceIndex]); break;
        case OpcUaType_ByteString: ID_OpcUa_ByteString_CopyTo(&source->Value.Array.Value.ByteStringArray[sourceIndex], &target->Value.Array.Value.ByteStringArray[targetIndex]); break;
        case OpcUaType_XmlElement: ID_OpcUa_ByteString_CopyTo(&source->Value.Array.Value.XmlElementArray[sourceIndex], &target->Value.Array.Value.XmlElementArray[targetIndex]); break;
        case OpcUaType_NodeId: ID_OpcUa_NodeId_CopyTo(&source->Value.Array.Value.NodeIdArray[sourceIndex], &target->Value.Array.Value.NodeIdArray[targetIndex]); break;
		case OpcUaType_ExpandedNodeId: ID_AddressSpace_CopyExpandedNodeId(&source->Value.Array.Value.ExpandedNodeIdArray[sourceIndex], &target->Value.Array.Value.ExpandedNodeIdArray[targetIndex]); break;
        case OpcUaType_StatusCode: target->Value.Array.Value.StatusCodeArray[targetIndex] = source->Value.Array.Value.StatusCodeArray[sourceIndex]; break;
        case OpcUaType_LocalizedText: uStatus = ID_AddressSpace_CopyLocalizedText(&target->Value.Array.Value.LocalizedTextArray[targetIndex], &source->Value.Array.Value.LocalizedTextArray[sourceIndex]); break;
        case OpcUaType_ExtensionObject: uStatus = ID_AddressSpace_CopyExtensionObject(&target->Value.Array.Value.ExtensionObjectArray[targetIndex], &source->Value.Array.Value.ExtensionObjectArray[sourceIndex]); break;
        case OpcUaType_DataValue: uStatus = ID_AddressSpace_CopyDataValue(&target->Value.Array.Value.DataValueArray[targetIndex], &source->Value.Array.Value.DataValueArray[sourceIndex]); break;
        case OpcUaType_QualifiedName:
            ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&target->Value.Array.Value.QualifiedNameArray[targetIndex].Name, &source->Value.Array.Value.QualifiedNameArray[sourceIndex].Name);
            target->Value.Array.Value.QualifiedNameArray[targetIndex].NamespaceIndex = source->Value.Array.Value.QualifiedNameArray[sourceIndex].NamespaceIndex;
            break;
        default:
            uStatus = OpcUa_BadNotSupported;
            break;
    }
    
    return uStatus;
}

// need to initialize Variant before CopyVariant
OpcUa_StatusCode ID_AddressSpace_CopyVariant(OpcUa_Variant * source, OpcUa_Variant * target) {
    OpcUa_StatusCode uStatus = OpcUa_Good;
    target->Datatype = source->Datatype;
    if (source->ArrayType == OpcUa_VariantArrayType_Scalar) {
        switch ((int)source->Datatype) {
            case OpcUaType_Boolean: target->Value.Boolean = source->Value.Boolean; break;
            case OpcUaType_SByte: target->Value.SByte = source->Value.SByte; break;
            case OpcUaType_Byte: target->Value.Byte = source->Value.Byte; break;
            case OpcUaType_Int16: target->Value.Int16 = source->Value.Int16; break;
            case OpcUaType_UInt16: target->Value.UInt16 = source->Value.UInt16; break;
            case OpcUaType_Int32: target->Value.Int32 = source->Value.Int32; break;
            case OpcUaType_UInt32: target->Value.UInt32 = source->Value.UInt32; break;
            case OpcUaType_Int64: target->Value.Int64 = source->Value.Int64; break;
            case OpcUaType_UInt64: target->Value.UInt64 = source->Value.UInt64; break;
            case OpcUaType_Float: target->Value.Float = source->Value.Float; break;    
            case OpcUaType_Double: target->Value.Double = source->Value.Double; break;
            case OpcUaType_String: ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&target->Value.String, &source->Value.String); break;
            case OpcUaType_DateTime: memcpy(&target->Value.DateTime, &source->Value.DateTime, sizeof(OpcUa_DateTime)); break;
            case OpcUaType_Guid: OpcUa_Guid_Copy(target->Value.Guid, source->Value.Guid); break;
            case OpcUaType_ByteString: ID_OpcUa_ByteString_CopyTo(&source->Value.ByteString, &target->Value.ByteString); break;
            case OpcUaType_XmlElement: ID_OpcUa_ByteString_CopyTo(&source->Value.XmlElement, &target->Value.XmlElement); break;
            case OpcUaType_NodeId: ID_OpcUa_NodeId_CopyTo(source->Value.NodeId, target->Value.NodeId); break;
            case OpcUaType_ExpandedNodeId: ID_AddressSpace_CopyExpandedNodeId(source->Value.ExpandedNodeId, target->Value.ExpandedNodeId); break;
            case OpcUaType_StatusCode: target->Value.StatusCode = source->Value.StatusCode; break;
            case OpcUaType_LocalizedText: uStatus = ID_AddressSpace_CopyLocalizedText(target->Value.LocalizedText, source->Value.LocalizedText); break;
            case OpcUaType_ExtensionObject: uStatus = ID_AddressSpace_CopyExtensionObject(target->Value.ExtensionObject, source->Value.ExtensionObject); break;
            case OpcUaType_DataValue: uStatus = ID_AddressSpace_CopyDataValue(target->Value.DataValue, source->Value.DataValue); break;
            case OpcUaType_QualifiedName:
                ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&target->Value.QualifiedName->Name, &source->Value.QualifiedName->Name);
                target->Value.QualifiedName->NamespaceIndex = source->Value.QualifiedName->NamespaceIndex;
                break;
            default:
                uStatus = OpcUa_BadNotSupported;
                break;
        }
    } else if (source->ArrayType == OpcUa_VariantArrayType_Array) {
        OpcUa_UInt32 Length = source->Value.Array.Length;
        target->Value.Array.Length = source->Value.Array.Length;
        if (Length == 0) {
            return OpcUa_GoodNoData;
        }
        
        for (OpcUa_UInt32 i = 0; i < Length; i++) {
            uStatus = ID_AddressSpace_CopyVariantArray(source, i, target, i);
            if (OpcUa_IsBad(uStatus)) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unable to copy variant with Array Type (%d) & Data Type (%d) from %p to %p\n", source->ArrayType, source->Datatype, source, target);
                return uStatus;
            }
        }
    }
    
    return uStatus;
}

void ID_AddressSpace_InitializeExtensionObject(OpcUa_ExtensionObject * Extension, OpcUa_ExtensionObjectEncoding Encoding, OpcUa_ExpandedNodeId * EncodeableNodeId, OpcUa_EncodeableType * Type) {
    // Copy parameters in
    Extension->Encoding = Encoding;
    Extension->TypeId.ServerIndex = EncodeableNodeId->ServerIndex;
    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&Extension->TypeId.NamespaceUri, &EncodeableNodeId->NamespaceUri);

    if (Encoding == OpcUa_ExtensionObjectEncoding_EncodeableObject) {  
        // initialize the body
        Extension->Body.EncodeableObject.Type = Type;
        OpcUa_UInt32 AllocSize = Type->AllocationSize;
        Extension->Body.EncodeableObject.Object = malloc(AllocSize);
        Type->Initialize(Extension->Body.EncodeableObject.Object);
        
        Extension->TypeId.NodeId.NamespaceIndex = ID_NAMESPACE_INDEX_OPC;
        Extension->TypeId.NodeId.IdentifierType = OpcUa_IdType_Numeric;
        Extension->TypeId.NodeId.Identifier.Numeric = Type->BinaryEncodingTypeId;
    }
}

void ID_AddressSpace_SetupVariantOnVariable(void * Node, OpcUa_Int32 Length, OpcUa_BuiltInType BuiltInType, OpcUa_ExtensionObjectEncoding Encoding, OpcUa_ExpandedNodeId * EncodeableNodeId, OpcUa_EncodeableType * Type) {
    // setup default scalar configuration
    ID_VariableTypeNode * LocalNode = Node;
    LocalNode->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
    LocalNode->ValueRank = OpcUa_ValueRanks_Scalar;
    LocalNode->NoOfArrayDimensions = 0;
    if (LocalNode->ArrayDimensions != NULL) { free(LocalNode->ArrayDimensions); }
    LocalNode->ArrayDimensions = NULL;

    int VariantArrayType = OpcUa_VariantArrayType_Scalar;
    if (Length != 0) {
        // set the valueRank depending on whether Length is equal to ValueRank_Any
        // if not assume sufficient to shove OneDimension down
        LocalNode->ValueRank = Length == ID_VALUERANK_ANY_INDICATOR ? OpcUa_ValueRanks_Any : OpcUa_ValueRanks_OneDimension;
        if (LocalNode->ValueRank == OpcUa_ValueRanks_OneDimension) {
            VariantArrayType = OpcUa_VariantArrayType_Array;

            LocalNode->Value.ArrayType = OpcUa_VariantArrayType_Array;
            LocalNode->NoOfArrayDimensions = 1;
            LocalNode->ArrayDimensions = malloc(sizeof(OpcUa_UInt32));
            LocalNode->ArrayDimensions[0] = 0;
            if (Length != ID_LENGTH_UNKNOWN_INDICATOR) {
                LocalNode->ArrayDimensions[0] = Length;
            } else {
                // set to zero so next part will work correctly
                Length = 0;
            }
        }
    }
    
    /* setup variant */
    if (LocalNode->ValueRank != OpcUa_ValueRanks_Any) {
        LocalNode->Value.Datatype = BuiltInType;
        LocalNode->Value.ArrayType = (OpcUa_Byte)VariantArrayType;
        if (LocalNode->Value.ArrayType == OpcUa_VariantArrayType_Array) {
            LocalNode->Value.Value.Array.Length = Length;
        }
        
        // intiialize the variant object
        // caller ensures that the Value is empty prior to calling this function
        ID_AddressSpace_InitializeVariant(&LocalNode->Value);

        // if Type is specified, intiialize the ExtensionObject as necessary
        if (Type != NULL) {
            if (LocalNode->Value.ArrayType == OpcUa_VariantArrayType_Array) {
                for (int i = 0; i < LocalNode->Value.Value.Array.Length; i++) {
                    ID_AddressSpace_InitializeExtensionObject(&LocalNode->Value.Value.Array.Value.ExtensionObjectArray[i], Encoding, EncodeableNodeId, Type);
                }
            } else if (LocalNode->Value.ArrayType == OpcUa_VariantArrayType_Scalar) {
                ID_AddressSpace_InitializeExtensionObject(LocalNode->Value.Value.ExtensionObject, Encoding, EncodeableNodeId, Type);
            }
        }
    }
}

OpcUa_Boolean ID_AddressSpace_AreIdsEqual(OpcUa_NodeId * pValue1, OpcUa_NodeId * pValue2) {
    if (pValue1 == NULL || pValue2 == NULL) {
        return OpcUa_False;
    }
    
    if (pValue1->NamespaceIndex != pValue2->NamespaceIndex) {
        return OpcUa_False;
    }
    
    if (pValue1->IdentifierType != pValue2->IdentifierType) {
        return OpcUa_False;
    }
    
    switch (pValue1->IdentifierType) {
        case OpcUa_IdentifierType_Numeric:
            if (pValue1->Identifier.Numeric == pValue2->Identifier.Numeric) {
                return OpcUa_True;
            }
            
            return OpcUa_False;
            break;
        case OpcUa_IdentifierType_String: {
            OpcUa_UInt32 len = OpcUa_String_StrLen(&pValue1->Identifier.String);
            return OpcUa_String_StrnCmp(&pValue1->Identifier.String, &pValue2->Identifier.String, len, OpcUa_False) == 0 ? OpcUa_True : OpcUa_False;
            break;}
        case OpcUa_IdentifierType_Opaque:
            if (pValue1->Identifier.ByteString.Length != pValue2->Identifier.ByteString.Length) {
                return OpcUa_False;
            }
            
            int i = 0;
            for (i = 0; i < pValue1->Identifier.ByteString.Length; i++) {
                if (pValue1->Identifier.ByteString.Data[i] != pValue2->Identifier.ByteString.Data[i]) {
                    return OpcUa_False;
                }
            }
            
            return OpcUa_True;
            break;
        case OpcUa_IdentifierType_Guid:
            return OpcUa_Guid_IsEqual(pValue1->Identifier.Guid, pValue2->Identifier.Guid);
            break;
    }
    
    return OpcUa_True;
}

#pragma warning(push)
#pragma warning(disable: 4204)
// Convenience function to create the NodeVersion property on a Node with ParentNodeId
// the NodeVersion has a default value set to "1"
void ID_AddressSpace_CreateNodeVersionProperty(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, OpcUa_NodeId * ParentNodeId, ID_NodeType ParentType, ID_VariableNode ** VarNode) {
    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;
    
    ID_VariableNode * NVersion;
    OpcUa_NodeId DataTypeId = { OpcUa_IdType_Numeric, ID_NAMESPACE_INDEX_OPC, OpcUaId_String };
    OpcUa_NodeId NodeId = { OpcUa_IdType_Numeric, ID_NAMESPACE_INDEX, ID_Indexer_GetNextIndex(Indexer) };
    ID_AddressSpace_CreateProperty(AddressSpace, ParentNodeId, ParentType, &NVersion, 0, "NodeVersion", &NodeId, &DataTypeId, 0, &ID_ModellingRuleMandatoryNode, OpcUaType_String, 0, NULL, NULL);
    NVersion->Value.Datatype = OpcUaType_String;
    OpcUa_String_AttachCopy(&NVersion->Value.Value.String, "1");
    
    if (VarNode != NULL) {
        *VarNode = NVersion;
    }
}
#pragma warning(pop)

void ID_AddressSpace_CreateProperty(ID_AddressSpaceHandle AddressSpace, OpcUa_NodeId * ParentNodeId, ID_NodeType ParentType,
                                    ID_VariableNode ** Node, OpcUa_Int BrowseNamespace, OpcUa_StringA BrowseName, OpcUa_NodeId * NodeId,
                                    OpcUa_NodeId * DataTypeId, OpcUa_Int32 Length, OpcUa_NodeId * ModellingRuleId,
                                    OpcUa_BuiltInType BuiltInType, OpcUa_ExtensionObjectEncoding Encoding, 
                                    OpcUa_ExpandedNodeId * EncodeableNodeId, OpcUa_EncodeableType * Type) {
    OpcUa_NodeId ID_HasPropertyNode = ID_HasProperty;  
    OpcUa_NodeId ID_PropertyTypeNode = ID_PropertyType;
    OpcUa_NodeId ID_HasModellingRuleNode = ID_HasModellingRule;
    OpcUa_NodeId ID_HasTypeDefinitionNode = ID_HasTypeDefinition;

    // Create variable node with intiial parameters
    ID_VariableNode * LocalNode;
    ID_AddressSpace_CreateVariableNode(AddressSpace, &LocalNode, NULL);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(LocalNode, (OpcUa_UInt16)BrowseNamespace, BrowseName);
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(LocalNode, BrowseName);
    LocalNode->BaseAttribute.NodeClass = OpcUa_NodeClass_Variable;
    LocalNode->AccessLevel = 0;
    LocalNode->UserAccessLevel = 0;
    LocalNode->Historizing = OpcUa_False;
    LocalNode->MinimumSamplingInterval = -1;
    
    ID_OpcUa_NodeId_CopyTo(NodeId, &LocalNode->BaseAttribute.NodeId);
    ID_OpcUa_NodeId_CopyTo(DataTypeId, &LocalNode->DataType);
    
    // Setup the variant on the variable
    // DOES not set a value, CALLER must set a value if needed
    ID_AddressSpace_SetupVariantOnVariable(LocalNode, Length, BuiltInType, Encoding, EncodeableNodeId, Type);

    // Construct relevant references
    OpcUa_NodeId * ParentLink = &ID_HasPropertyNode;
    ID_AddressSpace_AddBiReference(AddressSpace, &LocalNode->BaseAttribute.NodeId, ID_NodeType_Variable, &ID_PropertyTypeNode, ID_NodeType_VariableType, &ID_HasTypeDefinitionNode);
    ID_AddressSpace_AddUniReference(AddressSpace, &LocalNode->BaseAttribute.NodeId, ID_NodeType_Variable, ModellingRuleId, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(AddressSpace, ParentNodeId, ParentType, &LocalNode->BaseAttribute.NodeId, ID_NodeType_Variable, ParentLink);
    
    if (Node != NULL) {
        *Node = LocalNode;
    }
}

#pragma warning(push)
#pragma warning(disable: 4204)
void ID_CreateComponentProperties(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_VariableNode * Node) {
    OpcUa_NodeId ID_ModellingRuleOptionalNode = ID_ModellingRuleOptional;
    OpcUa_QualifiedName Search;
    OpcUa_QualifiedName_Initialize(&Search);

    // all of these properties are built on the non-OPC namespace
    Search.NamespaceIndex = ID_NAMESPACE_INDEX;

    // create NodeVersion if not exists
    OpcUa_String_AttachCopy(&Search.Name, "NodeVersion");
    if (ID_AddressSpace_HuntForReferenceWithNode(AddressSpace, (ID_BaseAttributes*)Node, Search) == NULL) {
        ID_AddressSpace_CreateNodeVersionProperty(AddressSpace, Indexer, &Node->BaseAttribute.NodeId, ID_NodeType_Variable, NULL);
    }
    
    // create LocalTime if not exists
    OpcUa_String_AttachCopy(&Search.Name, "LocalTime");
    if (ID_AddressSpace_HuntForReferenceWithNode(AddressSpace, (ID_BaseAttributes*)Node, Search) == NULL) {
        OpcUa_NodeId Id = ID_OPC_IDNodeId(ID_Indexer_GetNextIndex(Indexer));
        OpcUa_NodeId TypeId = ID_OPCNodeId(OpcUaId_TimeZoneDataType);
        OpcUa_ExpandedNodeId ETypeId = ID_OPCExpandedNodeId(OpcUaId_TimeZoneDataType_Encoding_DefaultBinary);
        ID_AddressSpace_CreateProperty(AddressSpace, &Node->BaseAttribute.NodeId, ID_NodeType_Variable, NULL, 0, "LocalTime", &Id, &TypeId, 0, &ID_ModellingRuleOptionalNode, OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &ETypeId, &OpcUa_TimeZoneDataType_EncodeableType);
    }

    // create allowNulls if not exist
    OpcUa_String_AttachCopy(&Search.Name, "AllowNulls");
    if (ID_AddressSpace_HuntForReferenceWithNode(AddressSpace, (ID_BaseAttributes*)Node, Search) == NULL) {
        OpcUa_NodeId Id = ID_OPC_IDNodeId(ID_Indexer_GetNextIndex(Indexer));
        OpcUa_NodeId TypeId = ID_OPCNodeId(OpcUaId_Boolean);
        ID_AddressSpace_CreateProperty(AddressSpace, &Node->BaseAttribute.NodeId, ID_NodeType_Variable, NULL, 0, "AllowNulls", &Id, &TypeId, 0, &ID_ModellingRuleOptionalNode, OpcUaType_Boolean, 0, NULL, NULL);
    }

    // create ValueAsText for enumerations if not exist
    OpcUa_NodeId EnumerationType = ID_OPCNodeId(OpcUaId_Enumeration);
    OpcUa_NodeId * DataTypeId = &Node->DataType;
    OpcUa_String_AttachCopy(&Search.Name, "ValueAsText");
    if ((DataTypeId->Identifier.Numeric == OpcUaId_Enumeration || ID_AddressSpace_IsTypeASubtypeofB(AddressSpace, DataTypeId, &EnumerationType)) &&
        ID_AddressSpace_HuntForReferenceWithNode(AddressSpace, (ID_BaseAttributes*)Node, Search) == NULL) {

        OpcUa_NodeId Id = ID_OPC_IDNodeId(ID_Indexer_GetNextIndex(Indexer));
        OpcUa_NodeId TypeId = ID_OPCNodeId(OpcUaId_LocalizedText);
        ID_AddressSpace_CreateProperty(AddressSpace, &Node->BaseAttribute.NodeId, ID_NodeType_Variable, NULL, 0, "ValueAsText", &Id, &TypeId, 0, &ID_ModellingRuleOptionalNode, OpcUaType_LocalizedText, 0, NULL, NULL);
    }
    
    // create MaxStringLength for String datatypes if not exist
    OpcUa_String_AttachCopy(&Search.Name, "MaxStringLength");
    if (DataTypeId->Identifier.Numeric == OpcUaId_String && 
        ID_AddressSpace_HuntForReferenceWithNode(AddressSpace, (ID_BaseAttributes*)Node, Search) == NULL) {

        OpcUa_NodeId Id = ID_OPC_IDNodeId(ID_Indexer_GetNextIndex(Indexer));
        OpcUa_NodeId TypeId = ID_OPCNodeId(OpcUaId_UInt32);
        ID_AddressSpace_CreateProperty(AddressSpace, &Node->BaseAttribute.NodeId, ID_NodeType_Variable, NULL, 0, "MaxStringLength", &Id, &TypeId, 0, &ID_ModellingRuleOptionalNode, OpcUaType_UInt32, 0, NULL, NULL);
    }
    
    // create MaxByteStringLength for ByteString datatypes if not exist
    OpcUa_String_AttachCopy(&Search.Name, "MaxByteStringLength");
    if (DataTypeId->Identifier.Numeric == OpcUaId_ByteString &&
        ID_AddressSpace_HuntForReferenceWithNode(AddressSpace, (ID_BaseAttributes*)Node, Search) == NULL) {

        OpcUa_NodeId Id = ID_OPC_IDNodeId(ID_Indexer_GetNextIndex(Indexer));
        OpcUa_NodeId TypeId = ID_OPCNodeId(OpcUaId_UInt32);
        ID_AddressSpace_CreateProperty(AddressSpace, &Node->BaseAttribute.NodeId, ID_NodeType_Variable, NULL, 0, "MaxByteStringLength", &Id, &TypeId, 0, &ID_ModellingRuleOptionalNode, OpcUaType_UInt32, 0, NULL, NULL);
    }
    
    // create maxArrayLength for nonscalars if not exist
    OpcUa_String_AttachCopy(&Search.Name, "MaxArrayLength");
    if (Node->ValueRank != OpcUa_ValueRanks_Scalar &&
        ID_AddressSpace_HuntForReferenceWithNode(AddressSpace, (ID_BaseAttributes*)Node, Search) == NULL) {

        OpcUa_NodeId Id = ID_OPC_IDNodeId(ID_Indexer_GetNextIndex(Indexer));
        OpcUa_NodeId TypeId = ID_OPCNodeId(OpcUaId_UInt32);
        ID_AddressSpace_CreateProperty(AddressSpace, &Node->BaseAttribute.NodeId, ID_NodeType_Variable, NULL, 0, "MaxArrayLength", &Id, &TypeId, 0, &ID_ModellingRuleOptionalNode, OpcUaType_UInt32, 0, NULL, NULL);
    }
    
    // create EngineeringUnits for numeric datatypes if not exist
    OpcUa_String_AttachCopy(&Search.Name, "EngineeringUnits");
    if (ID_AddressSpace_IsNumericDataType(&Node->DataType) == OpcUa_True &&
        ID_AddressSpace_HuntForReferenceWithNode(AddressSpace, (ID_BaseAttributes*)Node, Search) == NULL) {
        OpcUa_NodeId Id = ID_OPC_IDNodeId(ID_Indexer_GetNextIndex(Indexer));
        OpcUa_NodeId TypeId = ID_OPCNodeId(OpcUaId_EUInformation);

        OpcUa_ExpandedNodeId ETypeId;
        OpcUa_ExpandedNodeId_Initialize(&ETypeId);
        ETypeId.ServerIndex = 0; ID_OpcUa_NodeId_CopyTo(&TypeId, &ETypeId.NodeId);
        ID_AddressSpace_CreateProperty(AddressSpace, &Node->BaseAttribute.NodeId, ID_NodeType_Variable, NULL, 0, "EngineeringUnits", &Id, &TypeId, 0, &ID_ModellingRuleOptionalNode, OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &ETypeId, &OpcUa_EUInformation_EncodeableType);
    }

    OpcUa_QualifiedName_Clear(&Search);
}
#pragma warning(pop)

void ID_AddressSpace_CreateComponent(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer,
                                     OpcUa_NodeId * ParentNodeId, ID_NodeType ParentType, ID_VariableNode ** NodePtr,
                                     OpcUa_Int BrowseNamespace, OpcUa_StringA BrowseName, OpcUa_NodeId * NodeId, OpcUa_NodeId * DataTypeId, 
                                     OpcUa_Int32 Length, OpcUa_NodeId * ComponentType, OpcUa_NodeId * ModellingRule,
                                     OpcUa_BuiltInType BuiltInType, OpcUa_ExtensionObjectEncoding Encoding, OpcUa_ExpandedNodeId * EncodeableNodeId, OpcUa_EncodeableType * Type) {
    OpcUa_NodeId ID_HasTypeDefinitionNode = ID_HasTypeDefinition;
    OpcUa_NodeId ID_HasModellingRuleNode = ID_HasModellingRule;
    OpcUa_NodeId ID_HasComponentNode = ID_HasComponent;
    
    // Creates a variable node
    ID_VariableNode * VarNode = NULL;
    ID_AddressSpace_CreateVariableNode(AddressSpace, &VarNode, NULL);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(VarNode, (OpcUa_UInt16)BrowseNamespace, BrowseName);
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(VarNode, BrowseName);

    // Copy datatype nodes
    ID_OpcUa_NodeId_CopyTo(DataTypeId, &VarNode->DataType);
    ID_OpcUa_NodeId_CopyTo(NodeId, &VarNode->BaseAttribute.NodeId);

    // Inherit the references
    ID_AddressSpace_InheritReferences(AddressSpace, Indexer, ComponentType, NodeId);

    // Setup the variant on the variable by allowing for custom override
    // THIS mechanism WILL NOT COPY the variant from the Variable type
    // due to ability for override. MANUAL copying is required if necessary
    ID_AddressSpace_SetupVariantOnVariable(VarNode, Length, BuiltInType, Encoding, EncodeableNodeId, Type);

    // Add relevant references
    ID_AddressSpace_AddBiReference(AddressSpace, NodeId, ID_NodeType_Variable, ComponentType, ID_NodeType_VariableType, &ID_HasTypeDefinitionNode);
    ID_AddressSpace_AddUniReference(AddressSpace, NodeId, ID_NodeType_Variable, ModellingRule, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(AddressSpace, ParentNodeId, ParentType, NodeId, ID_NodeType_Variable, &ID_HasComponentNode);
    
    // Create component properties
    ID_CreateComponentProperties(AddressSpace, Indexer, VarNode);

    if (NodePtr != NULL) {
        *NodePtr = VarNode;
    }
}

OpcUa_Boolean ID_AddressSpace_IsNumericDataType(OpcUa_NodeId * DataType) {
    if (DataType->IdentifierType != OpcUa_IdType_Numeric) {
        return OpcUa_False;
    }

    if (DataType->NamespaceIndex != ID_NAMESPACE_INDEX_OPC) {
        return OpcUa_False;
    }
    
    switch (DataType->Identifier.Numeric) {
        case 26:
        case 27:
        case 28:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            return OpcUa_True;
            break;
        default:
            return OpcUa_False;
            break;
    }
}

void * ID_AddressSpace_HuntForReferenceWithNodeId(ID_AddressSpaceHandle Handle, OpcUa_NodeId * Source, OpcUa_QualifiedName BrowseName) {
    ID_BaseAttributes * Attrs = (ID_BaseAttributes*)ID_AddressSpace_GetNode(Handle, Source);
    return ID_AddressSpace_HuntForReferenceWithNode(Handle, Attrs, BrowseName);
}

void * ID_AddressSpace_HuntForReferenceWithNode(ID_AddressSpaceHandle Handle, ID_BaseAttributes * Source, OpcUa_QualifiedName BrowseName) {
    for (OpcUa_UInt32 i = 0; i < Source->NoOfReferences; i++) {
        ID_ReferenceNode * RefNode = Source->References[i];
        ID_BaseAttributes * RefAttrs = (ID_BaseAttributes*)ID_AddressSpace_GetNode(Handle, &RefNode->TargetNodeId);
        if (RefAttrs != NULL) {
            // Check if QualifiedName is the same
            if (RefAttrs->BrowseName.NamespaceIndex == BrowseName.NamespaceIndex) {
                char * RefAttrsRaw = OpcUa_String_GetRawString(&RefAttrs->BrowseName.Name);
                char * BrowseNameRaw = OpcUa_String_GetRawString(&BrowseName.Name);
                if (RefAttrsRaw && BrowseNameRaw != NULL && strcmp(RefAttrsRaw, BrowseNameRaw) == 0) {
                    return RefAttrs;
                }
            }
        }
    }

    return NULL;
}

OpcUa_Boolean ID_AddressSpace_IsTypeASubtypeofB(ID_AddressSpaceHandle Handle, OpcUa_NodeId * Sub, OpcUa_NodeId * Parent) {
    OpcUa_NodeId ID_HasSubtypeNode = ID_HasSubtype;

    ID_BaseAttributes * AAttrs = ID_AddressSpace_GetNode(Handle, Sub);
    ID_BaseAttributes * BAttrs = ID_AddressSpace_GetNode(Handle, Parent);
    if (AAttrs == NULL || BAttrs == NULL) {
        return OpcUa_False;
    }
    
    if (AAttrs == BAttrs) {
        return OpcUa_True;
    }
    
    // find the inverse HasSubtype to find the parent of the tyep Sub
    OpcUa_NodeId * ParentReference = NULL;
    for (OpcUa_UInt32 i = 0; i < AAttrs->NoOfReferences; i++) {
        ID_ReferenceNode * RefNode = AAttrs->References[i];
        if (RefNode->IsInverse == OpcUa_True && ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasSubtypeNode) == OpcUa_True) {
            ParentReference = &RefNode->TargetNodeId;
            break;
        }
    }
    
    if (ParentReference == NULL) {
        return OpcUa_False;
    }
    
    ID_BaseAttributes * RefParentNode = ID_AddressSpace_GetNode(Handle, ParentReference);
    if (RefParentNode == NULL) {
        return OpcUa_False;
    }
    
    if (BAttrs == RefParentNode) {
        return OpcUa_True;
    }

    // recursively check next type till the top is reached
    return ID_AddressSpace_IsTypeASubtypeofB(Handle, &RefParentNode->NodeId, Parent);
}

void ID_CreateArgument(ID_AddressSpaceHandle AddressSpace, ID_MethodNode * Node, ID_VariableNode ** ArgumentNode, OpcUa_Boolean IsOut, OpcUa_NodeId Id, OpcUa_UInt32 Size, ID_InputArgsInfo * Args) {
    if (Size == 0) {
        return;
    }

    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;
    ID_VariableNode * Argument;
    char * BrowseName = IsOut == OpcUa_True ? "OutputArguments" : "InputArguments";
    OpcUa_NodeId DataTypeId = ID_OPCNodeId(OpcUaId_Argument);
    OpcUa_ExpandedNodeId ArgumentExpanded;
    OpcUa_ExpandedNodeId_Initialize(&ArgumentExpanded);
    ArgumentExpanded.NodeId.Identifier.Numeric = OpcUaId_Argument_Encoding_DefaultBinary;
    ID_AddressSpace_CreateProperty(AddressSpace, &Node->BaseAttribute.NodeId, ID_NodeType_Method, &Argument, 0, BrowseName, &Id, &DataTypeId, Size, &ID_ModellingRuleMandatoryNode, OpcUaType_ExtensionObject, OpcUa_ExtensionObjectEncoding_EncodeableObject, &ArgumentExpanded, &OpcUa_Argument_EncodeableType);

    for (OpcUa_UInt32 i = 0; i < Size; i++) {
        ID_InputArgsInfo ArgInfo = Args[i];
        OpcUa_Argument * Arg = Argument->Value.Value.Array.Value.ExtensionObjectArray[i].Body.EncodeableObject.Object;
        Arg->ValueRank = ArgInfo.ValueRank;
        if (Arg->ValueRank == OpcUa_ValueRanks_OneDimension) {
            Arg->ArrayDimensions = malloc(sizeof(OpcUa_UInt32));
            Arg->ArrayDimensions[0] = ArgInfo.OneDimensionLength;
        }

        ID_OpcUa_NodeId_CopyTo(&ArgInfo.DataTypeIdentifier, &Arg->DataType);
        OpcUa_String_AttachCopy(&Arg->Name, Args[i].Name);

        ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&Arg->Description.Locale, &ArgInfo.Description->Locale);
        ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&Arg->Description.Text, &ArgInfo.Description->Text);
    }

    // set read/write
    ID_CONFIGURE_VARIABLE_AS_READ_ONLY(Argument);
    if (ArgumentNode != NULL) {
        *ArgumentNode = Argument;
    }
}

#pragma warning(push)
#pragma warning(disable: 4204)
void ID_AddressSpaceHelper_CreateMethodNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_MethodNode ** Node, OpcUa_NodeId Id, OpcUa_Int BrowseNamespace, char * BrowseName,
                                            OpcUa_Int32 InputNum, OpcUa_NodeId InputArgId, ID_InputArgsInfo * InputArgs,
                                            OpcUa_Int32 OutputNum, OpcUa_NodeId OutputArgId, ID_InputArgsInfo * OutputArgs) {
    ID_MethodNode * LocalNode;
    ID_AddressSpace_CreateMethodNode(AddressSpace, &LocalNode, NULL);
    LocalNode->Executable = OpcUa_False;
    LocalNode->Function = NULL;
    LocalNode->UserExecutable = OpcUa_False;
    LocalNode->BaseAttribute.NodeClass = OpcUa_NodeClass_Method;
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(LocalNode, (OpcUa_UInt16)BrowseNamespace, BrowseName);
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(LocalNode, BrowseName);
    ID_OpcUa_NodeId_CopyTo(&Id, &LocalNode->BaseAttribute.NodeId);
    ID_AddressSpace_CreateNodeVersionProperty(AddressSpace, Indexer, &LocalNode->BaseAttribute.NodeId, ID_NodeType_Method, NULL);

    ID_CreateArgument(AddressSpace, LocalNode, NULL, OpcUa_False, InputArgId, InputNum, InputArgs);
    ID_CreateArgument(AddressSpace, LocalNode, NULL, OpcUa_True, OutputArgId, OutputNum, OutputArgs);

    if (Node != NULL) {
        *Node = LocalNode;
    }
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4204)
void ID_AddressSpaceHelper_CreateObjectType(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, ID_ObjectTypeNode * Parent, ID_ObjectTypeNode ** NodePtr,
                                            OpcUa_Int BrowseNamespace, OpcUa_StringA BrowseName, OpcUa_Boolean IsAbstract, OpcUa_NodeId NodeId,
                                            OpcUa_UInt32 PropertyCount, ID_PropertyAddition * Properties) {
    OpcUa_NodeId ID_HasSubtypeNode = ID_HasSubtype;
    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;

    ID_ObjectTypeNode * LocalNode;
    if (Parent != NULL) { ID_AddressSpace_CloneObjectTypeNode(Handle, Indexer, Parent, &NodeId, &LocalNode); }
    else {
        ID_AddressSpace_CreateObjectTypeNode(Handle, &LocalNode, NULL);
        ID_OpcUa_NodeId_CopyTo(&NodeId, &LocalNode->BaseAttribute.NodeId);
    }

    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(LocalNode, (OpcUa_UInt16)BrowseNamespace, BrowseName);
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(LocalNode, BrowseName);
    LocalNode->IsAbstract = IsAbstract;
    if (Parent != NULL) { ID_AddressSpace_AddBiReference(Handle, &Parent->BaseAttribute.NodeId, ID_NodeType_ObjectType, &NodeId, ID_NodeType_ObjectType, &ID_HasSubtypeNode); }
    else {
        OpcUa_NodeId IconId = { OpcUa_IdType_Numeric, ID_NAMESPACE_INDEX, ID_Indexer_GetNextIndex(Indexer) };
        OpcUa_NodeId ImageDataTypeId = ID_OPCNodeId(OpcUaId_Image);
        ID_AddressSpace_CreateProperty(Handle, &NodeId, ID_NodeType_ObjectType, NULL, 0, "Icon", &IconId, &ImageDataTypeId, 0, &ID_ModellingRuleMandatoryNode, OpcUaType_ByteString, 0, NULL, NULL);
        ID_AddressSpace_CreateNodeVersionProperty(Handle, Indexer, &NodeId, ID_NodeType_ObjectType, NULL);
    }

    for (OpcUa_UInt32 i = 0; i < PropertyCount; i++) {
        ID_PropertyAddition * Property = &Properties[i];
        OpcUa_NodeId PId = ID_OPCNodeId(Property->Id);
        PId.NamespaceIndex = (OpcUa_UInt16)Property->NamespaceIndex;

        OpcUa_NodeId PTypeId = ID_OPCNodeId(Property->Type);
        OpcUa_Int32 Length = Property->Length;
        OpcUa_BuiltInType BType = Property->VariantType;
        OpcUa_ExtensionObjectEncoding Encoding = Property->VariantEncoding;
        OpcUa_ExpandedNodeId * VariantNode = Property->VariantTypeId;
        OpcUa_EncodeableType * Type = Property->EncodeableType;
        if (Property->IsProperty == OpcUa_True) {
            ID_AddressSpace_CreateProperty(Handle, &NodeId, ID_NodeType_ObjectType, NULL, Property->NameNamespace, Property->Name, &PId, &PTypeId, Length, Property->ModellingRule, BType, Encoding, VariantNode, Type);
        }
        else {
            ID_AddressSpace_CreateComponent(Handle, Indexer, &NodeId, ID_NodeType_ObjectType, NULL, Property->NameNamespace, Property->Name, &PId, &PTypeId, Length, Property->ComponentType, Property->ModellingRule, BType, Encoding, VariantNode, Type);
        }
    }

    if (NodePtr != NULL) {
        *NodePtr = LocalNode;
    }
}
#pragma warning(pop)

void ID_AddressSpaceHelper_CreateVariable(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, OpcUa_NodeId * Parent, ID_NodeType ParentType, OpcUa_NodeId * VariableId, OpcUa_Int BrowseNamespace, char * BrowseName, OpcUa_NodeId * VariableType, OpcUa_NodeId * ModellingRule) {
    OpcUa_NodeId ID_HasModellingRuleNode = ID_HasModellingRule;
    OpcUa_NodeId ID_HasComponentNode = ID_HasComponent;
    
    ID_VariableTypeNode * VarTypeNode = (ID_VariableTypeNode*)ID_AddressSpace_GetVariableNode(Handle, VariableType);
    ID_VariableNode * VarNode = ID_AddressSpace_CreateVariableFromVariableType(Handle, Indexer, VarTypeNode, VariableId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(VarNode, (OpcUa_UInt16)BrowseNamespace, BrowseName);
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(VarNode, BrowseName);
    ID_AddressSpace_AddUniReference(Handle, VariableId, ID_NodeType_Variable, ModellingRule, &ID_HasModellingRuleNode, OpcUa_False);
    ID_AddressSpace_AddBiReference(Handle, Parent, ParentType, VariableId, ID_NodeType_Variable, &ID_HasComponentNode);
}

void ID_AddressSpaceHelper_CreateVariableTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, ID_VariableTypeNode ** NodePtr,
                                                  OpcUa_NodeId NodeId, OpcUa_Int BrowseNamespace, OpcUa_StringA BrowseName, OpcUa_Boolean IsAbstract,
                                                  OpcUa_Int32 Length, OpcUa_NodeId DataTypeId, OpcUa_NodeId * ParentNodeId, ID_VariantStructure * MainStructure,
                                                  int ChildrenCount, int * ChildNameNamespaces, char ** ChildNames, int * ChildNodeId, int * ChildDataTypeId,
                                                  int * ChildTypeDataTypeId, int * ChildLengths, ID_VariantStructure * Structures) {

    OpcUa_NodeId ID_HasSubtypeNode = ID_HasSubtype;
    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;
    ID_VariableTypeNode * Node;
    ID_AddressSpace_CreateVariableTypeNode(AddressSpace, &Node, NULL);

    Node->BaseAttribute.NodeClass = OpcUa_NodeClass_VariableType;
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(Node, (OpcUa_UInt16)BrowseNamespace, BrowseName);
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(Node, BrowseName);
    ID_OpcUa_NodeId_CopyTo(&NodeId, &Node->BaseAttribute.NodeId);
    ID_OpcUa_NodeId_CopyTo(&DataTypeId, &Node->DataType);
    Node->IsAbstract = IsAbstract;
    ID_AddressSpace_SetupVariantOnVariable(Node, Length, MainStructure->BuiltInType, MainStructure->Encoding, MainStructure->ExpandedNodeId, MainStructure->Type);

    if (strcmp(BrowseName, "PropertyType") != 0 || BrowseNamespace != 0) {
        ID_AddressSpace_CreateNodeVersionProperty(AddressSpace, Indexer, &NodeId, ID_NodeType_VariableType, NULL);
    }

    if (ParentNodeId != NULL) {
        ID_AddressSpace_AddBiReference(AddressSpace, ParentNodeId, ID_NodeType_VariableType, &Node->BaseAttribute.NodeId, ID_NodeType_VariableType, &ID_HasSubtypeNode);
    }

    for (int i = 0; i < ChildrenCount; i++) {
#pragma warning(push)
#pragma warning(disable: 4204)
        OpcUa_NodeId ChildNodeValue = ID_OPCNodeId(ChildNodeId[i]);
        OpcUa_NodeId ChildValueDataType = ID_OPCNodeId(ChildDataTypeId[i]);
        OpcUa_NodeId ChildParentDataType = ID_OPCNodeId(ChildTypeDataTypeId[i]);
#pragma warning(pop)

        OpcUa_Boolean IsComponent = ChildParentDataType.Identifier.Numeric != OpcUaId_PropertyType;
        ID_VariantStructure variantInfo = Structures[i];
        OpcUa_NodeId * ModellingRule = variantInfo.ModellingRule;
        if (ModellingRule == NULL) { ModellingRule = &ID_ModellingRuleMandatoryNode; }

        if (IsComponent == OpcUa_False) {
            ID_AddressSpace_CreateProperty(AddressSpace, &NodeId, ID_NodeType_VariableType, NULL, ChildNameNamespaces[i], ChildNames[i], &ChildNodeValue, &ChildValueDataType, ChildLengths[i], ModellingRule, variantInfo.BuiltInType, variantInfo.Encoding, variantInfo.ExpandedNodeId, variantInfo.Type);
        }
        else {
            ID_AddressSpace_CreateComponent(AddressSpace, Indexer, &NodeId, ID_NodeType_VariableType, NULL, ChildNameNamespaces[i], ChildNames[i], &ChildNodeValue, &ChildValueDataType, ChildLengths[i], &ChildParentDataType, ModellingRule, variantInfo.BuiltInType, variantInfo.Encoding, variantInfo.ExpandedNodeId, variantInfo.Type);
        }
    }

    if (NodePtr != NULL) {
        *NodePtr = Node;
    }
}

void ID_AddressSpaceHelper_CreateReferenceTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer,
                                                   OpcUa_NodeId * ParentNode, ID_ReferenceTypeNode ** Node,
                                                   OpcUa_Boolean Symmetric, OpcUa_Boolean IsAbstract,
                                                   OpcUa_Int NameNamespace, OpcUa_StringA Name, OpcUa_StringA InverseName, OpcUa_NodeId Identifier) {
    OpcUa_NodeId ID_HasSubtypeNode = ID_HasSubtype;
    ID_ReferenceTypeNode * LocalNode;
    ID_AddressSpace_CreateReferenceTypeNode(AddressSpace, &LocalNode, NULL);
    LocalNode->Symmetric = Symmetric;
    LocalNode->IsAbstract = IsAbstract;
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(LocalNode, (OpcUa_UInt16)NameNamespace, Name);
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(LocalNode, Name);
    OpcUa_String_AttachCopy(&LocalNode->InverseName.Locale, "en-US");
    OpcUa_String_AttachCopy(&LocalNode->InverseName.Text, InverseName);
    ID_OpcUa_NodeId_CopyTo(&Identifier, &LocalNode->BaseAttribute.NodeId);
    ID_AddressSpace_CreateNodeVersionProperty(AddressSpace, Indexer, &LocalNode->BaseAttribute.NodeId, ID_NodeType_ReferenceType, NULL);
    if (ParentNode != NULL) {
        ID_AddressSpace_AddBiReference(AddressSpace, ParentNode, ID_NodeType_ReferenceType, &LocalNode->BaseAttribute.NodeId, ID_NodeType_ReferenceType, &ID_HasSubtypeNode);
    }

    if (Node != NULL) {
        *Node = LocalNode;
    }
}

void ID_AddressSpaceHelper_CreateDataTypeNode(ID_AddressSpaceHandle AddressSpace, ID_IndexerHandle Indexer, OpcUa_NodeId * ParentNode,
                                              ID_DataTypeNode ** Node, OpcUa_Boolean IsAbstract, OpcUa_Int NameNamespace, OpcUa_StringA Name, OpcUa_NodeId Identifier,
                                              OpcUa_UInt32 NumEnumValues, char ** Locales, char ** Texts, OpcUa_NodeId EnumId, int * ValueIds, OpcUa_NodeId * StructureEncodingId) {
    OpcUa_NodeId ID_HasSubtypeNode = ID_HasSubtype;
    OpcUa_NodeId ID_ModellingRuleMandatoryNode = ID_ModellingRuleMandatory;
    OpcUa_NodeId ID_HasTypeDefinitionNode = ID_HasTypeDefinition;
    ID_DataTypeNode * LocalNode;
    ID_AddressSpace_CreateDataTypeNode(AddressSpace, &LocalNode, NULL);

    LocalNode->IsAbstract = IsAbstract;
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(LocalNode, (OpcUa_UInt16)NameNamespace, Name);
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(LocalNode, Name);
    ID_OpcUa_NodeId_CopyTo(&Identifier, &LocalNode->BaseAttribute.NodeId);

    ID_AddressSpace_CreateNodeVersionProperty(AddressSpace, Indexer, &LocalNode->BaseAttribute.NodeId, ID_NodeType_DataType, NULL);
    if (ParentNode != NULL) {
        ID_AddressSpace_AddBiReference(AddressSpace, ParentNode, ID_NodeType_DataType, &LocalNode->BaseAttribute.NodeId, ID_NodeType_DataType, &ID_HasSubtypeNode);
    }

    if (NumEnumValues > 0) {
        char * BrowseName = ValueIds == NULL ? "EnumStrings" : "EnumValues";
#pragma warning (disable: 4204)
        OpcUa_NodeId EnumDataTypeId = { OpcUa_IdType_Numeric, 0, ValueIds == NULL ? OpcUaId_LocalizedText : OpcUaId_EnumValueType_Encoding_DefaultBinary };
        OpcUa_BuiltInType BuiltinType = ValueIds == NULL ? OpcUaType_LocalizedText : OpcUaType_ExtensionObject;
        OpcUa_ExtensionObjectEncoding Encoding = ValueIds == NULL ? 0 : OpcUa_ExtensionObjectEncoding_EncodeableObject;

        OpcUa_ExpandedNodeId EnumDataTypeExpandedNodeId;
        OpcUa_ExpandedNodeId_Initialize(&EnumDataTypeExpandedNodeId);
        EnumDataTypeExpandedNodeId.ServerIndex = 0;
        EnumDataTypeExpandedNodeId.NodeId.Identifier.Numeric = EnumDataTypeId.Identifier.Numeric;

        OpcUa_EncodeableType * Type = ValueIds == NULL ? NULL : &OpcUa_EnumValueType_EncodeableType;

        ID_VariableNode * EnumNode;
        ID_AddressSpace_CreateProperty(AddressSpace, &LocalNode->BaseAttribute.NodeId, ID_NodeType_DataType, &EnumNode, 0, BrowseName, &EnumId, &EnumDataTypeId, NumEnumValues, &ID_ModellingRuleMandatoryNode, BuiltinType, Encoding, &EnumDataTypeExpandedNodeId, Type);
        for (OpcUa_UInt32 i = 0; i < NumEnumValues; i++) {
            if (ValueIds == NULL) {
                OpcUa_String_AttachCopy(&EnumNode->Value.Value.Array.Value.LocalizedTextArray[i].Locale, Locales[i]);
                OpcUa_String_AttachCopy(&EnumNode->Value.Value.Array.Value.LocalizedTextArray[i].Text, Texts[i]);
            }
            else {
                OpcUa_EnumValueType * EnumValue = EnumNode->Value.Value.Array.Value.ExtensionObjectArray[i].Body.EncodeableObject.Object;
                EnumValue->Value = ValueIds[i];
                OpcUa_String_AttachCopy(&EnumValue->DisplayName.Locale, Locales[i]);
                OpcUa_String_AttachCopy(&EnumValue->DisplayName.Text, Texts[i]);
            }
        }
    }

    // check if a subtype of structure...
    // if subtype - bridge hasEncoding unidirectional
    OpcUa_NodeId StructureId = { OpcUa_IdType_Numeric, 0, OpcUaId_Structure };
    if (ID_AddressSpace_IsTypeASubtypeofB(AddressSpace, &LocalNode->BaseAttribute.NodeId, &StructureId) == OpcUa_True) {
        OpcUa_NodeId DataTypeEncodingTypeId = ID_OPCNodeId(OpcUaId_DataTypeEncodingType);
        ID_ObjectTypeNode * DataTypeEncodingType = ID_AddressSpace_GetObjectTypeNode(AddressSpace, &DataTypeEncodingTypeId);
        OpcUa_NodeId StructureEncoding;
        OpcUa_NodeId_Initialize(&StructureEncoding);
        if (StructureEncodingId != NULL) {
            ID_OpcUa_NodeId_CopyTo(StructureEncodingId, &StructureEncoding);
        }
        else {
            StructureEncoding.Identifier.Numeric = ID_Indexer_GetNextIndex(Indexer);
            StructureEncoding.IdentifierType = OpcUa_IdType_Numeric;
            StructureEncoding.NamespaceIndex = ID_NAMESPACE_INDEX;
        }

        ID_ObjectNode * EncodingTypeNode = ID_AddressSpace_CreateObjectFromObjectType(AddressSpace, Indexer, DataTypeEncodingType, &StructureEncoding);
        ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(EncodingTypeNode, 0, "Default Binary");
        ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(EncodingTypeNode, "Default Binary");

        OpcUa_NodeId HasEncoding = ID_OPCNodeId(OpcUaId_HasEncoding);
        OpcUa_NodeId TypeDefinition = ID_OPCNodeId(OpcUaId_DataTypeEncodingType);
        ID_AddressSpace_AddBiReference(AddressSpace, &LocalNode->BaseAttribute.NodeId, ID_NodeType_DataType, &EncodingTypeNode->BaseAttribute.NodeId, ID_NodeType_Object, &HasEncoding);
        ID_AddressSpace_AddUniReference(AddressSpace, &EncodingTypeNode->BaseAttribute.NodeId, ID_NodeType_Object, &TypeDefinition, &ID_HasTypeDefinitionNode, OpcUa_False);
    }

    if (Node != NULL) {
        *Node = LocalNode;
    }
}