/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "ExposedFunctions.h"
#include "../Indexer/Aliaser.h"
#include "../SessionManager/SessionManager.h"
#include "../AddressSpace/AddressSpace.h"
#include "../Indexer/Continuer.h"

extern ID_ContinuerHandle GlobalContinuerHandle;
extern ID_AliaserHandle GlobalAliaserHandle;
extern ID_AddressSpaceHandle GlobalAddressSpaceHandle;
extern ID_SessionManagerHandle GlobalSessionManagerHandle;

OpcUa_StatusCode ID_UA_RegisterNodes(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                     OpcUa_Int32 nNoOfNodesToRegister, const OpcUa_NodeId* pNodesToRegister, 
                                     OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfRegisteredNodeIds,
                                     OpcUa_NodeId** pRegisteredNodeIds) {
    
	OpcUa_NodeId * AliasNodes = NULL;
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_RegisterNodes");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfNodesToRegister, pNodesToRegister);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(pNoOfRegisteredNodeIds, pRegisteredNodeIds);
    *pNoOfRegisteredNodeIds = 0;
	*pRegisteredNodeIds = NULL;
    
    ID_CHECK_SESSION();
    
    if (nNoOfNodesToRegister <= 0) {
        uStatus = OpcUa_BadNothingToDo;
        goto Error;
    }
    
    *pNoOfRegisteredNodeIds = nNoOfNodesToRegister;
    AliasNodes = malloc(sizeof(OpcUa_NodeId) * nNoOfNodesToRegister);
	memset(AliasNodes, 0x00, sizeof(OpcUa_NodeId) * nNoOfNodesToRegister);
    for (int i = 0; i < nNoOfNodesToRegister; i++) {  
        OpcUa_NodeId_Initialize(&AliasNodes[i]);
        if (ID_AddressSpace_GetNode(GlobalAddressSpaceHandle, (OpcUa_NodeId*)&pNodesToRegister[i])) {
            OpcUa_NodeId * AliasNode = ID_Aliaser_AddAlias(GlobalAliaserHandle, (OpcUa_NodeId*)&pNodesToRegister[i]);
            if (AliasNode == NULL) {
                uStatus = OpcUa_BadNodeIdInvalid;
                goto Error;
            }
        
            ID_OpcUa_NodeId_CopyTo(AliasNode, &AliasNodes[i]);
        } else {
            ID_OpcUa_NodeId_CopyTo((OpcUa_NodeId*)&pNodesToRegister[i], &AliasNodes[i]);
        }
    }
    
    *pRegisteredNodeIds = AliasNodes;
    
    Error: ;
    if (OpcUa_IsNotGood(uStatus)) {
		if (AliasNodes != NULL) {
			for (int i = 0; i < nNoOfNodesToRegister; i++) {
				OpcUa_NodeId_Clear(&AliasNodes[i]);
			}
		}
        
        free(AliasNodes);
        *pNoOfRegisteredNodeIds = 0;
    }
    
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}
    
OpcUa_StatusCode ID_UA_UnregisterNodes(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                       OpcUa_Int32 nNoOfNodesToUnregister, const OpcUa_NodeId* pNodesToUnregister,
                                       OpcUa_ResponseHeader* pResponseHeader) {
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_UnregisterNodes");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfNodesToUnregister, pNodesToUnregister);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    
    ID_CHECK_SESSION();
    
    if (nNoOfNodesToUnregister == 0) {
        uStatus = OpcUa_BadNothingToDo;
        goto Error;
    }
    
    for (int i = 0; i < nNoOfNodesToUnregister; i++) {
        ID_Aliaser_RemoveAliasWithAlias(GlobalAliaserHandle, (OpcUa_NodeId*)&pNodesToUnregister[i]);
    }
    
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}