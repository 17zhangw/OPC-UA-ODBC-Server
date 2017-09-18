/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "SessionManager.h"
#include "../Indexer/Indexer.h"
#include "../AddressSpace/AddressSpaceHelper.h"

extern ID_AddressSpaceHandle GlobalAddressSpaceHandle;
extern ID_IndexerHandle GlobalIndexerHandle;

typedef struct _ID_CertificateAssociation_ {
    OpcUa_UInt32 SecureChannelId; // can be used to link a session to a certificate association
    OpcUa_ByteString Certificate;
} ID_CertificateAssociation;

typedef struct _ID_SessionManagerInternal_ {
    OpcUa_Int32 Count;
    OpcUa_Int32 Capacity;
    OpcUa_Int32 Increment;
    ID_Session ** Sessions;

    OpcUa_Boolean SessionsBeingMutated; // Controls access to Sessions
    OpcUa_Mutex SessionMutex; // Control access to SessionsBeingMutated and Sessions
    
    OpcUa_Int32 AssociatedCount;
    OpcUa_Int32 AssociatedCapacity;
    OpcUa_Int32 AssociatedIncrement;
    OpcUa_Mutex AssociationMutex; // control access to Associations
    ID_CertificateAssociation ** Associations;
    
    OpcUa_Double AccumulatedElapsedTime;
} ID_SessionManagerInternal;

void ID_SessionManager_Create(ID_SessionManagerHandle * Handle) {
    ID_SessionManagerInternal * Internal = malloc(sizeof(ID_SessionManagerInternal));
    memset(Internal, 0x00, sizeof(ID_SessionManagerInternal));
    Internal->Count = 0;
    Internal->Capacity = 10;
    Internal->Increment = 10;
    Internal->Sessions = malloc(sizeof(ID_Session *) * Internal->Capacity);
    memset(Internal->Sessions, 0x00, sizeof(ID_Session *) * Internal->Capacity);
    Internal->SessionsBeingMutated = OpcUa_False;
    Internal->AccumulatedElapsedTime = 0;
    
    Internal->AssociatedCount = 0;
    Internal->AssociatedCapacity = 10;
    Internal->AssociatedIncrement = 10;
    Internal->Associations = malloc(sizeof(ID_CertificateAssociation *) * Internal->AssociatedCapacity);
    memset(Internal->Associations, 0x00, sizeof(ID_Session *) * Internal->AssociatedCapacity);

    OPCUA_P_MUTEX_CREATE(&Internal->SessionMutex);
    OPCUA_P_MUTEX_CREATE(&Internal->AssociationMutex);
    *Handle = Internal;
}

ID_Session * ID_SessionManager_GetSession(ID_SessionManagerHandle Handle, OpcUa_NodeId * AuthenticationToken) {
    ID_Session * UserSession = NULL;
    ID_SessionManagerInternal * Internal = Handle;
    OPCUA_P_MUTEX_LOCK(Internal->SessionMutex); // lock to make sure that the loop is consistent

    for (int i = 0; i < Internal->Count; i++) {
        ID_Session * InnerUserSession = Internal->Sessions[i];

        // only return a session if the idledTime is less than timeout
        if (ID_AddressSpace_AreIdsEqual(&InnerUserSession->AuthenticationToken, AuthenticationToken) == OpcUa_True &&
            InnerUserSession->IdledTime <= InnerUserSession->SessionTimeout) {
            UserSession = InnerUserSession;
        }
    }
    
    OPCUA_P_MUTEX_UNLOCK(Internal->SessionMutex);
    return UserSession;
}

OpcUa_Boolean ID_SessionManager_IsSessionValid(ID_SessionManagerHandle Handle, OpcUa_NodeId * AuthenticationToken, OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, OpcUa_Boolean Override) {
    OpcUa_ReferenceParameter(Handle);

    // ensure that time is still valid
    ID_Session * SingleSession = ID_SessionManager_GetSession(Handle, AuthenticationToken);
    if (SingleSession == NULL) { return OpcUa_False; }

    // ensure session is not being created or added
    if (SingleSession->IsBeingMutated == OpcUa_True) { return OpcUa_False; }

    // ensure activation or override in ActivateSession
    if (Override == OpcUa_False && SingleSession->IsActivated == OpcUa_False) { return OpcUa_False; }

    OpcUa_UInt32 SecureChannelId = 0;
    OpcUa_Endpoint_GetMessageSecureChannelId(hEndpoint, hContext, &SecureChannelId);

    // make sure that securechannelid is correct
    if (SingleSession->SecureChannelId == SecureChannelId) {
        SingleSession->IdledTime = 0;
        return OpcUa_True;
    }

    return OpcUa_False;
}

ID_Session * ID_SessionManager_AddSession(ID_SessionManagerHandle Handle, OpcUa_UInt32 SecureChannelId, OpcUa_String * SessionName, OpcUa_ApplicationDescription * ClientDescription, OpcUa_ByteString * ClientNonce, OpcUa_ByteString * ClientCertificate) {
    // lock the mutex to prevent multiaccess and iterative procedures
    // will not be overly messed up due to index shifting
    ID_SessionManagerInternal * Internal = Handle;
    OPCUA_P_MUTEX_LOCK(Internal->SessionMutex);

    // shift the pointers
    if (Internal->Count >= Internal->Capacity) {
        Internal->Sessions = realloc(Internal->Sessions, sizeof(ID_Session *) * (Internal->Capacity + Internal->Increment));
        Internal->Capacity += Internal->Increment;
    }
    
    ID_Session * SingleSession = malloc(sizeof(ID_Session));
    Internal->Sessions[Internal->Count] = SingleSession;
    Internal->Count++;
    
    memset(SingleSession, 0x00, sizeof(ID_Session));
    SingleSession->IsBeingMutated = OpcUa_True;
    OPCUA_P_MUTEX_UNLOCK(Internal->SessionMutex);

    // create an objectnode linked to the session
    // THIS IS ONLY FOR THE CONFORMANCE TEST
    // IN REALITY THE SESSION DIAGNOSTICS OBJECT IS NOT CREATED LIKE THIS
    // REALITY CREATES THE OpcUa_SessionDiagnosticsDataType
    ID_ObjectNode * SessionNode = NULL;
    ID_AddressSpace_CreateObjectNode(GlobalAddressSpaceHandle, &SessionNode, NULL);
    
    // initialize all session variables
    OpcUa_LocalizedText_Initialize(&SingleSession->ApplicationName);
    OpcUa_NodeId_Initialize(&SingleSession->AuthenticationToken);
    OpcUa_ByteString_Initialize(&SingleSession->ClientCertificate);
    OpcUa_ByteString_Initialize(&SingleSession->ClientNonce);
    OpcUa_String_Initialize(&SingleSession->ProductUri);
    OpcUa_ByteString_Initialize(&SingleSession->ServerNonce);
    OpcUa_NodeId_Initialize(&SingleSession->SessionId);
    OpcUa_String_Initialize(&SingleSession->SessionName);
    SingleSession->SessionDBEnvHandle = NULL;
    SingleSession->SessionDBConnHandle = NULL;
    
    // copy over valid information
    SingleSession->SecureChannelId = SecureChannelId;
    if (ClientDescription != NULL) {
        OpcUa_String_AttachCopy(&SingleSession->ApplicationName.Locale, OpcUa_String_GetRawString(&ClientDescription->ApplicationName.Locale));
        OpcUa_String_AttachCopy(&SingleSession->ApplicationName.Text, OpcUa_String_GetRawString(&ClientDescription->ApplicationName.Text));
        OpcUa_String_AttachCopy(&SingleSession->ProductUri, OpcUa_String_GetRawString(&ClientDescription->ProductUri));
    }
    
    if (ClientCertificate != NULL && ClientNonce->Length > 0) {
        ID_OpcUa_ByteString_CopyTo(ClientCertificate, &SingleSession->ClientCertificate);
    }
    
    if (ClientNonce != NULL && ClientNonce->Length > 0) {
        ID_OpcUa_ByteString_CopyTo(ClientNonce, &SingleSession->ClientNonce);
    }
    
    if (SessionName != NULL && OpcUa_String_GetRawString(SessionName) != NULL) {
        OpcUa_String_AttachCopy(&SingleSession->SessionName, OpcUa_String_GetRawString(SessionName));
        SessionNode->BaseAttribute.BrowseName.NamespaceIndex = ID_NAMESPACE_INDEX;
        OpcUa_String_AttachCopy(&SessionNode->BaseAttribute.BrowseName.Name, OpcUa_String_GetRawString(SessionName));
    } else {
        OpcUa_String_AttachCopy(&SingleSession->SessionName, "Unknown_UACLIENT");
        SessionNode->BaseAttribute.BrowseName.NamespaceIndex = ID_NAMESPACE_INDEX;
        OpcUa_String_AttachCopy(&SessionNode->BaseAttribute.BrowseName.Name, "Unknown_UACLIENT");
    }
    
    // generate an authentication token
    SingleSession->AuthenticationToken.Identifier.Numeric = ID_Indexer_GetNextIndex(GlobalIndexerHandle);
    SingleSession->AuthenticationToken.NamespaceIndex = ID_NAMESPACE_INDEX;
    SingleSession->AuthenticationToken.IdentifierType = OpcUa_IdType_Numeric;
    
    // generate a sessioNId and copy the sessionId back to the SessionNode
    SingleSession->SessionId.Identifier.Numeric = ID_Indexer_GetNextIndex(GlobalIndexerHandle);
    SingleSession->SessionId.NamespaceIndex = ID_NAMESPACE_INDEX;
    SingleSession->SessionId.IdentifierType = OpcUa_IdType_Numeric;
    ID_OpcUa_NodeId_CopyTo(&SingleSession->SessionId, &SessionNode->BaseAttribute.NodeId);
    
    OpcUa_ExtensionObject_Initialize(&SingleSession->IdentityToken);    
    SingleSession->IdledTime = 0;
    SingleSession->SessionDBConnHandle = NULL;
    SingleSession->SessionDBEnvHandle = NULL;
    OpcUa_Mutex_Create(&SingleSession->PreparedStatementsMutex);
    return SingleSession;
}

void ID_SessionManager_ValidateSessionLifetime(ID_SessionManagerHandle Handle, OpcUa_UInt32 ElapsedCount) {
    ID_SessionManagerInternal * Internal = Handle;

    // keep accumulating time if sessionsbeingmutated
    if (Internal->SessionsBeingMutated == OpcUa_True) {
        Internal->AccumulatedElapsedTime += ElapsedCount;
        return;
    }
    
    for (int i = 0; i < Internal->Count; i++) {
        ID_Session * SingleSession = Internal->Sessions[i];
        if (SingleSession->IsBeingMutated == OpcUa_False) {
            SingleSession->IdledTime += ElapsedCount + Internal->AccumulatedElapsedTime;
            if (SingleSession->IdledTime > SingleSession->SessionTimeout) {
                if (Internal->SessionsBeingMutated == OpcUa_False) {
                    // prevent IsSessionValid() from validating the session & prevent another call to this function
                    SingleSession->IsBeingMutated = OpcUa_True;
                    ID_SessionManager_RemoveSession(Handle, &SingleSession->AuthenticationToken);
                    Internal->SessionsBeingMutated = OpcUa_False;
                }
            }
        }
    }
    
    Internal->AccumulatedElapsedTime = 0;
}

void ID_SessionManager_RemoveSession(ID_SessionManagerHandle Handle, OpcUa_NodeId * AuthenticationToken) {
    ID_SessionManagerInternal * Internal = Handle;
    OPCUA_P_MUTEX_LOCK(Internal->SessionMutex); // lock
    Internal->SessionsBeingMutated = OpcUa_True;
    
    for (int i = 0; i < Internal->Count; i++) {
        ID_Session * SingleSession = Internal->Sessions[i];
        if (ID_AddressSpace_AreIdsEqual(AuthenticationToken, &SingleSession->AuthenticationToken) == OpcUa_True) {
            // Remove the address space node
            ID_AddressSpace_RemoveNode(GlobalAddressSpaceHandle, &SingleSession->SessionId, ID_NodeType_Object);
            
            // Adjust the pointer as necessary
            for (int j = i + 1; j < Internal->Count; j++) {
                Internal->Sessions[j - 1] = Internal->Sessions[j];
            }

            Internal->Sessions[Internal->Count - 1] = NULL;
            Internal->Count--;

            // Clear stored information
            OpcUa_LocalizedText_Clear(&SingleSession->ApplicationName);
            OpcUa_NodeId_Clear(&SingleSession->AuthenticationToken);
            OpcUa_NodeId_Clear(&SingleSession->SessionId);
            OpcUa_ByteString_Clear(&SingleSession->ClientCertificate);
            OpcUa_ByteString_Clear(&SingleSession->ClientNonce);
            OpcUa_ByteString_Clear(&SingleSession->ServerNonce);
            OpcUa_String_Clear(&SingleSession->ProductUri);
            OpcUa_String_Clear(&SingleSession->SessionName);
            OpcUa_ExtensionObject_Clear(&SingleSession->IdentityToken);
            
            // remove all PreparedStatements
            OPCUA_P_MUTEX_LOCK(SingleSession->PreparedStatementsMutex);
            for (OpcUa_UInt32 p = 0; p < SingleSession->NumPreparedStatements; p++) {
                ID_PreparedStatement * PStmt = SingleSession->PreparedStatements[p];
                ID_PreparedStatement_Free(PStmt);
                free(PStmt);
                SingleSession->PreparedStatements[p] = NULL;
            }
            
            free(SingleSession->PreparedStatements);
            OPCUA_P_MUTEX_UNLOCK(SingleSession->PreparedStatementsMutex);
            OpcUa_Mutex_Delete(&SingleSession->PreparedStatementsMutex);
            
            // disconnect and free database handles
            if (SingleSession->SessionDBConnHandle != NULL) {
                SQLDisconnect(SingleSession->SessionDBConnHandle);
                SQLFreeHandle(SQL_HANDLE_DBC, SingleSession->SessionDBConnHandle);
                SingleSession->SessionDBConnHandle = NULL;
            }
            
            if (SingleSession->SessionDBEnvHandle != NULL) {
                SQLFreeHandle(SQL_HANDLE_ENV, SingleSession->SessionDBEnvHandle);
                SingleSession->SessionDBEnvHandle = NULL;
            }

            free(SingleSession);
            break;
        }
    }

    Internal->SessionsBeingMutated = OpcUa_False;
    OPCUA_P_MUTEX_UNLOCK(Internal->SessionMutex);
}

void ID_SessionManager_Destroy(ID_SessionManagerHandle * Handle) {
    ID_SessionManagerInternal * Internal = *Handle;
    while (Internal->Count > 0) {
        ID_Session * S = Internal->Sessions[Internal->Count - 1];
        ID_SessionManager_RemoveSession(Internal, &S->AuthenticationToken);
    }
    free(Internal->Sessions);
    
    OPCUA_P_MUTEX_LOCK(Internal->AssociationMutex);
    for (int i = 0; i < Internal->AssociatedCount; i++) {
        ID_CertificateAssociation * Association = Internal->Associations[i];
        OpcUa_ByteString_Clear(&Association->Certificate);
        free(Association);
        Internal->Associations[i] = NULL;
    }
    OPCUA_P_MUTEX_UNLOCK(Internal->AssociationMutex);
    
    OPCUA_P_MUTEX_DELETE(&Internal->SessionMutex);
    OPCUA_P_MUTEX_DELETE(&Internal->AssociationMutex);
    free(Internal->Associations);
    free(Internal);
    *Handle = NULL;
}

void ID_SessionManager_AssociateSecureChannelIdWithCertificate(ID_SessionManagerHandle Handle, OpcUa_UInt32 SecureChannelId, OpcUa_ByteString * Certificate) {
    ID_SessionManagerInternal * Internal = Handle;
    // try to copy into an existing association
    for (int i = 0; i < Internal->AssociatedCount; i++) {
        if (Internal->Associations[i]->SecureChannelId == SecureChannelId) {
            OpcUa_ByteString_Clear(&Internal->Associations[i]->Certificate);
            ID_OpcUa_ByteString_CopyTo(Certificate, &Internal->Associations[i]->Certificate);
            return;
        }
    }
    
    OPCUA_P_MUTEX_LOCK(Internal->AssociationMutex);
    // shift pointers
    if (Internal->AssociatedCount >= Internal->AssociatedCapacity) {
        Internal->AssociatedCapacity += Internal->AssociatedIncrement;
        Internal->Associations = realloc(Internal->Associations, sizeof(ID_CertificateAssociation *) * Internal->AssociatedCapacity);
    }
    
    // create association
    ID_CertificateAssociation * Association = malloc(sizeof(ID_CertificateAssociation));
    memset(Association, 0x00, sizeof(ID_CertificateAssociation));
    Association->SecureChannelId = SecureChannelId;
    ID_OpcUa_ByteString_CopyTo(Certificate, &Association->Certificate);
    Internal->Associations[Internal->AssociatedCount] = Association;
    Internal->AssociatedCount++;
    OPCUA_P_MUTEX_UNLOCK(Internal->AssociationMutex);
}

OpcUa_ByteString * ID_SessionManager_GetCertificateAssociatedWithId(ID_SessionManagerHandle Handle, OpcUa_UInt32 SecureChannelId) {
    ID_SessionManagerInternal * Internal = Handle;
    for (int i = 0; i < Internal->AssociatedCount; i++) {
        if (Internal->Associations[i]->SecureChannelId == SecureChannelId) {
            return &Internal->Associations[i]->Certificate;
        }
    }
    
    return NULL;
}

void ID_SessionManager_UnassociateSecureChannelId(ID_SessionManagerHandle Handle, OpcUa_UInt32 SecureChannelId) {
    ID_SessionManagerInternal * Internal = Handle;
    for (int i = 0; i < Internal->AssociatedCount; i++) {
        if (Internal->Associations[i]->SecureChannelId == SecureChannelId) {
            OPCUA_P_MUTEX_LOCK(Internal->AssociationMutex);

            ID_CertificateAssociation * Association = Internal->Associations[i];
            OpcUa_ByteString_Clear(&Association->Certificate);
            free(Association);
            
            // shift pointers
            for (int j = i + 1; j < Internal->AssociatedCount; j++) {
                Internal->Associations[j - 1] = Internal->Associations[j];
            }
            
            Internal->Associations[Internal->AssociatedCount - 1] = NULL;
            Internal->AssociatedCount--;

            OPCUA_P_MUTEX_UNLOCK(Internal->AssociationMutex);
            return;
        }
    }
}

void ID_PreparedStatement_Free(ID_PreparedStatement * PStmt) {
    SQLFreeHandle(SQL_HANDLE_STMT, PStmt->Statement);
    PStmt->Statement = NULL;
    
    for (int b = 0; b < PStmt->NumBuffers; b++) {
        free(PStmt->Buffers[b]);
        free(PStmt->InputIndicators[b]);
    }
    
    if (PStmt->Buffers != NULL) free(PStmt->Buffers);
    if (PStmt->BufferLengths != NULL) free(PStmt->BufferLengths);
    if (PStmt->InputIndicators != NULL) free(PStmt->InputIndicators);
}