/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "SecurityManager.h"
#include <opcua_pki.h>

void ID_UASecurityManager_Create(ID_SecurityHandle * Manager) {
    OpcUa_Int iValidationCode = 0;
    OpcUa_Handle CertificateStore = NULL;
    ID_SecurityHandle Handle = malloc(sizeof(ID_UASecurityManager));
    memset(Handle, 0x00, sizeof(ID_UASecurityManager));
    
    // Configure the store PKI
    Handle->StoreConfig.PkiType = OpcUa_OpenSSL_PKI;
    Handle->StoreConfig.Flags = OPCUA_P_PKI_OPENSSL_ADD_UNTRUSTED_LIST_TO_ROOT_CERTIFICATES | OPCUA_P_PKI_OPENSSL_REQUIRE_CHAIN_CERTIFICATE_IN_TRUST_LIST;
    Handle->StoreConfig.CertificateTrustListLocation = ID_TRUST_CERTIFICATE_PATH;
    Handle->StoreConfig.CertificateRevocationListLocation = ID_REVOKED_CERTIFICATE_PATH;
    Handle->StoreConfig.CertificateUntrustedListLocation = ID_UNTRUSTED_CERTIFICATE_PATH;
    OpcUa_PKIProvider_Create(&Handle->StoreConfig, &Handle->PkiProvider);
    
    // Load and validate server certiifcate
    Handle->PkiProvider.OpenCertificateStore(&Handle->PkiProvider, &CertificateStore);
    Handle->PkiProvider.LoadCertificate(&Handle->PkiProvider, ID_CERTIFICATE_PATH, CertificateStore, &Handle->ServerCertificate);
    Handle->PkiProvider.ValidateCertificate(&Handle->PkiProvider, &Handle->ServerCertificate, CertificateStore, &iValidationCode);
    
    // Load the private key
    OpcUa_Key_Initialize(&Handle->ServerPrivateKey);
    Handle->ServerPrivateKey.Type = OpcUa_Crypto_KeyType_Rsa_Private;
    Handle->PkiProvider.LoadPrivateKeyFromFile(ID_PRIVATE_KEY_PATH, OpcUa_Crypto_Encoding_PKCS12, ID_PRIVATE_KEY_PASSWORD, &Handle->ServerPrivateKey.Key);
    Handle->PkiProvider.CloseCertificateStore(&Handle->PkiProvider, &CertificateStore);
    
    Handle->SecurityPolicyConfigurations = malloc(sizeof(OpcUa_Endpoint_SecurityPolicyConfiguration) * ID_NUM_SECURITY_POLICIES);
    struct ID_SecurityPolicy securityPolicies[ID_NUM_SECURITY_POLICIES] = ID_SECURITY_POLICIES;
    for (int i = 0; i < ID_NUM_SECURITY_POLICIES; i++) {
        struct ID_SecurityPolicy * securityPolicy = &securityPolicies[i];
        OpcUa_String_Initialize(&Handle->SecurityPolicyConfigurations[i].sSecurityPolicy);
        OpcUa_String_AttachCopy(&Handle->SecurityPolicyConfigurations[i].sSecurityPolicy, securityPolicy->SecurityPolicy);
        Handle->SecurityPolicyConfigurations[i].pbsClientCertificate = &Handle->ServerCertificate;
        Handle->SecurityPolicyConfigurations[i].uMessageSecurityModes = securityPolicy->SecurityMode;
    }

    *Manager = Handle;
}

OpcUa_Int ID_UASecurityManager_ValidateCertificate(ID_SecurityHandle Manager, OpcUa_ByteString * Certificate) {
    OpcUa_Handle CertificateStore = NULL;
    OpcUa_Int iValidationCode = 0;
    Manager->PkiProvider.OpenCertificateStore(&Manager->PkiProvider, &CertificateStore);
    Manager->PkiProvider.ValidateCertificate(&Manager->PkiProvider, Certificate, CertificateStore, &iValidationCode);
    Manager->PkiProvider.CloseCertificateStore(&Manager->PkiProvider, &CertificateStore);
    return iValidationCode;
}

void ID_UASecurityManager_Destroy(ID_SecurityHandle * Manager) {
    ID_SecurityHandle Handle = *Manager;
    OpcUa_ByteString_Clear(&Handle->ServerCertificate);
    OpcUa_ByteString_Clear(&Handle->ServerSignature);
    OpcUa_Key_Clear(&Handle->ServerPrivateKey);
    
    OpcUa_String_Clear(&Handle->SecurityPolicyConfigurations[0].sSecurityPolicy);
    OpcUa_String_Clear(&Handle->SecurityPolicyConfigurations[1].sSecurityPolicy);
    free(Handle->SecurityPolicyConfigurations);
    
    OpcUa_PKIProvider_Delete(&Handle->PkiProvider);    
    free(Handle);
    *Manager = NULL;
}