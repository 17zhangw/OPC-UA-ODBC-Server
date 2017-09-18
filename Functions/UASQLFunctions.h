/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef UASQLFUNCTIONS_H
#define UASQLFUNCTIONS_H

#include "../Definitions/id_opcua.h"

char * ID_ExtractErrorA(char *fn, SQLHANDLE handle, SQLSMALLINT type, OpcUa_Boolean ShouldFree);
wchar_t * ID_ExtractErrorW(wchar_t * fn, SQLHANDLE handle, SQLSMALLINT type, OpcUa_Boolean ShouldFree);

char * ID_ConvertNullTerminatedWideToMultiByte(wchar_t * multiByte, char ** errorMessage);
wchar_t * ID_ConvertNullTerminatedMultiByteToWide(char * multiByte, char ** errorMessage);

void ID_Base64Encode(const unsigned char* buffer, size_t length, char** b64text, int * b64length);
void ID_Base64Decode(char* b64message, unsigned char** buffer, size_t* length);

OpcUa_StatusCode ID_UASQLConnectA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLConnectW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLSetCommitA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLSetCommitW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLCommitA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLCommitW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLDisconnectA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLDisconnectW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLAllocHandleA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLAllocHandleW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLCloseHandleA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLCloseHandleW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLResetParamsHandleA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLResetParamsHandleW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLFreeHandleA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLFreeHandleW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLExecDirectA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLExecDirectW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLInsertA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLInsertW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLUpdateA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLUpdateW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLDeleteA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLDeleteW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLSelectA(OpcUa_NodeId * AuthToken, int NoOfInputArguments, OpcUa_Variant * InputArguments, int * NoOfOutputArguments, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLSelectW(OpcUa_NodeId * AuthToken, int NoOfInputArguments, OpcUa_Variant * InputArguments, int * NoOfOutputArguments, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLFetchA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLFetchW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLPrepareA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLPrepareW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLBindA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLBindW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

OpcUa_StatusCode ID_UASQLExecuteA(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);
OpcUa_StatusCode ID_UASQLExecuteW(OpcUa_NodeId * AuthToken, int NoInputArgs, OpcUa_Variant * InputArgs, int * NoOutputArgs, OpcUa_Variant ** OutputArgs, OpcUa_StatusCode * InputCodes);

#endif /* UASQLFUNCTIONS_H */
