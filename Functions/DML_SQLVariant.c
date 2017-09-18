/*
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "UASQLFunctions.h"
#include "UASQLInternal.h"
#include "../AddressSpace/OPCDerivations.h"
#include <wchar.h>

extern ID_SessionManagerHandle GlobalSessionManagerHandle;

/* UASQLDelete A + W Functions */
#define ID_LEN(IsUnicode, xUni, xMulti) (IsUnicode == OpcUa_True ? wcslen(xUni) : strlen(xMulti))

void ID_PRINT(OpcUa_Boolean IsUnicode, wchar_t* xUni, char* xMulti, int xSize, void * xFmt, ...) {
    if (IsUnicode == OpcUa_True) {
        va_list argPtr;
        va_start(argPtr, (wchar_t*)xFmt);
        vswprintf(xUni, xSize, (wchar_t*)xFmt, argPtr);
        va_end(argPtr);
    } else {
        va_list argPtr;
        va_start(argPtr, (char*)xFmt);
        ID_UASQLPrintToBufferWithVarArgA(xMulti, xSize, (char*)xFmt, argPtr);
        va_end(argPtr);
    }
}

#define ID_NULL_TERMINATE(IsUnicode, xUni, xMulti, xIndex) \
    if (IsUnicode == OpcUa_True) { \
        xUni[xIndex] = L'\0'; \
    } else { \
        xMulti[xIndex] = '\0'; \
    } \

OpcUa_StatusCode ID_UASQLDelete(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    ID_CHECK_INPUT_TYPE_ARG(InputArgs, 1, OpcUaType_String);
    OpcUa_ReferenceParameter(NoInputArgs);

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    int bufferSize = 0;
    SQLLEN changeCount = 0;

    char * jsonRequest = NULL;
    cJSON * jsonObject = NULL;

    char * preparedBuffer = NULL;
    char * tableName = NULL;
    char * query = NULL;

    wchar_t * widePreparedBuffer = NULL;
    wchar_t * wideTableName = NULL;
    wchar_t * wideQuery = NULL;

    ID_Session * UserSession = NULL;
    ID_PreparedStatement * PStmt = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLDelete");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLDelete (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);

    HSTMT StatementHandle = PStmt->Statement;
    jsonRequest = OpcUa_String_GetRawString(&InputArgs[1].Value.String);
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Input Request - \n %s \n", jsonRequest);

    jsonObject = cJSON_Parse(jsonRequest);
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "table_name", cJSON_String);

    tableName = cJSON_GetObjectItem(jsonObject, "table_name")->valuestring;
    if (IsUnicode) {
        wideTableName = ID_ConvertNullTerminatedMultiByteToWide(tableName, &errorStr);
        if (wideTableName == NULL) {
            result = OpcUa_False;
            shouldAttachOwner = OpcUa_True;
            goto Error;
        }
    }

    query = NULL;
    if (cJSON_GetObjectItem(jsonObject, "query") != NULL) {
        query = cJSON_GetObjectItem(jsonObject, "query")->valuestring;
        if (IsUnicode) {
            wideQuery = ID_ConvertNullTerminatedMultiByteToWide(query, &errorStr);
            if (wideQuery == NULL) {
                result = OpcUa_False;
                shouldAttachOwner = OpcUa_True;
                goto Error;
            }
        }
    }

    bufferSize = (int)(ID_LEN(IsUnicode, L"delete from ", "delete from ") + ID_LEN(IsUnicode, wideTableName, tableName) +
                       ID_LEN(IsUnicode, L" where ", " where ") + ID_LEN(IsUnicode, wideQuery, query) + 1);
    size_t elementSize = IsUnicode == OpcUa_True ? sizeof(wchar_t) : sizeof(char);
    void ** bufferPtr = IsUnicode == OpcUa_True ? (void**)&widePreparedBuffer : (void**)&preparedBuffer;
    *bufferPtr = malloc(elementSize * bufferSize);
    memset(*bufferPtr, 0x00, elementSize * bufferSize);

    void * fmtPtr = NULL;
    if (IsUnicode == OpcUa_True) { fmtPtr = query == NULL ? L"delete from %s" : L"delete from %s where %s"; }
    else { fmtPtr = query == NULL ? "delete from %s" : "delete from %s where %s"; }
    
    void * tblPtr = IsUnicode == OpcUa_True ? (void*)wideTableName : (void*)tableName;
    void * queryPtr = IsUnicode == OpcUa_True ? (void*)wideQuery : (void*)query;
    ID_PRINT(IsUnicode, widePreparedBuffer, preparedBuffer, bufferSize, fmtPtr, tblPtr, queryPtr);
    ID_NULL_TERMINATE(IsUnicode, widePreparedBuffer, preparedBuffer, (bufferSize - 1));

    SQLRETURN retVal = SQLAllocHandle(SQL_HANDLE_STMT, UserSession->SessionDBConnHandle, &StatementHandle);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHDBC", SQL_HANDLE_DBC, UserSession->SessionDBConnHandle);

    retVal = SQL_SUCCESS;
    if (IsUnicode == OpcUa_True) {
        retVal = SQLExecDirectW(StatementHandle, (SQLWCHAR*)widePreparedBuffer, SQL_NTSL);
    } else {
        retVal = SQLExecDirect(StatementHandle, (SQLCHAR*)preparedBuffer, SQL_NTS);
    }

    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

    retVal = SQLRowCount(StatementHandle, &changeCount);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

Error:;
    cJSON_Delete(jsonObject);
    if (preparedBuffer != NULL) { free(preparedBuffer); }
    if (widePreparedBuffer != NULL) { free(widePreparedBuffer); }
    if (wideTableName != NULL) { free(wideTableName); }
    if (wideQuery != NULL) { free(wideQuery); }

    *NoOutputArgs = 3;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 3);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 3);
    ID_INITIALIZE_OUTPUT();
    OutputArguments[2].Datatype = OpcUaType_UInt32;
    OutputArguments[2].ArrayType = OpcUa_VariantArrayType_Scalar;
    OutputArguments[2].Value.UInt32 = (OpcUa_UInt32)changeCount;
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLDeleteA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLDelete(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLDeleteW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLDelete(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLInsert/SQLUpdate A + W Functions */

void ID_SQLInsert(OpcUa_Boolean IsUnicode, ID_Session * UserSession, cJSON * rowArray, void * tableName, void ** columns, SQLHANDLE StatementHandle, OpcUa_Boolean * pResult, OpcUa_Boolean * pShouldAttach, char ** pError) {
    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    SQLRETURN retVal = 0;

    int numRowElements = cJSON_GetArraySize(rowArray);
    void ** dataBuffers = malloc(sizeof(void*) * numRowElements);
    SQLLEN * dataLengthBuffers = malloc(sizeof(SQLLEN) * numRowElements);
    SQLLEN ** indicatorPointers = malloc(sizeof(SQLLEN*) * numRowElements);
    memset(dataLengthBuffers, 0x00, sizeof(SQLLEN) * numRowElements);
    memset(indicatorPointers, 0x00, sizeof(SQLLEN*) * numRowElements);
    memset(dataBuffers, 0x00, sizeof(void*) * numRowElements);

    for (SQLUSMALLINT i = 1; i <= numRowElements; i++) {
        SQLSMALLINT DataTypeInfo = 0;
        SQLULEN ColumnSizeInfo = 0;
        SQLSMALLINT DecimalDigitsInfo = 0;
        OpcUa_Boolean retStatus = OpcUa_True;

        if (IsUnicode == OpcUa_True) {
            retStatus = ID_UASQLDescribeParameterW(UserSession, (wchar_t*)tableName, (wchar_t*)columns[i - 1], &DataTypeInfo, &ColumnSizeInfo, &DecimalDigitsInfo);
        } else {
            retStatus = ID_UASQLDescribeParameterA(UserSession, (char*)tableName, (char*)columns[i - 1], &DataTypeInfo, &ColumnSizeInfo, &DecimalDigitsInfo);
        }

        if (retStatus != OpcUa_True) {
            result = OpcUa_False;
            errorStr = "could not retrieve information concerning one or more columns";
            goto Error;
        }

        cJSON * element = cJSON_GetArrayItem(rowArray, i - 1);
        if (DataTypeInfo == SQL_BINARY || DataTypeInfo == SQL_VARBINARY || DataTypeInfo == SQL_LONGVARBINARY) {
            if (strlen(element->valuestring) < 2) {
                result = OpcUa_False;
                errorStr = "supplied parameter value is too short/invalid base64";
                goto Error;
            }
        }

        SQLSMALLINT inputCType = SQL_C_CHAR;
        void * dataPointer = NULL;
        ID_CreateBufferParameterAttachment(IsUnicode, element, i - 1, DataTypeInfo, dataLengthBuffers, indicatorPointers, dataBuffers, &inputCType, &dataPointer);

        retVal = SQLBindParameter(StatementHandle, i, SQL_PARAM_INPUT, inputCType, DataTypeInfo, ColumnSizeInfo, DecimalDigitsInfo, dataPointer, dataLengthBuffers[i - 1], indicatorPointers[i - 1]);
        ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);
    }

    retVal = SQLExecute(StatementHandle);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

Error:;
    free(dataLengthBuffers);
    ID_FREE_BUFFERS(indicatorPointers, numRowElements);
    free(indicatorPointers);
    ID_FREE_BUFFERS(dataBuffers, numRowElements);
    *pResult = result;
    *pShouldAttach = shouldAttachOwner;
    *pError = errorStr;
}

// current UASQLUpdate implementation on Linux
// does not allow for the query component to be SQL bound
// that requires manual calls to Prepare(), Bind(), and Execute()
OpcUa_StatusCode ID_UASQLUpdate(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    ID_CHECK_INPUT_TYPE_ARG(InputArgs, 1, OpcUaType_String);
    OpcUa_ReferenceParameter(NoInputArgs);

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    int bufIndex = 0;
    int bufLength = 0;
    SQLLEN changeCount = 0;

    void ** attrNames = NULL;
    int numAttrs = 0;

    char * jsonRequest = NULL;
    cJSON * jsonObject = NULL;

    char * preparedBuffer = NULL;
    char * tableName = NULL;
    char * query = NULL;
    wchar_t * wideTableName = NULL;
    wchar_t * wideQuery = NULL;
    wchar_t * widePreparedBuffer = NULL;

    ID_Session * UserSession = NULL;
    ID_PreparedStatement * PStmt = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLUpdate");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLUpdate function\n");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);

    jsonRequest = OpcUa_String_GetRawString(&InputArgs[1].Value.String);
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Input Request -\n %s\n", jsonRequest);

    jsonObject = cJSON_Parse(jsonRequest);
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "table_name", cJSON_String);
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "attrs", cJSON_Array);
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "values", cJSON_Array);

    cJSON * attrsArray = cJSON_GetObjectItem(jsonObject, "attrs");
    cJSON * rowArray = cJSON_GetObjectItem(jsonObject, "values");
    ID_CHECK_ARRAY_SIZE_AT_LEAST(attrsArray, 1);
    ID_CHECK_ARRAY_SIZE_AT_LEAST(rowArray, 1);
    ID_ASSERT_ARRAY_SIZES_EQUAL(attrsArray, rowArray);

    // check query existance and add buffer length
    if (cJSON_GetObjectItem(jsonObject, "query") != NULL) {
        query = cJSON_GetObjectItem(jsonObject, "query")->valuestring;
        if (IsUnicode == OpcUa_True) {
            wideQuery = ID_ConvertNullTerminatedMultiByteToWide(query, &errorStr);
            if (wideQuery == NULL) {
                result = OpcUa_False;
                shouldAttachOwner = OpcUa_True;
                goto Error;
            }
        }

        bufLength += (int)(ID_LEN(IsUnicode, L"where ", "where ") + ID_LEN(IsUnicode, wideQuery, query));
    }

    // check table name and add buffer length
    tableName = cJSON_GetObjectItem(jsonObject, "table_name")->valuestring;
    if (IsUnicode == OpcUa_True) {
        wideTableName = ID_ConvertNullTerminatedMultiByteToWide(tableName, &errorStr);
        if (wideTableName == NULL) {
            result = OpcUa_False;
            shouldAttachOwner = OpcUa_True;
            goto Error;
        }
    }
    bufLength += (int)(ID_LEN(IsUnicode, L"update ", "update ") + ID_LEN(IsUnicode, wideTableName, tableName) + ID_LEN(IsUnicode, L" set ", " set "));

    // if first element of attrs array is of dictionary type
    // then know that attrs array requires validation
    OpcUa_Boolean IsDictStyle = OpcUa_False;
    if (cJSON_GetArrayItem(attrsArray, 0)->type == cJSON_Object) {
        IsDictStyle = OpcUa_True;
        OpcUa_Boolean retCode = OpcUa_True;
        if (IsUnicode == OpcUa_True) { retCode = ID_UASQLValidateAttributesW(UserSession, attrsArray, wideTableName); }
        else { retCode = ID_UASQLValidateAttributesA(UserSession, attrsArray, tableName); }

        if (retCode == OpcUa_False) {
            result = OpcUa_False;
            errorStr = "attribute specification do not match server definitions";
            goto Error;
        }
    }

    // get all attributes and count the length
    numAttrs = cJSON_GetArraySize(attrsArray);
    attrNames = malloc((IsUnicode == OpcUa_True ? sizeof(wchar_t*) : sizeof(char*)) * numAttrs);
    memset(attrNames, 0x00, (IsUnicode == OpcUa_True ? sizeof(wchar_t*) : sizeof(char*)) * numAttrs);
    for (int i = 0; i < numAttrs; i++) {
        cJSON * attributeDict = cJSON_GetArrayItem(attrsArray, i);
        char * nAttrName = attributeDict->valuestring;
        wchar_t * wAttrName = NULL;
        if (IsDictStyle == OpcUa_True) {
            attributeDict = cJSON_GetArrayItem(attributeDict, 0);
            nAttrName = attributeDict->string;
        }

        if (IsUnicode == OpcUa_True) {
            wAttrName = ID_ConvertNullTerminatedMultiByteToWide(nAttrName, &errorStr);
            if (wAttrName == NULL) {
                result = OpcUa_False;
                shouldAttachOwner = OpcUa_True;
                goto Error;
            }
        }

        bufLength += (int)(ID_LEN(IsUnicode, wAttrName, nAttrName) + (i == numAttrs - 1 ? 3 : 4));
        attrNames[i] = IsUnicode == OpcUa_True ? (void*)wAttrName : (void*)nAttrName;
    }

    bufLength++; // null-terminator position
    size_t elementSize = IsUnicode == OpcUa_True ? sizeof(wchar_t) : sizeof(char);
    void ** bufPtr = IsUnicode == OpcUa_True ? (void**)&widePreparedBuffer : (void**)&preparedBuffer;
    *bufPtr = malloc(elementSize * bufLength);
    memset(*bufPtr, 0x00, elementSize * bufLength);

    void * fmtPtr = IsUnicode == OpcUa_True ? (void*)L"update %s set " : (void*)"update %s set ";
    void * tblPtr = IsUnicode == OpcUa_True ? (void*)wideTableName : (void*)tableName;
    ID_PRINT(IsUnicode, widePreparedBuffer, preparedBuffer, bufLength - bufIndex, fmtPtr, tblPtr);
    bufIndex += (int)(ID_LEN(IsUnicode, L"update  set ", "update  set ") + ID_LEN(IsUnicode, wideTableName, tableName));

    for (int i = 0; i < numAttrs; i++) {
        void * attrFmtPtr = IsUnicode == OpcUa_True ? (void*)L"%s=?%s " : (void*)"%s=?%s ";
        void * terminator = NULL;
        if (IsUnicode == OpcUa_True) { 
            if (i == numAttrs - 1) { terminator = L""; } 
            else { terminator = L","; }
        }
        else {
            if (i == numAttrs - 1) { terminator = ""; }
            else { terminator = ","; }
        }

        ID_PRINT(IsUnicode, widePreparedBuffer + bufIndex, preparedBuffer + bufIndex, bufLength - bufIndex, attrFmtPtr, attrNames[i], terminator);
        bufIndex += (int)ID_LEN(IsUnicode, attrNames[i], attrNames[i]) + (i == numAttrs - 1 ? 3 : 4);
    }

    if (query != NULL) {
        void * qFmtPtr = IsUnicode == OpcUa_True ? (void*)L"where %s" : (void*)"where %s";
        void * queryPtr = IsUnicode == OpcUa_True ? (void*)wideQuery : (void*)query;
        ID_PRINT(IsUnicode, widePreparedBuffer + bufIndex, preparedBuffer + bufIndex, bufLength - bufIndex, qFmtPtr, queryPtr);
        bufIndex += (int)(ID_LEN(IsUnicode, wideQuery, query) + ID_LEN(IsUnicode, L"where ", "where "));
    }
    ID_NULL_TERMINATE(IsUnicode, widePreparedBuffer, preparedBuffer, bufIndex);

    HSTMT StatementHandle = PStmt->Statement;
    SQLRETURN retVal = SQL_SUCCESS;
    
    if (IsUnicode == OpcUa_True) {
        SQLPrepareW(StatementHandle, (SQLWCHAR*)widePreparedBuffer, SQL_NTSL);
        ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);
        ID_SQLInsert(IsUnicode, UserSession, rowArray, wideTableName, attrNames, StatementHandle, &result, &shouldAttachOwner, &errorStr);
        if (result == OpcUa_False) {
            goto Error;
        }
    } else {
        SQLPrepare(StatementHandle, (SQLCHAR*)preparedBuffer, SQL_NTS);
        ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);
        ID_SQLInsert(IsUnicode, UserSession, rowArray, tableName, attrNames, StatementHandle, &result, &shouldAttachOwner, &errorStr);
        if (result == OpcUa_False) {
            goto Error;
        }
    }
    
    retVal = SQLRowCount(StatementHandle, &changeCount);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

    retVal = SQLFreeStmt(StatementHandle, SQL_CLOSE);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

    retVal = SQLFreeStmt(StatementHandle, SQL_RESET_PARAMS);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

Error:;
    cJSON_Delete(jsonObject);
    if (preparedBuffer != NULL) { free(preparedBuffer); }
    if (widePreparedBuffer != NULL) { free(widePreparedBuffer); }
    if (attrNames != NULL) { 
        if (IsUnicode == OpcUa_True) {
            for (int i = 0; i < numAttrs; i++) { 
                if (attrNames[i] != NULL) { 
                    free(attrNames[i]); 
                } 
            }
        }
        free(attrNames); 
    }

    if (wideTableName != NULL) { free(wideTableName); }
    if (wideQuery != NULL) { free(wideQuery); }

    *NoOutputArgs = 3;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 3);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 3);
    ID_INITIALIZE_OUTPUT();
    OutputArguments[2].Datatype = OpcUaType_UInt32;
    OutputArguments[2].ArrayType = OpcUa_VariantArrayType_Scalar;
    OutputArguments[2].Value.UInt32 = (OpcUa_UInt32)changeCount;
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLUpdateW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLUpdate(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLUpdateA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLUpdate(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLInsert(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    ID_CHECK_INPUT_TYPE_ARG(InputArgs, 1, OpcUaType_String);
    OpcUa_ReferenceParameter(NoInputArgs);

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    
    void ** attrNames = NULL;
    int numAttrs = 0;
    int bufIndex = 0;
    int bufLength = 0;

    char * jsonRequest = NULL;
    cJSON * jsonObject = NULL;

    char * preparedBuffer = NULL;
    char * tableName = NULL;
    wchar_t * wideTableName = NULL;
    wchar_t * widePreparedBuffer = NULL;

    ID_Session * UserSession = NULL;
    ID_PreparedStatement * PStmt = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLInsert");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLInsert function\n");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);

    jsonRequest = OpcUa_String_GetRawString(&InputArgs[1].Value.String);
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Input Request - \n %s\n", jsonRequest);

    jsonObject = cJSON_Parse(jsonRequest);
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "table_name", cJSON_String);
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "attrs", cJSON_Array);
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "rows", cJSON_Array);

    cJSON * attrsArray = cJSON_GetObjectItem(jsonObject, "attrs");
    cJSON * rowArrays = cJSON_GetObjectItem(jsonObject, "rows");
    ID_CHECK_ARRAY_SIZE_AT_LEAST(attrsArray, 1);
    ID_CHECK_ARRAY_SIZE_AT_LEAST(rowArrays, 1);

    // assert each row is equal size to attrs; each row is a array
    int numRows = cJSON_GetArraySize(rowArrays);
    for (int i = 0; i < numRows; i++) {
        cJSON * rowArray = cJSON_GetArrayItem(rowArrays, i);
        if (rowArray->type != cJSON_Array) {
            result = OpcUa_False;
            errorStr = "ERR:1002:malformed input rows not multidimensional array";
            goto Error;
        }

        ID_ASSERT_ARRAY_SIZES_EQUAL(attrsArray, rowArray);
    }

    // setup insert into statement
    tableName = cJSON_GetObjectItem(jsonObject, "table_name")->valuestring;
    if (IsUnicode == OpcUa_True) {
        wideTableName = ID_ConvertNullTerminatedMultiByteToWide(tableName, &errorStr);
        if (tableName == NULL) {
            result = OpcUa_False;
            shouldAttachOwner = OpcUa_True;
            goto Error;
        }
    }
    bufLength += (int)(ID_LEN(IsUnicode, L"insert into  (", "insert into  (") + ID_LEN(IsUnicode, wideTableName, tableName));

    // validate attributes
    OpcUa_Boolean IsDictStyle = OpcUa_False;
    if (cJSON_GetArrayItem(attrsArray, 0)->type == cJSON_Object) {
        IsDictStyle = OpcUa_True;
        OpcUa_Boolean retCode = OpcUa_True;
        if (IsUnicode == OpcUa_True) { retCode = ID_UASQLValidateAttributesW(UserSession, attrsArray, wideTableName); }
        else { retCode = ID_UASQLValidateAttributesA(UserSession, attrsArray, tableName); }
        
        if (retCode == OpcUa_False) {
            errorStr = "attribute specification does not match server definitions";
            result = OpcUa_False;
            goto Error;
        }
    }

    // count attribute sizes
    numAttrs = cJSON_GetArraySize(attrsArray);
    attrNames = malloc((IsUnicode == OpcUa_True ? sizeof(wchar_t*) : sizeof(char*)) * numAttrs);
    memset(attrNames, 0x00, (IsUnicode == OpcUa_True ? sizeof(wchar_t*) : sizeof(char*)) * numAttrs);
    for (int i = 0; i < numAttrs; i++) {
        cJSON * attributeDict = cJSON_GetArrayItem(attrsArray, i);
        char * attrName = attributeDict->valuestring;
        wchar_t * wideAttrName = NULL;
        if (IsDictStyle) { // correct attrName pointer if dict configuration
            attributeDict = cJSON_GetArrayItem(attributeDict, 0);
            attrName = attributeDict->string;
        }

        if (IsUnicode == OpcUa_True) {
            wideAttrName = ID_ConvertNullTerminatedMultiByteToWide(attrName, &errorStr);
            if (wideAttrName == NULL) {
                result = OpcUa_False;
                shouldAttachOwner = OpcUa_True;
                goto Error;
            }
        }

        bufLength += (int)(ID_LEN(IsUnicode, wideAttrName, attrName) + (i == numAttrs - 1 ? 0 : 1));
        attrNames[i] = IsUnicode == OpcUa_True ? (void*)wideAttrName : (void*)attrName;
    }

    // parameter marker either ?, or ?)
    bufLength += (int)(ID_LEN(IsUnicode, L") values (", ") values (") + numAttrs * 2);
    bufLength++; // null terminator

    size_t elementSize = IsUnicode == OpcUa_True ? sizeof(wchar_t) : sizeof(char);
    void ** bufPtr = IsUnicode == OpcUa_True ? (void**)&widePreparedBuffer : (void**)&preparedBuffer;
    *bufPtr = malloc(elementSize * bufLength);
    memset(*bufPtr, 0x00, elementSize * bufLength);

    void * fmtPtr = IsUnicode == OpcUa_True ? (void*)L"insert into %s (" : (void*)"insert into %s (";
    void * tblPtr = IsUnicode == OpcUa_True ? (void*)wideTableName : (void*)tableName;
    ID_PRINT(IsUnicode, widePreparedBuffer, preparedBuffer, bufLength, fmtPtr, tblPtr);
    bufIndex += (int)(ID_LEN(IsUnicode, fmtPtr, fmtPtr) - 2 + ID_LEN(IsUnicode, wideTableName, tableName)); // subtract 2 to remove %s length

    for (int i = 0; i < numAttrs; i++) {
        void * attrFmtPtr = IsUnicode == OpcUa_True ? (void*)L"%s%s" : (void*)"%s%s";
        void * endPtr = NULL;
        if (IsUnicode == OpcUa_True) {
            if (i == numAttrs - 1) { endPtr = L""; }
            else { endPtr = L","; }
        } else {
            if (i == numAttrs - 1) { endPtr = ""; }
            else { endPtr = ","; }
        }

        ID_PRINT(IsUnicode, widePreparedBuffer + bufIndex, preparedBuffer + bufIndex, bufLength - bufIndex, attrFmtPtr, attrNames[i], endPtr);
        bufIndex += (int)(ID_LEN(IsUnicode, attrNames[i], attrNames[i]) + (i == numAttrs - 1 ? 0 : 1));
    }

    fmtPtr = IsUnicode == OpcUa_True ? (void*)L") values (" : (void*)") values (";
    ID_PRINT(IsUnicode, widePreparedBuffer + bufIndex, preparedBuffer + bufIndex, bufLength - bufIndex, fmtPtr);
    bufIndex += (int)(ID_LEN(IsUnicode, fmtPtr, fmtPtr));
    for (int i = 0; i < numAttrs; i++) {
        void * endPtr = NULL;
        if (IsUnicode == OpcUa_True) {
            if (i == numAttrs - 1) { endPtr = L"?)"; }
            else { endPtr = L"?,"; }
        } else {
            if (i == numAttrs - 1) { endPtr = "?)"; }
            else { endPtr = "?,"; }
        }

        ID_PRINT(IsUnicode, widePreparedBuffer + bufIndex, preparedBuffer + bufIndex, bufLength - bufIndex, endPtr);
        bufIndex += 2;
    }
    ID_NULL_TERMINATE(IsUnicode, widePreparedBuffer, preparedBuffer, bufIndex);

    HSTMT StatementHandle = PStmt->Statement;
    SQLRETURN retVal = SQL_SUCCESS;
    if (IsUnicode == OpcUa_True) { retVal = SQLPrepareW(StatementHandle, (SQLWCHAR*)widePreparedBuffer, SQL_NTSL); } 
    else { retVal = SQLPrepare(StatementHandle, (SQLCHAR*)preparedBuffer, SQL_NTS); }
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

    for (int rowIndex = 0; rowIndex < numRows; rowIndex++) {
        void * tblPointer = IsUnicode == OpcUa_True ? (void*)wideTableName : (void*)tableName;
        cJSON * rowArray = cJSON_GetArrayItem(rowArrays, rowIndex);
        ID_SQLInsert(IsUnicode, UserSession, rowArray, tblPointer, attrNames, StatementHandle, &result, &shouldAttachOwner, &errorStr);
        if (result == OpcUa_False) {
            retVal = SQLFreeStmt(StatementHandle, SQL_CLOSE);
            ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);
            goto Error;
        }

        retVal = SQLFreeStmt(StatementHandle, SQL_CLOSE);
        ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

        retVal = SQLFreeStmt(StatementHandle, SQL_RESET_PARAMS);
        ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);
    }

Error:;
    cJSON_Delete(jsonObject);
    if (preparedBuffer != NULL) { free(preparedBuffer); }
    if (attrNames != NULL) { 
        if (IsUnicode == OpcUa_True) {
            for (int i = 0; i < numAttrs; i++) {
                if (attrNames[i] != NULL) {
                    free(attrNames[i]);
                }
            }
        }
        free(attrNames); 
    }
    if (widePreparedBuffer != NULL) { free(widePreparedBuffer); }
    if (wideTableName != NULL) { free(wideTableName); }

    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLInsertA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLInsert(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLInsertW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLInsert(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}