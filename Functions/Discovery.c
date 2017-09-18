/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "ExposedFunctions.h"
#include "../AddressSpace/AddressSpace.h"
#include "../SessionManager/SecurityManager.h"

#define ENDPOINTURL_LARGE_PAYLOAD 1024
#define ENDPOINTURL_NUM_ILLEGAL 5

extern ID_SecurityHandle GlobalSecurityHandle;

void ID_FillServerApplicationDescription(OpcUa_ApplicationDescription * Servers) {
    Servers->ApplicationType = OpcUa_ApplicationType_Server;
    OpcUa_String_AttachCopy(&Servers->ApplicationUri, ID_SERVER_URI);
    OpcUa_String_AttachCopy(&Servers->ProductUri, ID_PRODUCT_URI);
    OpcUa_String_AttachCopy(&Servers->ApplicationName.Locale, ID_APPLICATION_LOCALE);
    OpcUa_String_AttachCopy(&Servers->ApplicationName.Text, ID_APPLICATION_NAME);
    
    Servers->NoOfDiscoveryUrls = 1;
    Servers->DiscoveryUrls = malloc(sizeof(OpcUa_String));
    OpcUa_String_Initialize(Servers->DiscoveryUrls);
    OpcUa_String_AttachCopy(Servers->DiscoveryUrls, ID_ENDPOINT_URL);
}

OpcUa_StatusCode ID_UA_FindServers(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                   const OpcUa_String* pEndpointUrl, OpcUa_Int32 nNoOfLocaleIds, const OpcUa_String* pLocaleIds,
                                   OpcUa_Int32 nNoOfServerUris, const OpcUa_String* pServerUris, OpcUa_ResponseHeader* pResponseHeader,
                                   OpcUa_Int32* pNoOfServers, OpcUa_ApplicationDescription** pServers) {
    OpcUa_ReferenceParameter(pEndpointUrl);
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_FindServers");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfLocaleIds, pLocaleIds);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfServerUris, pServerUris);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfServers);
    OpcUa_ReturnErrorIfArgumentNull(pServers);
    *pNoOfServers = 0;
    *pServers = NULL;
    
    // based on the test cases
    char * pEndpoint = OpcUa_String_GetRawString(pEndpointUrl);
    if (pEndpoint != NULL) {
        // check payload size (CTT Discovery Find Servers Self 006 1.02.335)
        if ((int)strlen(pEndpoint) >= ENDPOINTURL_LARGE_PAYLOAD) {
            *pNoOfServers = 0;
            goto Error;
        }
        
        // hostname restricted as per RFC952 and RFC1123
        // use second '/' or any '?' or any '&'... to detect a payload
        // valid URL syntax mandates [protocol]://
        if (strstr(pEndpoint, "opc.tcp://") == pEndpoint ||
            strstr(pEndpoint, "http://") == pEndpoint ||
            strstr(pEndpoint, "https://") == pEndpoint) {
            
            char * ptr = strstr(pEndpoint, "://") + 3; // shift to right after '/'
            char illegalCharacters[ENDPOINTURL_NUM_ILLEGAL] = {'/','?','&','+','\\'};
            for (int i = 0; i < ENDPOINTURL_NUM_ILLEGAL; i++) {
                if (strchr(ptr, illegalCharacters[i]) != NULL) {
                    *pNoOfServers = 0;
                    goto Error;
                }
            }
        }
    }
    
    // Part 4 1.03 Specifications
    // 5.4.2
    // the server shall always return a record that describes itself
    // however in some cases more than one record may be returned
    *pNoOfServers = 1;
    OpcUa_ApplicationDescription * Servers = malloc(sizeof(OpcUa_ApplicationDescription));
    OpcUa_ApplicationDescription_Initialize(Servers);
    ID_FillServerApplicationDescription(Servers);
    *pServers = Servers;
    
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}

// copy endpoint information into the descriptions
void ID_FillServerEndpoints(OpcUa_EndpointDescription * Endpoint) {
    struct ID_SecurityPolicy ID_SecurityPolicies[ID_NUM_SECURITY_POLICIES] = ID_SECURITY_POLICIES;
    for (int i = 0; i < ID_NUM_ENDPOINTS; i++) {
        OpcUa_EndpointDescription_Initialize(&Endpoint[i]);
        OpcUa_String_AttachCopy(&Endpoint[i].EndpointUrl, ID_ENDPOINT_URL);

        OpcUa_ApplicationDescription_Initialize(&Endpoint[i].Server);
        ID_FillServerApplicationDescription(&Endpoint[i].Server);

        OpcUa_String_AttachCopy(&Endpoint[i].TransportProfileUri, OpcUa_TransportProfile_UaTcp);
        ID_OpcUa_ByteString_CopyTo(&GlobalSecurityHandle->ServerCertificate, &Endpoint[i].ServerCertificate);

        Endpoint[i].SecurityLevel = (OpcUa_Byte)(ID_NUM_ENDPOINTS - i);
        Endpoint[i].SecurityMode = ID_SecurityPolicies[i].MessageMode;
        OpcUa_String_AttachCopy(&Endpoint[i].SecurityPolicyUri, ID_SecurityPolicies[i].SecurityPolicy);
        Endpoint[i].NoOfUserIdentityTokens = 1;

        Endpoint[i].UserIdentityTokens = malloc(sizeof(OpcUa_UserTokenPolicy));
        OpcUa_UserTokenPolicy_Initialize(Endpoint[i].UserIdentityTokens);
        OpcUa_String_AttachCopy(&Endpoint[i].UserIdentityTokens->SecurityPolicyUri, ID_SecurityPolicies[i].SecurityPolicy);
        OpcUa_String_AttachCopy(&Endpoint[i].UserIdentityTokens->PolicyId, ID_SecurityPolicies[i].PolicyId);
        Endpoint[i].UserIdentityTokens->TokenType = ID_SecurityPolicies[i].TokenType;
    }
}

OpcUa_StatusCode ID_UA_GetEndpoints(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                    const OpcUa_String* pEndpointUrl, OpcUa_Int32 nNoOfLocaleIds, const OpcUa_String* pLocaleIds,
                                    OpcUa_Int32 nNoOfProfileUris, const OpcUa_String* pProfileUris, OpcUa_ResponseHeader* pResponseHeader,
                                    OpcUa_Int32* pNoOfEndpoints, OpcUa_EndpointDescription** pEndpoints) {
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_GetEndpoints");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArgumentNull(pEndpointUrl);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfLocaleIds, pLocaleIds);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfProfileUris, pProfileUris);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfEndpoints);
    OpcUa_ReturnErrorIfArgumentNull(pEndpoints);
    *pNoOfEndpoints = 0;
    *pEndpoints = NULL;
    
    if (nNoOfProfileUris > 0) {
        OpcUa_Boolean TransportFound = OpcUa_False;
        for (int i = 0; i < nNoOfProfileUris; i++) {
            char * profileUri = OpcUa_String_GetRawString(&pProfileUris[i]);
            if (strcmp(profileUri, OpcUa_TransportProfile_UaTcp) == 0) {
                TransportFound = OpcUa_True;
                break;
            }
        } 
        
        if (TransportFound == OpcUa_False) {
            uStatus = OpcUa_Good;
            goto Error;
        }
    }
    
    *pNoOfEndpoints = ID_NUM_ENDPOINTS;
    OpcUa_EndpointDescription * Endpoint = malloc(sizeof(OpcUa_EndpointDescription) * ID_NUM_ENDPOINTS);
    memset(Endpoint, 0x00, sizeof(OpcUa_EndpointDescription) * ID_NUM_ENDPOINTS);
    ID_FillServerEndpoints(Endpoint);
    *pEndpoints = Endpoint;
    
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}