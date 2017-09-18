/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef SECURITYMANAGER_H
#define SECURITYMANAGER_H

#include "../Definitions/id_opcua.h"

#ifdef __cplusplus
#extern "C" {
#endif

typedef struct _SecurityManager_ {
    OpcUa_ByteString ServerCertificate;
    OpcUa_ByteString ServerSignature;
    OpcUa_PKIProvider PkiProvider;
    OpcUa_Key ServerPrivateKey;
    OpcUa_P_OpenSSL_CertificateStore_Config StoreConfig;
    OpcUa_Endpoint_SecurityPolicyConfiguration * SecurityPolicyConfigurations;
} ID_UASecurityManager;

typedef ID_UASecurityManager * ID_SecurityHandle;

// Create the SecuritY Manager
void ID_UASecurityManager_Create(ID_SecurityHandle * Manager);

// Validates a Certificate
OpcUa_Int ID_UASecurityManager_ValidateCertificate(ID_SecurityHandle Manager, OpcUa_ByteString * Certificate);

// Destroys the SecurityManager
void ID_UASecurityManager_Destroy(ID_SecurityHandle * Manager);

#ifdef __cplusplus
}
#endif

#endif /* SECURITYMANAGER_H */
