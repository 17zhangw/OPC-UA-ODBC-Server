/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifdef __linux__
#include <sys/time.h>
#endif

#include "../Definitions/id_opcua.h"
#include "ExposedFunctions.h"

#include <opcua_stringtable.h>
#include "../UASpace/Initialize.h"

#include "../Indexer/Continuer.h"
#include "../Indexer/Aliaser.h"
#include "../SessionManager/SessionManager.h"
#include "../AddressSpace/AddressSpaceHelper.h"

extern OpcUa_StringTable OpcUa_ProxyStub_g_NamespaceUris;
extern ID_SessionManagerHandle GlobalSessionManagerHandle;
extern ID_ContinuerHandle GlobalContinuerHandle;
extern ID_AddressSpaceHandle GlobalAddressSpaceHandle;
extern ID_AliaserHandle GlobalAliaserHandle;

OpcUa_Boolean ID_UA_CheckNodeAgainstMask(OpcUa_NodeClass Type, OpcUa_UInt32 Mask) {
    if ((Mask & Type) != 0)
        return OpcUa_True;
    return OpcUa_False;
}

void ID_Browse(OpcUa_Int32 nRequestedMaxReferencesPerNode, OpcUa_BrowseResult * pResults, OpcUa_BrowseDescription * BrowseRequest, OpcUa_NodeId * SessionId, ID_UAContinuationPoint * ExistingPoint) {
    OpcUa_NodeId ID_HasTypeDefinitionNode = ID_HasTypeDefinition;    

    OpcUa_NodeId * BrowseRequestNodeId = &BrowseRequest->NodeId;
    if (ID_Aliaser_GetOriginal(GlobalAliaserHandle, BrowseRequestNodeId) != NULL) {
        BrowseRequestNodeId = ID_Aliaser_GetOriginal(GlobalAliaserHandle, BrowseRequestNodeId);
    }
    ID_BaseAttributes * Node = ID_AddressSpace_GetNode(GlobalAddressSpaceHandle, BrowseRequestNodeId);
    
    if (Node != NULL) {
        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "Searching %s\n", OpcUa_String_GetRawString(&Node->DisplayName.Text));
        OpcUa_Boolean IsContinuationRequired = OpcUa_False;
        OpcUa_Int NextStart = 0;
        int startingIndex = ExistingPoint == NULL ? 0 : ExistingPoint->Offset;
        
        for (OpcUa_UInt32 r = startingIndex; r < Node->NoOfReferences; r++) {
            ID_ReferenceNode * Ref = Node->References[r];

            // check for browse direction
            if (BrowseRequest->BrowseDirection == OpcUa_BrowseDirection_Both || 
               (Ref->IsInverse == OpcUa_True && BrowseRequest->BrowseDirection == OpcUa_BrowseDirection_Inverse) || 
               (Ref->IsInverse == OpcUa_False && BrowseRequest->BrowseDirection == OpcUa_BrowseDirection_Forward)) {
                
                // check the validity of the reference
                OpcUa_Boolean IsNullId = OpcUa_NodeId_IsNull(&BrowseRequest->ReferenceTypeId);
                if (IsNullId == OpcUa_True || 
                    ID_AddressSpace_AreIdsEqual(&Ref->ReferenceTypeId, &BrowseRequest->ReferenceTypeId) == OpcUa_True ||
                    (BrowseRequest->IncludeSubtypes == OpcUa_True && ID_AddressSpace_IsTypeASubtypeofB(GlobalAddressSpaceHandle, &Ref->ReferenceTypeId, &BrowseRequest->ReferenceTypeId))) {

                    // check to make sure node is valid
                    ID_BaseAttributes * Target = ID_AddressSpace_GetNode(GlobalAddressSpaceHandle, &Ref->TargetNodeId);
                    if (BrowseRequest->NodeClassMask == 0 || 
                        ID_UA_CheckNodeAgainstMask(Target->NodeClass, BrowseRequest->NodeClassMask) == OpcUa_True) {     
                        
                        OpcUa_Trace(OPCUA_TRACE_LEVEL_DEBUG, "BrowseName: %s; Reference: %d\n", OpcUa_String_GetRawString(&Target->DisplayName.Text), Ref->ReferenceTypeId.Identifier.Numeric);
                        if (pResults->NoOfReferences >= nRequestedMaxReferencesPerNode && nRequestedMaxReferencesPerNode != 0) {
                            IsContinuationRequired = OpcUa_True;
                            NextStart = r;
                            break;
                        }

                        // copy information depending on the resultmask
                        pResults->References = realloc(pResults->References, sizeof(OpcUa_ReferenceDescription) * (pResults->NoOfReferences + 1));
                        OpcUa_ReferenceDescription_Initialize(&pResults->References[pResults->NoOfReferences]);
                        OpcUa_ReferenceDescription * RefDescription = &pResults->References[pResults->NoOfReferences];
                        if ((BrowseRequest->ResultMask & 1) != 0) {
                            ID_OpcUa_NodeId_CopyTo(&Ref->ReferenceTypeId, &RefDescription->ReferenceTypeId);
                        }

                        if ((BrowseRequest->ResultMask & 2) != 0) {
                            RefDescription->IsForward = Ref->IsInverse == OpcUa_False;
                        }

                        if ((BrowseRequest->ResultMask & 4) != 0) {
                            RefDescription->NodeClass = Target->NodeClass;
                        }

                        if ((BrowseRequest->ResultMask & 8) != 0) {
                            RefDescription->BrowseName.NamespaceIndex = Target->BrowseName.NamespaceIndex;
                            char * BrowseNameValue = OpcUa_String_GetRawString(&Target->BrowseName.Name);
                            if (BrowseNameValue != NULL) {
                                OpcUa_String_AttachCopy(&RefDescription->BrowseName.Name, BrowseNameValue);
                            }
                        }

                        if ((BrowseRequest->ResultMask & 16) != 0) {
                            char * TargetLocale = OpcUa_String_GetRawString(&Target->DisplayName.Locale);
                            char * TargetText = OpcUa_String_GetRawString(&Target->DisplayName.Text);
                            if (TargetLocale != NULL) {
                                OpcUa_String_AttachCopy(&RefDescription->DisplayName.Locale, TargetLocale);
                            }
                            
                            if (TargetText != NULL) {
                                OpcUa_String_AttachCopy(&RefDescription->DisplayName.Text, TargetText);
                            }
                        }

                        if ((BrowseRequest->ResultMask & 32) != 0 &&
                            (Target->NodeClass == OpcUa_NodeClass_Object || Target->NodeClass == OpcUa_NodeClass_Variable)) {
                            
                            OpcUa_Boolean isFound = OpcUa_False;
                            for (OpcUa_UInt32 ref = 0; ref < Target->NoOfReferences; ref++) {
                                ID_ReferenceNode * TargetRef = Target->References[ref];
                                if (ID_AddressSpace_AreIdsEqual(&TargetRef->ReferenceTypeId, &ID_HasTypeDefinitionNode) == 0 && 
                                    TargetRef->IsInverse == OpcUa_False) {
                                    
                                    RefDescription->TypeDefinition.ServerIndex = 0;
                                    ID_OpcUa_NodeId_CopyTo(&TargetRef->TargetNodeId, &RefDescription->TypeDefinition.NodeId);
                                    isFound = OpcUa_True;
                                    break;
                                }
                            }
                            
                            if (!isFound) {
                                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "No Type Definition found for object");
                            }
                        }

                        RefDescription->NodeId.ServerIndex = 0;
                        RefDescription->NodeId.NodeId = Target->NodeId;
                        pResults->NoOfReferences++;
                    }
                }
            }
        }

        // handle the continuation point logic
        if (IsContinuationRequired != OpcUa_False) {
            ID_UAContinuationPoint * Point = NULL;
            if (ExistingPoint != NULL) {
                ExistingPoint->Offset = NextStart;
                Point = ExistingPoint;
            } else {
                Point = ID_Continuer_AddContinuationPoint(GlobalContinuerHandle, BrowseRequest, nRequestedMaxReferencesPerNode);
                ID_OpcUa_NodeId_CopyTo(SessionId, &Point->SessionId);
                Point->Offset = NextStart;
            }
            
            if (Point != NULL) {
                pResults->ContinuationPoint.Length = Point->Identifier.Length;
                pResults->ContinuationPoint.Data = malloc(sizeof(OpcUa_Byte) * pResults->ContinuationPoint.Length);
                memcpy(pResults->ContinuationPoint.Data, Point->Identifier.Data, sizeof(OpcUa_Byte) * pResults->ContinuationPoint.Length);
            }
        } else if (IsContinuationRequired == OpcUa_False && ExistingPoint != NULL) {
            ID_Continuer_RemoveContinuationPoint(GlobalContinuerHandle, &ExistingPoint->Identifier);
            OpcUa_ByteString_Clear(&pResults->ContinuationPoint);
        }

        pResults->StatusCode = OpcUa_Good;
    } else {
        pResults->StatusCode = OpcUa_BadNodeIdUnknown;
    }
}

OpcUa_StatusCode ID_UA_Browse(OpcUa_Endpoint hEndpoint,OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                              const OpcUa_ViewDescription* pView, OpcUa_UInt32 nRequestedMaxReferencesPerNode, OpcUa_Int32 nNoOfNodesToBrowse,
                              const OpcUa_BrowseDescription* pNodesToBrowse, OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults,
                              OpcUa_BrowseResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    OpcUa_NodeId ID_OrganizesNode = ID_Organizes;
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_Browse");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArgumentNull(pView);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfNodesToBrowse, pNodesToBrowse);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfResults);
    OpcUa_ReturnErrorIfArgumentNull(pResults);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfDiagnosticInfos);
    OpcUa_ReturnErrorIfArgumentNull(pDiagnosticInfos);
    *pNoOfResults = 0;
    *pResults = NULL;
    *pNoOfDiagnosticInfos = 0;
    *pDiagnosticInfos = NULL;
    ID_ViewNode * ViewOfInterest = NULL;
    
    ID_CHECK_SESSION();
    
    if (!OpcUa_NodeId_IsNull((OpcUa_NodeId*)&pView->ViewId)) {
        ViewOfInterest = ID_AddressSpace_GetViewNode(GlobalAddressSpaceHandle, (OpcUa_NodeId*)&pView->ViewId);
        if (ViewOfInterest == NULL || ((ID_BaseAttributes*)ViewOfInterest)->NodeClass != OpcUa_NodeClass_View) {
            uStatus = OpcUa_BadViewIdUnknown;
            goto Error;
        }
    }
    
    // ignore View since no view exists but default address space
    *pNoOfResults = nNoOfNodesToBrowse;
    OpcUa_BrowseResult * BrowseResults = malloc(sizeof(OpcUa_BrowseResult) * nNoOfNodesToBrowse);
    for (int i = 0; i < nNoOfNodesToBrowse; i++) {
        OpcUa_BrowseResult_Initialize(&BrowseResults[i]);
        OpcUa_BrowseDescription BrowseRequest = pNodesToBrowse[i];
        
        // validate browse direction
        if (BrowseRequest.BrowseDirection != OpcUa_BrowseDirection_Both &&
            BrowseRequest.BrowseDirection != OpcUa_BrowseDirection_Forward &&
            BrowseRequest.BrowseDirection != OpcUa_BrowseDirection_Inverse) {
            (&BrowseResults[i])->StatusCode = OpcUa_BadBrowseDirectionInvalid;
            continue;
        }
        
        // validate referencetype
        if (!OpcUa_NodeId_IsNull((OpcUa_NodeId *)&BrowseRequest.ReferenceTypeId)) {
            ID_ReferenceTypeNode * RefTypeNode = ID_AddressSpace_GetReferenceTypeNode(GlobalAddressSpaceHandle, (OpcUa_NodeId*)&BrowseRequest.ReferenceTypeId);
            if (RefTypeNode == NULL || ((ID_BaseAttributes*)RefTypeNode)->NodeClass != OpcUa_NodeClass_ReferenceType) {
                (&BrowseResults[i])->StatusCode = OpcUa_BadReferenceTypeIdInvalid;
                continue;
            }
        }
        
        // validate Node presence in view
        if (!OpcUa_NodeId_IsNull((OpcUa_NodeId *)&pView->ViewId)) {
            OpcUa_Boolean isFound = OpcUa_False;            
            for (OpcUa_UInt32 n = 0; n < ViewOfInterest->BaseAttribute.NoOfReferences; n++) {
                ID_ReferenceNode * RefNode = ViewOfInterest->BaseAttribute.References[n];
                if (ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_OrganizesNode)) {
                    if (ID_AddressSpace_AreIdsEqual(&RefNode->TargetNodeId, &BrowseRequest.NodeId)) {
                        isFound = OpcUa_True;
                        goto exitView;
                    }
                }
            }
            
            exitView: ;
            if (isFound == OpcUa_False) {
                (&BrowseResults[i])->StatusCode = OpcUa_BadNodeNotInView;
                continue;
            }
        }
        
        ID_Browse(nRequestedMaxReferencesPerNode, &BrowseResults[i], &BrowseRequest, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken, NULL);
    }
    
    *pResults = BrowseResults;
    if (nNoOfNodesToBrowse == 0) {
        uStatus = OpcUa_BadNothingToDo;
    }
    
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}

OpcUa_StatusCode ID_UA_BrowseNext(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext,const OpcUa_RequestHeader* pRequestHeader,
                                  OpcUa_Boolean bReleaseContinuationPoints, OpcUa_Int32 nNoOfContinuationPoints, 
                                  const OpcUa_ByteString* pContinuationPoints, OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults,
                                  OpcUa_BrowseResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_BrowseNext");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfContinuationPoints, pContinuationPoints);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfResults);
    OpcUa_ReturnErrorIfArgumentNull(pResults);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfDiagnosticInfos);
    OpcUa_ReturnErrorIfArgumentNull(pDiagnosticInfos);
    *pNoOfResults = 0;
    *pResults = NULL;
    *pNoOfDiagnosticInfos = 0;
    *pDiagnosticInfos = NULL;
    
    ID_CHECK_SESSION();
    
    *pNoOfResults = nNoOfContinuationPoints;
    OpcUa_BrowseResult * BrowseResult = malloc(sizeof(OpcUa_BrowseResult) * nNoOfContinuationPoints);
    for (int i = 0; i < nNoOfContinuationPoints; i++) {
        OpcUa_BrowseResult_Initialize(&BrowseResult[i]);
        ID_UAContinuationPoint * Point = ID_Continuer_GetContinuationPoint(GlobalContinuerHandle, (OpcUa_ByteString*)&pContinuationPoints[i]);
        if (Point == NULL) {
            BrowseResult[i].StatusCode = OpcUa_BadContinuationPointInvalid;
        } else if (bReleaseContinuationPoints == OpcUa_True) {
            ID_Continuer_RemoveContinuationPoint(GlobalContinuerHandle, &Point->Identifier);
        } else {
            ID_Browse(Point->BatchSize, &BrowseResult[i], &Point->Browse, (OpcUa_NodeId*)&pRequestHeader->AuthenticationToken, Point);
        }
    }
    
    *pResults = BrowseResult;
    if (nNoOfContinuationPoints == 0) {
        uStatus = OpcUa_BadNothingToDo;
    }
    
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}

void ID_UA_TranscribeBrowseResults(OpcUa_RelativePath * Path, OpcUa_Int32 PathNum, ID_BaseAttributes * Source, OpcUa_BrowsePathResult * BrowsePathResult) {
    if (Source == NULL) {
        return;
    }
    
    if (PathNum >= Path->NoOfElements) {
        return;
    }
    
    OpcUa_RelativePathElement Criteria = Path->Elements[PathNum];
    for (OpcUa_UInt32 i = 0; i < Source->NoOfReferences; i++) {
        ID_ReferenceNode * RefNode = Source->References[i];
        if (Criteria.IsInverse == RefNode->IsInverse) {
            OpcUa_Boolean IsNullReference = OpcUa_NodeId_IsNull(&Criteria.ReferenceTypeId);

            // check if reference is NULL OR
            // check if Reference Ids are directly equivalent
            // check if IncludeSubtypes is on and validate subtype
            if (IsNullReference ||
                ID_AddressSpace_AreIdsEqual(&Criteria.ReferenceTypeId, &RefNode->ReferenceTypeId) == OpcUa_True || 
                (Criteria.IncludeSubtypes == OpcUa_True && ID_AddressSpace_IsTypeASubtypeofB(GlobalAddressSpaceHandle, &RefNode->ReferenceTypeId, &Criteria.ReferenceTypeId) == OpcUa_True)) {
                
                OpcUa_NodeId * RefTarget = &RefNode->TargetNodeId;
                if (ID_Aliaser_GetOriginal(GlobalAliaserHandle, RefTarget) != NULL) {
                    RefTarget = ID_Aliaser_GetOriginal(GlobalAliaserHandle, RefTarget);
                }
                
                char * TargetMatch = OpcUa_String_GetRawString(&Criteria.TargetName.Name);
                ID_BaseAttributes * Target = ID_AddressSpace_GetNode(GlobalAddressSpaceHandle, RefTarget);
                
                // check if last element and target not specified OR
                // check if browsenames are equal
                if ((TargetMatch == NULL && PathNum == Path->NoOfElements - 1) ||
                    (Target->BrowseName.NamespaceIndex == Criteria.TargetName.NamespaceIndex &&
                     strcmp(OpcUa_String_GetRawString(&Target->BrowseName.Name), TargetMatch) == 0)) {

                    // write results in if last step of the way
                    if (PathNum == Path->NoOfElements - 1) {
                        BrowsePathResult->Targets = realloc(BrowsePathResult->Targets, sizeof(OpcUa_BrowsePathTarget) * (BrowsePathResult->NoOfTargets + 1));
                        OpcUa_BrowsePathTarget_Initialize(&BrowsePathResult->Targets[BrowsePathResult->NoOfTargets]);
                        BrowsePathResult->Targets[BrowsePathResult->NoOfTargets].RemainingPathIndex = OpcUa_UInt32_Max;
                        BrowsePathResult->Targets[BrowsePathResult->NoOfTargets].TargetId.ServerIndex = 0;
                        ID_OpcUa_NodeId_CopyTo(&RefNode->TargetNodeId, &BrowsePathResult->Targets[BrowsePathResult->NoOfTargets].TargetId.NodeId);
                        BrowsePathResult->NoOfTargets++;
                    } else {
                        // recursively go next
                        ID_UA_TranscribeBrowseResults(Path, PathNum + 1, Target, BrowsePathResult);
                    }
                }
            }
        }
    }
}

OpcUa_Variant * ID_GetInputArguments(ID_AddressSpaceHandle Handle, ID_MethodNode * MethodNode) {
    OpcUa_NodeId ID_HasPropertyNode = ID_HasProperty;
    for (OpcUa_UInt32 i = 0; i < MethodNode->BaseAttribute.NoOfReferences; i++) {
        ID_ReferenceNode * RefNode = MethodNode->BaseAttribute.References[i];
        if (RefNode->IsInverse == OpcUa_False && ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasPropertyNode) == OpcUa_True) {
            ID_VariableNode * Attrs = ID_AddressSpace_GetVariableNode(Handle, &RefNode->TargetNodeId);
            if (Attrs->BaseAttribute.BrowseName.NamespaceIndex == 0 &&
                strcmp(OpcUa_String_GetRawString(&Attrs->BaseAttribute.BrowseName.Name), "InputArguments") == 0) {
                return &Attrs->Value;
            }
        }
    }
    
    return NULL;
}

OpcUa_StatusCode ID_UA_Call(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                            OpcUa_Int32 nNoOfMethodsToCall, const OpcUa_CallMethodRequest* pMethodsToCall, 
                            OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_CallMethodResult** pResults,
                            OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    
    OpcUa_NodeId ID_HasTypeDefinitionNode = ID_HasTypeDefinition;
    OpcUa_NodeId ID_HasComponentNode = ID_HasComponent;
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_Call");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfMethodsToCall, pMethodsToCall);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfResults);
    OpcUa_ReturnErrorIfArgumentNull(pResults);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfDiagnosticInfos);
    OpcUa_ReturnErrorIfArgumentNull(pDiagnosticInfos);
    *pNoOfResults = 0;
    *pResults = NULL;
    
    ID_CHECK_SESSION();
    
    *pNoOfResults = nNoOfMethodsToCall;
    *pNoOfDiagnosticInfos = 0;
    *pDiagnosticInfos = NULL;
    OpcUa_CallMethodResult * Results = malloc(sizeof(OpcUa_CallMethodResult) * nNoOfMethodsToCall);
    for (int i = 0; i < nNoOfMethodsToCall; i++) {
        OpcUa_CallMethodResult_Initialize(&Results[i]);
        OpcUa_CallMethodRequest Request = pMethodsToCall[i];
        OpcUa_NodeId * MethodId = &Request.MethodId;
        OpcUa_NodeId * ObjectId = &Request.ObjectId;
        if (ID_Aliaser_GetOriginal(GlobalAliaserHandle, MethodId) != NULL) {
            MethodId = ID_Aliaser_GetOriginal(GlobalAliaserHandle, MethodId);
        }
        
        if (ID_Aliaser_GetOriginal(GlobalAliaserHandle, ObjectId) != NULL) {
            ObjectId = ID_Aliaser_GetOriginal(GlobalAliaserHandle, ObjectId);
        }
        
        ID_MethodNode * MethodNode = ID_AddressSpace_GetMethodNode(GlobalAddressSpaceHandle, MethodId);
        if (MethodNode == NULL) {
            Results[i].StatusCode = OpcUa_BadMethodInvalid;
            continue;
        }
        
        // evaluate executability
        if (MethodNode->Executable == OpcUa_False && MethodNode->UserExecutable == OpcUa_False) {
            Results[i].StatusCode = OpcUa_BadUserAccessDenied;
            continue;
        }
        
        // evaluate defined
        if (MethodNode->Function == NULL) {
            Results[i].StatusCode = OpcUa_BadNotImplemented;
            continue;
        }
        
        // find owner of the method
        OpcUa_NodeId * OwningComponent = NULL;
        for (OpcUa_UInt32 refIndex = 0; refIndex < MethodNode->BaseAttribute.NoOfReferences; refIndex++) {
            ID_ReferenceNode * RefNode = MethodNode->BaseAttribute.References[refIndex];
            if (RefNode->IsInverse == OpcUa_True && ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasComponentNode) == OpcUa_True) {
                OwningComponent = &RefNode->TargetNodeId;
                break;
            }
        }
        
        // make sure owner is supplied
        if (OwningComponent == NULL || OpcUa_NodeId_IsNull(&Request.ObjectId) == OpcUa_True) {
            Results[i].StatusCode = OpcUa_BadNodeIdInvalid;
            continue;
        }

        // test if object
        OpcUa_NodeId * SuppliedObjType = NULL;
        ID_AddressSpaceNode * SuppliedObjNode = ID_AddressSpace_FindNodeWithID(GlobalAddressSpaceHandle, &Request.ObjectId);
        if (SuppliedObjNode->Type == ID_NodeType_Object) {
            ID_ObjectNode * SuppliedObj = &SuppliedObjNode->Node.ObjNode;
            for (OpcUa_UInt32 refIndex = 0; refIndex < SuppliedObj->BaseAttribute.NoOfReferences; refIndex++) {
                ID_ReferenceNode * RefNode = SuppliedObj->BaseAttribute.References[refIndex];
                if (RefNode->IsInverse == OpcUa_False && ID_AddressSpace_AreIdsEqual(&RefNode->ReferenceTypeId, &ID_HasTypeDefinitionNode) == OpcUa_True) {
                    SuppliedObjType = &RefNode->TargetNodeId;
                    break;
                }
            }
        }
        
        // return badtypemismatch under two circumstances
        // (1) ids do not directly match
        // (2) subtype objecttype incorrect - depends on input parameter
        if (ID_AddressSpace_AreIdsEqual(OwningComponent, ObjectId) == OpcUa_False && 
            ID_AddressSpace_IsTypeASubtypeofB(GlobalAddressSpaceHandle, ObjectId, OwningComponent) == OpcUa_False &&
            ID_AddressSpace_IsTypeASubtypeofB(GlobalAddressSpaceHandle, SuppliedObjType, OwningComponent) == OpcUa_False) {
            Results[i].StatusCode = OpcUa_BadTypeMismatch;
            continue;
        }
        
        // validate input arguments
        OpcUa_Variant * VariantArgs = ID_GetInputArguments(GlobalAddressSpaceHandle, MethodNode);
        if (VariantArgs == NULL) {
            Results[i].StatusCode = OpcUa_BadArgumentsMissing;
            continue;
        }
        
        if (VariantArgs->Value.Array.Length > Request.NoOfInputArguments) {
            Results[i].StatusCode = OpcUa_BadArgumentsMissing;
            continue;
        }
        
        if (VariantArgs->Value.Array.Length < Request.NoOfInputArguments) {
            Results[i].StatusCode = OpcUa_BadTooManyArguments;
            continue;
        }
        
        // initialize output
        int NoOfOutputArguments = 0; 
        OpcUa_Variant * OutputArguments = NULL;
        Results[i].NoOfInputArgumentResults = VariantArgs->Value.Array.Length;
        Results[i].InputArgumentResults = malloc(sizeof(OpcUa_StatusCode) * Results[i].NoOfInputArgumentResults);
        memset(Results[i].InputArgumentResults, 0x00, sizeof(OpcUa_StatusCode) * Results[i].NoOfInputArgumentResults);
        
        // calls function
        Results[i].StatusCode = MethodNode->Function((OpcUa_NodeId *)&pRequestHeader->AuthenticationToken, Request.NoOfInputArguments, Request.InputArguments, &NoOfOutputArguments, &OutputArguments, Results->InputArgumentResults);
        Results[i].NoOfOutputArguments = NoOfOutputArguments;
        Results[i].OutputArguments = OutputArguments;
    }
    
    *pResults = Results;
    
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}

OpcUa_StatusCode ID_UA_TranslateBrowsePathsToNodeIds(OpcUa_Endpoint hEndpoint,OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                                                     OpcUa_Int32 nNoOfBrowsePaths, const OpcUa_BrowsePath* pBrowsePaths, 
                                                     OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, 
                                                     OpcUa_BrowsePathResult** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, 
                                                     OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_TranslateBrowsePathsToNodeIds");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfBrowsePaths, pBrowsePaths);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfResults);
    OpcUa_ReturnErrorIfArgumentNull(pResults);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfDiagnosticInfos);
    OpcUa_ReturnErrorIfArgumentNull(pDiagnosticInfos);
    *pNoOfResults = 0;
    *pResults = NULL;
    *pNoOfDiagnosticInfos = 0;
    *pDiagnosticInfos = NULL;
    
    ID_CHECK_SESSION();
    
    if (nNoOfBrowsePaths == 0) {
        uStatus = OpcUa_BadNothingToDo;
        goto Error;
    }
    
    *pNoOfResults = nNoOfBrowsePaths;
    OpcUa_BrowsePathResult * Results = malloc(sizeof(OpcUa_BrowsePathResult) * nNoOfBrowsePaths);
    for (int i = 0; i < nNoOfBrowsePaths; i++) {
        OpcUa_BrowsePath BrowsePath = pBrowsePaths[i];
        OpcUa_BrowsePathResult_Initialize(&Results[i]);
        OpcUa_NodeId * Node = &BrowsePath.StartingNode;
        if (ID_Aliaser_GetOriginal(GlobalAliaserHandle, Node) != NULL) {
            Node = ID_Aliaser_GetOriginal(GlobalAliaserHandle, Node);
        }
        
        ID_BaseAttributes * StartingNode = ID_AddressSpace_GetNode(GlobalAddressSpaceHandle, Node);
        if (StartingNode == NULL) {
            Results[i].StatusCode = OpcUa_BadNodeIdUnknown;
        } else {
            OpcUa_RelativePath RelativePath = BrowsePath.RelativePath;
            if (RelativePath.NoOfElements == 0) {
                Results[i].StatusCode = OpcUa_BadNothingToDo;
                continue;
            }
            
            // checks to make sure that relativepath names are set
            // PENDING CONFIRMATION as Part 4 TranslateBrowsePathToNodeIds conflicts with 7.26
            OpcUa_Boolean IsFailed = OpcUa_False;
            for (int e = 0; e < RelativePath.NoOfElements; e++) {
                OpcUa_String * BrowseStr = &RelativePath.Elements[e].TargetName.Name;
                if (OpcUa_String_IsNull(BrowseStr) || OpcUa_String_IsEmpty(BrowseStr) || OpcUa_String_StrLen(BrowseStr) == 0) {
                    Results[i].StatusCode = OpcUa_BadBrowseNameInvalid;
                    IsFailed = OpcUa_True;
                    break;
                }
            }
            
            if (IsFailed == OpcUa_True) {
                continue;
            }
            
            // transcribe results
            ID_UA_TranscribeBrowseResults(&BrowsePath.RelativePath, 0, StartingNode, &Results[i]);
            if (Results[i].NoOfTargets == 0) {
                Results[i].StatusCode = OpcUa_BadNoMatch;
            } else {
                Results[i].StatusCode = OpcUa_Good;
            }
        }
    }
    
    *pResults = Results;
    
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}

OpcUa_Boolean ID_IsNumericRangeValid (char * str) {
    for (size_t i = 0; i < strlen(str); i++) {
        switch (str[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case ':':
                break;
            default:
                return OpcUa_False;
                break;
        }
    }
    
    return OpcUa_True;
}

int ID_FindFirstPartNumericRange(char * str) {
    int numChars = 0;
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] != ':') {
            numChars++;
        } else {
            break;
        }
    }
    
    char * tempStr = malloc(sizeof(char) * (numChars+1));
    memcpy(tempStr, str, sizeof(char) * numChars);
    tempStr[numChars] = '\0';
    int val = strtol(tempStr, NULL, 10);
    free(tempStr);
    return val;
}

int ID_FindSecondPartNumericRange(char * str) {
    char * colonPointer = strchr(str, ':') + 1;
    int numChars = 0;
    for (size_t i = 0; i < strlen(colonPointer); i++) {
        numChars++;
    }
    
    char * tempStr = malloc(sizeof(char) * (numChars+1));
    memcpy(tempStr, colonPointer, sizeof(char) * numChars);
    tempStr[numChars] = '\0';
    int val = strtol(tempStr, NULL, 10);
    free(tempStr);
    return val;
}

OpcUa_StatusCode ID_ConstructRange(char * RangeStr, int * pLowBound, int * pHighBound, int * pNumElements, ID_VariableNode * NodeAttributes) {
    int LowBound = 0; 
    int HighBound = 0; 
    int Elements = 0;
    if (RangeStr != NULL) {
        if (ID_IsNumericRangeValid(RangeStr) == OpcUa_False) {
            return OpcUa_BadIndexRangeInvalid;
        }

        if (RangeStr != NULL && strlen(RangeStr) > 0) {
            LowBound = ID_FindFirstPartNumericRange(RangeStr);
            if (LowBound < 0) {
                return OpcUa_BadIndexRangeInvalid;
            }

            if (strchr(RangeStr, ':') == NULL) {
                HighBound = LowBound;
            } else {
                HighBound = ID_FindSecondPartNumericRange(RangeStr);
                if (LowBound >= HighBound) {
                    return OpcUa_BadIndexRangeInvalid;
                }
            }

            if (HighBound > ((ID_VariableNode*)NodeAttributes)->Value.Value.Array.Length) {
                return OpcUa_BadIndexRangeNoData;
            }

            Elements = HighBound - LowBound + 1;
        }
    } else {
        LowBound = 0;
        HighBound = NodeAttributes->Value.Value.Array.Length - 1;
        Elements = HighBound - LowBound + 1;
    }
    
    *pNumElements = Elements;
    *pHighBound = HighBound;
    *pLowBound = LowBound;
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UA_Read(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader, OpcUa_Double nMaxAge,
                            OpcUa_TimestampsToReturn eTimestampsToReturn, OpcUa_Int32 nNoOfNodesToRead, const OpcUa_ReadValueId* pNodesToRead,
                            OpcUa_ResponseHeader* pResponseHeader, OpcUa_Int32* pNoOfResults, OpcUa_DataValue** pResults,
                            OpcUa_Int32* pNoOfDiagnosticInfos, OpcUa_DiagnosticInfo** pDiagnosticInfos) {    
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_Read");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfNodesToRead, pNodesToRead);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfResults);
    OpcUa_ReturnErrorIfArgumentNull(pResults);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfDiagnosticInfos);
    OpcUa_ReturnErrorIfArgumentNull(pDiagnosticInfos);
    *pNoOfResults = 0;
    *pResults = NULL;
    *pNoOfDiagnosticInfos = 0;
    *pDiagnosticInfos = NULL;
    
    ID_CHECK_SESSION();
    
    if (nNoOfNodesToRead == 0) {
        uStatus = OpcUa_BadNothingToDo;
        goto Error;
    }
    
    if (nMaxAge < 0) {
        uStatus = OpcUa_BadMaxAgeInvalid;
        goto Error;
    }
    
    // verify eTimestampsToReturn
    if (eTimestampsToReturn != OpcUa_TimestampsToReturn_Both &&
        eTimestampsToReturn != OpcUa_TimestampsToReturn_Server &&
        eTimestampsToReturn != OpcUa_TimestampsToReturn_Source &&
        eTimestampsToReturn != OpcUa_TimestampsToReturn_Neither) {
        uStatus = OpcUa_BadTimestampsToReturnInvalid;
        goto Error;
    }
        
    *pNoOfResults = nNoOfNodesToRead;
    OpcUa_DataValue * DataValues = malloc(sizeof(OpcUa_DataValue) * nNoOfNodesToRead);    
    // note: ignoring nMaxAge parameter
    for (int i = 0; i < nNoOfNodesToRead; i++) {
        OpcUa_DataValue_Initialize(&DataValues[i]);
        OpcUa_ReadValueId ReadRequest = pNodesToRead[i];
        OpcUa_NodeId * NodeId = &ReadRequest.NodeId;
        if (NodeId->NamespaceIndex == ID_NAMESPACE_INDEX_ALIAS) {
            NodeId = ID_Aliaser_GetOriginal(GlobalAliaserHandle, NodeId);
        }
        
        ID_AddressSpaceNode * ASNode = ID_AddressSpace_FindNodeWithID(GlobalAddressSpaceHandle, NodeId);
        if (ASNode != NULL) {
            if (ReadRequest.AttributeId >= 1 && ReadRequest.AttributeId <= 22) {
                // continue if dataencoding requested is not specified or Default Binary
                OpcUa_Boolean IsDataEncodingEmpty = OpcUa_String_GetRawString(&ReadRequest.DataEncoding.Name) == NULL;                
                if (IsDataEncodingEmpty || (strcmp("Default Binary", OpcUa_String_GetRawString(&ReadRequest.DataEncoding.Name)) == 0 && ReadRequest.DataEncoding.NamespaceIndex == 0)) {
                    
                    // check AccessLevel if variable node
                    ID_BaseAttributes * NodeAttributes = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(ASNode, ASNode->Type);
                    if (ReadRequest.AttributeId == OpcUa_Attributes_Value && NodeAttributes->NodeClass == OpcUa_NodeClass_Variable) {
                        ID_VariableNode * VarNode = (ID_VariableNode*)NodeAttributes;
                        if (VarNode->AccessLevel == 0 || VarNode->UserAccessLevel == 0 ||
                            (VarNode->AccessLevel & 1) == 0 || (VarNode->UserAccessLevel & 1) == 0) {
                            DataValues[i].StatusCode = OpcUa_BadUserAccessDenied;
                            continue;
                        }
                    }
                    
                    // copy attributes into the response as needed
                    switch (ReadRequest.AttributeId) {
                        case OpcUa_Attributes_NodeId:{
                            DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                            DataValues[i].Value.Datatype = OpcUaType_NodeId;
                            DataValues[i].Value.Value.NodeId = malloc(sizeof(OpcUa_NodeId));
                            OpcUa_NodeId_Initialize(DataValues[i].Value.Value.NodeId);
                            ID_OpcUa_NodeId_CopyTo(&NodeAttributes->NodeId, DataValues[i].Value.Value.NodeId);
                            break;}
                        case OpcUa_Attributes_NodeClass:{
                            DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                            DataValues[i].Value.Datatype = OpcUaType_Int32;
                            DataValues[i].Value.Value.Int32 = NodeAttributes->NodeClass;
                            break;}
                        case OpcUa_Attributes_BrowseName:{
                            DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                            DataValues[i].Value.Datatype = OpcUaType_QualifiedName;
                            DataValues[i].Value.Value.QualifiedName = malloc(sizeof(OpcUa_QualifiedName));
                            OpcUa_QualifiedName_Initialize(DataValues[i].Value.Value.QualifiedName);
                            
                            DataValues[i].Value.Value.QualifiedName->NamespaceIndex = NodeAttributes->BrowseName.NamespaceIndex;
                            ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&DataValues[i].Value.Value.QualifiedName->Name, &NodeAttributes->BrowseName.Name);
                            break;}
                        case OpcUa_Attributes_DisplayName:{
                            DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                            DataValues[i].Value.Datatype = OpcUaType_LocalizedText;
                            DataValues[i].Value.Value.LocalizedText = malloc(sizeof(OpcUa_LocalizedText));
                            OpcUa_LocalizedText_Initialize(DataValues[i].Value.Value.LocalizedText);
                            
                            ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&DataValues[i].Value.Value.LocalizedText->Locale, &NodeAttributes->DisplayName.Locale);
                            ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&DataValues[i].Value.Value.LocalizedText->Text, &NodeAttributes->DisplayName.Text);
                            break;}
                        case OpcUa_Attributes_Description:{
                        case OpcUa_Attributes_UserWriteMask:
                            // need to support description attribute at some point in time...
                            DataValues[i].StatusCode = OpcUa_BadNotReadable;
                            continue;
                            break;}
                        case OpcUa_Attributes_WriteMask:{
                            DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                            DataValues[i].Value.Datatype = OpcUaType_UInt32;
                            DataValues[i].Value.Value.UInt32 = NodeAttributes->WriteMask;
                            break;}
                        case OpcUa_Attributes_IsAbstract:{
                            if (ASNode->Type == ID_NodeType_DataType || ASNode->Type == ID_NodeType_ReferenceType || ASNode->Type == ID_NodeType_ObjectType || ASNode->Type == ID_NodeType_VariableType) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Boolean;
                                if (ASNode->Type == ID_NodeType_VariableType) {
                                    DataValues[i].Value.Value.Boolean = ((ID_VariableTypeNode*)NodeAttributes)->IsAbstract;
                                } else {
                                    // force cast as alignment same
                                    DataValues[i].Value.Value.Boolean = ((ID_DataTypeNode*)NodeAttributes)->IsAbstract;
                                }
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_Symmetric:{
                            if (ASNode->Type == ID_NodeType_ReferenceType) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Boolean;
                                DataValues[i].Value.Value.Boolean = ((ID_ReferenceTypeNode*)NodeAttributes)->Symmetric;
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_InverseName:{
                            if (ASNode->Type == ID_NodeType_ReferenceType) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_LocalizedText;
                                DataValues[i].Value.Value.LocalizedText = malloc(sizeof(OpcUa_LocalizedText));
                                OpcUa_LocalizedText_Initialize(DataValues[i].Value.Value.LocalizedText);
                                
                                ID_ReferenceTypeNode * RefTypeNode = (ID_ReferenceTypeNode*)NodeAttributes;
                                ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&DataValues[i].Value.Value.LocalizedText->Locale, &RefTypeNode->InverseName.Locale);
                                ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&DataValues[i].Value.Value.LocalizedText->Text, &RefTypeNode->InverseName.Text);
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_ContainsNoLoops:{
                            if (ASNode->Type == ID_NodeType_View) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Boolean;
                                DataValues[i].Value.Value.Boolean = ((ID_ViewNode*)NodeAttributes)->ContainsNoLoops;
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_EventNotifier:{
                            if (ASNode->Type == ID_NodeType_View || ASNode->Type == ID_NodeType_Object) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Byte;
                                DataValues[i].Value.Value.Byte = ((ID_ViewNode*)NodeAttributes)->EventNotifier; // run a force cast
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_Value:{
                            if (ASNode->Type == ID_NodeType_Variable || ASNode->Type == ID_NodeType_VariableType) {
                                ID_VariableNode * CastedVariable = (ID_VariableNode*)NodeAttributes;

                                // deny reading if not scalar or array
                                if (CastedVariable->Value.ArrayType != OpcUa_VariantArrayType_Scalar &&
                                    CastedVariable->Value.ArrayType != OpcUa_VariantArrayType_Array) {
                                    DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                    continue;
                                }
                                
                                DataValues[i].Value.ArrayType = CastedVariable->Value.ArrayType;
                                DataValues[i].Value.Datatype = CastedVariable->Value.Datatype;
                                if (CastedVariable->Value.ArrayType == OpcUa_VariantArrayType_Scalar) {
                                    ID_AddressSpace_InitializeVariant(&DataValues[i].Value);
                                    ID_AddressSpace_CopyVariant(&CastedVariable->Value, &DataValues[i].Value);
                                } else if (CastedVariable->Value.ArrayType == OpcUa_VariantArrayType_Array) {
                                    // find the range of interest
                                    char * RangeStr = OpcUa_String_GetRawString(&ReadRequest.IndexRange);
                                    int LowBound = 0;
                                    int HighBound = 0;
                                    int Elements = 0;
                                    OpcUa_StatusCode code = ID_ConstructRange(RangeStr, &LowBound, &HighBound, &Elements, (ID_VariableNode*)NodeAttributes);
                                    if (code != OpcUa_Good) {
                                        DataValues[i].StatusCode = code;
                                        continue;
                                    }

                                    // copy variant portions into the result set
                                    DataValues[i].Value.Value.Array.Length = Elements;
                                    ID_AddressSpace_InitializeVariant(&DataValues[i].Value);
                                    int curTargetIndex = 0;
                                    for (int c = LowBound; c <= HighBound; c++) {
                                        ID_AddressSpace_CopyVariantArray(&CastedVariable->Value, c, &DataValues[i].Value, curTargetIndex);
                                        curTargetIndex++;
                                    }
                                }
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            
                            // setup timestamp values
                            OpcUa_DateTime Time = OPCUA_P_DATETIME_UTCNOW();
                            if (eTimestampsToReturn == OpcUa_TimestampsToReturn_Both || eTimestampsToReturn == OpcUa_TimestampsToReturn_Server) {
                                DataValues[i].ServerTimestamp.dwHighDateTime = Time.dwHighDateTime;
                                DataValues[i].ServerTimestamp.dwLowDateTime = Time.dwLowDateTime;
                            }
        
                            if (eTimestampsToReturn == OpcUa_TimestampsToReturn_Both || eTimestampsToReturn == OpcUa_TimestampsToReturn_Source) {
                                DataValues[i].SourceTimestamp.dwHighDateTime = Time.dwHighDateTime;
                                DataValues[i].SourceTimestamp.dwLowDateTime = Time.dwLowDateTime;
                            }
                            break;}
                        case OpcUa_Attributes_DataType:{
                            if (ASNode->Type == ID_NodeType_Variable || ASNode->Type == ID_NodeType_VariableType) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_NodeId;
                                DataValues[i].Value.Value.NodeId = malloc(sizeof(OpcUa_NodeId));
                                OpcUa_NodeId_Initialize(DataValues[i].Value.Value.NodeId);
                                ID_OpcUa_NodeId_CopyTo(&((ID_VariableNode*)NodeAttributes)->DataType, DataValues[i].Value.Value.NodeId);
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_ValueRank:{
                            if (ASNode->Type == ID_NodeType_Variable || ASNode->Type == ID_NodeType_VariableType) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Int32;
                                DataValues[i].Value.Value.Int32 = ((ID_VariableNode*)NodeAttributes)->ValueRank;
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_ArrayDimensions:{
                            if (ASNode->Type == ID_NodeType_Variable || ASNode->Type == ID_NodeType_VariableType) {
                                if (((ID_VariableNode*)NodeAttributes)->ValueRank <= 0) {
                                    // need to set to NULL upon read request
                                    DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                    DataValues[i].Value.Datatype = OpcUaType_Null;
                                } else {
                                    DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Array;
                                    DataValues[i].Value.Datatype = OpcUaType_UInt32;
                                    DataValues[i].Value.Value.Array.Length = ((ID_VariableNode*)NodeAttributes)->NoOfArrayDimensions;
                                    if (DataValues[i].Value.Value.Array.Length > 0) {
                                        DataValues[i].Value.Value.Array.Value.UInt32Array = malloc(sizeof(OpcUa_UInt32) * DataValues[i].Value.Value.Array.Length);
                                        memcpy(DataValues[i].Value.Value.Array.Value.UInt32Array, ((ID_VariableNode*)NodeAttributes)->ArrayDimensions, sizeof(OpcUa_UInt32) * DataValues[i].Value.Value.Array.Length);
                                    }
                                }
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_AccessLevel:{
                            if (ASNode->Type == ID_NodeType_Variable) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Byte;
                                DataValues[i].Value.Value.Boolean = ((ID_VariableNode*)NodeAttributes)->AccessLevel;
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_UserAccessLevel:{
                            if (ASNode->Type == ID_NodeType_Variable) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Byte;
                                DataValues[i].Value.Value.Boolean = ((ID_VariableNode*)NodeAttributes)->UserAccessLevel;
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_MinimumSamplingInterval:{
                            if (ASNode->Type == ID_NodeType_Variable) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Double;
                                DataValues[i].Value.Value.Double = ((ID_VariableNode*)NodeAttributes)->MinimumSamplingInterval;
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_Historizing:{
                            if (ASNode->Type == ID_NodeType_Variable) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Boolean;
                                DataValues[i].Value.Value.Boolean = ((ID_VariableNode*)NodeAttributes)->Historizing;
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_Executable:{
                            if (ASNode->Type == ID_NodeType_Method) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Boolean;
                                DataValues[i].Value.Value.Boolean = ((ID_MethodNode*)NodeAttributes)->Executable;
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        case OpcUa_Attributes_UserExecutable:{
                            if (ASNode->Type == ID_NodeType_Method) {
                                DataValues[i].Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                DataValues[i].Value.Datatype = OpcUaType_Boolean;
                                DataValues[i].Value.Value.Boolean = ((ID_MethodNode*)NodeAttributes)->UserExecutable;
                            } else {
                                DataValues[i].StatusCode = OpcUa_BadNotReadable;
                                continue;
                            }
                            break;}
                        default:{
                            DataValues[i].StatusCode = OpcUa_BadAttributeIdInvalid;
                            continue;
                            break;}
                    }
                    
                    DataValues[i].StatusCode = OpcUa_Good;
                } else {
                    DataValues[i].StatusCode = OpcUa_BadDataEncodingUnsupported;
                }
            } else {
                DataValues[i].StatusCode = OpcUa_BadAttributeIdInvalid;
            }
        } else {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "Unknown NodeId ns=%d;i=%d\n", ReadRequest.NodeId.NamespaceIndex, ReadRequest.NodeId.Identifier.Numeric);
            DataValues[i].StatusCode = OpcUa_BadNodeIdUnknown;
        }
    }
    
    *pResults = DataValues;
    
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return OpcUa_Good;
}

OpcUa_Boolean ID_CheckWriteMaskValidity(OpcUa_UInt32 WriteMask, OpcUa_Int32 AttributeId) {
    switch (AttributeId) {
        case OpcUa_Attributes_NodeId:
            return (WriteMask & 16384) != 0;
        case OpcUa_Attributes_NodeClass:
            return (WriteMask & 8192) != 0;
        case OpcUa_Attributes_BrowseName:
            return (WriteMask & 4) != 0;
        case OpcUa_Attributes_DisplayName:
            return (WriteMask & 64) != 0;
        case OpcUa_Attributes_Description:
            return (WriteMask & 32) != 0;
        case OpcUa_Attributes_WriteMask:
            return (WriteMask & 1048576) != 0;
        case OpcUa_Attributes_UserWriteMask:
            return (WriteMask & 262144) != 0;
        case OpcUa_Attributes_IsAbstract:
            return (WriteMask & 2048) != 0;
        case OpcUa_Attributes_Symmetric:
            return (WriteMask & 32768) != 0;
        case OpcUa_Attributes_InverseName:
            return (WriteMask & 1024) != 0;
        case OpcUa_Attributes_ContainsNoLoops:
            return (WriteMask & 8) != 0;
        case OpcUa_Attributes_EventNotifier:
            return (WriteMask & 128) != 0;
        case OpcUa_Attributes_Value: // only applies to variable type
            return (WriteMask & 2097152) != 0;
        case OpcUa_Attributes_DataType:
            return (WriteMask & 16) != 0;
        case OpcUa_Attributes_ValueRank:
            return (WriteMask & 524288) != 0;
        case OpcUa_Attributes_ArrayDimensions:
            return (WriteMask & 2) != 0;
        case OpcUa_Attributes_AccessLevel:
            return (WriteMask & 1) != 0;
        case OpcUa_Attributes_UserAccessLevel:
            return (WriteMask & 65536) != 0;
        case OpcUa_Attributes_MinimumSamplingInterval:
            return (WriteMask & 4096) != 0;
        case OpcUa_Attributes_Historizing:
            return (WriteMask & 512) != 0;
        case OpcUa_Attributes_Executable:
            return (WriteMask & 256) != 0;
        case OpcUa_Attributes_UserExecutable:
            return (WriteMask & 131072) != 0;
            break;
        default:
            break;
    }
    return OpcUa_False;
}

OpcUa_StatusCode ID_CheckWriteTypeValidity(OpcUa_WriteValue WriteValue, ID_VariableNode * VarNode) {
    if (WriteValue.Value.Value.Datatype == OpcUaType_ExtensionObject) {
        // get a pointer to EncodeableType
        OpcUa_EncodeableType * Type = NULL;
        OpcUa_StatusCode uStatus = OpcUa_Good;
        if (WriteValue.Value.Value.ArrayType == OpcUa_VariantArrayType_Scalar) {
            Type = WriteValue.Value.Value.Value.ExtensionObject->Body.EncodeableObject.Type;
        } else if (WriteValue.Value.Value.ArrayType == OpcUa_VariantArrayType_Array) {
            Type = WriteValue.Value.Value.Value.Array.Value.ExtensionObjectArray[0].Body.EncodeableObject.Type;
        }
        
        // setup the OpcUa_NodeId namespaceindex by defaulting to 0 or using the NamespaceUri
        OpcUa_NodeId RequestTypeId;
        RequestTypeId.IdentifierType = OpcUa_IdType_Numeric;
        RequestTypeId.Identifier.Numeric = Type->TypeId;
        if (Type->NamespaceUri == NULL) {
            RequestTypeId.NamespaceIndex = 0;
        } else {
            OpcUa_String * NamespaceUri = malloc(sizeof(OpcUa_String));
            OpcUa_String_Initialize(NamespaceUri);
            OpcUa_String_AttachReadOnly(NamespaceUri, Type->NamespaceUri);
            uStatus = OpcUa_StringTable_FindIndex(&OpcUa_ProxyStub_g_NamespaceUris, NamespaceUri, (OpcUa_Int32*)&RequestTypeId.NamespaceIndex);
            OpcUa_String_Clear(NamespaceUri);
            free(NamespaceUri);
        }

        if (OpcUa_IsBad(uStatus)) {
            return OpcUa_BadDataTypeIdUnknown;
        }

        // make sure that writing extensionobject
        if (WriteValue.Value.Value.Datatype != OpcUaType_ExtensionObject) {
            return OpcUa_BadWriteNotSupported;
        }

        // make sure that types are correct
        if (ID_AddressSpace_IsTypeASubtypeofB(GlobalAddressSpaceHandle, &RequestTypeId, &VarNode->DataType) == OpcUa_False) {
            return OpcUa_BadTypeMismatch;
        }
        
        return OpcUa_Good;
    }
    
    // check subtypes OR variant datatype as needed
    OpcUa_NodeId IntegerId = ID_OPCNodeId(OpcUaId_Integer);
    OpcUa_NodeId UIntegerId = ID_OPCNodeId(OpcUaId_UInteger);
    if (ID_AddressSpace_AreIdsEqual(&VarNode->DataType, &IntegerId)) {
        switch (WriteValue.Value.Value.Datatype) {
            case OpcUaType_SByte:
            case OpcUaType_Int16:
            case OpcUaType_Int32:
            case OpcUaType_Int64:
                break;
            default:
                return OpcUa_BadTypeMismatch;
        }
    } else if (ID_AddressSpace_AreIdsEqual(&VarNode->DataType, &UIntegerId)) {
        switch (WriteValue.Value.Value.Datatype) {
            case OpcUaType_Byte:
            case OpcUaType_UInt16:
            case OpcUaType_UInt32:
            case OpcUaType_UInt64:
                break;
            default:
                return OpcUa_BadTypeMismatch;
        }
    } else if (VarNode->Value.Datatype != WriteValue.Value.Value.Datatype) {
        return OpcUa_BadTypeMismatch;
    }
    
    return OpcUa_Good;
}

OpcUa_StatusCode ID_UA_Write(OpcUa_Endpoint hEndpoint, OpcUa_Handle hContext, const OpcUa_RequestHeader* pRequestHeader,
                             OpcUa_Int32 nNoOfNodesToWrite, const OpcUa_WriteValue* pNodesToWrite, OpcUa_ResponseHeader* pResponseHeader,
                             OpcUa_Int32* pNoOfResults, OpcUa_StatusCode** pResults, OpcUa_Int32* pNoOfDiagnosticInfos, 
                             OpcUa_DiagnosticInfo** pDiagnosticInfos) {
    
    OpcUa_InitializeStatus(OpcUa_Module_Server, "UA_Write");
    OpcUa_ReturnErrorIfArgumentNull(hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(hContext);
    OpcUa_ReturnErrorIfArgumentNull(pRequestHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(nNoOfNodesToWrite, pNodesToWrite);
    OpcUa_ReturnErrorIfArgumentNull(pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfResults);
    OpcUa_ReturnErrorIfArgumentNull(pResults);
    OpcUa_ReturnErrorIfArgumentNull(pNoOfDiagnosticInfos);
    OpcUa_ReturnErrorIfArgumentNull(pDiagnosticInfos);
    *pNoOfResults = 0;
    *pResults = NULL;
    *pNoOfDiagnosticInfos = 0;
    *pDiagnosticInfos = NULL;
    
    ID_CHECK_SESSION();
    
    if (nNoOfNodesToWrite == 0) {
        uStatus = OpcUa_BadNothingToDo;
        goto Error;
    }
    
    *pNoOfResults = nNoOfNodesToWrite;
    OpcUa_StatusCode * ResultCodes = malloc(sizeof(OpcUa_StatusCode) * nNoOfNodesToWrite);
    for (int i = 0; i < nNoOfNodesToWrite; i++) {
        OpcUa_StatusCode_Initialize(&ResultCodes[i]);
        OpcUa_WriteValue WriteValue = pNodesToWrite[i];
        OpcUa_NodeId * NodeId = &WriteValue.NodeId;
        if (WriteValue.NodeId.NamespaceIndex == ID_NAMESPACE_INDEX_ALIAS) {
            NodeId = ID_Aliaser_GetOriginal(GlobalAliaserHandle, NodeId);
        }
        
        ID_AddressSpaceNode * Node = ID_AddressSpace_FindNodeWithID(GlobalAddressSpaceHandle, NodeId);
        if (Node != NULL) {
            if (WriteValue.AttributeId >= 1 && WriteValue.AttributeId <= 22) {
                // check writemask or AccessLevel depending on Variable/VariableType
                ID_BaseAttributes * NodeAttributes = (ID_BaseAttributes*)ID_AddressSpace_AccessNodeBody(Node, Node->Type);
                if (WriteValue.AttributeId != OpcUa_Attributes_Value || Node->Type == ID_NodeType_VariableType) {
                    if (ID_CheckWriteMaskValidity(NodeAttributes->WriteMask, WriteValue.AttributeId) == OpcUa_False) {
                        ResultCodes[i] = OpcUa_BadNotWritable;
                        continue;
                    }
                } else if ((Node->Type == ID_NodeType_Variable && (Node->Node.VarNode.UserAccessLevel & 2) == 0) ||
                           Node->Type != ID_NodeType_Variable) {
                    ResultCodes[i] = OpcUa_BadNotWritable;
                    continue;
                }
                
                // Write all other attributes except for Value here
                if (WriteValue.AttributeId != OpcUa_Attributes_Value) {
                    // check that type length is correct (scalar OR not SCALAR)
                    if (WriteValue.Value.Value.ArrayType == OpcUa_VariantArrayType_Scalar || 
                       (WriteValue.Value.Value.ArrayType == OpcUa_VariantArrayType_Array && WriteValue.AttributeId == OpcUa_Attributes_ArrayDimensions)) {
                        switch (WriteValue.AttributeId) {
                            case OpcUa_Attributes_NodeId:{
                                if (WriteValue.Value.Value.Datatype == OpcUaType_NodeId) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ID_OpcUa_NodeId_CopyTo(WriteValue.Value.Value.Value.NodeId, &NodeAttributes->NodeId);
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_NodeClass:{
                                if (WriteValue.Value.Value.Datatype == OpcUaType_Int32) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    NodeAttributes->NodeClass = WriteValue.Value.Value.Value.Int32;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_BrowseName:{
                                // write browsename depending on supplied type
                                if (WriteValue.Value.Value.Datatype == OpcUaType_QualifiedName) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    OpcUa_QualifiedName_Clear(&NodeAttributes->BrowseName);
                                    NodeAttributes->BrowseName.NamespaceIndex = WriteValue.Value.Value.Value.QualifiedName->NamespaceIndex;
                                    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&NodeAttributes->BrowseName.Name, &WriteValue.Value.Value.Value.QualifiedName->Name);
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                } else if (WriteValue.Value.Value.Datatype == OpcUaType_String) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    OpcUa_String_Clear(&NodeAttributes->BrowseName.Name);
                                    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&NodeAttributes->BrowseName.Name, &WriteValue.Value.Value.Value.String);
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_DisplayName:{
                                if (WriteValue.Value.Value.Datatype == OpcUaType_LocalizedText) {
                                    // make sure that supplied locale is valid
                                    char * localeSupplied = OpcUa_String_GetRawString(&WriteValue.Value.Value.Value.LocalizedText->Locale);
                                    if (localeSupplied == NULL ||
                                        strcmp(localeSupplied, ID_APPLICATION_LOCALE) == 0 ||
                                        strcmp(localeSupplied, ID_APPLICATION_LOCALE_LINGUAL) == 0) {

                                        OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                        OpcUa_LocalizedText_Clear(&NodeAttributes->DisplayName);
                                        ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&NodeAttributes->DisplayName.Locale, &WriteValue.Value.Value.Value.LocalizedText->Locale);
                                        ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&NodeAttributes->DisplayName.Text, &WriteValue.Value.Value.Value.LocalizedText->Text);
                                        ResultCodes[i] = OpcUa_Good;
                                        OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                        continue;
                                    }
                                    
                                    ResultCodes[i] = OpcUa_BadWriteNotSupported;
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_Description:{
                            case OpcUa_Attributes_UserWriteMask:
                                // function not implemented
                                break;}
                            case OpcUa_Attributes_WriteMask:{
                                if (WriteValue.Value.Value.Datatype == OpcUaType_UInt32) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    NodeAttributes->WriteMask = WriteValue.Value.Value.Value.UInt32;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_IsAbstract:{
                                if (WriteValue.Value.Value.Datatype == OpcUaType_Boolean) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    if (Node->Type == ID_NodeType_VariableType) {
                                        ((ID_VariableTypeNode*)NodeAttributes)->IsAbstract = WriteValue.Value.Value.Value.Boolean;
                                    } else {
                                        ((ID_DataTypeNode*)NodeAttributes)->IsAbstract = WriteValue.Value.Value.Value.Boolean;
                                    }
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_Symmetric:{
                                if (WriteValue.Value.Value.Datatype == OpcUaType_Boolean && Node->Type == ID_NodeType_ReferenceType) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ((ID_ReferenceTypeNode*)NodeAttributes)->Symmetric = WriteValue.Value.Value.Value.Boolean;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_InverseName:{
                                if (WriteValue.Value.Value.Datatype == OpcUaType_LocalizedText && Node->Type == ID_NodeType_ReferenceType) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    OpcUa_LocalizedText_Clear(&((ID_ReferenceTypeNode*)NodeAttributes)->InverseName);
                                    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&((ID_ReferenceTypeNode*)NodeAttributes)->InverseName.Locale, &WriteValue.Value.Value.Value.LocalizedText->Locale);
                                    ID_SAFE_STRING_COPY_TO_TARGET_FROM_SOURCE(&((ID_ReferenceTypeNode*)NodeAttributes)->InverseName.Text, &WriteValue.Value.Value.Value.LocalizedText->Text);
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_ContainsNoLoops:{
                                if (WriteValue.Value.Value.Datatype == OpcUaType_Boolean && Node->Type == ID_NodeType_View) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ((ID_ViewNode*)NodeAttributes)->ContainsNoLoops = WriteValue.Value.Value.Value.Boolean;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_EventNotifier:{
                                if (WriteValue.Value.Value.Datatype == OpcUaType_Byte && (Node->Type == ID_NodeType_View || Node->Type == ID_NodeType_Object)) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ((ID_ObjectNode*)NodeAttributes)->EventNotifier = WriteValue.Value.Value.Value.Byte;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_DataType: {
                                if (Node->Type == ID_NodeType_VariableType || Node->Type == ID_NodeType_Variable) {
                                    if (WriteValue.Value.Value.Datatype == OpcUaType_NodeId) {
                                        OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);

                                        // requires making an extra call to Write() in order to actually
                                        // update the data contents contained within an ExtensionObject()
                                        ID_VariableNode * VarNode = (ID_VariableNode*)NodeAttributes;
                                        ID_OpcUa_NodeId_CopyTo(WriteValue.Value.Value.Value.NodeId, &VarNode->DataType);
                                        ResultCodes[i] = OpcUa_Good;
                                        OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                        continue;
                                    }
                                }
                                break;}
                            case OpcUa_Attributes_ValueRank:{
                                if (Node->Type == ID_NodeType_VariableType || Node->Type == ID_NodeType_Variable) {
                                    if (WriteValue.Value.Value.Datatype == OpcUaType_Int32) {
                                        if (WriteValue.Value.Value.Value.Int32 != OpcUa_ValueRanks_OneDimension &&
                                            WriteValue.Value.Value.Value.Int32 != OpcUa_ValueRanks_Scalar) {
                                            ResultCodes[i] = OpcUa_BadWriteNotSupported;
                                            continue;
                                        }
                                        
                                        OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                        ID_VariableNode * VarNode = (ID_VariableNode*)NodeAttributes;
                                        // check if valueranks has changed
                                        // user's responsibility to fill new data responsibly
                                        if (VarNode->ValueRank != WriteValue.Value.Value.Value.Int32) {
                                            // if becoming scalar, WIPE ARRAY and RESET VARIANT
                                            if (WriteValue.Value.Value.Value.Int32 == OpcUa_ValueRanks_Scalar) {
                                                for (int element = 0; element < VarNode->Value.Value.Array.Length; element++) {
                                                    ID_AddressSpace_EmptyVariantArray(&VarNode->Value, element);
                                                }
                                                
                                                void * ArrayPtr = VarNode->Value.Value.Array.Value.Array;
                                                VarNode->Value.Value.Array.Length = 0;
                                                free(ArrayPtr);
                                                VarNode->Value.Value.Array.Value.Array = NULL;
                                                
                                                VarNode->Value.ArrayType = OpcUa_VariantArrayType_Scalar;
                                                ID_AddressSpace_InitializeVariant(&VarNode->Value);
                                                if (VarNode->ArrayDimensions != NULL) {
                                                    free(VarNode->ArrayDimensions);
                                                    VarNode->NoOfArrayDimensions = 0;
                                                    VarNode->ArrayDimensions = NULL;
                                                }
                                            } else if (WriteValue.Value.Value.Value.Int32 == OpcUa_ValueRanks_OneDimension) {
                                                if (VarNode->ArrayDimensions == NULL) {
                                                    VarNode->ArrayDimensions = malloc(sizeof(OpcUa_UInt32));
                                                    VarNode->NoOfArrayDimensions = 1;
                                                }
                                                
                                                VarNode->ArrayDimensions[0] = 1;
                                                OpcUa_Byte VariantType = VarNode->Value.Datatype;
                                                OpcUa_Variant_Clear(&VarNode->Value);
                                                
                                                VarNode->Value.Datatype = VariantType;
                                                VarNode->Value.ArrayType = OpcUa_VariantArrayType_Array;
                                                VarNode->Value.Value.Array.Length = 1;
                                                ID_AddressSpace_InitializeVariant(&VarNode->Value);
                                            }
                                        }
                                        
                                        VarNode->ValueRank = WriteValue.Value.Value.Value.Int32;                                        
                                        ResultCodes[i] = OpcUa_Good;
                                        OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                        continue;
                                    }
                                }
                                break;}
                            case OpcUa_Attributes_ArrayDimensions:{
                                if (Node->Type == ID_NodeType_Variable || Node->Type == ID_NodeType_VariableType) {
                                    if (WriteValue.Value.Value.Datatype == OpcUaType_UInt32 && WriteValue.Value.Value.ArrayType == OpcUa_VariantArrayType_Array) {
                                        if (WriteValue.Value.Value.Value.Array.Length != 1) {
                                            ResultCodes[i] = OpcUa_BadWriteNotSupported; // only support unidimensional array
                                            continue;
                                        }

                                        if (WriteValue.Value.Value.Value.Array.Value.UInt32Array[0] < 0 ||
                                            WriteValue.Value.Value.Value.Array.Value.UInt32Array[0] >= OpcUa_Int32_Max) {
                                            ResultCodes[i] = OpcUa_BadWriteNotSupported;
                                            continue;
                                        }
                                        
                                        OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                        // enlarge or shrink the arraydimensions as needed
                                        ID_VariableNode * VarNode = (ID_VariableNode*)NodeAttributes;
                                        VarNode->NoOfArrayDimensions = WriteValue.Value.Value.Value.Array.Length;
                                        VarNode->ArrayDimensions = realloc(VarNode->ArrayDimensions, sizeof(OpcUa_UInt32) * VarNode->NoOfArrayDimensions);
                                        memcpy(VarNode->ArrayDimensions, WriteValue.Value.Value.Value.Array.Value.UInt32Array, sizeof(OpcUa_UInt32) * VarNode->NoOfArrayDimensions);
                                        if ((OpcUa_Int32)VarNode->ArrayDimensions[0] > VarNode->Value.Value.Array.Length) {
                                            int expansion = VarNode->ArrayDimensions[0] - VarNode->Value.Value.Array.Length;
                                            ID_AddressSpace_ExpandVariantArray(&VarNode->Value, expansion);
                                        } else if ((OpcUa_Int32)VarNode->ArrayDimensions[0] < VarNode->Value.Value.Array.Length) {
                                            int shrink = VarNode->Value.Value.Array.Length - VarNode->ArrayDimensions[0];
                                            ID_AddressSpace_ShrinkVariantArray(&VarNode->Value, shrink);
                                        }
                                        
                                        ResultCodes[i] = OpcUa_Good;
                                        OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                        continue;
                                    }
                                }
                                break;}
                            case OpcUa_Attributes_AccessLevel:{
                                if (Node->Type == ID_NodeType_Variable && WriteValue.Value.Value.Datatype == OpcUaType_Byte) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ((ID_VariableNode*)NodeAttributes)->AccessLevel = WriteValue.Value.Value.Value.Byte;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_UserAccessLevel:{
                                if (Node->Type == ID_NodeType_Variable && WriteValue.Value.Value.Datatype == OpcUaType_Byte) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ((ID_VariableNode*)NodeAttributes)->UserAccessLevel = WriteValue.Value.Value.Value.Byte;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_MinimumSamplingInterval:{
                                if (Node->Type == ID_NodeType_Variable && WriteValue.Value.Value.Datatype == OpcUaType_Double) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ((ID_VariableNode*)NodeAttributes)->MinimumSamplingInterval = WriteValue.Value.Value.Value.Double;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_Historizing:{
                                if (Node->Type == ID_NodeType_Variable && WriteValue.Value.Value.Datatype == OpcUaType_Boolean) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ((ID_VariableNode*)NodeAttributes)->Historizing = WriteValue.Value.Value.Value.Boolean;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_Executable:{
                                if (Node->Type == ID_NodeType_Method && WriteValue.Value.Value.Datatype == OpcUaType_Boolean) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ((ID_MethodNode*)NodeAttributes)->Executable = WriteValue.Value.Value.Value.Boolean;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            case OpcUa_Attributes_UserExecutable:{
                                if (Node->Type == ID_NodeType_Method && WriteValue.Value.Value.Datatype == OpcUaType_Boolean) {
                                    OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                    ((ID_MethodNode*)NodeAttributes)->UserExecutable = WriteValue.Value.Value.Value.Boolean;
                                    ResultCodes[i] = OpcUa_Good;
                                    OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                    continue;
                                }
                                break;}
                            default:
                                break;
                        }
                    }
                } else {
                    if (Node->Type == ID_NodeType_VariableType || Node->Type == ID_NodeType_Variable) {
                        if (WriteValue.Value.Value.ArrayType == OpcUa_VariantArrayType_Scalar && ((ID_VariableNode*)NodeAttributes)->ValueRank == OpcUa_ValueRanks_Scalar) {
                            if (WriteValue.Value.Value.Datatype != OpcUaType_ExtensionObject ||
                               (WriteValue.Value.Value.Value.ExtensionObject != NULL && 
                                WriteValue.Value.Value.Value.ExtensionObject->Encoding == OpcUa_ExtensionObjectEncoding_EncodeableObject)) {
                                
                                // check validity of write
                                ID_VariableNode * VarNode = (ID_VariableNode*)NodeAttributes;
                                OpcUa_StatusCode code = ID_CheckWriteTypeValidity(WriteValue, VarNode);
                                if (code != OpcUa_Good) {
                                    ResultCodes[i] = code;
                                    continue;
                                }
                                
                                // write the variant in
                                OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                code = ID_AddressSpace_CopyVariant(&WriteValue.Value.Value, &((ID_VariableNode*)NodeAttributes)->Value);
                                ResultCodes[i] = code;
                                OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                continue;
                            }
                        } else if (WriteValue.Value.Value.ArrayType == OpcUa_VariantArrayType_Array && ((ID_VariableNode*)NodeAttributes)->ValueRank == OpcUa_ValueRanks_OneDimension) {
                            // compute the range if necessary
                            char * RangeStr = OpcUa_String_GetRawString(&WriteValue.IndexRange);
                            int LowBound = 0; 
                            int HighBound = 0; 
                            int Elements = 0;
                            OpcUa_StatusCode code = ID_ConstructRange(RangeStr, &LowBound, &HighBound, &Elements, (ID_VariableNode*)NodeAttributes);
                            if (code != OpcUa_Good) {
                                ResultCodes[i] = code;
                                continue;
                            }
                            
                            // currently the algorithm can handle differing structural types
                            // however, the current algorithm cannot handle changing of the data type that
                            // are not based upon the ExtensionObject schema
                            if (WriteValue.Value.Value.Value.Array.Length != 0 &&
                               (WriteValue.Value.Value.Datatype != OpcUaType_ExtensionObject ||
                                (WriteValue.Value.Value.Value.Array.Value.ExtensionObjectArray != NULL && 
                                 WriteValue.Value.Value.Value.Array.Value.ExtensionObjectArray[0].Encoding == OpcUa_ExtensionObjectEncoding_EncodeableObject))) {
                                 
                                ID_VariableNode * VarNode = (ID_VariableNode*)NodeAttributes;
                                code = ID_CheckWriteTypeValidity(WriteValue, VarNode);
                                if (code != OpcUa_Good) {
                                    ResultCodes[i] = code;
                                    continue;
                                }

                                OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                                
                                // dump the existing variant element
                                // write the new variant element directly in
                                int curTargetIndex = 0;
                                OpcUa_Boolean isCopySuccessful = OpcUa_True;
                                for (int c = LowBound; c <= HighBound; c++) {
                                    code = OpcUa_Good;
                                    ID_AddressSpace_EmptyVariantArray(&((ID_VariableNode*)NodeAttributes)->Value, c);
                                    code = ID_AddressSpace_CopyVariantArray(&WriteValue.Value.Value, curTargetIndex, &((ID_VariableNode*)NodeAttributes)->Value, c);
                                    curTargetIndex++;
                                    
                                    if (OpcUa_IsBad(code)) {
                                        ResultCodes[i] = code;
                                        isCopySuccessful = OpcUa_False;
                                        break;
                                    }
                                }
                                
                                OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                                if (isCopySuccessful == OpcUa_False) {
                                    continue;
                                }

                                ResultCodes[i] = OpcUa_Good;
                                continue;
                            }
                        } else if (WriteValue.Value.Value.Datatype == OpcUaType_ByteString &&
                                   WriteValue.Value.Value.ArrayType == OpcUa_VariantArrayType_Scalar &&
                                   ((ID_VariableNode*)NodeAttributes)->Value.Datatype == OpcUaType_Byte &&
                                   ((ID_VariableNode*)NodeAttributes)->Value.ArrayType == OpcUa_VariantArrayType_Array) {
                            // bytestring writing into byte[]
                            OPCUA_P_MUTEX_LOCK(NodeAttributes->NodeMutex);
                            ID_VariableNode * VarNode = (ID_VariableNode*)NodeAttributes;
                            if (VarNode->Value.Value.Array.Length > 0) {
                                free(VarNode->Value.Value.Array.Value.ByteArray);
                            }

                            VarNode->Value.Value.Array.Length = WriteValue.Value.Value.Value.ByteString.Length;
                            VarNode->Value.Value.Array.Value.ByteArray = malloc(sizeof(OpcUa_Byte) * VarNode->Value.Value.Array.Length);
                            memcpy(VarNode->Value.Value.Array.Value.ByteArray, WriteValue.Value.Value.Value.ByteString.Data, sizeof(OpcUa_Byte) * VarNode->Value.Value.Array.Length);
                            ResultCodes[i] = OpcUa_Good;
                            OPCUA_P_MUTEX_UNLOCK(NodeAttributes->NodeMutex);
                            continue;
                        } 
                    }
                }
                
                ResultCodes[i] = OpcUa_BadTypeMismatch;
            } else {
                ResultCodes[i] = OpcUa_BadAttributeIdInvalid;
            }
        } else {
            ResultCodes[i] = OpcUa_BadNodeIdUnknown;
        }
    }
    
    *pResults = ResultCodes;
    Error: ;
    ID_UA_InitializeResponseHeader(pResponseHeader, pRequestHeader, uStatus);
    return uStatus;
}