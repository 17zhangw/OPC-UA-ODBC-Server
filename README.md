## OPC UA ODBC Server

A running OPC UA Server (Release 1.03) that was implemented in C for both Windows and Linux distributions. The OPC UA Server runs with binary encoding over TCP. This specific implementation of the OPC UA Server exposes through the address space a set of ODBC functions. In other words, the current implementation encapsulates an ODBC-supported database abstractly through method calls rather than directly exposing the internal database structure and internal data directly in the address space.

<b>
This server was originally developed as part of a year-long research project for an academic course (AP Research). 
This particular version of the source code is released with explicit permission from iData Inc. for which I am grateful for. 
However, this repository has been and is no longer actively maintained since August 2017 due to contractual clauses 
between myself and iData Inc. concerning this codebase.
</b>

### Overview

This repository contains the following pieces of information. Although the Visual Studio 2015 and Netbeans (for Ubuntu/CentOS) projects have been distributed in this repository, the projects still require tweaking to account for library search paths and other more system-specific configurations. The codebase has been separated into various folders depending on the specific function of that particular code within the larger context (i.e AddressSpace infrastructure are in the AddressSpace folder, OPC UA function definitions and database methods are in the Functions folder).

1. README.md
2. Visual Studio 2015 Project
3. Netbeans C/C++ Project
4. Source Code in Subfolders

### Supported Functions

At the current moment, the following functions are supported. For allowing the end-user to selectively target an ODBC driver, both wchar_t and multibyte char functions are exposed. The prefixing to functions mirrors the convention surrounding ODBC with the 'W' suffix indicating Unicode Driver and 'A' indicating ANSI Driver. Although these functions have been tested primarily against SQL Server 2014 and 2017, other databases will work, although minor adjustments may be necessary for optimization purposes and/or to improve compatibility. 

The currently publicly exposed functions are:

1. SQLConnect
2. SQLSetCommit
3. SQLCommit
4. SQLDisconnect
5. SQLAllocHandle
6. SQLCloseHandle
7. SQLResetParamsHandle
8. SQLFreeHandle
9. SQLExecDirect
10. SQLInsert
11. SQLUpdate
12. SQLDelete
13. SQLSelect
14. SQLFetch
15. SQLPrepare
16. SQLBind
17. SQLExecute

The privately depended upon functions are:

1. SQLColumns
2. SQLBindCol
3. SQLDescribeCol
4. SQLFetch
5. SQLGetInfo (SQL_DYNAMIC_CURSOR_ATTRIBUTES1)
6. SQLNumResultCols
7. SQLFetchScroll
8. SQLGetDiagRec

**Notes/Caveats**
- Storing BINARY data into the database is supported through the functions provided. However, the BINARY data when passed into the function as an OPC UA parameter must be encoded in Base64 without any line breaks.
- All publicly exposed functions require the first parameter to be an UINT32 value that was passed into SQLAllocHandle.
- Most publicly exposed functions accept a second parameter that must be JSON-encoded.
- The functions return up to a maximum of 3 values - with the first parameter indicating success/failure of the database operation, the second parameter representing a textual JSON-encoded result if applicable, and the third result indicates the number of affected rows.
- The JSON definitions can be accessed via querying the description attribute of the Argument or in UASpace/InitializeCustom.c

### Configuration Details

- Address Space Node IDs and Namespace are defined in Definitions/id_opcua_db_def.h
- Server Information configuration in Definitions/id_opcua_serverinfo.h
- Parameters concerning communication (including PKI) in Definitions/id_opcua_comm.h
- By default, the server listens on port 4840 and uses PKI for verification of client certificates

### Sample Function Call Sequences
1. Insert, Update, Delete, Select, and Fetch Sequence
```
SQLAllocHandle()
...
SQLInsert()
...
SQLUpdate()
...
SQLDelete()
...
SQLSelect()
SQLFetch()
SQLFetch()
SQLFreeHandle()
```
2. Prepared Statements, SQLResetParams Sequence
```
SQLAllocHandle()
SQLPrepare()
SQLBind()
...
SQLBind()
SQLExecute()
SQLResetParamsHandle()
SQLBind()
SQLExecute()
SQLFreeHandle()
```
3. Connection, SQLSetCommit, SQLCommit Sequence
```
SQLConnect()
SQLSetCommit()
...(various operations)
SQLCommit()
SQLDisconnect()
```

### Dependencies
1. OpenSSL 1.0.2j with development headers
2. OPC UA UA-AnsiC Stack
3. cJSON library by [DaveGamble](https://github.com/DaveGamble)
4. ODBC in addition to ODBC headers (sql.h, sqlext.h)
5. Various system libraries

### References
1. [OPC UA AnsiC Stack](https://github.com/OPCFoundation/UA-AnsiC/)
2. [OpenSSL](https://www.openssl.org)
3. [OPC UA Specifications](https://opcfoundation.org/developer-tools/specifications-unified-architecture/)

### Contact
Please refer any contact to <17zhangw@gmail.com> with 'OPC UA' in the subject header.

### License
© 2017. William Zhang
