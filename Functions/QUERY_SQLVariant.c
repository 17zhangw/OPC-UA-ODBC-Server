/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "UASQLFunctions.h"
#include "UASQLInternal.h"

static const OpcUa_UInt32 ID_SINGULAR_FETCH_LIMIT = 100;
static const SQLLEN SEGMENT_BUFFER_SIZE = 512;
extern ID_SessionManagerHandle GlobalSessionManagerHandle;

SQLSMALLINT ID_GetSupportedFetchStyle(SQLHANDLE DatabaseConnHandle) {
    SQLUINTEGER fetchInfo = 0;
    SQLGetInfo(DatabaseConnHandle, SQL_DYNAMIC_CURSOR_ATTRIBUTES1, &fetchInfo, 0, NULL);
    if ((fetchInfo & SQL_CA1_RELATIVE) != 0) {
        return SQL_FETCH_PRIOR;
    }
    
    return SQL_FETCH_ABSOLUTE;
}

OpcUa_Boolean ID_Fetch(OpcUa_Boolean IsUnicode, SQLHANDLE StatementHandle, SQLSMALLINT fetchStyle,  int fetchSize, char ** outputText, int * moreExist, char ** errorMessage) {
    OpcUa_Boolean result = OpcUa_True;
    char * errorStr = NULL;
    cJSON * attributeObject = NULL;
    cJSON * rowObject = NULL;
    cJSON * root = NULL;
    int * BinaryIndicators = NULL;
    *outputText = NULL;
    *moreExist = 0;
    *errorMessage = NULL;
    OpcUa_Boolean HasReachedLimit = OpcUa_False;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    
    SQLSMALLINT numResultColumns = 0;
    SQLRETURN retVal = SQLNumResultCols(StatementHandle, &numResultColumns);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

    if (IsUnicode == OpcUa_True) {
        attributeObject = ID_UASQLGetQueryAttributesW(StatementHandle, numResultColumns, &BinaryIndicators, &errorStr);
    } else {
        attributeObject = ID_UASQLGetQueryAttributesA(StatementHandle, numResultColumns, &BinaryIndicators, &errorStr);
    }
    
    if (attributeObject == NULL) {
        if (BinaryIndicators != NULL) { free(BinaryIndicators); BinaryIndicators = NULL; }
        result = OpcUa_False;
        goto Error;
    }

    rowObject = cJSON_CreateArray();
    int numRows = 0;
    while (SQL_SUCCEEDED(retVal = SQLFetch(StatementHandle))) {
        if (numRows >= fetchSize) {
            if (fetchStyle == SQL_FETCH_PRIOR) {
                retVal = SQLFetchScroll(StatementHandle, SQL_FETCH_PRIOR, 0);
                ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);
            }
            
            HasReachedLimit = OpcUa_True;
            break;
        }
        
        cJSON * rowElement = cJSON_CreateArray();
        for (SQLUSMALLINT i = 1; i <= numResultColumns; i++) {
            SQLLEN BufferMarker = 0;
            SQLLEN ContiguousBufferIndex = 0;

            OpcUa_Boolean IsParts = OpcUa_False;
            SQLSMALLINT cType = SQL_C_CHAR;
            SQLSMALLINT cTypeSize = sizeof(SQLCHAR);
            if (BinaryIndicators[i - 1] == 1) {
                cType = SQL_C_BINARY;
            } else if (IsUnicode == OpcUa_True) {
                cType = SQL_C_WCHAR;
                cTypeSize = sizeof(SQLWCHAR);
            }

            SQLCHAR * MultiByteBuffer = NULL;
            SQLWCHAR * WideBuffer = NULL;
            void * SegmentBuffer = malloc(cTypeSize * SEGMENT_BUFFER_SIZE);
            memset(SegmentBuffer, 0x01, cTypeSize * SEGMENT_BUFFER_SIZE);

            // these exist for debugging purposes
#pragma warning(push)
#pragma warning(disable: 4189)
            SQLWCHAR * SegmentBufferWatch = SegmentBuffer;
            SQLCHAR * SegmentBufferWatchNarrow = SegmentBuffer;
#pragma warning(pop)            

            // 2nd-last argument notifies of the byte length of the passed-in input buffer
            // last argument BufferMarker notifies of two things:
            // - how much data remaining
            // - how much useful bytes currently in the buffer
            while ((retVal = SQLGetData(StatementHandle, i, cType, SegmentBuffer, cTypeSize * SEGMENT_BUFFER_SIZE, &BufferMarker)) != SQL_NO_DATA) {
                if (retVal != SQL_SUCCESS && retVal != SQL_SUCCESS_WITH_INFO) { // make sure that SQLGetData succeeded
                    cJSON_Delete(rowElement); rowElement = NULL;
                    if (MultiByteBuffer != NULL) { free(MultiByteBuffer); MultiByteBuffer = NULL; }
                    if (WideBuffer != NULL) { free(WideBuffer); WideBuffer = NULL; }
                    if (SegmentBuffer != NULL) { free(SegmentBuffer); SegmentBuffer = NULL; }

                    if (IsUnicode == OpcUa_True) {
                        wchar_t * eStr = ID_ExtractErrorW(L"SQLHSTMT", StatementHandle, SQL_HANDLE_STMT, OpcUa_False);
                        char * tempErrMessage = NULL;
                        errorStr = ID_ConvertNullTerminatedWideToMultiByte(eStr, &tempErrMessage);
                        if (errorStr == NULL) {
                            errorStr = tempErrMessage;
                        }
                    } else {
                        errorStr = ID_ExtractErrorA("SQLHSTMT", StatementHandle, SQL_HANDLE_STMT, OpcUa_False);
                    }

                    result = OpcUa_False;
                    goto Error;
                }

                if (BufferMarker == SQL_NULL_DATA) { // column value is null
                    cJSON_AddItemToArray(rowElement, cJSON_CreateNull());
                    break;
                }

                if (retVal == SQL_SUCCESS_WITH_INFO || IsParts) { // truncation
                    IsParts = OpcUa_True;
                    int IsBinary = BinaryIndicators[i - 1];

                    SQLLEN NumBytes = (BufferMarker >= (cTypeSize * SEGMENT_BUFFER_SIZE)) || (BufferMarker == SQL_NO_TOTAL) ? (cTypeSize * (SEGMENT_BUFFER_SIZE - (1-IsBinary))) : BufferMarker;
                    if (IsBinary == 1 || IsUnicode == OpcUa_False) {
                        if (MultiByteBuffer == NULL) MultiByteBuffer = malloc(NumBytes);
                        else MultiByteBuffer = realloc(MultiByteBuffer, ContiguousBufferIndex + NumBytes);
                        memcpy(MultiByteBuffer + ContiguousBufferIndex, SegmentBuffer, NumBytes);
                    } else if (IsUnicode == OpcUa_True) {
                        if (WideBuffer == NULL) WideBuffer = malloc(NumBytes);
                        else WideBuffer = realloc(WideBuffer, cTypeSize * ContiguousBufferIndex + NumBytes);
                        memcpy(WideBuffer + ContiguousBufferIndex, SegmentBuffer, NumBytes);
                    }

                    ContiguousBufferIndex += (int)NumBytes / cTypeSize;
                } else if (retVal == SQL_SUCCESS && BinaryIndicators[i - 1] == 0) { // single shot fully retrieved non-binary type
                    char * narrowStr = NULL;
                    if (IsUnicode == OpcUa_True) {
                        char * tempEMessage = NULL;
                        narrowStr = ID_ConvertNullTerminatedWideToMultiByte(SegmentBuffer, &tempEMessage);
                        if (narrowStr == NULL) {
                            if (rowElement != NULL) { cJSON_Delete(rowElement); rowElement = NULL; }
                            if (MultiByteBuffer != NULL) { free(MultiByteBuffer); MultiByteBuffer = NULL; }
                            if (WideBuffer != NULL) { free(WideBuffer); WideBuffer = NULL; }
                            if (SegmentBuffer != NULL) { free(SegmentBuffer); SegmentBuffer = NULL; }
                            result = OpcUa_False;
                            errorStr = tempEMessage;
                            goto Error;
                        }
                    } else {
                        narrowStr = (char*)SegmentBuffer;
                    }
                   
                    cJSON_AddItemToArray(rowElement, cJSON_CreateString(narrowStr));
                    if (IsUnicode == OpcUa_True) { free(narrowStr); }
                    break;
                } else if (retVal == SQL_SUCCESS && BinaryIndicators[i - 1] == 1) { // one shot retrieve binary data
                    IsParts = OpcUa_True; // set to true to initiate binary cleanup handling
                    MultiByteBuffer = malloc(sizeof(SQLCHAR) * BufferMarker);
                    memcpy(MultiByteBuffer, SegmentBuffer, sizeof(SQLCHAR) * BufferMarker);
                    ContiguousBufferIndex = (int)BufferMarker;
                    break;
                }
            }

            free(SegmentBuffer);
            SegmentBuffer = NULL;
            if (IsParts) { // parts cleanup
                if (BinaryIndicators[i - 1] == 0) { // character type data, need to null-terminate
                    char * narrowStr = NULL;
                    if (IsUnicode == OpcUa_True) {
                        WideBuffer = realloc(WideBuffer, sizeof(SQLWCHAR) * (ContiguousBufferIndex + 1));
                        WideBuffer[ContiguousBufferIndex] = L'\0';

                        char * tempEMessage = NULL;
                        narrowStr = ID_ConvertNullTerminatedWideToMultiByte((wchar_t*)WideBuffer, &tempEMessage);
                        if (narrowStr == NULL) {
                            if (rowElement != NULL) { cJSON_Delete(rowElement); }
                            if (MultiByteBuffer != NULL) { free(MultiByteBuffer); MultiByteBuffer = NULL; }
                            if (WideBuffer != NULL) { free(WideBuffer); WideBuffer = NULL; }
                            result = OpcUa_False;
                            errorStr = tempEMessage;
                            goto Error;
                        }
                    } else if (IsUnicode == OpcUa_False) {
                        MultiByteBuffer = realloc(MultiByteBuffer, sizeof(SQLCHAR) * (ContiguousBufferIndex + 1));
                        MultiByteBuffer[ContiguousBufferIndex] = '\0';
                        narrowStr = (char*)MultiByteBuffer;
                    }

                    cJSON_AddItemToArray(rowElement, cJSON_CreateString(narrowStr));
                    if (IsUnicode == OpcUa_True) { free(narrowStr); }
                }
                else if (BinaryIndicators[i - 1] == 1) { // binary type data
                    char * base64Text = NULL;
                    int base64Length = 0;
                    ID_Base64Encode((SQLCHAR*)MultiByteBuffer, ContiguousBufferIndex, &base64Text, &base64Length);

                    // append null terminator
                    base64Text = realloc(base64Text, sizeof(char) * (base64Length + 1));
                    base64Text[base64Length] = '\0';
                    cJSON_AddItemToArray(rowElement, cJSON_CreateString(base64Text));
                    free(base64Text);
                }

                if (MultiByteBuffer != NULL) { free(MultiByteBuffer); MultiByteBuffer = NULL; }
                if (WideBuffer != NULL) { free(WideBuffer); WideBuffer = NULL; }
            }
        }

        cJSON_AddItemToArray(rowObject, rowElement);
        numRows++;
        if (numRows >= fetchSize && fetchStyle != SQL_FETCH_PRIOR) {
            HasReachedLimit = OpcUa_True;
            break;
        }
    }

    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "attrs", attributeObject);
    cJSON_AddItemToObject(root, "rows", rowObject);
    cJSON_AddBoolToObject(root, "hasMore", HasReachedLimit);
    *outputText = cJSON_PrintUnformatted(root);
    
    Error: ;
    if (root != NULL) {
        cJSON_Delete(root);
    } else {
        if (attributeObject != NULL) { cJSON_Delete(attributeObject); }
        if (rowObject != NULL) { cJSON_Delete(rowObject); }
    }

    if (BinaryIndicators != NULL) { free(BinaryIndicators); }
    if (errorStr != NULL) { *errorMessage = errorStr; }
    *moreExist = HasReachedLimit == OpcUa_False ? 0 : 1;
    return result;
}

OpcUa_StatusCode ID_UASQLSelect(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoOfInputArguments, OpcUa_Variant * InputArguments, int * NoOfOutputArguments, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArguments, 0);
    ID_CHECK_INPUT_TYPE_ARG(InputArguments, 1, OpcUaType_String);
    
    OpcUa_ReferenceParameter(NoOfInputArguments);
    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    char * query = NULL;
    wchar_t * wideQuery = NULL;
    char * output = NULL;
    ID_PreparedStatement * PStmt = NULL;
    ID_Session * UserSession = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLSelect");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLSelect (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    query = OpcUa_String_GetRawString(&InputArguments[1].Value.String);
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Query - %s - \n", query);

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArguments[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);

    SQLHANDLE StatementHandle = PStmt->Statement;
    SQLRETURN retVal = SQLSetStmtAttr(StatementHandle, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_STATIC, 0);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);
    
    if (IsUnicode == OpcUa_True) {
        char * errorMessage = NULL;
        wideQuery = ID_ConvertNullTerminatedMultiByteToWide(query, &errorMessage);
        if (wideQuery == NULL) {
            if (errorMessage != NULL) {
                errorStr = errorMessage;
                shouldAttachOwner = OpcUa_True;
                result = OpcUa_False;
            }

            goto Error;
        }

        retVal = SQLExecDirectW(StatementHandle, (SQLWCHAR*)wideQuery, SQL_NTSL);
        ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);
        free(wideQuery);
        wideQuery = NULL;
    }
    else {
        retVal = SQLExecDirect(StatementHandle, (SQLCHAR*)query, SQL_NTS);
        ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);
    }
    
    int moreExist = 0;
    SQLSMALLINT fetchStyle = ID_GetSupportedFetchStyle(UserSession->SessionDBConnHandle);
    result = ID_Fetch(IsUnicode, StatementHandle, fetchStyle, ID_SINGULAR_FETCH_LIMIT, &output, &moreExist, &errorStr);
    shouldAttachOwner = OpcUa_True;

Error:;
    if (wideQuery != NULL) { free(wideQuery); }

    *NoOfOutputArguments = 3;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 3);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 3);
    ID_INITIALIZE_OUTPUT();
    OutputArguments[2].Datatype = OpcUaType_String;
    OutputArguments[2].ArrayType = OpcUa_VariantArrayType_Scalar;
    OpcUa_String_Initialize(&OutputArguments[2].Value.String);
    OpcUa_String_AttachWithOwnership(&OutputArguments[2].Value.String, output);
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLFetch(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 1);
    
    OpcUa_UInt32 FetchLimit = InputArgs[1].Value.UInt32;
    if (FetchLimit == 0 || FetchLimit > ID_SINGULAR_FETCH_LIMIT) {
        FetchLimit = ID_SINGULAR_FETCH_LIMIT;
    }
    
    OpcUa_ReferenceParameter(NoInputArgs);
    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    char * output = NULL;
    ID_PreparedStatement * PStmt = NULL;
    ID_Session * UserSession = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLFetch");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLFetch (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");
    
    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);
    
    int moreExist = 0;
    SQLHANDLE StatementHandle = PStmt->Statement;
    SQLSMALLINT fetchStyle = ID_GetSupportedFetchStyle(UserSession->SessionDBConnHandle);
    result = ID_Fetch(IsUnicode, StatementHandle, fetchStyle, FetchLimit, &output, &moreExist, &errorStr);
    shouldAttachOwner = OpcUa_True;
    
    Error: ;
    *NoOutputArgs = 3;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 3);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 3);
    ID_INITIALIZE_OUTPUT();
    OutputArguments[2].Datatype = OpcUaType_String;
    OutputArguments[2].ArrayType = OpcUa_VariantArrayType_Scalar;
    OpcUa_String_Initialize(&OutputArguments[2].Value.String);
    OpcUa_String_AttachWithOwnership(&OutputArguments[2].Value.String, output);
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

/* Routing */
OpcUa_StatusCode ID_UASQLSelectA(OpcUa_NodeId * AuthToken, int NoOfInputArguments, OpcUa_Variant * InputArguments, int * NoOfOutputArguments, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLSelect(OpcUa_False, AuthToken, NoOfInputArguments, InputArguments, NoOfOutputArguments, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLSelectW(OpcUa_NodeId * AuthToken, int NoOfInputArguments, OpcUa_Variant * InputArguments, int * NoOfOutputArguments, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLSelect(OpcUa_True, AuthToken, NoOfInputArguments, InputArguments, NoOfOutputArguments, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLFetchA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLFetch(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLFetchW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLFetch(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}