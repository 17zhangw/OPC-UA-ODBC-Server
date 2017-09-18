/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "Initialize.h"
#include "../SessionManager/SessionManager.h"
#include "../Functions/UASQLFunctions.h"
#include "../Functions/UASQLInternal.h"

extern ID_SessionManagerHandle GlobalSessionManagerHandle;

// --- initialization ---
#pragma warning(push)
#pragma warning(disable: 4204)
ID_MethodNode * ID_CreateDBMethod(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, OpcUa_NodeId * DatabaseId, 
                                  int NoInput, ID_InputArgsInfo * Input, int NoOutput, ID_InputArgsInfo * Output,
                                  OpcUa_NodeId MethodId, char * MethodName) {
    OpcUa_NodeId ID_HasComponentNode = ID_HasComponent;
    OpcUa_NodeId ID_OrganizesNode = ID_Organizes;

    OpcUa_NodeId DatabaseViewId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_VIEW);;
    
    ID_MethodNode * Method;
    OpcUa_NodeId InputId = ID_OPC_IDNodeId(NoInput > 0 ? ID_Indexer_GetNextIndex(Indexer) : 0);
    OpcUa_NodeId OutputId = ID_OPC_IDNodeId(NoOutput > 0 ? ID_Indexer_GetNextIndex(Indexer) : 0);
    ID_AddressSpaceHelper_CreateMethodNode(Handle, Indexer, &Method, MethodId, ID_NAMESPACE_INDEX, MethodName, NoInput, InputId, Input, NoOutput, OutputId, Output);
    ID_AddressSpace_AddBiReference(Handle, DatabaseId, ID_NodeType_Object, &MethodId, ID_NodeType_Method, &ID_HasComponentNode);
    ID_AddressSpace_AddBiReference(Handle, &DatabaseViewId, ID_NodeType_View, &MethodId, ID_NodeType_Method, &ID_OrganizesNode);
    Method->Executable = OpcUa_True;
    Method->UserExecutable = OpcUa_True;
    return Method;
}

void ID_CreateWideMultiDBMethod(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, OpcUa_NodeId * DatabaseId,
                                int NoInput, ID_InputArgsInfo * Input, int NoOutput, ID_InputArgsInfo * Output,
                                OpcUa_Boolean HasUnicode, OpcUa_NodeId UnicodeMethodId, char * UnicodeMethodName, ID_Method_FuncPtr* UnicodeFunc,
                                OpcUa_Boolean HasAnsiUTF8, OpcUa_NodeId AnsiUTF8MethodId, char * AnsiUTF8MethodName, ID_Method_FuncPtr* AnsiUTF8Func) {
    if (HasUnicode == OpcUa_True) {
        ID_MethodNode * UnicodeNode = ID_CreateDBMethod(Handle, Indexer, DatabaseId, NoInput, Input, NoOutput, Output, UnicodeMethodId, UnicodeMethodName);
        UnicodeNode->Function = UnicodeFunc;
    }

    if (HasAnsiUTF8 == OpcUa_True) {
        ID_MethodNode * AnsiUTF8Node = ID_CreateDBMethod(Handle, Indexer, DatabaseId, NoInput, Input, NoOutput, Output, AnsiUTF8MethodId, AnsiUTF8MethodName);
        AnsiUTF8Node->Function = AnsiUTF8Func;
    }
}

void ID_Initialize_CustomObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer) {
    OpcUa_NodeId ID_OrganizesNode = ID_Organizes;
    OpcUa_Int NumAlloc = 3;
    ID_SETUP_METHOD_LOCALIZED(NumAlloc);
    
    OpcUa_NodeId DatabaseViewId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_VIEW);
    ID_ViewNode * DatabaseViewNode = NULL;
    ID_AddressSpace_CreateViewNode(Handle, &DatabaseViewNode, NULL);
    ID_OpcUa_NodeId_CopyTo(&DatabaseViewId, &DatabaseViewNode->BaseAttribute.NodeId);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(DatabaseViewNode, 1, "DatabaseView");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(DatabaseViewNode, "DatabaseView");
    ID_AddressSpace_CreateNodeVersionProperty(Handle, Indexer, &DatabaseViewId, ID_NodeType_View, NULL);
    
    OpcUa_NodeId BaseObjectType = ID_OPCNodeId(OpcUaId_BaseObjectType);
    ID_ObjectTypeNode * BaseObjectTypeNode = ID_AddressSpace_GetObjectTypeNode(Handle, &BaseObjectType);
    ID_ObjectTypeNode * DatabaseObjectTypeNode = NULL;
    OpcUa_NodeId DatabaseObjectTypeId = {OpcUa_IdType_Numeric,ID_NAMESPACE_INDEX,ID_Indexer_GetNextIndex(Indexer)};
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &DatabaseObjectTypeNode, 1, "DatabaseObjectType", OpcUa_False, DatabaseObjectTypeId, 0, NULL);
    ID_AddressSpace_AddBiReference(Handle, &DatabaseViewId, ID_NodeType_View, &DatabaseObjectTypeId, ID_NodeType_ObjectType, &ID_OrganizesNode);
    
    OpcUa_NodeId DatabaseObjectId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_OBJECT);
    ID_ObjectNode * DatabaseObject = ID_AddressSpace_CreateObjectFromObjectType(Handle, Indexer, DatabaseObjectTypeNode, &DatabaseObjectId);
    ID_AddressSpace_AddBiReference(Handle, &DatabaseViewId, ID_NodeType_View, &DatabaseObjectId, ID_NodeType_Object, &ID_OrganizesNode);
    ID_SET_BROWSE_NAME_FOR_ATTRIBUTES(DatabaseObject, ID_NAMESPACE_INDEX, "DatabaseObject");
    ID_SET_DISPLAY_NAME_FOR_ATTRIBUTES(DatabaseObject, "DatabaseObject");

    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 0, "Status of operation success/failure");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 1, "Error message if applicable");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 2, "JSON-encoded result string");
    ID_InputArgsInfo SQLStandardOutput[2] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_Boolean), "status", &OutputDescs[0]},
                                             {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "error", &OutputDescs[1]}};
    
    // sqlconnect method
    OpcUa_NodeId WSQLConnectMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLCONNECT);
    OpcUa_NodeId ASQLConnectMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLCONNECT);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "DSN used to connect to database");
    ID_InputArgsInfo SQLConnectInputArgs[1] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "dsn", &InputDescs[0]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 1, SQLConnectInputArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLConnectMethodId, "SQLConnectW", ID_UASQLConnectW,
                               OpcUa_True, ASQLConnectMethodId, "SQLConnectA", ID_UASQLConnectA);
    
    // sqlsetcommit method
    OpcUa_NodeId WSQLSetCommitMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLSETCOMMIT);
    OpcUa_NodeId ASQLSetCommitMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLSETCOMMIT);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Flag to indicate whether Database AutoCommits");
    ID_InputArgsInfo SQLSetCommitInputArgs[1] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_Boolean), "isAutoCommit", &InputDescs[0]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 1, SQLSetCommitInputArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLSetCommitMethodId, "SQLSetCommitW", ID_UASQLSetCommitW,
                               OpcUa_True, ASQLSetCommitMethodId, "SQLSetCommitA", ID_UASQLSetCommitA);
    
    // sqlcommit method
    OpcUa_NodeId WSQLCommitMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLCOMMIT);
    OpcUa_NodeId ASQLCommitMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLCOMMIT);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Flag to indicate whether shoud commit changes");
    ID_InputArgsInfo SQLCommitMethodInputArgs[1] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_Boolean), "isCommit", &InputDescs[0]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 1, SQLCommitMethodInputArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLCommitMethodId, "SQLCommitW", ID_UASQLCommitW,
                               OpcUa_True, ASQLCommitMethodId, "SQLCommitA", ID_UASQLCommitA);
    
    // sqlexecdirect method
    OpcUa_NodeId WSQLExecDirectMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLEXECDIRECT);
    OpcUa_NodeId ASQLExecDirectMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLEXECDIRECT);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 0, "Client-Supplied Pre-Allocated Statement Handle Identifier");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "DDL for the Database to Execute");
    ID_InputArgsInfo SQLExecDirectInputArgs[2] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]},
                                                  {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "ddl", &InputDescs[1]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 2, SQLExecDirectInputArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLExecDirectMethodId, "SQLExecDirectW", ID_UASQLExecDirectW,
                               OpcUa_True, ASQLExecDirectMethodId, "SQLExecDirectA", ID_UASQLExecDirectA);

    // sqlselect method
    OpcUa_NodeId WSQLSelectMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLSELECT);
    OpcUa_NodeId ASQLSelectMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLSELECT);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "Query to be executed");
    ID_InputArgsInfo SQLSelectMethodArgs[2] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]},
                                               {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "query", &InputDescs[1]}};
    ID_InputArgsInfo SQLOutParameters[3] = {{0},{0}, {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "result", &OutputDescs[2]}};
    memcpy(SQLOutParameters, SQLStandardOutput, 2 * sizeof(ID_InputArgsInfo));
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 2, SQLSelectMethodArgs, 3, SQLOutParameters,
                               OpcUa_True, WSQLSelectMethodId, "SQLSelectW", ID_UASQLSelectW,
                               OpcUa_True, ASQLSelectMethodId, "SQLSelectA", ID_UASQLSelectA);
    
    // sqlinsert method
    OpcUa_NodeId WSQLInsertMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLINSERT);
    OpcUa_NodeId ASQLInsertMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLINSERT);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 2, "number of affected rows");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "Input JSON to be used in the format: {\"table_name\":\"#tbl_name\",\"attrs\": \
                                       [{\"#attr1\":\"#attr1_type\"}...], \"rows\":[[#val1...]...]} OR \
                                       {\"table_name\":\"#tbl_name\",\"attrs\":[\"#attr1\"...],\"rows\":[[#val1...]...]}");
    ID_InputArgsInfo SQLInsertMethodArgs[2] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]},
                                               {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "input", &InputDescs[1]}};
    ID_InputArgsInfo SQLInsertOut = {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "count", &OutputDescs[2]};
    SQLOutParameters[2] = SQLInsertOut;
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 2, SQLInsertMethodArgs, 3, SQLOutParameters,
                               OpcUa_True, WSQLInsertMethodId, "SQLInsertW", ID_UASQLInsertW,
                               OpcUa_True, ASQLInsertMethodId, "SQLInsertA", ID_UASQLInsertA);
    
    // sqldelete method
    OpcUa_NodeId WSQLDeleteMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLDELETE);
    OpcUa_NodeId ASQLDeleteMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLDELETE);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "JSON request formatted as follows: {\"table_name\":\"#table_name\", \"query\":\"#query\"}");
    ID_InputArgsInfo SQLDeleteMethodArgs[2] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]},
                                               {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "input", &InputDescs[1]}};
    ID_InputArgsInfo SQLDeleteOut = {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "count", &OutputDescs[2]};
    SQLOutParameters[2] = SQLDeleteOut;
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 2, SQLDeleteMethodArgs, 3, SQLOutParameters,
                               OpcUa_True, WSQLDeleteMethodId, "SQLDeleteW", ID_UASQLDeleteW,
                               OpcUa_True, ASQLDeleteMethodId, "SQLDeleteA", ID_UASQLDeleteA);
    
    // sqlupdate method
    OpcUa_NodeId WSQLUpdateMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLUPDATE);
    OpcUa_NodeId ASQLUpdateMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLUPDATE);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "Input JSON to be used in the format: {\"table_name\":\"#tbl_name\",\"attrs\": \
                                       [{\"#attr1\":\"#attr1_type\"}...], \"values\":[#val1...], \"query\":\"#query\"} OR \
                                       {\"table_name\":\"#tbl_name\",\"attrs\":[\"attr1\"...],\"values\":[\"#val1...],\"query\":\"#query\"}");
    ID_InputArgsInfo SQLUpdateMethodArgs[2] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]},
                                               {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "input", &InputDescs[1]}};
    ID_InputArgsInfo SQLUpdateOut = {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "count", &OutputDescs[2]};
    SQLOutParameters[2] = SQLUpdateOut;
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 2, SQLUpdateMethodArgs, 3, SQLOutParameters,
                               OpcUa_True, WSQLUpdateMethodId, "SQLUpdateW", ID_UASQLUpdateW,
                               OpcUa_True, ASQLUpdateMethodId, "SQLUpdateA", ID_UASQLUpdateA);
    
    // sqldisconnect method
    OpcUa_NodeId WSQLDisconnectMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLDISCONNECT);
    OpcUa_NodeId ASQLDisconnectMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLDISCONNECT);
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 0, NULL, 2, SQLStandardOutput,
                               OpcUa_True, WSQLDisconnectMethodId, "SQLDisconnectW", ID_UASQLDisconnectW,
                               OpcUa_True, ASQLDisconnectMethodId, "SQLDisconnectA", ID_UASQLDisconnectA);
    
    // sqlfetch method
    OpcUa_NodeId WSQLFetchMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLFETCH);
    OpcUa_NodeId ASQLFetchMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLFETCH);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "number of rows to be requested; 0 uses default setting");
    ID_InputArgsInfo SQLFetchMethodArgs[2] = { {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]},
                                               {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "numResults", &InputDescs[1]}};
    ID_InputArgsInfo SQLFetchOut = {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "result", &OutputDescs[2]};
    SQLOutParameters[2] = SQLFetchOut;
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 2, SQLFetchMethodArgs, 3, SQLOutParameters,
                               OpcUa_True, WSQLFetchMethodId, "SQLFetchW", ID_UASQLFetchW,
                               OpcUa_True, ASQLFetchMethodId, "SQLFetchA", ID_UASQLFetchA);
    
    // sqlprepare method
    OpcUa_NodeId WSQLPrepareMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLPREPARE);
    OpcUa_NodeId ASQLPrepareMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLPREPARE);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "statement to prepare");
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 2, "statement handle");
    ID_InputArgsInfo SQLPrepareMethodArgs[2] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]},
                                                {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "statement", &InputDescs[1]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 2, SQLPrepareMethodArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLPrepareMethodId, "SQLPrepareW", ID_UASQLPrepareW,
                               OpcUa_True, ASQLPrepareMethodId, "SQLPrepareA", ID_UASQLPrepareA);
    
    // sqlbind method
    OpcUa_NodeId WSQLBindMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLBIND);
    OpcUa_NodeId ASQLBindMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLBIND);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(InputDescs, 1, "Input JSON to be used in the format: {\"table_name\":\"#tbl_name\",\"attr\":\"#attr_name\",\"value\":#val,\"paramMarker\":#marker_1-index_based}");
    ID_InputArgsInfo SQLBindMethodArgs[2] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]},
                                             {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_String), "input", &InputDescs[1]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 2, SQLBindMethodArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLBindMethodId, "SQLBindW", ID_UASQLBindW,
                               OpcUa_True, ASQLBindMethodId, "SQLBindA", ID_UASQLBindA);
    
    // sqlexecute method
    OpcUa_NodeId WSQLExecutePreparedMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLEXECUTE);
    OpcUa_NodeId ASQLExecutePreparedMethodId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLEXECUTE);
    ID_ATTACH_DESCRIPTION_FOR_ARGUMENT(OutputDescs, 2, "number of affected rows");
    ID_InputArgsInfo SQLExecutePreparedMethodArgs[1] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]}};
    ID_InputArgsInfo SQLExecutePreparedOut = {OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "count", &OutputDescs[2]};
    SQLOutParameters[2] = SQLExecutePreparedOut;
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 1, SQLExecutePreparedMethodArgs, 3, SQLOutParameters,
                               OpcUa_True, WSQLExecutePreparedMethodId, "SQLExecuteW", ID_UASQLExecuteW,
                               OpcUa_True, ASQLExecutePreparedMethodId, "SQLExecuteA", ID_UASQLExecuteA);
    
    // sqlallochandle method
    OpcUa_NodeId WSQLAllocHandleId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLALLOCHANDLE);
    OpcUa_NodeId ASQLAllocHandleId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLALLOCHANDLE);
    ID_InputArgsInfo SQLAllocHandleArgs[1] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 1, SQLAllocHandleArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLAllocHandleId, "SQLAllocHandleW", ID_UASQLAllocHandleW,
                               OpcUa_True, ASQLAllocHandleId, "SQLAllocHandleA", ID_UASQLAllocHandleA);
    
    // sqlclosehandle method
    OpcUa_NodeId WSQLCloseHandleId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLCLOSE);
    OpcUa_NodeId ASQLCloseHandleId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLCLOSE);
    ID_InputArgsInfo SQLCloseHandleArgs[1] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 1, SQLCloseHandleArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLCloseHandleId, "SQLCloseHandleW", ID_UASQLCloseHandleW,
                               OpcUa_True, ASQLCloseHandleId, "SQLCloseHandleA", ID_UASQLCloseHandleA);
    
    // sqlresetparamshandle method
    OpcUa_NodeId WSQLResetParamsHandleId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLRESETPARAMS);
    OpcUa_NodeId ASQLResetParamsHandleId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLRESETPARAMS);
    ID_InputArgsInfo SQLResetParamsHandleArgs[1] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "Handle", &InputDescs[0]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 1, SQLResetParamsHandleArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLResetParamsHandleId, "SQLResetParamsW", ID_UASQLResetParamsHandleW,
                               OpcUa_True, ASQLResetParamsHandleId, "SQLResetParamsA", ID_UASQLResetParamsHandleA);
    
    // sqlfreehandle method
    OpcUa_NodeId WSQLFreeHandleId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_UNICODE_SQLFREEHANDLE);
    OpcUa_NodeId ASQLFreeHandleId = ID_OPC_IDNodeId(ID_OPCUA_DATABASE_ANSI_UTF8_SQLFREEHANDLE);
    ID_InputArgsInfo SQLFreeHandleArgs[1] = {{OpcUa_ValueRanks_Scalar, 0, ID_OPCNodeId(OpcUaId_UInt32), "handle", &InputDescs[0]}};
    ID_CreateWideMultiDBMethod(Handle, Indexer, &DatabaseObjectId, 1, SQLFreeHandleArgs, 2, SQLStandardOutput,
                               OpcUa_True, WSQLFreeHandleId, "SQLFreeHandleW", ID_UASQLFreeHandleW,
                               OpcUa_True, ASQLFreeHandleId, "SQLFreeHandleA", ID_UASQLFreeHandleA);
    
    ID_FREE_METHOD_LOCALIZED(NumAlloc);
}
#pragma warning(pop)