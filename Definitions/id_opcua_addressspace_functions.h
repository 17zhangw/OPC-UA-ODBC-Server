/*
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ID_OPCUA_ADDRESSSPACE_FUNCTIONS_H
#define ID_OPCUA_ADDRESSSPACE_FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DEBUG
#define ID_ASSERT(xExpression)          assert(xExpression);
#else
#define ID_ASSERT(xExpression) \
    if (!(xExpression)) \
    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Assertion failed! %s\n", #xExpression); \

#endif

#define ID_INITIALIZE_ARRAY_WITH_TYPE_OF_LENGTH(xVar, xLength, xType) \
    for (int i = 0; i < xLength; i++) { \
        OpcUa_##xType##_Initialize(&xVar->Value.Array.Value.xType##Array[i]); \
    } \

#define ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(xBaseAttributes, xNamespace, xBrowse) \
    ((ID_BaseAttributes*)xBaseAttributes)->BrowseName.NamespaceIndex = xNamespace; \
    OpcUa_String_AttachCopy(&((ID_BaseAttributes*)xBaseAttributes)->BrowseName.Name, xBrowse); \

#define ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(xBaseAttributes, xDisplay) \
    OpcUa_String_AttachCopy(&((ID_BaseAttributes*)xBaseAttributes)->DisplayName.Locale, ID_APPLICATION_LOCALE); \
    OpcUa_String_AttachCopy(&((ID_BaseAttributes*)xBaseAttributes)->DisplayName.Text, xDisplay); \

#define ID_COPY_DISPLAY_NAME_FROM_NODE_TO_NODE(xSourceAttributes, xDestinationAttributes) \
    char * xSourceLocale = OpcUa_String_GetRawString(&((ID_BaseAttributes*)xSourceAttributes)->DisplayName.Locale); \
    if (xSourceLocale != NULL) \
        OpcUa_String_AttachCopy(&((ID_BaseAttributes*)xDestinationAttributes)->DisplayName.Locale, xSourceLocale); \
    char * xSourceStr = OpcUa_String_GetRawString(&((ID_BaseAttributes*)xSourceAttributes)->DisplayName.Text); \
    if (xSourceStr != NULL) \
        OpcUa_String_AttachCopy(&((ID_BaseAttributes*)xDestinationAttributes)->DisplayName.Text, xSourceStr); \

#define ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(xTarget, xSource) \
    if (OpcUa_String_GetRawString(xSource) != NULL) { \
        OpcUa_String_AttachCopy(xTarget, OpcUa_String_GetRawString(xSource)); \
    } \

#define ID_ALLOC_METHOD_LOCALIZED(xCount) \
    InputDescs = malloc(sizeof(OpcUa_LocalizedText) * xCount); \
    OutputDescs = malloc(sizeof(OpcUa_LocalizedText) * xCount); \
    for (int i = 0; i < xCount; i++) { \
        OpcUa_LocalizedText_Initialize(&InputDescs[i]); \
        OpcUa_LocalizedText_Initialize(&OutputDescs[i]); \
        OpcUa_String_AttachCopy(&InputDescs[i].Locale, ID_APPLICATION_LOCALE); \
        OpcUa_String_AttachCopy(&OutputDescs[i].Locale, ID_APPLICATION_LOCALE); \
    } \

#define ID_SETUP_METHOD_LOCALIZED(xCount) \
    OpcUa_LocalizedText * InputDescs = NULL; \
    OpcUa_LocalizedText * OutputDescs = NULL; \
    ID_ALLOC_METHOD_LOCALIZED(xCount); \

#define ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(xLocal, xIndex, xStr) \
    OpcUa_String_AttachCopy(&xLocal[xIndex].Text, xStr); \

#define ID_FREE_METHOD_LOCALIZED(xCount) \
    for (int i = 0; i < xCount; i++) { \
        OpcUa_LocalizedText_Clear(&InputDescs[i]); \
        OpcUa_LocalizedText_Clear(&OutputDescs[i]); \
    } \
    free(InputDescs); \
    free(OutputDescs); \

#define ID_REALLOC_METHOD_LOCALIZED(xOrig, xCount) \
    ID_FREE_METHOD_LOCALIZED(xOrig); \
    ID_ALLOC_METHOD_LOCALIZED(xCount); \

#define ID_CONFIGURE_VARIABLE_AS_READ_ONLY(xVar) \
    xVar->AccessLevel = 1; \
    xVar->UserAccessLevel = 1; \

#define ID_MAP_NODE_TO_NEW_ID(xObj, xId) \
    OpcUa_NodeId New##xObj##Id = ID_OPCNodeId(xId); \
    ID_AddressSpace_MapNodeToId(Handle, &xObj->BaseAttribute.NodeId, &New##xObj##Id); \

typedef struct _ID_PropertyInfo_ {
    OpcUa_Int NameNamespace;
    char * Name;
    OpcUa_Int32 Id;
    OpcUa_Int32 NamespaceIndex;
    OpcUa_Int32 Length;
    OpcUa_Int32 Type;
    OpcUa_Boolean IsProperty;
    OpcUa_NodeId * ModellingRule;
    OpcUa_NodeId * ComponentType;

    OpcUa_BuiltInType VariantType;
    OpcUa_ExtensionObjectEncoding VariantEncoding;
    OpcUa_ExpandedNodeId * VariantTypeId;
    OpcUa_EncodeableType * EncodeableType;
} ID_PropertyAddition;

typedef struct _ID_InputArgsInfo_ {
    OpcUa_Int32 ValueRank;
    OpcUa_Int32 OneDimensionLength;
    OpcUa_NodeId DataTypeIdentifier;
    char * Name;
    OpcUa_LocalizedText * Description;
} ID_InputArgsInfo;

typedef struct _ID_VariantStructure_ {
    OpcUa_BuiltInType BuiltInType;
    OpcUa_ExtensionObjectEncoding Encoding;
    OpcUa_ExpandedNodeId * ExpandedNodeId;
    OpcUa_EncodeableType * Type;
    OpcUa_NodeId * ModellingRule; // NULL defaults to mandatory
} ID_VariantStructure;

#ifdef __cplusplus
}
#endif

#endif