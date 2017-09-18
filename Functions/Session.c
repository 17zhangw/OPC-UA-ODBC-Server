/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "ExposedFunctions.h"
#include "../Indexer/Continuer.h"
#include "../SessionManager/SecurityManager.h"
#include "../SessionManager/SessionManager.h"
#include "../AddressSpace/AddressSpaceCopier.h"

extern ID_SessionManagerHandle GlobalSessionManagerHandle;
extern ID_SecurityHandle GlobalSecurityHandle;
extern ID_ContinuerHandle GlobalContinuerHandle;

OpcUa_Boolean ID_IsByteStringsEqual(OpcUa_ByteString * B1, OpcUa_ByteString * B2) {
    if (B1->Length != B2->Length) {
        return OpcUa_False;
    }
    
    for (int i = 0; i < B1->Length; i++) {
        if (B1->Data[i] != B2->Data[i]) {
            return OpcUa_False;
        }
    }
    
    return OpcUa_True;
}

OpcUa_StatusCode ID_UA_CreateSession(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                     const OpcUa_ApplicationDescription* pClientDescription, const OpcUa_String* pServerUri,
                                     const OpcUa_String* pEndpointUrl, const OpcUa_String* pSessionName, const OpcUa_ByteString* pClientNonce,
                                     const OpcUa_ByteString* pClientCertificate, OpcUa_Double nRequestedSessionTimeout,
                                     OpcUa_UInt32 nMaxResponseMessageSize, OpcUa_ResponseHeader* pResponseHeader, OpcUa_NodeId* pSessionId,
                                  OpcUa_NodeId* pAuthenticationToken, OpcUa_Double* pRevisedSessionTimeout, OpcUa_ByteString* pServerNonce,
                                  OpcUa_ByteString* pServerCertificate, OpcUa_Int32* pNoOfServerEndpoints, 
                                  OpcUa_EndpointDescription** pServerEndpoints, OpcUa_Int32* pNoOfServerSoftwareCertificates,
                                  OpcUa_SignedSoftwareCertificate** pServerSoftwareCertificates, OpcUa_SignatureData* pServerSignature,
                                  OpcUa_UInt32* pMaxRequestMessageSize) {
    OpcUa_ReferenceParameter(nMaxResponseMessageSize);
    OpcUa_ReferenceParameter(nRequestedSessionTimeout);
    ID_Session * UserSession = NULL;
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_CreateSession");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArgumentNull(pClientDescription);
    OpcUa_ReturnErrorIfArgumentNull(pServerUri);
    OpcUa_ReturnErrorIfArgumentNull(pEndpointUrl);
    OpcUa_ReturnErrorIfArgumentNull(pSessionName);
    OpcUa_ReturnErrorIfArgumentNull(pClientNonce);
    OpcUa_ReturnErrorIfArgumentNull(pClientCertificate);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pSessionId);
    OpcUa_ReturnErrorIfArgumentNull(pAuthenticationToken);
    OpcUa_ReturnErrorIfArgumentNull(pRevisedSessionTimeout);
    OpcUa_ReturnErrorIfArgumentNull(pServerNonce);
    OpcUa_ReturnErrorIfArgumentNull(pServerCertificate);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfServerEndpoints);
    OpcUa_ReturnErrorIfArgumentNull(pServerEndpoints);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfServerSoftwareCertificates);
    OpcUa_ReturnErrorIfArgumentNull(pServerSoftwareCertificates);
    OpcUa_ReturnErrorIfArgumentNull(pServerSignature);
    OpcUa_ReturnErrorIfArgumentNull(pMaxRequestMessageSize);
    
    if (pRequestHeader->Timestamp.dwHighDateTime == 0 && pRequestHeader->Timestamp.dwLowDateTime == 0) {
        uStatus = OpcUa_BadInvalidTimestamp;
        goto Error;
    }
    
    OpcUa_UInt32 SecureChannelId = 0;
    OpcUa_Boolean IsSecurityUsed = OpcUa_False;
    OpcUa_Endpoint_SecurityPolicyConfiguration SecurityConfiguration;
    OpcUa_String_Initialize(&SecurityConfiguration.sSecurityPolicy);
    SecurityConfiguration.pbsClientCertificate = NULL;
    struct ID_SecurityPolicy ID_SecurityPolicies[ID_NUM_SECURITY_POLICIES] = ID_SECURITY_POLICIES;

    OpcUa_Endpoint_GetMessageSecureChannelSecurityPolicy(hEndpoint, hContext, &SecurityConfiguration); 
    OpcUa_Endpoint_GetMessageSecureChannelId(hEndpoint, hContext, &SecureChannelId);
    
    UserSession = ID_SessionManager_AddSession(GlobalSessionManagerHandle, SecureChannelId, (OpcUa_String*)pSessionName, (OpcUa_ApplicationDescription*)pClientDescription, (OpcUa_ByteString*)pClientNonce, (OpcUa_ByteString*)pClientCertificate);
    ID_OpcUa_NodeId_CopyTo(&UserSession->SessionId, pSessionId);
    ID_OpcUa_NodeId_CopyTo(&UserSession->AuthenticationToken, pAuthenticationToken);
    UserSession->SessionTimeout = ID_SESSION_TIMEOUT;
    *pRevisedSessionTimeout = UserSession->SessionTimeout;
    if (strcmp(OpcUa_SecurityPolicy_None, OpcUa_String_GetRawString(&SecurityConfiguration.sSecurityPolicy)) != 0) {
        if (strcmp(ID_SecurityPolicies[ID_USERNAME_SECURITY_POLICY_INDEX].SecurityPolicy, OpcUa_String_GetRawString(&SecurityConfiguration.sSecurityPolicy)) == 0) {
            IsSecurityUsed = OpcUa_True;
        } else {
            uStatus = OpcUa_BadSecurityPolicyRejected;
            goto Error;
        }
    }
    
    if (IsSecurityUsed == OpcUa_True) {
        size_t NumBytes;
        OpcUa_Key ServerPublicKey;
        OpcUa_Byte * SignatureMessage = NULL;
        OpcUa_CryptoProvider * CProvider;
        OpcUa_UInt32 KeyLengthBits = 0;
        OpcUa_ByteString Data;
        
        OpcUa_ByteString * OriginalCertificate = ID_SessionManager_GetCertificateAssociatedWithId(GlobalSessionManagerHandle, SecureChannelId);
        if (ID_IsByteStringsEqual(OriginalCertificate, (OpcUa_ByteString*)pClientCertificate) == OpcUa_False) {
            uStatus = OpcUa_BadCertificateInvalid;
            goto Error;
        }
        
        if (pClientNonce->Length < 32) {
            uStatus = OpcUa_BadNonceInvalid;
            goto Error;
        }
        
        // validate certificate
        if (ID_UASecurityManager_ValidateCertificate(GlobalSecurityHandle, (OpcUa_ByteString*)pClientCertificate) != 0) {
            uStatus = OpcUa_BadCertificateInvalid;
            goto Error;
        }
        
        // generate server nonce
        UserSession->ServerNonce.Length = 32;
        UserSession->ServerNonce.Data = malloc(sizeof(OpcUa_Byte) * 32);
        srand((unsigned int)time(NULL));
        for (int i = 0; i < UserSession->ServerNonce.Length; i++) {
            UserSession->ServerNonce.Data[i] = rand() % UCHAR_MAX;
        }
        
        OpcUa_ByteString_Initialize(pServerNonce);
        OpcUa_ByteString_Initialize(pServerCertificate);
        ID_OpcUa_ByteString_CopyTo(&UserSession->ServerNonce, pServerNonce);
        ID_OpcUa_ByteString_CopyTo(&GlobalSecurityHandle->ServerCertificate, pServerCertificate);
        
        // create a signature
        NumBytes = pClientCertificate->Length + pClientNonce->Length;
        Data.Length = (OpcUa_UInt32)NumBytes;
        SignatureMessage = malloc(sizeof(OpcUa_Byte) * NumBytes);
        memset(SignatureMessage, 0x00, sizeof(OpcUa_Byte) * NumBytes);
        memcpy(SignatureMessage, pClientCertificate->Data, sizeof(OpcUa_Byte) * pClientCertificate->Length);
        memcpy(SignatureMessage + sizeof(OpcUa_Byte) * pClientCertificate->Length, pClientNonce->Data, sizeof(OpcUa_Byte) * pClientNonce->Length);
        Data.Data = SignatureMessage;
        
        CProvider = malloc(sizeof(OpcUa_CryptoProvider));
        OpcUa_Key_Initialize(&ServerPublicKey);
        OpcUa_CryptoProvider_Create(OpcUa_String_GetRawString(&SecurityConfiguration.sSecurityPolicy), CProvider);
        CProvider->GetPublicKeyFromCert(CProvider, &GlobalSecurityHandle->ServerCertificate, NULL, &ServerPublicKey);
        if (OPCUA_CRYPTO_KEY_ISNOHANDLE((&ServerPublicKey))) {
            ServerPublicKey.Key.Data = malloc(sizeof(OpcUa_Byte) * ServerPublicKey.Key.Length);
            CProvider->GetPublicKeyFromCert(CProvider, &GlobalSecurityHandle->ServerCertificate, NULL, &ServerPublicKey);
        }

        OpcUa_SignatureData_Initialize(pServerSignature);
        OpcUa_String_AttachCopy(&pServerSignature->Algorithm, ID_SecurityPolicies[ID_USERNAME_SECURITY_POLICY_INDEX].AsymmetricSignatureAlgorithm);
        
        CProvider->GetAsymmetricKeyLength(CProvider, ServerPublicKey, &KeyLengthBits);
        pServerSignature->Signature.Length = KeyLengthBits/8;
        pServerSignature->Signature.Length = CProvider->MaximumAsymmetricKeyLength;
        pServerSignature->Signature.Data = malloc(sizeof(OpcUa_Byte) * pServerSignature->Signature.Length);        
        CProvider->AsymmetricSign(CProvider, Data, &GlobalSecurityHandle->ServerPrivateKey, &pServerSignature->Signature);
        
        OpcUa_Key_Clear(&ServerPublicKey);
        OpcUa_CryptoProvider_Delete(CProvider);
        free(CProvider);
        free(SignatureMessage);
    }
    
    /* endpoints */
    *pNoOfServerEndpoints = 2;
    OpcUa_EndpointDescription * Endpoint = malloc(sizeof(OpcUa_EndpointDescription) * 2);
    memset(Endpoint, 0x00, sizeof(OpcUa_EndpointDescription) * 2);
    ID_FillServerEndpoints(Endpoint);
    *pServerEndpoints = Endpoint;
    
    *pNoOfServerSoftwareCertificates = 0;
    *pServerSoftwareCertificates = NULL;
    *pMaxRequestMessageSize = 0;
    UserSession->IsBeingMutated = OpcUa_False;
    
    Error: ;
    if (uStatus != OpcUa_Good && UserSession != NULL) {
        ID_SessionManager_RemoveSession(GlobalSessionManagerHandle, &UserSession->AuthenticationToken);
    }
    
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}

OpcUa_StatusCode ID_VerifyClientSignature(ID_Session * UserSession, OpcUa_SignatureData * pClientSignature) {
    OpcUa_StatusCode uStatus = OpcUa_Good;
    OpcUa_ByteString SignedData;
    OpcUa_CryptoProvider * CProvider = NULL;
    OpcUa_Key ClientPublicKey;
    struct ID_SecurityPolicy ID_SecurityPolicies[ID_NUM_SECURITY_POLICIES] = ID_SECURITY_POLICIES;

    OpcUa_ByteString_Initialize(&SignedData);
    CProvider = malloc(sizeof(OpcUa_CryptoProvider));
    OpcUa_CryptoProvider_Create(ID_SecurityPolicies[ID_USERNAME_SECURITY_POLICY_INDEX].SecurityPolicy, CProvider);
    OpcUa_Key_Initialize(&ClientPublicKey);

    SignedData.Length = GlobalSecurityHandle->ServerCertificate.Length + UserSession->ServerNonce.Length;
    SignedData.Data = malloc(sizeof(OpcUa_Byte) * SignedData.Length);
    memset(SignedData.Data, 0x00, sizeof(OpcUa_Byte) * SignedData.Length);
    memcpy(SignedData.Data, GlobalSecurityHandle->ServerCertificate.Data, sizeof(OpcUa_Byte) * GlobalSecurityHandle->ServerCertificate.Length);
    memcpy(SignedData.Data + sizeof(OpcUa_Byte) * GlobalSecurityHandle->ServerCertificate.Length, UserSession->ServerNonce.Data, sizeof(OpcUa_Byte) * UserSession->ServerNonce.Length);

    CProvider->GetPublicKeyFromCert(CProvider, &UserSession->ClientCertificate, NULL, &ClientPublicKey);
    if (OPCUA_CRYPTO_KEY_ISNOHANDLE((&ClientPublicKey))) {
        ClientPublicKey.Key.Data = malloc(sizeof(OpcUa_Byte) * ClientPublicKey.Key.Length);
        CProvider->GetPublicKeyFromCert(CProvider, &UserSession->ClientCertificate, NULL, &ClientPublicKey);
    }

    uStatus = CProvider->AsymmetricVerify(CProvider, SignedData, &ClientPublicKey, (OpcUa_ByteString*)&pClientSignature->Signature);

    OpcUa_Key_Clear(&ClientPublicKey);
    OpcUa_CryptoProvider_Delete(CProvider);
    OpcUa_ByteString_Clear(&SignedData);
    free(CProvider);
    return uStatus;
}

OpcUa_StatusCode ID_ValidateUserNamePassword(ID_Session * UserSession, OpcUa_UserNameIdentityToken * UserNameIdentity) {
    OpcUa_CryptoProvider * cProvider = NULL;
    OpcUa_StatusCode uStatus = OpcUa_Good;
    if (UserNameIdentity == NULL) {
        uStatus = OpcUa_BadIdentityTokenInvalid;
        goto Error;
    }
    
    if (OpcUa_String_GetRawString(&UserNameIdentity->PolicyId) == NULL ||
        OpcUa_String_GetRawString(&UserNameIdentity->UserName) == NULL ||
        OpcUa_String_GetRawString(&UserNameIdentity->EncryptionAlgorithm) == NULL) {
        uStatus = OpcUa_BadIdentityTokenInvalid;
        goto Error;
    }
    
    struct ID_SecurityPolicy ID_SecurityPolicies[ID_NUM_SECURITY_POLICIES] = ID_SECURITY_POLICIES;
    if (strcmp(OpcUa_String_GetRawString(&UserNameIdentity->PolicyId), ID_SecurityPolicies[ID_USERNAME_SECURITY_POLICY_INDEX].PolicyId) != 0 ||
        strcmp(OpcUa_String_GetRawString(&UserNameIdentity->UserName), ID_USER_USERNAME) != 0 ||
        strcmp(OpcUa_String_GetRawString(&UserNameIdentity->EncryptionAlgorithm), ID_SecurityPolicies[ID_USERNAME_SECURITY_POLICY_INDEX].AsymmetricEncryptionAlgorithm) != 0) {
        uStatus = OpcUa_BadIdentityTokenInvalid;
        goto Error;
    }        

#define IS_MANTIS_ISSUE_3863_SUCCESSFUL 1
#if !IS_MANTIS_ISSUE_3863_SUCCESSFUL
    OpcUa_Byte * PasswordBytes;
    OpcUa_UInt32 PasswordLength;
    OpcUa_CryptoProvider * CProvider = NULL;
    OpcUa_Byte EncryptedLengthBytes[4];
    OpcUa_UInt32 EncryptedLength = 0;
    OpcUa_Byte * PlainText = NULL;
    OpcUa_UInt32 PlainTextLength = 0;

    PasswordBytes = UserNameIdentity->Password.Data;
    PasswordLength = UserNameIdentity->Password.Length;

    CProvider = malloc(sizeof(OpcUa_CryptoProvider));
    OpcUa_CryptoProvider_Create(ID_SecurityPolicies[ID_USERNAME_SECURITY_POLICY_INDEX].SecurityPolicy, CProvider);

    memcpy(EncryptedLengthBytes, UserNameIdentity->Password.Data, sizeof(OpcUa_Byte) * 4);
    EncryptedLength = *(int*)EncryptedLengthBytes;
    CProvider->AsymmetricDecrypt(CProvider, UserNameIdentity->Password.Data + sizeof(OpcUa_Byte) * 4, EncryptedLength, &GlobalSecurityHandle->ServerPrivateKey, NULL, &PlainTextLength);        
    PlainText = malloc(sizeof(OpcUa_Byte) * PlainTextLength);
    CProvider->AsymmetricDecrypt(CProvider, UserNameIdentity->Password.Data + sizeof(OpcUa_Byte) * 4, EncryptedLength, &GlobalSecurityHandle->ServerPrivateKey, PlainText, &PlainTextLength);

    if (PlainTextLength < 32 || PlainTextLength - 32 != kAddressSpace_PasswordLength) {
        uStatus = OpcUa_BadIdentityTokenInvalid;
        free(PlainText);
        OpcUa_CryptoProvider_Delete(CProvider);
        free(CProvider);
        goto Error;
    }

    OpcUa_CryptoProvider_Delete(CProvider);
    free(CProvider);
    CProvider = NULL;

    for (int i = 32; i > 0; i--) {
        if (UserSession->ServerNonce.Data[32 - i] != PlainText[PlainTextLength - i]) {
            free(PlainText);
            uStatus = OpcUa_BadIdentityTokenRejected;
            goto Error;
        }
    }

    for (OpcUa_UInt32 i = 0; i < ID_USER_PASSWORD_LENGTH; i++) {
        if (PlainText[i] != ID_USER_PASSWORD[i]) {
            free(PlainText);
            uStatus = OpcUa_BadIdentityTokenRejected;
            goto Error;
        }
    }

    free(PlainText);
#else
    cProvider = malloc(sizeof(OpcUa_CryptoProvider));
    memset(cProvider, 0x00, sizeof(OpcUa_CryptoProvider));
    OpcUa_CryptoProvider_Create(ID_SecurityPolicies[ID_USERNAME_SECURITY_POLICY_INDEX].SecurityPolicy, cProvider);
    
    OpcUa_Byte * plainText = NULL;
    OpcUa_UInt32 plainTextLength = 0;
    uStatus = cProvider->AsymmetricDecrypt(cProvider, UserNameIdentity->Password.Data, UserNameIdentity->Password.Length, &GlobalSecurityHandle->ServerPrivateKey, NULL, &plainTextLength);
    if (OpcUa_IsBad(uStatus)) {
        uStatus = OpcUa_BadIdentityTokenRejected;
        goto Error;
    }
    
    plainText = malloc(sizeof(OpcUa_Byte) * plainTextLength);
    memset(plainText, 0x00, plainTextLength);
    uStatus = cProvider->AsymmetricDecrypt(cProvider, UserNameIdentity->Password.Data, UserNameIdentity->Password.Length, &GlobalSecurityHandle->ServerPrivateKey, plainText, &plainTextLength);
    if (OpcUa_IsBad(uStatus)) {
        free(plainText);
        uStatus = OpcUa_BadIdentityTokenRejected;
        goto Error;
    }
    
    OpcUa_Byte * pData = plainText + 4;
    OpcUa_Int32 DataLength = plainTextLength - 4;
    if (DataLength < UserSession->ServerNonce.Length || DataLength != UserSession->ServerNonce.Length + (OpcUa_Int32)ID_USER_PASSWORD_LENGTH) {
        free(plainText);
        uStatus = OpcUa_BadIdentityTokenRejected;
        goto Error;
    }
    
    // check server nonce
    for (int i = 0; i < UserSession->ServerNonce.Length; i++) {
        if (UserSession->ServerNonce.Data[i] != pData[DataLength - UserSession->ServerNonce.Length + i]) {
            free(plainText);
            uStatus = OpcUa_BadIdentityTokenRejected;
            goto Error;
        }
    }
    
    // check server password
    for (OpcUa_UInt32 i = 0; i < ID_USER_PASSWORD_LENGTH; i++) {
        if (ID_USER_PASSWORD[i] != pData[i]) {
            free(plainText);
            uStatus = OpcUa_BadIdentityTokenRejected;
            goto Error;
        }
    }
    
    free(plainText);
    OpcUa_CryptoProvider_Delete(cProvider);
    free(cProvider);
    cProvider = NULL;
    
#endif
    
    Error: 
    if (cProvider != NULL) {
        OpcUa_CryptoProvider_Delete(cProvider);
        free(cProvider);
    }
    
    return uStatus;
}

OpcUa_StatusCode ID_UA_ActivateSession(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                       const OpcUa_SignatureData* pClientSignature, OpcUa_Int32 nNoOfClientSoftwareCertificates,
                                       const OpcUa_SignedSoftwareCertificate* pClientSoftwareCertificates, OpcUa_Int32 nNoOfLocaleIds,
                                       const OpcUa_String* pLocaleIds, const OpcUa_ExtensionObject* pUserIdentityToken,
                                       const OpcUa_SignatureData* pUserTokenSignature, OpcUa_ResponseHeader* pResponseHeader,
                                       OpcUa_ByteString* pServerNonce, OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults,
                                       OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_ActivateSession");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArgumentNull(pClientSignature);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfClientSoftwareCertificates, pClientSoftwareCertificates);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfLocaleIds, pLocaleIds);
    OpcUa_ReturnErrorIfArgumentNull(pUserIdentityToken)
    OpcUa_ReturnErrorIfArgumentNull(pUserTokenSignature)
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pServerNonce);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfResults);
    OpcUa_ReturnErrorIfArgumentNull(pResults);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfDiagnosticInfos);
    OpcUa_ReturnErrorIfArgumentNull(pDiagnosticInfos);
    
    ID_Session * UserSession = NULL;
    OpcUa_Boolean IsSecurity = OpcUa_False;
    OpcUa_Endpoint_SecurityPolicyConfiguration EndpointSecurity;
    OpcUa_String_Initialize(&EndpointSecurity.sSecurityPolicy);
    EndpointSecurity.pbsClientCertificate = NULL;
    OpcUa_Endpoint_GetMessageSecureChannelSecurityPolicy(hEndpoint, hContext, &EndpointSecurity);
    struct ID_SecurityPolicy ID_SecurityPolicies[ID_NUM_SECURITY_POLICIES] = ID_SECURITY_POLICIES;
    if (strcmp(OpcUa_SecurityPolicy_None, OpcUa_String_GetRawString(&EndpointSecurity.sSecurityPolicy)) != 0) {
        if (strcmp(ID_SecurityPolicies[ID_USERNAME_SECURITY_POLICY_INDEX].SecurityPolicy, OpcUa_String_GetRawString(&EndpointSecurity.sSecurityPolicy)) == 0) {
            IsSecurity = OpcUa_True;
        } else {
            uStatus = OpcUa_BadSecurityPolicyRejected;
            goto Error;
        }
    }
    
    UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken);
    if (UserSession == NULL) {
        uStatus = OpcUa_BadSessionIdInvalid; // session not exists
        goto Error;
    }
    
    if (UserSession->IsActivated) { // tampering with existing session
        OpcUa_UInt32 SecureChannelId = 0;
        OpcUa_Endpoint_GetMessageSecureChannelId(hEndpoint, hContext, &SecureChannelId);
        if (UserSession->SecureChannelId != SecureChannelId) {
            // consider transferring session
            OpcUa_ByteString * Cert = ID_SessionManager_GetCertificateAssociatedWithId(GlobalSessionManagerHandle, SecureChannelId);
            if (UserSession->ClientCertificate.Length != Cert->Length) {
                uStatus = OpcUa_BadUserAccessDenied;
                goto Error;
            }
            
            for (int i = 0; i < UserSession->ClientCertificate.Length; i++) {
                if (UserSession->ClientCertificate.Data[i] != Cert->Data[i]) {
                    uStatus = OpcUa_BadUserAccessDenied;
                    goto Error;
                }
            }
            
            if (!ID_AreTypesEqual((OpcUa_EncodeableType*)UserSession->IdentityToken.Body.EncodeableObject.Type, pUserIdentityToken->Body.EncodeableObject.Type)) {
                uStatus = OpcUa_BadUserAccessDenied;
                goto Error;
            }
            
            if (IsSecurity) {
                // because only single user, just need to run through identitytoken validation
                OpcUa_UserNameIdentityToken * Supplied = pUserIdentityToken->Body.EncodeableObject.Object;
                uStatus = ID_ValidateUserNamePassword(UserSession, Supplied);
                if (OpcUa_IsBad(uStatus)) {
                    goto Error;
                }
                
                srand((unsigned int)time(NULL));
                for (int i = 0; i < UserSession->ServerNonce.Length; i++) {
                    UserSession->ServerNonce.Data[i] = rand() % UCHAR_MAX;
                }
                OpcUa_ByteString_Initialize(pServerNonce);
                ID_OpcUa_ByteString_CopyTo(&UserSession->ServerNonce, pServerNonce);
            } else {
                OpcUa_AnonymousIdentityToken * Anonymous = pUserIdentityToken->Body.EncodeableObject.Object;
                OpcUa_AnonymousIdentityToken * Existing = pUserIdentityToken->Body.EncodeableObject.Object;
                char * ProvidedPolicy = OpcUa_String_GetRawString(&Anonymous->PolicyId);
                char * ExistingPolicy = OpcUa_String_GetRawString(&Existing->PolicyId);
                if (ProvidedPolicy == NULL || ExistingPolicy == NULL || strcmp(ProvidedPolicy, ExistingPolicy) != 0) {
                    uStatus = OpcUa_BadIdentityTokenRejected;
                    goto Error;
                }
            }
            
            UserSession->SecureChannelId = SecureChannelId; // successful transfer
            UserSession->IdledTime = 0;
            goto Error;
        } else {
            uStatus = OpcUa_BadIdentityChangeNotSupported;
            goto Error;
        }
    }
    
    if (pRequestHeader->Timestamp.dwHighDateTime == 0 && pRequestHeader->Timestamp.dwLowDateTime == 0) {
        uStatus = OpcUa_BadInvalidTimestamp;
        goto Error;
    }
    
    ID_SessionManager_IsSessionValid(GlobalSessionManagerHandle, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken, hEndpoint, hContext, OpcUa_True);
    if (IsSecurity == OpcUa_True) {
        if (OpcUa_IsBad(ID_VerifyClientSignature(UserSession, (OpcUa_SignatureData*)pClientSignature))) { 
            uStatus = OpcUa_BadSignatureInvalid;
            goto Error;
        }
    }
    
    // if security and username identity token
    if (IsSecurity && strcmp(pUserIdentityToken->Body.EncodeableObject.Type->TypeName, OpcUa_UserNameIdentityToken_EncodeableType.TypeName) == 0) {
        OpcUa_UserNameIdentityToken * UserNameIdentity = pUserIdentityToken->Body.EncodeableObject.Object;
        uStatus = ID_ValidateUserNamePassword(UserSession, UserNameIdentity);
        if (OpcUa_IsBad(uStatus)) {
            goto Error;
        }
        
        srand((unsigned int)time(NULL));
        for (int i = 0; i < UserSession->ServerNonce.Length; i++) {
            UserSession->ServerNonce.Data[i] = rand() % UCHAR_MAX;
        }
        OpcUa_ByteString_Initialize(pServerNonce);
        ID_OpcUa_ByteString_CopyTo(&UserSession->ServerNonce, pServerNonce);
    } else if (!IsSecurity && strcmp(pUserIdentityToken->Body.EncodeableObject.Type->TypeName, OpcUa_AnonymousIdentityToken_EncodeableType.TypeName) == 0) {
        // ensure None and Anonymous
        OpcUa_AnonymousIdentityToken * AnonymousIdentity = pUserIdentityToken->Body.EncodeableObject.Object;
        if (strcmp(OpcUa_String_GetRawString(&AnonymousIdentity->PolicyId), ID_SecurityPolicies[ID_ANONYMOUS_SECURITY_POLICY_INDEX].PolicyId) != 0) {
            uStatus = OpcUa_BadIdentityTokenInvalid;
            goto Error;
        }
    } else {
        uStatus = OpcUa_BadIdentityTokenRejected;
        goto Error;
    }
    
    UserSession->IsActivated = OpcUa_True;
    
    /* copy extension object for identity */
    UserSession->IdentityToken.TypeId.ServerIndex = pUserIdentityToken->TypeId.ServerIndex;
    UserSession->IdentityToken.Encoding = pUserIdentityToken->Encoding;
    UserSession->IdentityToken.BodySize = pUserIdentityToken->BodySize;
    UserSession->IdentityToken.Body.EncodeableObject.Type = pUserIdentityToken->Body.EncodeableObject.Type;
    ID_OpcUa_NodeId_CopyTo(&pUserIdentityToken->TypeId.NodeId, &UserSession->IdentityToken.TypeId.NodeId);
    char * namespaceUri = OpcUa_String_GetRawString(&pUserIdentityToken->TypeId.NamespaceUri);
    if (namespaceUri != NULL) {
        OpcUa_String_AttachCopy(&UserSession->IdentityToken.TypeId.NamespaceUri, namespaceUri);
    }
    
    UserSession->IdentityToken.Body.EncodeableObject.Object = malloc(pUserIdentityToken->Body.EncodeableObject.Type->AllocationSize);
    pUserIdentityToken->Body.EncodeableObject.Type->Initialize(UserSession->IdentityToken.Body.EncodeableObject.Object);
    ID_AddressSpace_CopyExtensionObjectBody((OpcUa_ExtensionObject*)pUserIdentityToken, &UserSession->IdentityToken);
    
    Error:;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}
    
OpcUa_StatusCode ID_UA_CloseSession(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                    OpcUa_Boolean bDeleteSubscriptions, OpcUa_ResponseHeader* pResponseHeader) {
    OpcUa_ReferenceParameter(bDeleteSubscriptions);
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_CloseSession");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    
    ID_Session * UserSession = ID_SessionManager_GetSession(GlobalSessionManagerHandle, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken);
    if (UserSession == NULL) {
        uStatus = OpcUa_BadSessionIdInvalid; // session not exists
        goto Error;
    }
    
    if (ID_SessionManager_IsSessionValid(GlobalSessionManagerHandle, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken, hEndpoint, hContext, OpcUa_False) == OpcUa_False) {
        uStatus = OpcUa_BadSessionNotActivated;
        goto Error;
    }
    
    if (pRequestHeader->Timestamp.dwHighDateTime == 0 && pRequestHeader->Timestamp.dwLowDateTime == 0) {
        uStatus = OpcUa_BadInvalidTimestamp;
        goto Error;
    }
    
    ID_Continuer_RemoveContinuationPointWithSession(GlobalContinuerHandle, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken);
    ID_SessionManager_RemoveSession(GlobalSessionManagerHandle, (OpcUa_NodeId *)&pRequestHeader->AuthenticationToken);
    
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}
