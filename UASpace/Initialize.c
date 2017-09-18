/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "Initialize.h"

void ID_Initialize_OPCAddressSpace(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, OpcUa_Boolean IsTest) {
    ID_Initialize_AllStandardVariableTypes(Handle, Indexer);
    ID_Initialize_AllStandardReferenceTypes(Handle, Indexer);
    ID_Initialize_AllStandardMethods(Handle, Indexer);
    
    // need to initialize DataTypeEncodingType
    ID_ObjectTypeNode * BaseObjectTypeNode = NULL;
    OpcUa_NodeId BaseObjectType = ID_OPCNodeId(OpcUaId_BaseObjectType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, NULL, &BaseObjectTypeNode, 0, "BaseObjectType", OpcUa_False, BaseObjectType, 0, NULL);

    ID_ObjectTypeNode * DataTypeEncodingTypeNode;
    OpcUa_NodeId DataTypeEncodingTypeId = ID_OPCNodeId(OpcUaId_DataTypeEncodingType);
    ID_AddressSpaceHelper_CreateObjectType(Handle, Indexer, BaseObjectTypeNode, &DataTypeEncodingTypeNode, 0, "DataTypeEncodingType", OpcUa_False, DataTypeEncodingTypeId, 0, NULL);
    
    // continue with standard initialization
    ID_Initialize_AllStandardDataTypes(Handle, Indexer);
    ID_Initialize_AllStandardViews(Handle, Indexer);
    
    ID_Initialize_StandardObjectTypes(Handle, Indexer);
    ID_Initialize_StandardEventTypes(Handle, Indexer);
    
    ID_Initialize_ModellingRuleObjects(Handle, Indexer);
    ID_Initialize_ServerObjects(Handle, Indexer);
    ID_Initialize_CustomObjects(Handle, Indexer);
    if (IsTest) { ID_Initialize_TestObjects(Handle, Indexer); }

    ID_Initialize_OrganizationalObjects(Handle, Indexer);
    ID_AddressSpace_AnalyzeConflicts(Handle);
}

void ID_Initialize_ReserveIndexes(ID_IndexerHandle Indexer) {
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_VIEW);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_OBJECT);

    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLCONNECT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLSETCOMMIT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLCOMMIT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLDISCONNECT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLALLOCHANDLE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLCLOSE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLRESETPARAMS);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLFREEHANDLE);

    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLCONNECT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLSETCOMMIT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLCOMMIT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLDISCONNECT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLALLOCHANDLE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLCLOSE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLRESETPARAMS);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLFREEHANDLE);

    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLEXECDIRECT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLSELECT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLINSERT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLUPDATE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLDELETE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLFETCH);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLPREPARE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLBIND);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_UNICODE_SQLEXECUTE);

    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLEXECDIRECT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLSELECT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLINSERT);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLUPDATE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLDELETE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLFETCH);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLPREPARE);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLBIND);
    ID_Indexer_AddToReserved(Indexer, ID_OPCUA_DATABASE_ANSI_UTF8_SQLEXECUTE);

}