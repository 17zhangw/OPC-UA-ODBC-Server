/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ID_OPCUA_COMM_H
#define ID_OPCUA_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

#define ID_SESSION_TIMEOUT              120000
 
#define ID_USER_USERNAME                "esj"
#define ID_USER_PASSWORD                "esjesjNo1!"
#define ID_USER_PASSWORD_LENGTH         10

#ifdef __linux__
#define ID_CERTIFICATE_PATH             "/home/esj/PKI/certs/server.der"
#define ID_PRIVATE_KEY_PATH             "/home/esj/PKI/private/server.pfx"
#define ID_PRIVATE_KEY_PASSWORD         "server"
#define ID_TRUST_CERTIFICATE_PATH       "/home/esj/PKI/certs/"
#define ID_REVOKED_CERTIFICATE_PATH     "/home/esj/PKI/crl/"
#define ID_UNTRUSTED_CERTIFICATE_PATH   "/home/esj/PKI/crl/"

#define ID_ENDPOINT_URL                 "opc.tcp://192.168.1.54:4840"

#elif defined(_WIN32)
#define ID_CERTIFICATE_PATH             "C:\\PKI\\certs\\server.der"
#define ID_PRIVATE_KEY_PATH             "C:\\PKI\\private\\server.pfx"
#define ID_PRIVATE_KEY_PASSWORD         "server"
#define ID_TRUST_CERTIFICATE_PATH       "C:\\PKI\\certs"
#define ID_REVOKED_CERTIFICATE_PATH     "C:\\PKI\\crl"
#define ID_UNTRUSTED_CERTIFICATE_PATH   "C:\\PKI\\crl"

#define ID_ENDPOINT_URL                 "opc.tcp://192.168.11.23:4840"

#endif

#define ID_NUM_ENDPOINTS                2
#define ID_NUM_SECURITY_POLICIES        2
struct ID_SecurityPolicy {
    OpcUa_UInt16 TokenType;
    OpcUa_StringA PolicyId;
    OpcUa_StringA SecurityPolicy;
    OpcUa_StringA AsymmetricEncryptionAlgorithm;
    OpcUa_StringA AsymmetricSignatureAlgorithm;
    OpcUa_UInt16 SecurityMode;
    OpcUa_MessageSecurityMode MessageMode;
};

#define ID_USERNAME_SECURITY_POLICY_INDEX       0
#define ID_ANONYMOUS_SECURITY_POLICY_INDEX      1
#define ID_SECURITY_POLICIES {{OpcUa_UserTokenType_UserName, "2", OpcUa_SecurityPolicy_Basic128Rsa15, \
                               OpcUa_AlgorithmUri_Encryption_Rsa15, OpcUa_AlgorithmUri_Signature_RsaSha1, \
                               OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_SIGNANDENCRYPT, OpcUa_MessageSecurityMode_SignAndEncrypt}, \
                              {OpcUa_UserTokenType_Anonymous, "1", OpcUa_SecurityPolicy_None, NULL, NULL, \
                               OPCUA_SECURECHANNEL_MESSAGESECURITYMODE_NONE, OpcUa_MessageSecurityMode_None}}; \

#ifdef __cplusplus
}
#endif

#endif