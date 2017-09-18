/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "OPCDerivations.h"
#include <opcua_guid.h>

/*============================================================================
* OpcUa_ByteString_CopyTo
*===========================================================================*/
OpcUa_StatusCode ID_OpcUa_ByteString_CopyTo(const OpcUa_ByteString* a_pSource, OpcUa_ByteString* a_pDestination){
    OpcUa_InitializeStatus(OpcUa_Module_ProxyStub, "OpcUa_ByteString_CopyTo");
    OpcUa_ReturnErrorIfArgumentNull(a_pSource);
    OpcUa_ReturnErrorIfArgumentNull(a_pDestination);

    OpcUa_ByteString_Clear(a_pDestination);
    a_pDestination->Length = a_pSource->Length;
    if (a_pDestination->Length > 0) {
        a_pDestination->Data = (OpcUa_Byte*)OpcUa_Alloc(a_pDestination->Length);
        OpcUa_GotoErrorIfAllocFailed(a_pDestination->Data);
        OpcUa_MemCpy(a_pDestination->Data, a_pDestination->Length, a_pSource->Data, a_pSource->Length);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;
    OpcUa_ByteString_Clear(a_pDestination);
    OpcUa_FinishErrorHandling;
}

OpcUa_StatusCode ID_OpcUa_NodeId_CopyTo(const OpcUa_NodeId* a_pSource, OpcUa_NodeId* a_pDestination) {
    OpcUa_InitializeStatus(OpcUa_Module_ProxyStub, "OpcUa_NodeId_CopyTo");
    OpcUa_ReturnErrorIfArgumentNull(a_pSource);
    OpcUa_ReturnErrorIfArgumentNull(a_pDestination);
    OpcUa_NodeId_Clear(a_pDestination);
    a_pDestination->IdentifierType = a_pSource->IdentifierType;
    a_pDestination->NamespaceIndex = a_pSource->NamespaceIndex;

    switch (a_pSource->IdentifierType) {
        case OpcUa_IdentifierType_Numeric: {
            a_pDestination->Identifier.Numeric = a_pSource->Identifier.Numeric;
            break;
        }
        case OpcUa_IdentifierType_String: {
            OpcUa_String_StrnCpy(&a_pDestination->Identifier.String, &a_pSource->Identifier.String, OPCUA_STRING_LENDONTCARE);
            break;
        }
        case OpcUa_IdentifierType_Opaque: {
            a_pDestination->Identifier.ByteString.Length = a_pSource->Identifier.ByteString.Length;
            if (a_pDestination->Identifier.ByteString.Length > 0) {
                a_pDestination->Identifier.ByteString.Data = (OpcUa_Byte*)OpcUa_Alloc(a_pSource->Identifier.ByteString.Length);
                OpcUa_GotoErrorIfAllocFailed(a_pDestination->Identifier.ByteString.Data);
                OpcUa_MemCpy(a_pDestination->Identifier.ByteString.Data, a_pDestination->Identifier.ByteString.Length, a_pSource->Identifier.ByteString.Data, a_pSource->Identifier.ByteString.Length);
            } else {
                a_pDestination->Identifier.ByteString.Data = OpcUa_Null;
            }
            break;
        } case OpcUa_IdentifierType_Guid: {
            if (a_pSource->Identifier.Guid != OpcUa_Null) {
                a_pDestination->Identifier.Guid = (OpcUa_Guid*)OpcUa_Alloc(sizeof(OpcUa_Guid));
                OpcUa_GotoErrorIfAllocFailed(a_pDestination->Identifier.Guid);
                OpcUa_Guid_Copy(a_pDestination->Identifier.Guid, a_pSource->Identifier.Guid);
            } else {
                a_pDestination->Identifier.Guid = OpcUa_Null;
            }
            break;
        }
        default: {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
        }
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling
    OpcUa_NodeId_Clear(a_pDestination);
    OpcUa_FinishErrorHandling;
}

OpcUa_StatusCode ID_UASQLPrintToBufferA(char * buffer, int freeSpace, const char * format, ...) {
	va_list argumentList;
	va_start(argumentList, format);
    OpcUa_StatusCode uStatus = ID_UASQLPrintToBufferWithVarArgA(buffer, freeSpace, format, argumentList);
	va_end(argumentList);
	return uStatus;
}

OpcUa_StatusCode ID_UASQLCopyToBufferA(char * buffer, int freeSpace, char * copy) {
	OpcUa_StatusCode retVal = OpcUa_Good;
#ifdef _WIN32
	if ((size_t)freeSpace < strlen(copy) + 1) {
		retVal = OpcUa_Bad;
	}
	else {
		// cJSON.c has some sloppy memory buffer size code
		// note that freeSpace does not actually correspond directly to the 'freeSpace' of the buffer
		errno_t r = strcpy_s(buffer, freeSpace, copy);
		if (r != 0) {
			retVal = OpcUa_Bad;
		}
	}
#elif defined(__linux__)
	char * p = strcpy(buffer, copy);
	if (p == NULL) { retVal = OpcUa_Bad; }
#endif

	return retVal;
}

OpcUa_StatusCode ID_UASQLPrintToBufferWithVarArgA(char * buffer, int freeSpace, const char * format, va_list argPtr) {
    int numWritten = 0;

#ifdef _WIN32
    numWritten = vsprintf_s(buffer, freeSpace, format, argPtr);
#elif defined(__linux__)
    numWritten = vsprintf(buffer, format, argPtr);
#endif

    if (numWritten <= 0) {
        return OpcUa_Bad;
    }

    return OpcUa_Good;
}