/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef CONTINUER_H
#define CONTINUER_H

#include "../Definitions/id_opcua.h"

#ifdef __cplusplus
#extern "C" {
#endif

typedef struct _ID_UAContinuationPoint_ {
    OpcUa_NodeId SessionId;
    OpcUa_UInt32 Offset;
    OpcUa_UInt32 BatchSize;
    OpcUa_BrowseDescription Browse;
    OpcUa_ByteString Identifier;
} ID_UAContinuationPoint;

typedef void * ID_ContinuerHandle;

// Creates a Continuer
void ID_Continuer_Create(ID_ContinuerHandle * Handle);

// Add continuation point with BrowseRequest and offset
ID_UAContinuationPoint * ID_Continuer_AddContinuationPoint(ID_ContinuerHandle Handle, OpcUa_BrowseDescription * BrowseRequest, OpcUa_Int32 Offset);

// Get continuation point with identifier
ID_UAContinuationPoint * ID_Continuer_GetContinuationPoint(ID_ContinuerHandle Handle, OpcUa_ByteString * Identifier);

// Remove continuation point with Identifier
void ID_Continuer_RemoveContinuationPoint(ID_ContinuerHandle Handle, OpcUa_ByteString * Identifier);

// Remove continuation point with session identifier
void ID_Continuer_RemoveContinuationPointWithSession(ID_ContinuerHandle Handle, OpcUa_NodeId * Identifier);

// Destroy Continuer
void ID_Continuer_Destroy(ID_ContinuerHandle * Handle);

#ifdef __cplusplus
}
#endif

#endif /* CONTINUER_H */
