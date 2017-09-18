/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ADDRESSSPACECOPIER_H
#define ADDRESSSPACECOPIER_H

#include "AddressSpace.h"

#ifdef __cplusplus
#extern "C" {
#endif

// Checks if two OpcUa_EncodeableType are equal as the Stack copies types around
OpcUa_Boolean ID_AreTypesEqual(OpcUa_EncodeableType * Type1, OpcUa_EncodeableType * Type2);

// Copy the ExtensionbodY from Source to Target to ensure complete deep copying
OpcUa_StatusCode ID_AddressSpace_CopyExtensionObjectBody(OpcUa_ExtensionObject * Source, OpcUa_ExtensionObject * Target);

#ifdef __cplusplus
}
#endif

#endif /* ADDRESSSPACECOPIER_H */
