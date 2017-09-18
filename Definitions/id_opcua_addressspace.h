/*
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ID_OPCUA_ADDRESSSPACE_H
#define ID_OPCUA_ADDRESSSPACE_H

#ifdef __cplusplus
extern "C" {
#endif

#define ID_NAMESPACE_INDEX_OPC          0
#define ID_NAMESPACE_OPC_URI            "http://opcfoundation.org/UA/"

#define ID_NAMESPACE_INDEX              ID_OPCUA_DATABASE_NAMESPACE_ID // must exist on the same namespace as the database functions
#define ID_NAMESPACE_ID_URI             ID_SERVER_URI

#define ID_NAMESPACE_INDEX_ALIAS        2
#define ID_NAMESPACE_ALIAS_URI          "http://iData/OPC-UA/Alias"

#define ID_OPCNodeId(xValue)            { OpcUa_IdType_Numeric, 0, xValue }
#define ID_OPC_IDNodeId(xValue)           { OpcUa_IdType_Numeric, ID_NAMESPACE_INDEX, xValue }
#define ID_OPCExpandedNodeId(xValue)    { .NodeId = {OpcUa_IdType_Numeric,0,xValue}, .ServerIndex = 0}

#define ID_NAMING_RULE_TYPE_MANDATORY   1
#define ID_NAMING_RULE_TYPE_OPTIONAL    2
#define ID_NAMING_RULE_TYPE_CONSTRAINT  3

#define ID_LENGTH_UNKNOWN_INDICATOR     -200
#define ID_VALUERANK_ANY_INDICATOR      -100

#define ID_HasProperty                          ID_OPCNodeId(OpcUaId_HasProperty)
#define ID_HasComponent                         ID_OPCNodeId(OpcUaId_HasComponent)
#define ID_HasModellingRule                     ID_OPCNodeId(OpcUaId_HasModellingRule)
#define ID_HasTypeDefinition                    ID_OPCNodeId(OpcUaId_HasTypeDefinition)
#define ID_HasSubtype                           ID_OPCNodeId(OpcUaId_HasSubtype)
#define ID_Organizes                            ID_OPCNodeId(OpcUaId_Organizes)

#define ID_PropertyType                         ID_OPCNodeId(OpcUaId_PropertyType)
#define ID_BaseDataType                         ID_OPCNodeId(OpcUaId_BaseDataType)

#define ID_ModellingRuleOptional                ID_OPCNodeId(OpcUaId_ModellingRule_Optional)
#define ID_ModellingRuleMandatory               ID_OPCNodeId(OpcUaId_ModellingRule_Mandatory)
#define ID_ModellingRuleOptionalPlaceholder     ID_OPCNodeId(OpcUaId_ModellingRule_OptionalPlaceholder)
#define ID_ModellingRuleMandatoryPlaceholder    ID_OPCNodeId(OpcUaId_ModellingRule_MandatoryPlaceholder)
#define ID_ModellingRuleExposesItsArray         ID_OPCNodeId(OpcUaId_ModellingRule_ExposesItsArray)

#ifdef __cplusplus
}
#endif

#endif