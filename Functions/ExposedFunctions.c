/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "ExposedFunctions.h"
#include <opcua_core.h>

void ID_UA_InitializeResponseHeader(OpcUa_ResponseHeader * ResponseHeader, const OpcUa_RequestHeader * RequestHeader, OpcUa_StatusCode Status) {
    OpcUa_ResponseHeader_Initialize(ResponseHeader);
    ResponseHeader->RequestHandle = RequestHeader->RequestHandle;
    ResponseHeader->Timestamp = OpcUa_DateTime_UtcNow();
    if (RequestHeader->ReturnDiagnostics != 0x00000000) {
        ResponseHeader->ServiceDiagnostics.SymbolicId = -1;
        ResponseHeader->ServiceDiagnostics.NamespaceUri = -1;
        ResponseHeader->ServiceDiagnostics.LocalizedText = -1;
        ResponseHeader->ServiceDiagnostics.Locale = -1;
    }
    
    ResponseHeader->NoOfStringTable = 0;
    ResponseHeader->StringTable = NULL;
    ResponseHeader->ServiceResult = Status;
}