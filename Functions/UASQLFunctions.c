/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "UASQLFunctions.h"
#include "UASQLInternal.h"

extern ID_SessionManagerHandle GlobalSessionManagerHandle;

/* SQL Connect A + W Implementation */
OpcUa_StatusCode ID_UASQLConnect(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_TYPE_ARG(InputArgs, 0, OpcUaType_String);
    OpcUa_ReferenceParameter(NoInputArgs);

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    char * DSNConnect = NULL;
    wchar_t * DSNConnectW = NULL;
    ID_Session * UserSession = NULL;

    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLConnect");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLConnect (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    DSNConnect = OpcUa_String_GetRawString(&InputArgs[0].Value.String);
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Received DSN Connect String - %s -\n", DSNConnect);
    if (IsUnicode == OpcUa_True) {
        DSNConnectW = ID_ConvertNullTerminatedMultiByteToWide(DSNConnect, &errorStr);
        if (DSNConnectW == NULL) {
            result = OpcUa_False;
            shouldAttachOwner = OpcUa_True;
            goto Error;
        }
    }

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_UNINITIALIZED();

    SQLRETURN retVal = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &UserSession->SessionDBEnvHandle);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHENV", SQL_HANDLE_ENV, UserSession->SessionDBEnvHandle);
    SQLSetEnvAttr(UserSession->SessionDBEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_NTS);

    retVal = SQLAllocHandle(SQL_HANDLE_DBC, UserSession->SessionDBEnvHandle, &UserSession->SessionDBConnHandle);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHENV", SQL_HANDLE_ENV, UserSession->SessionDBEnvHandle);

    if (IsUnicode == OpcUa_True) {
        retVal = SQLDriverConnectW(UserSession->SessionDBConnHandle, NULL, (SQLWCHAR*)DSNConnectW, SQL_NTSL, NULL, 0, NULL, 0);
    } else {
        retVal = SQLDriverConnect(UserSession->SessionDBConnHandle, NULL, (SQLCHAR*)DSNConnect, SQL_NTS, NULL, 0, NULL, 0);
    }

    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHDBC", SQL_HANDLE_DBC, UserSession->SessionDBConnHandle);
    UserSession->IsConnected = OpcUa_True;

Error:;
    if (DSNConnectW != NULL) { free(DSNConnectW); }
    if (UserSession->IsConnected != OpcUa_True) {
        /* free useless handles if connection failed */
        if (UserSession->SessionDBConnHandle != NULL) {
            SQLFreeHandle(SQL_HANDLE_DBC, UserSession->SessionDBConnHandle);
            UserSession->SessionDBConnHandle = NULL;
        }

        if (UserSession->SessionDBEnvHandle != NULL) {
            SQLFreeHandle(SQL_HANDLE_ENV, UserSession->SessionDBEnvHandle);
            UserSession->SessionDBEnvHandle = NULL;
        }
    }

    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLConnectA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLConnect(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLConnectW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLConnect(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLSetCommit A + W Functions */
OpcUa_StatusCode ID_UASQLSetCommit(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_TYPE_ARG(InputArgs, 0, OpcUaType_Boolean);
    OpcUa_ReferenceParameter(NoInputArgs);

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    OpcUa_Boolean AutoCommit = OpcUa_True;
    ID_Session * UserSession = NULL;
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLSetCommit");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLSetCommit (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    AutoCommit = InputArgs[0].Value.Boolean;
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Committing with type - %s - \n", AutoCommit ? "ON" : "OFF");
    
    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    
    SQLRETURN retVal = SQL_SUCCESS;
    if (AutoCommit) {
        retVal = SQLSetConnectAttr(UserSession->SessionDBConnHandle, SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_ON, SQL_NTS);
    } else {
        retVal = SQLSetConnectAttr(UserSession->SessionDBConnHandle, SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_OFF, SQL_NTS);
    }

    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_DBC, UserSession->SessionDBConnHandle);
    
    Error: ;
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLSetCommitA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLSetCommit(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLSetCommitW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLSetCommit(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLCommit A + W Functions */
OpcUa_StatusCode ID_UASQLCommit(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_TYPE_ARG(InputArgs, 0, OpcUaType_Boolean);
    OpcUa_ReferenceParameter(NoInputArgs);

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    OpcUa_Boolean CommitRequest = OpcUa_False;
    char * errorStr = NULL;
    SQLSMALLINT TransAction = SQL_COMMIT;
    ID_Session * UserSession = NULL;
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLCommit");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLCommit (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    CommitRequest = InputArgs[0].Value.Boolean;
    TransAction = CommitRequest ? SQL_COMMIT : SQL_ROLLBACK;
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Transaction Action - %s - \n", CommitRequest ? "COMMIT" : "ROLLBACK");
    
    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    
    SQLRETURN retVal = SQLEndTran(SQL_HANDLE_DBC, UserSession->SessionDBConnHandle, TransAction);
    ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_DBC, UserSession->SessionDBConnHandle);
    
    Error: ;
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLCommitA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLCommit(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLCommitW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLCommit(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLDisconnect A + W Functions */
OpcUa_StatusCode ID_UASQLDisconnect(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
	OpcUa_ReferenceParameter(InputCodes);
	OpcUa_ReferenceParameter(NoInputArgs);
    OpcUa_ReferenceParameter(InputArgs);	

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    ID_Session * UserSession = NULL;
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLDisconnect");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLDisconnect (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    
    if (UserSession->SessionDBConnHandle != NULL) {
        if (UserSession->IsConnected) {
            // destroy all prepared statement handles
            for (OpcUa_UInt32 i = 0; i < UserSession->NumPreparedStatements; i++) {
                ID_PreparedStatement_Free(UserSession->PreparedStatements[i]);
                free(UserSession->PreparedStatements[i]);
                UserSession->PreparedStatements[i] = NULL;
            }
            
            if (UserSession->PreparedStatements != NULL) {
                free(UserSession->PreparedStatements);
                UserSession->NumPreparedStatements = 0;
                UserSession->PreparedStatements = NULL;
            }
            
            SQLRETURN retVal = SQLDisconnect(UserSession->SessionDBConnHandle);
            ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_DBC, UserSession->SessionDBConnHandle); 
            UserSession->IsConnected = OpcUa_False;
        }
        
        SQLRETURN retVal = SQLFreeHandle(SQL_HANDLE_DBC, UserSession->SessionDBConnHandle);
        ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_DBC, UserSession->SessionDBConnHandle); 
        UserSession->SessionDBConnHandle = NULL;
    }
    
    if (UserSession->SessionDBEnvHandle != NULL) {
        SQLRETURN retVal = SQLFreeHandle(SQL_HANDLE_ENV, UserSession->SessionDBEnvHandle);
        ID_CHECK_RETVAL(IsUnicode, retVal, "SQLHSTMT", SQL_HANDLE_DBC, UserSession->SessionDBConnHandle); 
        UserSession->SessionDBEnvHandle = NULL;
    }
    
    Error: ;
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLDisconnectA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLDisconnect(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLDisconnectW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLDisconnect(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLAllocHandle A + W Functions */
OpcUa_StatusCode ID_UASQLAllocHandle(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
	OpcUa_ReferenceParameter(NoInputArgs);

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    ID_Session * UserSession = NULL;
    ID_PreparedStatement * PStmt = NULL;
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLAllocHandle");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLAllocHandle (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    
    if (PStmt != NULL) {
        // destroy handle
        ID_PreparedStatement_Free(PStmt);
    } else {
        // create handle
        OPCUA_P_MUTEX_LOCK(UserSession->PreparedStatementsMutex);
        PStmt = malloc(sizeof(ID_PreparedStatement));
        memset(PStmt, 0x00, sizeof(ID_PreparedStatement));
        PStmt->Identifier = InputArgs[0].Value.UInt32;
        UserSession->PreparedStatements = realloc(UserSession->PreparedStatements, sizeof(ID_PreparedStatement*) * (UserSession->NumPreparedStatements + 1));
        UserSession->PreparedStatements[UserSession->NumPreparedStatements] = PStmt;
        UserSession->NumPreparedStatements++;
        OPCUA_P_MUTEX_UNLOCK(UserSession->PreparedStatementsMutex);
    }
    
    // allocate handle
    SQLRETURN retVal = SQLAllocHandle(SQL_HANDLE_STMT, UserSession->SessionDBConnHandle, &PStmt->Statement);
    ID_CHECK_RETVAL(IsUnicode, retVal, "HDBC", SQL_HANDLE_DBC, UserSession->SessionDBConnHandle);
    
    Error: ;
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLAllocHandleA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLAllocHandle(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLAllocHandleW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLAllocHandle(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLCloseHandle A + W Functions */
OpcUa_StatusCode ID_UASQLCloseHandle(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
	OpcUa_ReferenceParameter(NoInputArgs);

    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    ID_Session * UserSession = NULL;
    ID_PreparedStatement * PStmt = NULL;
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLCloseHandle");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLCloseHandle (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT(UserSession, PStmt, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);
    
    // close statement
    SQLRETURN retVal = SQLFreeStmt(PStmt->Statement, SQL_CLOSE);
    ID_CHECK_RETVAL(IsUnicode, retVal, "HSTMT", SQL_HANDLE_STMT, PStmt->Statement);
    
    Error: ;
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLCloseHandleA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLCloseHandle(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLCloseHandleW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLCloseHandle(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLResetParams A + W Functions */
OpcUa_StatusCode ID_UASQLResetParamsHandle(OpcUa_Boolean IsUnicode, OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    OpcUa_ReferenceParameter(NoInputArgs);
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    
    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    ID_Session * UserSession = NULL;
    ID_PreparedStatement * PStmt = NULL;
    int PStmtIndex = -1;
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLResetParamsHandle");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLResetParamsHandle (%s) function\n", IsUnicode == OpcUa_True ? "Unicode" : "ANSI/UTF8");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT_WITH_INDEX(UserSession, PStmt, PStmtIndex, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);
    
    SQLRETURN retVal = SQLFreeStmt(PStmt->Statement, SQL_RESET_PARAMS);
    ID_CHECK_RETVAL(IsUnicode, retVal, "HSTMT", SQL_HANDLE_STMT, PStmt->Statement);
    
    // force free buffers
    for (int i = 0; i < PStmt->NumBuffers; i++) {
        free(PStmt->Buffers[i]);
        free(PStmt->InputIndicators[i]);
    }
    
    free(PStmt->Buffers);
    free(PStmt->InputIndicators);
    free(PStmt->BufferLengths);
    
    PStmt->NumBuffers = 0;
    PStmt->Buffers = NULL;
    PStmt->InputIndicators = NULL;
    PStmt->BufferLengths = NULL;
    
    Error: ;
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLResetParamsHandleA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLResetParamsHandle(OpcUa_False, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLResetParamsHandleW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLResetParamsHandle(OpcUa_True, AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

/* SQLFreeHandle A + W Functions */
OpcUa_StatusCode ID_UASQLFreeHandle(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    OpcUa_ReferenceParameter(NoInputArgs);
    ID_CHECK_INPUT_UINT32_ARG(InputArgs, 0);
    
    OpcUa_Boolean result = OpcUa_True;
    OpcUa_Boolean shouldAttachOwner = OpcUa_False;
    char * errorStr = NULL;
    ID_Session * UserSession = NULL;
    ID_PreparedStatement * PStmt = NULL;
    int PStmtIndex = -1;
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UASQLFreeHandle");
    OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Beginning UASQLFreeHandle function\n");

    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, AuthToken);
    ID_CHECK_SESSION_ACTIVATED();
    ID_CHECK_DB_INITIALIZED();
    ID_FIND_PREPARED_STATEMENT_WITH_INDEX(UserSession, PStmt, PStmtIndex, InputArgs[0].Value.UInt32);
    ID_ASSERT_STATEMENT_HANDLE_NOT_NULL(PStmt);
    
    OPCUA_P_MUTEX_LOCK(UserSession->PreparedStatementsMutex);
    ID_PreparedStatement_Free(PStmt);
    for (OpcUa_UInt32 i = PStmtIndex + 1; i < UserSession->NumPreparedStatements; i++) {
        UserSession->PreparedStatements[i - 1] = UserSession->PreparedStatements[i];
    }
    
    free(PStmt);
    UserSession->NumPreparedStatements--;
    UserSession->PreparedStatements = realloc(UserSession->PreparedStatements, sizeof(ID_PreparedStatement*) * UserSession->NumPreparedStatements);
    OPCUA_P_MUTEX_UNLOCK(UserSession->PreparedStatementsMutex);

    Error: ;
    *NoOutputArgs = 2;
    OpcUa_Variant * OutputArguments = malloc(sizeof(OpcUa_Variant) * 2);
    memset(OutputArguments, 0, sizeof(OpcUa_Variant) * 2);
    ID_INITIALIZE_OUTPUT();
    *OutputArgs = OutputArguments;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UASQLFreeHandleA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    return ID_UASQLFreeHandle(AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}

OpcUa_StatusCode ID_UASQLFreeHandleW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes) {
    ID_CHECK_OPERATING_SYSTEM(ID_OS_WINDOWS);
    return ID_UASQLFreeHandle(AuthToken, NoInputArgs, InputArgs, NoOutputArgs, OutputArgs, InputCodes);
}