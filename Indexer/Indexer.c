/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "Indexer.h"

typedef struct {
    OpcUa_UInt32 nextIndex;
    
    OpcUa_UInt32 stashedCount;
    OpcUa_UInt32 stashedCapacity;
    OpcUa_UInt32 * stashedIndexes;
    OpcUa_Mutex stashedMutex;
    
    OpcUa_UInt32 reservedCount;
    OpcUa_UInt32 reservedCapacity;
    OpcUa_UInt32 * reservedIndexes;
    OpcUa_Mutex reservedMutex;
} ID_IndexerInternal;

void ID_Indexer_Create(ID_IndexerHandle * Indexer) {
    ID_IndexerInternal * Internal = malloc(sizeof(ID_IndexerInternal));
    Internal->nextIndex = 0;
    Internal->stashedCount = 0;
    Internal->stashedCapacity = 0;
    Internal->stashedIndexes = NULL;
    Internal->reservedCount = 0;
    Internal->reservedCapacity = 0;
    Internal->reservedIndexes = NULL;

    OPCUA_P_MUTEX_CREATE(&Internal->stashedMutex);
    OPCUA_P_MUTEX_CREATE(&Internal->reservedMutex);
    *Indexer = Internal;
}

void ID_Indexer_RecyleIndex(ID_IndexerHandle Indexer, OpcUa_UInt32 Index) {
    // add Index to stashed
    ID_IndexerInternal * Internal = Indexer;
    OPCUA_P_MUTEX_LOCK(Internal->stashedMutex);
    if (Internal->stashedCount >= Internal->stashedCapacity) {
        Internal->stashedCapacity++;
        Internal->stashedIndexes = realloc(Internal->stashedIndexes, sizeof(OpcUa_UInt32) * Internal->stashedCapacity);
    }
    
    Internal->stashedIndexes[Internal->stashedCount] = Index;
    Internal->stashedCount++;
    OPCUA_P_MUTEX_UNLOCK(Internal->stashedMutex);
}

void ID_Indexer_AddToReserved(ID_IndexerHandle Indexer, OpcUa_UInt32 Index) {
    // add Index to reserved
    ID_IndexerInternal * Internal = Indexer;
    OPCUA_P_MUTEX_LOCK(Internal->reservedMutex);
    if (Internal->reservedCount >= Internal->reservedCapacity) {
        Internal->reservedCapacity++;
        Internal->reservedIndexes = realloc(Internal->reservedIndexes, sizeof(OpcUa_UInt32) * Internal->reservedCapacity);
    }
    
    Internal->reservedIndexes[Internal->reservedCount] = Index;
    Internal->reservedCount++;
    OPCUA_P_MUTEX_UNLOCK(Internal->reservedMutex);
}

OpcUa_UInt32 ID_Indexer_GetNextIndex(ID_IndexerHandle Indexer) {
    ID_IndexerInternal * Internal = Indexer;
    if (Internal->stashedCount == 0) {
        // retrieve a value that has not been reserved already
        OpcUa_Boolean IsMatch = OpcUa_True;
        while (IsMatch == OpcUa_True) {
            Internal->nextIndex++;
            IsMatch = OpcUa_False;
            for (OpcUa_UInt32 i = 0; i < Internal->reservedCount; i++) {
                if (Internal->reservedIndexes[i] == Internal->nextIndex) {
                    IsMatch = OpcUa_True;
                    break;
                }
            }
        }
        
        return Internal->nextIndex;
    }
    
    // retrieve a stashed value
    OPCUA_P_MUTEX_LOCK(Internal->stashedMutex);
    OpcUa_UInt32 val = Internal->stashedIndexes[Internal->stashedCount - 1];
    Internal->stashedCount--;
    Internal->stashedCapacity--;
    Internal->stashedIndexes = realloc(Internal->stashedIndexes, sizeof(OpcUa_Int32) * Internal->stashedCapacity);
    OPCUA_P_MUTEX_UNLOCK(Internal->stashedMutex);
    return val;
}

void ID_Indexer_Delete(ID_IndexerHandle * Indexer) {
    ID_IndexerInternal * Internal = *Indexer;
    free(Internal->reservedIndexes);
    free(Internal->stashedIndexes);
    OPCUA_P_MUTEX_DELETE(&Internal->reservedMutex);
    OPCUA_P_MUTEX_DELETE(&Internal->stashedMutex);
    free(Internal);
    *Indexer = NULL;
}