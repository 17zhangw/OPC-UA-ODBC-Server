/*
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ID_OPCUA_DB_PRIVATE_H
#define ID_OPCUA_DB_PRIVATE_H

#define ID_CHECK_SESSION_ACTIVATED() \
if (UserSession == NULL || !UserSession->IsActivated) { \
result = OpcUa_False; \
errorStr = "ERR:0001:user session not valid"; \
goto Error; \
} \

#define ID_CHECK_DB_UNINITIALIZED() \
if (UserSession->SessionDBConnHandle != NULL || UserSession->SessionDBEnvHandle != NULL) { \
result = OpcUa_False; \
errorStr = "ERR:1000:existing connection still open"; \
goto Error; \
} \

#define ID_CHECK_DB_INITIALIZED() \
if (UserSession->SessionDBEnvHandle == NULL || UserSession->SessionDBConnHandle == NULL || !UserSession->IsConnected) { \
result = OpcUa_False; \
errorStr = "ERR:1001:database not setup"; \
goto Error; \
} \

#define ID_CHECK_RETVAL(xIsUnicode, xRet, xHandleStr, xHandle_Type, xHandle) \
    if (xIsUnicode == OpcUa_True) { \
        ID_CHECK_RETVAL_UNICODE(xRet, xHandleStr, xHandle_Type, xHandle); \
    } else { \
        ID_CHECK_RETVAL_ANSI_UTF8(xRet, xHandleStr, xHandle_Type, xHandle); \
    } \

#define ID_CHECK_RETVAL_ANSI_UTF8(xRet, xHandleStr, xHandle_Type, xHandle) \
if (xRet != SQL_SUCCESS && xRet != SQL_SUCCESS_WITH_INFO && xRet != SQL_NO_DATA) { \
    result = OpcUa_False; \
    errorStr = ID_ExtractErrorA(xHandleStr, xHandle, xHandle_Type, OpcUa_False); \
    shouldAttachOwner = OpcUa_True; \
    goto Error; \
} else if (xRet == SQL_SUCCESS_WITH_INFO) { \
    errorStr = ID_ExtractErrorA(xHandleStr, xHandle, xHandle_Type, OpcUa_False); \
    shouldAttachOwner = OpcUa_True; \
} else if (xRet == SQL_NO_DATA) { \
    errorStr = "no data altered"; \
} \

#define ID_CHECK_RETVAL_UNICODE(xRet, xHandleStr, xHandle_Type, xHandle) \
if (xRet != SQL_SUCCESS && xRet != SQL_SUCCESS_WITH_INFO && xRet != SQL_NO_DATA) { \
    result = OpcUa_False; \
    wchar_t * ctr = ID_ExtractErrorW(L##xHandleStr, xHandle, xHandle_Type, OpcUa_False); \
    char * eMessage = NULL; \
    errorStr = ID_ConvertNullTerminatedWideToMultiByte(ctr, &eMessage); \
    free(ctr); \
    if (eMessage != NULL) { \
	    errorStr = eMessage; \
    } \
    shouldAttachOwner = OpcUa_True; \
    goto Error; \
} else if (xRet == SQL_SUCCESS_WITH_INFO) { \
    wchar_t * ctr = ID_ExtractErrorW(L##xHandleStr, xHandle, xHandle_Type, OpcUa_False); \
    char * eMessage = NULL; \
    errorStr = ID_ConvertNullTerminatedWideToMultiByte(ctr, &eMessage); \
    free(ctr); \
    if (eMessage != NULL) { \
        errorStr = eMessage; \
    } \
    shouldAttachOwner = OpcUa_True; \
} else if (xRet == SQL_NO_DATA) { \
    errorStr = "no data altered"; \
} \

#define ID_INITIALIZE_OUTPUT() \
OutputArguments[0].Datatype = OpcUaType_Boolean; \
OutputArguments[0].ArrayType = OpcUa_VariantArrayType_Scalar; \
OutputArguments[0].Value.Boolean = result; \
OutputArguments[1].Datatype = OpcUaType_String; \
OutputArguments[1].ArrayType = OpcUa_VariantArrayType_Scalar; \
OpcUa_String_Initialize(&OutputArguments[1].Value.String); \
if (errorStr != NULL) { \
    if (shouldAttachOwner) { \
        OpcUa_String_AttachWithOwnership(&OutputArguments[1].Value.String, errorStr); \
    } else { \
        OpcUa_String_AttachCopy(&OutputArguments[1].Value.String, errorStr); \
    } \
} \

#define ID_CHECK_INPUT_UINT32_ARG(xArgs, xIndex) \
if (xArgs[xIndex].ArrayType != OpcUa_VariantArrayType_Scalar) { \
    InputCodes[0] = OpcUa_BadTypeMismatch; \
    return OpcUa_BadInvalidArgument; \
} \
switch (xArgs[xIndex].Datatype) { \
    case OpcUaType_UInt32: \
    case OpcUaType_UInt16: \
    case OpcUaType_Byte: \
       break; \
    default: \
        InputCodes[0] = OpcUa_BadTypeMismatch; \
        return OpcUa_BadInvalidArgument; \
} \

#define ID_CHECK_INPUT_TYPE_ARG(xArgs, xIndex, xType) \
if (xArgs[xIndex].ArrayType != OpcUa_VariantArrayType_Scalar || xArgs[xIndex].Datatype != xType) { \
    InputCodes[0] = OpcUa_BadTypeMismatch; \
    return OpcUa_BadInvalidArgument; \
} \
if (xType == OpcUaType_String && OpcUa_String_GetRawString(&xArgs[xIndex].Value.String) == NULL) { \
    InputCodes[0] = OpcUa_BadTypeMismatch; \
    return OpcUa_BadInvalidArgument; \
} \

#define ID_CHECK_OPERATING_SYSTEM(xOS) \
    if (ID_CURRENT_OS != xOS) { \
        return OpcUa_BadNotSupported; \
    } \

#define ID_FIND_PREPARED_STATEMENT_WITH_INDEX(xSession, xStmt, xLoc, xIdentifier) \
for (OpcUa_UInt32 i = 0; i < xSession->NumPreparedStatements; i++) { \
    ID_PreparedStatement * stmt = xSession->PreparedStatements[i]; \
    if (stmt->Identifier == xIdentifier) { \
        xStmt = stmt; \
        xLoc = i; \
    } \
} \

#define ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(xHandle) \
if (xHandle == NULL) { \
    result = OpcUa_False; \
    errorStr = "Requested statement identifier is not defined"; \
    goto Error; \
} \

#define ID_FIND_PREPARED_STATEMENT(xSession, xStmt, xIdentifier) \
OpcUa_UInt32 XXfindPreparedStatementXX_internal = 0; \
ID_FIND_PREPARED_STATEMENT_WITH_INDEX(xSession, xStmt, XXfindPreparedStatementXX_internal, xIdentifier); \

#define ID_CHECK_ARRAY_SIZE_AT_LEAST(cJSON, xLength) \
if (cJSON_GetArraySize(cJSON) < xLength) { \
    result = OpcUa_False; \
    errorStr = "ERR:1002:malformed input array is empty"; \
    goto Error; \
} \

#define ID_ASSERT_ARRAY_SIZES_EQUAL(obj1, obj2) \
if (cJSON_GetArraySize(obj1) != cJSON_GetArraySize(obj2)) { \
result = OpcUa_False; \
errorStr = "ERR:1002:malformed input attrs and values not equal length"; \
goto Error; \
} \

#define ID_CHECK_NON_EMPTY_ARRAY(cJSON) \
if (cJSON == NULL || cJSON_GetArrayItem(cJSON, 0) == NULL) { \
result = OpcUa_False; \
errorStr = "ERR:1002:malformed input"; \
goto Error; \
} \

#define ID_CHECK_JSON_KEY_EXISTS(cJSON, xKey, xType) \
if (cJSON == NULL || cJSON_GetObjectItem(cJSON, xKey) == NULL || (cJSON_GetObjectItem(cJSON, xKey))->type != xType) { \
    result = OpcUa_False; \
    errorStr = "ERR:1002:malformed input"; \
    goto Error; \
} \

#define ID_FREE_BUFFERS(xBuffers, xLength) \
for (int i = 0; i < xLength; i++) { \
    free(xBuffers[i]); \
} \
free(xBuffers); \
xBuffers = NULL; \

#endif