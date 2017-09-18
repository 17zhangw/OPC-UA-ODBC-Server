/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#include "UASQLFunctions.h"
#include "UASQLInternal.h"

#ifdef __linux__
#include <strings.h>
#endif

#ifdef _WIN32
char * ID_ConvertNullTerminatedWideToMultiByte(wchar_t * multiByte, char ** errorMessage) {
    int ret = -1;
    char * multiByteStr = NULL;
    int length = (int)wcslen(multiByte);
    int multiByteLength = WideCharToMultiByte(CP_UTF8, 0, multiByte, length, NULL, 0, NULL, NULL);
    if (multiByteLength != 0) {
        multiByteStr = malloc(sizeof(char) * (multiByteLength + 1));
        memset(multiByteStr, 0x00, sizeof(char) * (multiByteLength + 1));
        ret = WideCharToMultiByte(CP_UTF8, 0, multiByte, length, multiByteStr, multiByteLength, NULL, NULL);
    }

    if (ret != multiByteLength || ret == 0) {
        if (multiByteStr != NULL) {
            free(multiByteStr);
            multiByteStr = NULL;
        }

        char * eMessage = NULL;
        if (ret != 0) {
            eMessage = "translation of UTF8 to Unicode failed";
        }
        else {
            DWORD w = GetLastError();
            if (w == ERROR_INSUFFICIENT_BUFFER) { eMessage = "insufficient buffer"; }
            else if (w == ERROR_INVALID_FLAGS) { eMessage = "values supplied for flags"; }
            else if (w == ERROR_INVALID_PARAMETER) { eMessage = "invalid parameter"; }
            else if (w == ERROR_NO_UNICODE_TRANSLATION) { eMessage = "no unicode translation found"; }
        }

        if (eMessage != NULL && errorMessage != NULL) {
            char * buffer = malloc(sizeof(char) * (strlen(eMessage) + 1));
            strcpy_s(buffer, (strlen(eMessage) + 1), eMessage);
            *errorMessage = buffer;
        }
    }

    return multiByteStr;
}

wchar_t * ID_ConvertNullTerminatedMultiByteToWide(char * multiByte, char ** errorMessage) {
    int ret = -1;
    wchar_t * wideStr = NULL;
    int length = (int)strlen(multiByte);
    int wideLength = MultiByteToWideChar(CP_UTF8, 0, multiByte, length, NULL, 0);
    if (wideLength != 0) {
        wideStr = malloc(sizeof(wchar_t) * (wideLength + 1));
        memset(wideStr, 0x00, sizeof(wchar_t) * (wideLength + 1));
        ret = MultiByteToWideChar(CP_UTF8, 0, multiByte, length, wideStr, wideLength);
    }

    if (ret != wideLength || ret == 0) {
        if (wideStr != NULL) {
            free(wideStr);
            wideStr = NULL;
        }

        char * eMessage = NULL;
        if (ret != 0) {
            eMessage = "translation of UTF8 to Unicode failed";
        }
        else {
            DWORD w = GetLastError();
            if (w == ERROR_INSUFFICIENT_BUFFER) { eMessage = "insufficient buffer"; }
            else if (w == ERROR_INVALID_FLAGS) { eMessage = "values supplied for flags"; }
            else if (w == ERROR_INVALID_PARAMETER) { eMessage = "invalid parameter"; }
            else if (w == ERROR_NO_UNICODE_TRANSLATION) { eMessage = "no unicode translation found"; }
        }

        if (eMessage != NULL && errorMessage != NULL) {
            char * buffer = malloc(sizeof(char) * (strlen(eMessage) + 1));
            strcpy_s(buffer, (strlen(eMessage) + 1), eMessage);
            *errorMessage = buffer;
        }
    }

    return wideStr;
}
#else
char * ID_ConvertNullTerminatedWideToMultiByte(wchar_t * multiByte, char ** errorMessage) {
    char * eMessage = "function not supported; make sure to call correct function driver for the server";
    *errorMessage = malloc(sizeof(char) * (strlen(eMessage) + 1));
    strcpy(*errorMessage, eMessage);
    return NULL;
}

wchar_t * ID_ConvertNullTerminatedMultiByteToWide(char * multiByte, char ** errorMessage) {
    char * eMessage = "function not supported; make sure to call correct function driver for the server";
    *errorMessage = malloc(sizeof(char) * (strlen(eMessage) + 1));
    strcpy(*errorMessage, eMessage);
    return NULL;
}
#endif

OpcUa_Boolean ID_UASQLDescribeParameterA(ID_Session * UserSession, char * table, char * column, SQLSMALLINT * DataType, SQLULEN * ColumnSize, SQLSMALLINT * DecimalDigits) {
	SQLHANDLE StatementHandle = NULL;
	SQLAllocHandle(SQL_HANDLE_STMT, UserSession->SessionDBConnHandle, &StatementHandle);

	SQLLEN cbColumnSize;
	SQLLEN cbSQLDataType;
	SQLLEN cbDecimalDigits;
	SQLRETURN retCode = SQLColumns(StatementHandle, NULL, 0, NULL, 0, (SQLCHAR*)table, SQL_NTS, (SQLCHAR*)column, SQL_NTS);
	if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_STMT, StatementHandle);
		return OpcUa_False;
	}

	SQLBindCol(StatementHandle, 14, SQL_C_SSHORT, DataType, 0, &cbSQLDataType);
	SQLBindCol(StatementHandle, 9, SQL_C_SSHORT, DecimalDigits, 0, &cbDecimalDigits);
	SQLBindCol(StatementHandle, 7, SQL_C_SLONG, ColumnSize, 0, &cbColumnSize);
	retCode = SQLFetch(StatementHandle);

	SQLFreeHandle(SQL_HANDLE_STMT, StatementHandle);
	return (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO);
}

OpcUa_Boolean ID_UASQLDescribeParameterW(ID_Session * UserSession, wchar_t * table, wchar_t * column, SQLSMALLINT * DataType, SQLULEN * ColumnSize, SQLSMALLINT * DecimalDigits) {
	SQLHANDLE StatementHandle = NULL;
	SQLAllocHandle(SQL_HANDLE_STMT, UserSession->SessionDBConnHandle, &StatementHandle);
	
	SQLLEN cbColumnSize;
	SQLLEN cbSQLDataType;
	SQLLEN cbDecimalDigits;
	SQLRETURN retCode = SQLColumnsW(StatementHandle, NULL, 0, NULL, 0, (SQLWCHAR*)table, SQL_NTSL, (SQLWCHAR*)column, SQL_NTSL);
	if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_STMT, StatementHandle);
		return OpcUa_False;
	}

	SQLBindCol(StatementHandle, 14, SQL_C_SSHORT, DataType, 0, &cbSQLDataType);
	SQLBindCol(StatementHandle, 9, SQL_C_SSHORT, DecimalDigits, 0, &cbDecimalDigits);
	SQLBindCol(StatementHandle, 7, SQL_C_SLONG, ColumnSize, 0, &cbColumnSize);
	retCode = SQLFetch(StatementHandle);

	SQLFreeHandle(SQL_HANDLE_STMT, StatementHandle);
	return (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO);
}

OpcUa_Boolean ID_UASQLValidateAttributesA(ID_Session * UserSession, cJSON * attributeObject, char * table) {
	SQLHANDLE StatementHandle = NULL;
	OpcUa_Boolean Success = OpcUa_True;
	int numAttrs = cJSON_GetArraySize(attributeObject);
	SQLAllocHandle(SQL_HANDLE_STMT, UserSession->SessionDBConnHandle, &StatementHandle);

	for (int i = 0; i < numAttrs; i++) {
		cJSON * attributeRow = cJSON_GetArrayItem(attributeObject, i);
		int numElements = cJSON_GetArraySize(attributeObject);
		if (numElements == 0) {
			Success = OpcUa_False;
			goto Error;
		}

		cJSON * attributeDict = cJSON_GetArrayItem(attributeRow, 0);
		if (attributeDict->valuestring == NULL || strlen(attributeDict->valuestring) == 0) {
			Success = OpcUa_False;
			goto Error;
		}

		char * DTDescriptor = ID_UASQLGetDataTypeForColumnInTableA(StatementHandle, table, attributeDict->string);
		if (DTDescriptor == NULL) {
			Success = OpcUa_False;
			goto Error;
		}

		if (strlen(DTDescriptor) != strlen(attributeDict->valuestring)) {
			Success = OpcUa_False;
			goto Error;
		}

#ifdef _WIN32
		if (_strnicmp(DTDescriptor, attributeDict->valuestring, strlen(DTDescriptor)) != 0) {
			Success = OpcUa_False;
			goto Error;
		}
#else
		if (strcasecmp(DTDescriptor, attributeDict->valuestring) != 0) {
			Success = OpcUa_False;
			goto Error;
		}
#endif

		SQLFreeStmt(StatementHandle, SQL_UNBIND);
		SQLFreeStmt(StatementHandle, SQL_CLOSE);
	}

Error:;
	if (StatementHandle != NULL) {
		SQLFreeHandle(SQL_HANDLE_STMT, StatementHandle);
	}

	return Success;
}

OpcUa_Boolean ID_UASQLValidateAttributesW(ID_Session * UserSession, cJSON * attributeObject, wchar_t * table) {
	SQLHANDLE StatementHandle = NULL;
	OpcUa_Boolean Success = OpcUa_True;
	int numAttrs = cJSON_GetArraySize(attributeObject);
	SQLAllocHandle(SQL_HANDLE_STMT, UserSession->SessionDBConnHandle, &StatementHandle);

	for (int i = 0; i < numAttrs; i++) {
		cJSON * attributeRow = cJSON_GetArrayItem(attributeObject, i);
		int numElements = cJSON_GetArraySize(attributeObject);
		if (numElements == 0) {
			Success = OpcUa_False;
			goto Error;
		}

		cJSON * attributeDict = cJSON_GetArrayItem(attributeRow, 0);
		if (attributeDict->valuestring == NULL || strlen(attributeDict->valuestring) == 0) {
			Success = OpcUa_False;
			goto Error;
		}

		wchar_t * columnName = ID_ConvertNullTerminatedMultiByteToWide(attributeDict->string, NULL);
		if (columnName == NULL) {
			Success = OpcUa_False;
			goto Error;
		}

		char * DTDescriptor = ID_UASQLGetDataTypeForColumnInTableW(StatementHandle, table, columnName);
		free(columnName);
		if (DTDescriptor == NULL) {
			Success = OpcUa_False;
			goto Error;
		}

		if (strlen(DTDescriptor) != strlen(attributeDict->valuestring)) {
			Success = OpcUa_False;
			goto Error;
		}

#ifdef _WIN32
		if (_strnicmp(DTDescriptor, attributeDict->valuestring, strlen(DTDescriptor)) != 0) {
			Success = OpcUa_False;
			goto Error;
		}
#else
		if (strcasecmp(DTDescriptor, attributeDict->valuestring) != 0) {
			Success = OpcUa_False;
			goto Error;
		}
#endif

		SQLFreeStmt(StatementHandle, SQL_UNBIND);
		SQLFreeStmt(StatementHandle, SQL_CLOSE);
	}

Error:;
	if (StatementHandle != NULL) {
		SQLFreeHandle(SQL_HANDLE_STMT, StatementHandle);
	}

	return Success;
}

char * ID_UASQLGetDataTypeForColumnInTableA(SQLHANDLE StatementHandle, char * table, char * column) {
	SQLRETURN retVal = 0;
	retVal = SQLColumns(StatementHandle, NULL, 0, NULL, 0, (SQLCHAR*)table, SQL_NTS, (SQLCHAR*)column, SQL_NTS);
	if (retVal != SQL_SUCCESS && retVal != SQL_SUCCESS_WITH_INFO) {
		return NULL;
	}

	SQLSMALLINT SQLDataType = 0;
	SQLLEN cbSQLDataType = 0;
	SQLBindCol(StatementHandle, 14, SQL_C_SSHORT, &SQLDataType, 0, &cbSQLDataType);
	retVal = SQLFetch(StatementHandle);
	if (retVal != SQL_SUCCESS && retVal != SQL_SUCCESS_WITH_INFO) {
		return NULL;
	}

	return ID_UASQLGetDataTypeDescriptionForType(SQLDataType, NULL);
}

char * ID_UASQLGetDataTypeForColumnInTableW(SQLHANDLE StatementHandle, wchar_t * table, wchar_t * column) {
	SQLRETURN retVal = 0;
	retVal = SQLColumnsW(StatementHandle, NULL, 0, NULL, 0, (SQLWCHAR*)table, SQL_NTS, (SQLWCHAR*)column, SQL_NTS);
	if (retVal != SQL_SUCCESS && retVal != SQL_SUCCESS_WITH_INFO) {
		return NULL;
	}

	SQLSMALLINT SQLDataType = 0;
	SQLLEN cbSQLDataType = 0;
	SQLBindCol(StatementHandle, 14, SQL_C_SSHORT, &SQLDataType, 0, &cbSQLDataType);
	retVal = SQLFetch(StatementHandle);
	if (retVal != SQL_SUCCESS && retVal != SQL_SUCCESS_WITH_INFO) {
		return NULL;
	}

	return ID_UASQLGetDataTypeDescriptionForType(SQLDataType, NULL);
}

char * ID_UASQLGetDataTypeDescriptionForType(SQLSMALLINT DataType, int * Indicator) {
	char * DataTypeStr = NULL;
	if (Indicator != NULL) *Indicator = 0;

	switch (DataType) {
	case SQL_CHAR: DataTypeStr = "CHAR"; break;
	case SQL_VARCHAR: DataTypeStr = "VARCHAR"; break;
	case SQL_LONGVARCHAR: DataTypeStr = "LONG VARCHAR"; break;
	case SQL_WCHAR: DataTypeStr = "WCHAR"; break;
	case SQL_WVARCHAR: DataTypeStr = "WVARCHAR"; break;
	case SQL_WLONGVARCHAR: DataTypeStr = "WLONGVARCHAR"; break;
	case SQL_DECIMAL: DataTypeStr = "DECIMAL"; break;
	case SQL_NUMERIC: DataTypeStr = "NUMERIC"; break;
	case SQL_SMALLINT: DataTypeStr = "SMALLINT"; break;
	case SQL_INTEGER: DataTypeStr = "INTEGER"; break;
	case SQL_REAL: DataTypeStr = "REAL"; break;
	case SQL_FLOAT: DataTypeStr = "FLOAT"; break;
	case SQL_DOUBLE: DataTypeStr = "DOUBLE"; break;
	case SQL_BIT: DataTypeStr = "BIT"; break;
	case SQL_TINYINT: DataTypeStr = "TINYINT"; break;
	case SQL_BIGINT: DataTypeStr = "BIGINT"; break;
	case SQL_BINARY: DataTypeStr = "BINARY";
		if (Indicator != NULL) *Indicator = 1;
		break;
	case SQL_VARBINARY: DataTypeStr = "VARBINARY"; 
		if (Indicator != NULL) *Indicator = 1;
		break;
	case SQL_LONGVARBINARY: DataTypeStr = "LONGVARBINARY"; 
		if (Indicator != NULL) *Indicator = 1;
		break;
	case SQL_TYPE_DATE: DataTypeStr = "DATE"; break;
	case SQL_TYPE_TIME: DataTypeStr = "TIME"; break;
	case SQL_TYPE_TIMESTAMP: DataTypeStr = "TIMESTAMP"; break;
	case SQL_INTERVAL_MONTH: DataTypeStr = "INTERVAL MONTH"; break;
	case SQL_INTERVAL_YEAR: DataTypeStr = "INTERVAL YEAR"; break;
	case SQL_INTERVAL_YEAR_TO_MONTH: DataTypeStr = "INTERVAL YEAR TO MONTH"; break;
	case SQL_INTERVAL_DAY: DataTypeStr = "INTERVAL DAY"; break;
	case SQL_INTERVAL_HOUR: DataTypeStr = "INTERVAL HOUR"; break;
	case SQL_INTERVAL_MINUTE: DataTypeStr = "INTERVAL MINUTE"; break;
	case SQL_INTERVAL_SECOND: DataTypeStr = "INTERVAL SECOND"; break;
	case SQL_INTERVAL_DAY_TO_HOUR: DataTypeStr = "INTERVAL DAY TO HOUR"; break;
	case SQL_INTERVAL_DAY_TO_MINUTE: DataTypeStr = "INTERVAL DAY TO MINUTE"; break;
	case SQL_INTERVAL_DAY_TO_SECOND: DataTypeStr = "INTERVAL DAY TO SECOND"; break;
	case SQL_INTERVAL_HOUR_TO_MINUTE: DataTypeStr = "INTERVAL HOUR TO MINUTE"; break;
	case SQL_INTERVAL_HOUR_TO_SECOND: DataTypeStr = "INTERVAL HOUR TO SECOND"; break;
	case SQL_GUID: DataTypeStr = "GUID"; break;
	default: DataTypeStr = "UNKNOWN"; break;
	}
	return DataTypeStr;
}

cJSON * ID_UASQLGetQueryAttributesA(SQLHANDLE StatementHandle, int numResultColumns, int ** binaryIndicators, char ** errorMessage) {
	char * errorStr = NULL;
	SQLRETURN retVal = SQL_SUCCESS;
	OpcUa_Boolean shouldAttachOwner = OpcUa_Good;
	OpcUa_StatusCode result = OpcUa_Good;
	cJSON * attributeObject = cJSON_CreateArray();
	int * BinaryIndicators = malloc(sizeof(int) * numResultColumns);
	memset(BinaryIndicators, 0, sizeof(int) * numResultColumns);

	for (SQLUSMALLINT i = 0; i < numResultColumns; i++) {
		SQLSMALLINT ColNameLength = 0;
		SQLSMALLINT DataType = 0;
		SQLULEN ColumnSize = 0;
		SQLSMALLINT DecimalDigits = 0;
		SQLSMALLINT Nullable = 0;

		char ColNameBuffer[256];
		memset(ColNameBuffer, 0, sizeof(ColNameBuffer));
		retVal = SQLDescribeCol(StatementHandle, i + 1, (SQLCHAR*)ColNameBuffer, sizeof(ColNameBuffer), &ColNameLength, &DataType, &ColumnSize, &DecimalDigits, &Nullable);
		ColNameBuffer[ColNameLength] = '\0';
        ID_CHECK_RETVAL_ANSI_UTF8(retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

		// type name mapping
		char * DataTypeStr = ID_UASQLGetDataTypeDescriptionForType(DataType, &BinaryIndicators[i]);

		cJSON * singleColumn = cJSON_CreateObject();
		cJSON_AddItemToObject(singleColumn, ColNameBuffer, cJSON_CreateString(DataTypeStr));
		cJSON_AddItemToArray(attributeObject, singleColumn);
	}
	
	*binaryIndicators = BinaryIndicators;
	return attributeObject;

Error:;
	cJSON_Delete(attributeObject);
	*errorMessage = errorStr;
	return NULL;
}

cJSON * ID_UASQLGetQueryAttributesW(SQLHANDLE StatementHandle, int numResultColumns, int ** binaryIndicators, char ** errorMessage) {
    char * errorStr = NULL;
    SQLRETURN retVal = SQL_SUCCESS;
    OpcUa_Boolean shouldAttachOwner = OpcUa_Good;
    OpcUa_StatusCode result = OpcUa_Good;
    cJSON * attributeObject = cJSON_CreateArray();
    int * BinaryIndicators = malloc(sizeof(int) * numResultColumns);
    memset(BinaryIndicators, 0, sizeof(int) * numResultColumns);

    for (SQLUSMALLINT i = 0; i < numResultColumns; i++) {
        SQLSMALLINT ColNameLength = 0;
        SQLSMALLINT DataType = 0;
        SQLULEN ColumnSize = 0;
        SQLSMALLINT DecimalDigits = 0;
        SQLSMALLINT Nullable = 0;

        char * ColNameBuffer = NULL;
        wchar_t WColNameBuffer[256];
        memset(WColNameBuffer, 0, sizeof(WColNameBuffer));
        retVal = SQLDescribeColW(StatementHandle, i + 1, (SQLWCHAR*)WColNameBuffer, sizeof(WColNameBuffer), &ColNameLength, &DataType, &ColumnSize, &DecimalDigits, &Nullable);
        WColNameBuffer[ColNameLength] = L'\0';
        ID_CHECK_RETVAL_UNICODE(retVal, "SQLHSTMT", SQL_HANDLE_STMT, StatementHandle);

        ColNameBuffer = ID_ConvertNullTerminatedWideToMultiByte(WColNameBuffer, &errorStr);
        if (ColNameBuffer == NULL) {
            goto Error;
        }

        // type name mapping
        char * DataTypeStr = ID_UASQLGetDataTypeDescriptionForType(DataType, &BinaryIndicators[i]);

        cJSON * singleColumn = cJSON_CreateObject();
        cJSON_AddItemToObject(singleColumn, ColNameBuffer, cJSON_CreateString(DataTypeStr));
        cJSON_AddItemToArray(attributeObject, singleColumn);
        free(ColNameBuffer);
    }

    *binaryIndicators = BinaryIndicators;
    return attributeObject;
Error:;
    cJSON_Delete(attributeObject);
    *errorMessage = errorStr;
    return NULL;
}

char * ID_ExtractErrorA(char *fn, SQLHANDLE handle, SQLSMALLINT type, OpcUa_Boolean ShouldFree) {
    SQLSMALLINT	 i = 0;
    SQLINTEGER	 native;
    SQLCHAR	 state[ 7 ];
    SQLSMALLINT  textLen = 1024;
    SQLCHAR *	 text = malloc(sizeof(char) * textLen);
    memset(text, 0x00, sizeof(char) * textLen);
    SQLSMALLINT	 len;
    SQLRETURN	 ret;

    fprintf(stderr, "\n" "The driver reported the following diagnostics whilst running " "%s\n\n", fn);
    do {
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, text, textLen, &len);
        if (SQL_SUCCEEDED(ret))
            printf("%s:%hd:%d:%s\n", state, i, native, text);
    } while( ret == SQL_SUCCESS );
    
    if (ShouldFree == OpcUa_True) {
        free(text);
        return NULL;
    }
    
    return (char*)text;
}

wchar_t * ID_ExtractErrorW(wchar_t * fn, SQLHANDLE handle, SQLSMALLINT type, OpcUa_Boolean ShouldFree) {
	SQLSMALLINT	 i = 0;
	SQLINTEGER	 native;
	SQLWCHAR	 state[7];
	SQLSMALLINT  textLen = 1024;
	SQLWCHAR *	 text = malloc(sizeof(SQLWCHAR) * textLen);
	memset(text, 0x00, sizeof(char) * textLen);
	SQLSMALLINT	 len;
	SQLRETURN	 ret;

	fprintf(stderr, "\n" "The driver reported the following diagnostics whilst running " "%S\n\n", fn);
	do {
		ret = SQLGetDiagRecW(type, handle, ++i, state, &native, text, textLen, &len);
		if (SQL_SUCCEEDED(ret))
			printf("%S:%d:%d:%S\n", state, i, native, text);
	} while (ret == SQL_SUCCESS);

	if (ShouldFree == OpcUa_True) {
		free(text);
		return NULL;
	}

	return (wchar_t*)text;
}