/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef UASQLINTERNAL_H
#define UASQLINTERNAL_H

#include "../Definitions/id_opcua.h"
#include "../cJSON/cJSON.h"
#include "../SessionManager/SessionManager.h"

#ifdef __cplusplus
extern "C" {
#endif

cJSON * ID_UASQLGetQueryAttributesA(SQLHANDLE StatementHandle, int numResultColumns, int ** binaryIndicators, char ** errorMessage);
cJSON * ID_UASQLGetQueryAttributesW(SQLHANDLE StatementHandle, int numResultColumns, int ** binaryIndicators, char ** errorMessage);
char * ID_UASQLGetDataTypeDescriptionForType(SQLSMALLINT DataType, int * Indicator);
char * ID_UASQLGetDataTypeForColumnInTableA(SQLHANDLE StatementHandle, char * table, char * column);
char * ID_UASQLGetDataTypeForColumnInTableW(SQLHANDLE StatementHandle, wchar_t * table, wchar_t * column);
OpcUa_Boolean ID_UASQLValidateAttributesA(ID_Session * UserSession, cJSON * attributeObject, char * table);
OpcUa_Boolean ID_UASQLValidateAttributesW(ID_Session * UserSession, cJSON * attributeObject, wchar_t * table);
OpcUa_Boolean ID_UASQLDescribeParameterA(ID_Session * UserSession, char * table, char * column, SQLSMALLINT * DataType, SQLULEN * ColumnSize, SQLSMALLINT * DecimalDigits);
OpcUa_Boolean ID_UASQLDescribeParameterW(ID_Session * UserSession, wchar_t * table, wchar_t * column, SQLSMALLINT * DataType, SQLULEN * ColumnSize, SQLSMALLINT * DecimalDigits);
void ID_CreateBufferParameterAttachment(OpcUa_Boolean IsUnicode, cJSON * element, int index, SQLSMALLINT DataTypeInfo,
                                        SQLLEN * dataLengthBuffers, SQLLEN ** indicatorPointers,
                                        void ** dataBuffers, SQLSMALLINT * paramCType, void ** redirectPtr);

#ifdef __cplusplus
}
#endif

#endif