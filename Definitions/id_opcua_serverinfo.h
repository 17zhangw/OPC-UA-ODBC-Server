/*
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ID_OPCUA_SERVERINFO_H
#define ID_OPCUA_SERVERINFO_H

#ifdef __cplusplus
extern "C" {
#endif

#define ID_SERVER_URI                   "http://idata/UA"
#define ID_PRODUCT_URI                  "opc-ua.server.idata"
#define ID_MANUFACTURER_NAME            "iData"
#define ID_APPLICATION_NAME             "iData OPC-UA DB Server"
#define ID_PRODUCT_NAME                 ID_APPLICATION_NAME
#define ID_SOFTWARE_VERSION             "alpha-0.1.5"
#define ID_BUILD_NUMBER                 "10000"
#define ID_BUILD_DATE                   (OpcUa_DateTime){ 791598794, 30594561 }

#define ID_APPLICATION_LOCALE           "en-US"
#define ID_APPLICATION_LOCALE_LINGUAL   "en"
#define ID_APPLICATION_LOCALE_COUNTRY   "US"

#define ID_NUM_SUPPORTED_PROFILES       6
#define ID_SERVER_PROFILES {"http://opcfoundation.org/UA-Profile/Server/CoreFacet", \
                            "http://opcfoundation.org/UA-Profile/Security/UserToken/Server/UserNamePassword", \
                            "http://opcfoundation.org/UA/SecurityPolicy#None", \
                            "http://opcfoundation.org/UA-Profile/Server/Methods", \
                            "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary", \
                            "http://opcfoundation.org/UA-Profile/Server/NanoEmbeddedDevice"}; \

typedef enum {
    ID_OS_WINDOWS,
    ID_OS_LINUX
} ID_OS;

#ifdef _WIN32
#define ID_CURRENT_OS                   ID_OS_WINDOWS
#else
#define ID_CURRENT_OS                   ID_OS_LINUX
#endif

#ifdef __cplusplus
}
#endif

#endif