/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ALIASER_H
#define ALIASER_H

#include "../Definitions/id_opcua.h"

#ifdef __cplusplus
#extern "C" {
#endif

typedef void * ID_AliaserHandle;

// Create Aliaser object
void ID_Aliaser_Create(ID_AliaserHandle * AliaserHandle);

// Adds an alias for Original nodeid
OpcUa_NodeId * ID_Aliaser_AddAlias(ID_AliaserHandle AliaserHandle, OpcUa_NodeId * Original);

// Removes the Alias from storage
void ID_Aliaser_RemoveAliasWithAlias(ID_AliaserHandle AliaserHandle, OpcUa_NodeId * Alias);

// Removes alias using the Original requested nodeid
void ID_Aliaser_RemoveAliasWithOriginal(ID_AliaserHandle AliaserHandle, OpcUa_NodeId * Original);

// Remove all items from storage
void ID_Aliaser_Purge(ID_AliaserHandle AliaserHandle);

// Destroy aliaser
void ID_Aliaser_Delete(ID_AliaserHandle * AliaserHandle);

// Get the Original node for a given Alias
OpcUa_NodeId * ID_Aliaser_GetOriginal(ID_AliaserHandle AliaserHandle, OpcUa_NodeId * Alias);

#ifdef __cplusplus
}
#endif

#endif /* ALIASER_H */
