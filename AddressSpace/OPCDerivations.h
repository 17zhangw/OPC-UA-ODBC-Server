/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef OPCDERIVATIONS_H
#define OPCDERIVATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <opcua.h>

// Copy a bytestring from a_pSource to a_pDestination
OpcUa_StatusCode ID_OpcUa_ByteString_CopyTo(const OpcUa_ByteString* a_pSource, OpcUa_ByteString* a_pDestination);

// Copy a nodeid from a_pSource to a_pDestination
OpcUa_StatusCode ID_OpcUa_NodeId_CopyTo(const OpcUa_NodeId* a_pSource, OpcUa_NodeId* a_pDestination);

// Print to buffer
OpcUa_StatusCode ID_UASQLPrintToBufferA(char * buffer, int freeSpace, const char * format, ...);
OpcUa_StatusCode ID_UASQLPrintToBufferWithVarArgA(char * buffer, int freeSpace, const char * format, va_list argPtr);

// Copy to a buffer
OpcUa_StatusCode ID_UASQLCopyToBufferA(char * buffer, int freeSpace, char * copy);

#ifdef __cplusplus
}
#endif

#endif /* OPCDERIVATIONS_H */

