/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef INITIALIZE_H
#define INITIALIZE_H

#include "../Definitions/id_opcua.h"
#include "../Indexer/Indexer.h"
#include "../AddressSpace/AddressSpace.h"
#include "../AddressSpace/AddressSpaceHelper.h"

#ifdef __cplusplus
extern "C" {
#endif

    void ID_Initialize_AllStandardObjectTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_StandardObjectTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_StandardEventTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_AllStandardVariableTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_AllStandardMethods(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_AllStandardViews(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_AllStandardReferenceTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_AllStandardDataTypes(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_AllStandardObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    
    void ID_Initialize_OrganizationalObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_ServerObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_ModellingRuleObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    
    void ID_Initialize_CustomObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    void ID_Initialize_TestObjects(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer);
    
    void ID_Initialize_ReserveIndexes(ID_IndexerHandle Indexer);
    void ID_Initialize_OPCAddressSpace(ID_AddressSpaceHandle Handle, ID_IndexerHandle Indexer, OpcUa_Boolean IsTest);
#ifdef __cplusplus
}
#endif

#endif /* INITIALIZE_H */