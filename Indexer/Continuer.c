/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "Continuer.h"
#include "Aliaser.h"
#include "../AddressSpace/AddressSpaceHelper.h"
#include <opcua_mutex.h>

extern ID_AliaserHandle GlobalAliaserHandle;

typedef struct _ID_ContinuerInternal_ {
    OpcUa_UInt32 NoOfElements;
    OpcUa_UInt32 Capacity;
    ID_UAContinuationPoint ** ContinuationPoints;
    OpcUa_Mutex ContinuationMutex;
    OpcUa_UInt32 InternalCounter;
} ID_ContinuerInternal;

void ID_Continuer_Create(ID_ContinuerHandle * Handle) {
    ID_ContinuerInternal * Internal = malloc(sizeof(ID_ContinuerInternal));
    Internal->NoOfElements = 0;
    Internal->Capacity = 0;
    Internal->ContinuationPoints = NULL;
    OPCUA_P_MUTEX_CREATE(&Internal->ContinuationMutex);
    *Handle = Internal;
}

ID_UAContinuationPoint * ID_Continuer_AddContinuationPoint(ID_ContinuerHandle Handle, OpcUa_BrowseDescription * BrowseRequest, OpcUa_Int32 Offset) {
    // Allocate continuation point
    ID_UAContinuationPoint * Point = malloc(sizeof(ID_UAContinuationPoint));
    memset(Point, 0x00, sizeof(ID_UAContinuationPoint));
    Point->Offset = Offset;
    Point->BatchSize = Offset;
    
    // Copy over relevant information
    OpcUa_BrowseDescription_Initialize(&Point->Browse);
    Point->Browse.BrowseDirection = BrowseRequest->BrowseDirection;
    Point->Browse.IncludeSubtypes = BrowseRequest->IncludeSubtypes;
    Point->Browse.NodeClassMask = BrowseRequest->NodeClassMask;
    Point->Browse.ResultMask = BrowseRequest->ResultMask;
    
    OpcUa_NodeId_Initialize(&Point->Browse.NodeId);
    OpcUa_NodeId * BrowseNode = &BrowseRequest->NodeId;
    if (ID_Aliaser_GetOriginal(GlobalAliaserHandle, BrowseNode) != NULL) {
        BrowseNode = ID_Aliaser_GetOriginal(GlobalAliaserHandle, BrowseNode);
    }
    
    ID_OpcUa_NodeId_CopyTo(BrowseNode, &Point->Browse.NodeId);
    OpcUa_NodeId_Initialize(&BrowseRequest->ReferenceTypeId);
    ID_OpcUa_NodeId_CopyTo(&BrowseRequest->ReferenceTypeId, &Point->Browse.ReferenceTypeId);
    OpcUa_NodeId_Initialize(&Point->SessionId);
    
    // lock here to minimzie time holding the mutex
    ID_ContinuerInternal * Internal = Handle;
    OPCUA_P_MUTEX_LOCK(Internal->ContinuationMutex);

    // identifier is the internalcounter + 1 in string form
    int divisions = 1;
    int index = Internal->InternalCounter + 1;
    while (index / 10 != 0) {
        divisions++;
        index = index / 10;
    }
    char * buf = malloc(sizeof(char) * (divisions + 1));
    memset(buf, 0x00, sizeof(char) * (divisions + 1));
    snprintf(buf, divisions + 1, "%d", index);

    Point->Identifier.Length = divisions + 1;
    Point->Identifier.Data = malloc(sizeof(OpcUa_Byte) * Point->Identifier.Length);
    for (int i = 0; i < divisions + 1; i++) {
        Point->Identifier.Data[i] = buf[i];
    }

    free(buf);

    // shift the pointers
    if (Internal->NoOfElements >= Internal->Capacity) {
        Internal->Capacity += 10;
        Internal->ContinuationPoints = realloc(Internal->ContinuationPoints, sizeof(ID_UAContinuationPoint*) * Internal->Capacity);
    }

    Internal->ContinuationPoints[Internal->NoOfElements] = Point;
    Internal->NoOfElements++;    
    Internal->InternalCounter++;
    OPCUA_P_MUTEX_UNLOCK(Internal->ContinuationMutex);
    return Point;
}

ID_UAContinuationPoint * ID_Continuer_GetContinuationPoint(ID_ContinuerHandle Handle, OpcUa_ByteString * Identifier) {
    ID_ContinuerInternal * Internal = Handle;
    for (OpcUa_UInt32 i = 0; i < Internal->NoOfElements; i++) {
        ID_UAContinuationPoint * Point = Internal->ContinuationPoints[i];
        if (Point->Identifier.Length == Identifier->Length) {
            OpcUa_Boolean IsMatch = OpcUa_True;
            for (int j = 0; j < Point->Identifier.Length; j++) {
                if (Point->Identifier.Data[j] != Identifier->Data[j]) {
                    IsMatch = OpcUa_False;
                    break;
                }
            }
            
            if (IsMatch == OpcUa_True) {
                return Internal->ContinuationPoints[i];
            }
        }
    }

	return NULL;
}

void ID_Continuer_RemoveContinuationPoint(ID_ContinuerHandle Handle, OpcUa_ByteString * Identifier) {
    ID_ContinuerInternal * Internal = Handle;
    OPCUA_P_MUTEX_LOCK(Internal->ContinuationMutex); // lock for synchronization
    
    for (OpcUa_UInt32 i = 0; i < Internal->NoOfElements; i++) {
        ID_UAContinuationPoint * Point = Internal->ContinuationPoints[i];
        if (Point->Identifier.Length == Identifier->Length) {
            OpcUa_Boolean IsMatch = OpcUa_True;
            for (int j = 0; j < Point->Identifier.Length; j++) {
                if (Point->Identifier.Data[j] != Identifier->Data[j]) {
                    IsMatch = OpcUa_False;
                    break;
                }
            }
            
            if (IsMatch == OpcUa_True) {
                // delete the point
                OpcUa_ByteString_Clear(&Point->Identifier);
                OpcUa_BrowseDescription_Clear(&Point->Browse);
                OpcUa_NodeId_Clear(&Point->SessionId);
                free(Point);
                
                // shift the pointer
                for (OpcUa_UInt32 j = i + 1; j < Internal->NoOfElements; j++) {
                    Internal->ContinuationPoints[j - 1] = Internal->ContinuationPoints[j];
                }
                Internal->ContinuationPoints[Internal->NoOfElements] = NULL;
                Internal->NoOfElements--;

                OPCUA_P_MUTEX_UNLOCK(Internal->ContinuationMutex);
                return;
            }
        }
    }
}

void ID_Continuer_RemoveContinuationPointWithSession(ID_ContinuerHandle Handle, OpcUa_NodeId * Identifier) {
    ID_ContinuerInternal * Internal = Handle;
    for (int i = Internal->NoOfElements - 1; i >= 0; i--) {
        if (ID_AddressSpace_AreIdsEqual(Identifier, &Internal->ContinuationPoints[i]->SessionId) == OpcUa_True) {
            ID_Continuer_RemoveContinuationPoint(Handle, &Internal->ContinuationPoints[i]->Identifier);
        }
    }
}

void ID_Continuer_Destroy(ID_ContinuerHandle * Handle) {
    ID_ContinuerInternal * Internal = *Handle;
    for (OpcUa_UInt32 i = 0; i < Internal->NoOfElements; i++) {
        ID_UAContinuationPoint * Point = Internal->ContinuationPoints[i];
        OpcUa_ByteString_Clear(&Point->Identifier);
        OpcUa_BrowseDescription_Clear(&Point->Browse);
        free(Point);
    }
    
    OPCUA_P_MUTEX_DELETE(&Internal->ContinuationMutex);
    free(Internal->ContinuationPoints);
    free(Internal);
    *Handle = NULL;
}