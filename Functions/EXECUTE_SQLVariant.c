/*
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "UASQLFunctions.h"
#include "UASQLInternal.h"
#include <wchar.h>

extern ID_SessionManagerHandle GlobalSessionManagerHandle;

/* SQLExecDirect A + W Function */
OpcUa_StatusCode ID_UASQLExecDirect(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    ID_CHECK_INPUT_TYPE_ARG(InputArgs, 1, OpcUaType_String);

    OpcUa_ReferenceParameter(NoInputArgs);
    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * DDLToExecute = NULL;
    wchar_t * DDLToExecuteW = NULL;
    char * errorStr = NULL;
    ID_Session * UserSession = NULL;
    ID_PreparedStatement * PStmt = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLExecDirect");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLExecDirect (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    DDLToExecute = OpcUa_String_GetRawString(&InputArgs[1].Value.String);
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "DDL To Execute - %s - \n", DDLToExecute);
    
    if (IsUnicode == OpcUa_True) {
        DDLToExecuteW = ID_ConvertNullTerminatedMultiByteToWide(DDLToExecute, &errorStr);
        if (DDLToExecuteW == NULL) {
            result = OpcUa_False;
            shouldAttachOwner = OpcUa_True;
            goto Error;
        }
    }

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);

    HSTMT StatementHandle = PStmt->Statement;
    SQLRETURN retVal = SQL_SUCCESS;
    if (IsUnicode == OpcUa_True) {
        retVal = SQLExecDirectW(StatementHandle, (SQLWCHAR*)DDLToExecuteW, SQL_NTSL);
    } else {
        retVal = SQLExecDirect(StatementHandle, (SQLCHAR*)DDLToExecute, SQL_NTS);
    }
    
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

Error:;
    if (DDLToExecuteW != NULL) { free(DDLToExecuteW); }
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLExecDirectA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLExecDirect(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLExecDirectW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLExecDirect(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLPrepare A + W Functions */
OpcUa_StatusCode ID_UASQLPrepare(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    ID_CHECK_INPUT_TYPE_ARG(InputArgs, 1, OpcUaType_String);

    OpcUa_ReferenceParameter(NoInputArgs);
    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    ID_PreparedStatement * PStmt = NULL;
    ID_Session * UserSession = NULL;
    char * prepareStatement = OpcUa_String_GetRawString(&InputArgs[1].Value.String);
    wchar_t * prepareStatementW = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLPrepare");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLPrepare (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);

    SQLRETURN retVal = SQL_SUCCESS;
    if (IsUnicode == OpcUa_True) {
        prepareStatementW = ID_ConvertNullTerminatedMultiByteToWide(prepareStatement, &errorStr);
        if (prepareStatement == NULL) {
            result = OpcUa_False;
            shouldAttachOwner = OpcUa_True;
            goto Error;
        }

        retVal = SQLPrepareW(PStmt->Statement, (SQLWCHAR*)prepareStatementW, SQL_NTSL);
    } else {
        retVal = SQLPrepare(PStmt->Statement, (SQLCHAR *)prepareStatement, SQL_NTS);
    }

    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, PStmt->Statement);

Error:;
    if (prepareStatementW != NULL) { free(prepareStatementW); }
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLPrepareA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLPrepare(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLPrepareW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLPrepare(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLBind A + W Functions */

// first two parameter is json object describing value
// next four parameters are relevant buffers for BindParameter
// last parameters are concerning whether allocation or redirection to pointer
// IF invalid base64 is located, application will bind empty string
void ID_CreateBufferParameterAttachment(OpcUa_Boolean IsUnicode, cJSON * element, int index, SQLSMALLINT DataTypeInfo,
                                        SQLLEN * dataLengthBuffers, SQLLEN ** indicatorPointers,
                                        void ** dataBuffers, SQLSMALLINT * paramCType, void ** redirectPtr) {

    SQLSMALLINT inputCType = IsUnicode == OpcUa_True ? SQL_C_WCHAR : SQL_C_CHAR;
    void * dataPointer = NULL;

    if (element->type == cJSON_Number || element->type == cJSON_True || element->type == cJSON_False) {
        inputCType = SQL_C_DOUBLE;
        if (element->type == cJSON_Number) {
            dataBuffers[index] = malloc(sizeof(double));
            memset(dataBuffers[index], 0x00, sizeof(double));
            memcpy(dataBuffers[index], &element->valuedouble, sizeof(double));
        }
        else {
            int temp = element->type == cJSON_True ? 1 : 0;
            dataBuffers[index] = malloc(sizeof(int));
            memset(dataBuffers[index], 0x00, sizeof(int));
            int * data = dataBuffers[index];
            *data = temp;
            inputCType = SQL_C_SLONG;
        }

        indicatorPointers[index] = malloc(sizeof(SQLLEN));
        *indicatorPointers[index] = 0;
        dataPointer = dataBuffers[index];
    } else if (element->type == cJSON_String) {
        if (DataTypeInfo == SQL_BINARY || DataTypeInfo == SQL_VARBINARY || DataTypeInfo == SQL_LONGVARBINARY) {
            inputCType = SQL_C_BINARY;

            if (strlen(element->valuestring) >= 2) {
                ID_Base64Decode(element->valuestring, (unsigned char **)&dataBuffers[index], (size_t*)&dataLengthBuffers[index]);
                indicatorPointers[index] = malloc(sizeof(SQLLEN));
                *indicatorPointers[index] = dataLengthBuffers[index];
                dataPointer = dataBuffers[index];
            } else {
                // delayed error reporting (insurance policy)
                // insert NULL if the supplied base64 is invalid
                // this way, Base64Decode will not do poor memory access
                // this way will also ensure that user knows of the
                // poorly supplied WITH either a failed INSERT/UPDATE/BIND
                // or NULL inserted into the row (indicates failed decoding)
                dataPointer = NULL;
                dataLengthBuffers[index] = 0;
                indicatorPointers[index] = malloc(sizeof(SQLLEN));
                *indicatorPointers[index] = SQL_NULL_DATA;
            }
        }
        else {
            char * errorMessage = NULL;
            wchar_t * jsonString = NULL;
            if (IsUnicode == OpcUa_True) { jsonString = ID_ConvertNullTerminatedMultiByteToWide(element->valuestring, &errorMessage); }

            if (jsonString != NULL || IsUnicode == OpcUa_False) {
                size_t length = 0;
                size_t size = sizeof(char);
                if (IsUnicode == OpcUa_True) {
                    length = wcslen(jsonString);
                    size = sizeof(wchar_t);
                    dataBuffers[index] = jsonString;
                } else {
                    dataBuffers[index] = malloc(sizeof(char) * (strlen(element->valuestring) + 1));
                    memset(dataBuffers[index], 0x00, sizeof(char) * (strlen(element->valuestring) + 1));
                    memcpy(dataBuffers[index], element->valuestring, sizeof(char) * (strlen(element->valuestring)));
                    length = strlen(element->valuestring);
                }

                dataPointer = dataBuffers[index];
                dataLengthBuffers[index] = length * size;
                indicatorPointers[index] = malloc(sizeof(SQLLEN));
                *indicatorPointers[index] = dataLengthBuffers[index];
            }
            else {
                // insert '' string as NULL may not be a valid value
                printf("ERROR converting on Bind Parameter to Buffer: %s\n", errorMessage);

                wchar_t * emptyStr = malloc(sizeof(wchar_t) * 1);
                memset(emptyStr, 0x00, sizeof(wchar_t) * 1);
                emptyStr[0] = L'\0';

                dataBuffers[index] = emptyStr;
                dataLengthBuffers[index] = wcslen(emptyStr) * sizeof(wchar_t);
                indicatorPointers[index] = malloc(sizeof(SQLLEN));
                *indicatorPointers[index] = dataLengthBuffers[index];
                dataPointer = dataBuffers[index];
            }
            free(errorMessage);
        }
    }
    else if (element->type == cJSON_NULL) {
        dataPointer = NULL;
        dataLengthBuffers[index] = 0;
        indicatorPointers[index] = malloc(sizeof(SQLLEN));
        *indicatorPointers[index] = SQL_NULL_DATA;
    }

    *paramCType = inputCType;
    *redirectPtr = dataPointer;
}

OpcUa_StatusCode ID_UASQLBind(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    ID_CHECK_INPUT_TYPE_ARG(InputArgs, 1, OpcUaType_String);

    OpcUa_ReferenceParameter(NoInputArgs);
    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    cJSON * jsonObject = NULL;
    ID_PreparedStatement * PStmt = NULL;
    ID_Session * UserSession = NULL;

    wchar_t * TableNameWide = NULL;
    wchar_t * ColumnNameWide = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLBind");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLBind (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);

    jsonObject = cJSON_Parse(OpcUa_String_GetRawString(&InputArgs[1].Value.String));
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "table_name", cJSON_String);
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "attr", cJSON_String);
    ID_CHECK_JSON_KEY_EXISTS(jsonObject, "paramMarker", cJSON_Number);

    cJSON * value = cJSON_GetObjectItem(jsonObject, "value");
    if (value == NULL || (value->type != cJSON_Number && value->type != cJSON_String && value->type != cJSON_True && value->type != cJSON_False)) {
        result = OpcUa_False;
        errorStr = "ERR:1002:malformed input";
        goto Error;
    }

    int marker = cJSON_GetObjectItem(jsonObject, "paramMarker")->valueint;
    if (marker < 1) {
        result = OpcUa_False;
        errorStr = "bad param marker";
        goto Error;
    }

    SQLSMALLINT DataType = 0;
    SQLULEN ColumnSize = 0;
    SQLSMALLINT DecimalDigits = 0;
    char * tableName = cJSON_GetObjectItem(jsonObject, "table_name")->valuestring;
    char * columnName = cJSON_GetObjectItem(jsonObject, "attr")->valuestring;

    if (IsUnicode) {
        TableNameWide = ID_ConvertNullTerminatedMultiByteToWide(tableName, &errorStr);
        if (TableNameWide == NULL) {
            result = OpcUa_False;
            shouldAttachOwner = OpcUa_True;
            goto Error;
        }

        ColumnNameWide = ID_ConvertNullTerminatedMultiByteToWide(columnName, &errorStr);
        if (ColumnNameWide == NULL) {
            result = OpcUa_False;
            shouldAttachOwner = OpcUa_True;
            goto Error;
        }
    }

    if (IsUnicode == OpcUa_True) {
        result = ID_UASQLDescribeParameterW(UserSession, TableNameWide, ColumnNameWide, &DataType, &ColumnSize, &DecimalDigits);
    } else if (IsUnicode == OpcUa_False) {
        result = ID_UASQLDescribeParameterA(UserSession, tableName, columnName, &DataType, &ColumnSize, &DecimalDigits);
    }

    if (result == OpcUa_False) {
        errorStr = "unable to describe parameter";
        goto Error;
    }

    // reject base64 if the base64 data is too short (less than 4 characters)
    if (DataType == SQL_BINARY || DataType == SQL_VARBINARY || DataType == SQL_LONGVARBINARY) {
        if (strlen(value->valuestring) < 2) {
            errorStr = "supplied parameter value is too short/invalid base64";
            goto Error;
        }
    }

    // shift a lot of buffers
    PStmt->Buffers = realloc(PStmt->Buffers, sizeof(void*) * (PStmt->NumBuffers + 1));
    PStmt->InputIndicators = realloc(PStmt->InputIndicators, sizeof(SQLLEN*) * (PStmt->NumBuffers + 1));
    PStmt->BufferLengths = realloc(PStmt->BufferLengths, sizeof(SQLLEN) * (PStmt->NumBuffers + 1));

    SQLSMALLINT cType = 0;
    void * dataPointer = NULL;
    ID_CreateBufferParameterAttachment(IsUnicode, value, PStmt->NumBuffers, DataType, PStmt->BufferLengths, PStmt->InputIndicators, PStmt->Buffers, &cType, &dataPointer);

    SQLRETURN retVal = SQLBindParameter(PStmt->Statement, (SQLSMALLINT)marker, SQL_PARAM_INPUT, cType, DataType, ColumnSize, DecimalDigits, dataPointer, PStmt->BufferLengths[PStmt->NumBuffers], PStmt->InputIndicators[PStmt->NumBuffers]);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, PStmt->Statement);
    PStmt->NumBuffers++;

Error:;
    if (result == OpcUa_False && PStmt != NULL) {
        if (PStmt->Buffers != NULL && PStmt->Buffers[PStmt->NumBuffers] != NULL) {
            //resize them back?
        }
    }

    if (TableNameWide != NULL) { free(TableNameWide); }
    if (ColumnNameWide != NULL) { free(ColumnNameWide); }

    cJSON_Delete(jsonObject);
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLBindA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLBind(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLBindW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLBind(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLExecute A + W Functions */
OpcUa_StatusCode ID_UASQLExecute(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    OpcUa_ReferenceParameter(NoInputArgs);

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    ID_Session * UserSession = NULL;
    SQLLEN changeCount = 0;
    ID_PreparedStatement * PStmt = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLExecute");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLExecute (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);
    
    SQLRETURN retVal = SQLExecute(PStmt->Statement);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, PStmt->Statement);

    retVal = SQLRowCount(PStmt->Statement, &changeCount);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_STMT, PStmt->Statement);

Error:;
    *NoOutputArgs = 3;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 3);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 3);
    ID_INITIALIZE_OUTPUT();
    OutputArguments[2].Datatype = OpcUaType_UInt32;
    OutputArguments[2].ArrayType = OpcUa_VariantArrayType_Scalar;
    OutputArguments[2].Value.UInt32 = (OpcUa_Int32)changeCount;
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLExecuteA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLExecute(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLExecuteW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLExecute(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}