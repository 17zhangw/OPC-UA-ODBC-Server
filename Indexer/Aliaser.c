/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include <string.h>
#include <time.h>
#include "Aliaser.h"
#include "../AddressSpace/AddressSpaceHelper.h"

typedef struct {
    OpcUa_UInt32 capacity;
    OpcUa_UInt32 count;
    OpcUa_NodeId ** originalNodes;
    OpcUa_NodeId ** aliasNodes;
    OpcUa_Mutex AliasMutex;
} ID_AliaserInternal;

void ID_Aliaser_ExpandNodes(ID_AliaserInternal * Internal) {
    Internal->capacity = Internal->capacity + 10;
    Internal->originalNodes = realloc(Internal->originalNodes, sizeof(OpcUa_NodeId*) * Internal->capacity);
    Internal->aliasNodes = realloc(Internal->aliasNodes, sizeof(OpcUa_NodeId*) * Internal->capacity);
}

void ID_Aliaser_Create(ID_AliaserHandle * AliaserHandle) {
    ID_AliaserInternal * Internal = malloc(sizeof(ID_AliaserInternal));
    memset(Internal, 0, sizeof(ID_AliaserInternal));
    Internal->count = 0;
    Internal->capacity = 0;
    OPCUA_P_MUTEX_CREATE(&Internal->AliasMutex);
    *AliaserHandle = Internal;
}

OpcUa_NodeId * ID_Aliaser_AddAlias(ID_AliaserHandle AliaserHandle, OpcUa_NodeId * Original) {
    ID_AliaserInternal * Internal = AliaserHandle;
    for (OpcUa_UInt32 i = 0; i < Internal->count; i++) {
        if (ID_AddressSpace_AreIdsEqual(Original, Internal->originalNodes[i]) == OpcUa_True) {
            return Internal->aliasNodes[i];
        }
    }
    
    OPCUA_P_MUTEX_LOCK(Internal->AliasMutex);
    if (Internal->count >= Internal->capacity) {
        ID_Aliaser_ExpandNodes(Internal);
    }
 
    OpcUa_NodeId * Alias = malloc(sizeof(OpcUa_NodeId));
    OpcUa_NodeId_Initialize(Alias);
    
    Internal->originalNodes[Internal->count] = malloc(sizeof(OpcUa_NodeId));
    OpcUa_NodeId_Initialize(Internal->originalNodes[Internal->count]);
    Internal->aliasNodes[Internal->count] = Alias;
    
    ID_OpcUa_NodeId_CopyTo(Original, Internal->originalNodes[Internal->count]);
    Alias->IdentifierType = OpcUa_IdType_Numeric;
    Alias->NamespaceIndex = ID_NAMESPACE_INDEX_ALIAS;
    Alias->Identifier.Numeric = Internal->count + 1;
    Internal->count++;

    OPCUA_P_MUTEX_UNLOCK(Internal->AliasMutex);
    return Alias;
}

void ID_Aliaser_RemoveFromIndex(ID_AliaserInternal * Internal, OpcUa_UInt32 Index) {
    OPCUA_P_MUTEX_LOCK(Internal->AliasMutex);

    OpcUa_NodeId_Clear(Internal->aliasNodes[Index]);
    OpcUa_NodeId_Clear(Internal->originalNodes[Index]);
    free(Internal->aliasNodes[Index]);
    free(Internal->originalNodes[Index]);
            
    for (OpcUa_UInt32 j = Index + 1; j < Internal->count; j++) {
        Internal->aliasNodes[j - 1] = Internal->aliasNodes[j];
        Internal->originalNodes[j - 1] = Internal->originalNodes[j];
    }
            
    Internal->aliasNodes[Internal->count - 1] = NULL;
    Internal->originalNodes[Internal->count - 1] = NULL;
    Internal->count--;

    OPCUA_P_MUTEX_UNLOCK(Internal->AliasMutex);
}

void ID_Aliaser_RemoveAliasWithAlias(ID_AliaserHandle AliaserHandle, OpcUa_NodeId * Alias) {
    ID_AliaserInternal * Internal = AliaserHandle;
    for (OpcUa_UInt32 i = 0; i < Internal->count; i++) {
        if (ID_AddressSpace_AreIdsEqual(Internal->aliasNodes[i], Alias) == OpcUa_True) {
            ID_Aliaser_RemoveFromIndex(Internal, i);
            return;
        }
    }
}

void ID_Aliaser_RemoveAliasWithOriginal(ID_AliaserHandle AliaserHandle, OpcUa_NodeId * Original) {
    ID_AliaserInternal * Internal = AliaserHandle;
    for (OpcUa_UInt32 i = 0; i < Internal->count; i++) {
        if (ID_AddressSpace_AreIdsEqual(Internal->originalNodes[i], Original) == OpcUa_True) {
            ID_Aliaser_RemoveFromIndex(Internal, i);
            return;
        }
    }
}

void ID_Aliaser_Purge(ID_AliaserHandle AliaserHandle) {
    ID_AliaserInternal * Internal = AliaserHandle;
    OPCUA_P_MUTEX_LOCK(Internal->AliasMutex); // lock to synchronize purge

    for (OpcUa_UInt32 i = 0; i < Internal->count; i++) {
        OpcUa_NodeId_Clear(Internal->originalNodes[i]);
        OpcUa_NodeId_Clear(Internal->aliasNodes[i]);
        
        free(Internal->originalNodes[i]);
        free(Internal->aliasNodes[i]);
    }
    
    free(Internal->originalNodes);
    free(Internal->aliasNodes);
    
    Internal->originalNodes = NULL;
    Internal->aliasNodes = NULL;
    Internal->count = 0;
    Internal->capacity = 0;
    OPCUA_P_MUTEX_UNLOCK(Internal->AliasMutex);
}

void ID_Aliaser_Delete(ID_AliaserHandle * AliaserHandle) {
    ID_Aliaser_Purge(*AliaserHandle);
    OPCUA_P_MUTEX_DELETE(&((ID_AliaserInternal*)*AliaserHandle)->AliasMutex);
    free(*AliaserHandle);
    *AliaserHandle = NULL;
}

OpcUa_NodeId * ID_Aliaser_GetOriginal(ID_AliaserHandle AliaserHandle, OpcUa_NodeId * Alias) {
    ID_AliaserInternal * Internal = AliaserHandle;
    for (OpcUa_UInt32 i = 0; i < Internal->count; i++) {
        OpcUa_NodeId * Node = Internal->aliasNodes[i];
        if (ID_AddressSpace_AreIdsEqual(Node, Alias) == OpcUa_True) {
            return Internal->originalNodes[i];
        }
    }
    
    return NULL;
}