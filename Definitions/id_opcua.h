/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef ID_OPCUA_H
#define ID_OPCUA_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#pragma warning(push)
#pragma warning(disable: 4201)
#include <sql.h>
#include <sqlext.h>
#pragma warning(pop)

#include <opcua.h>
#include <opcua_core.h>
#include <opcua_mutex.h>
#include <opcua_types.h>
#include <opcua_p_types.h>
#include <opcua_builtintypes.h>
#include <opcua_p_interface.h>
#include <opcua_endpoint.h>
#include <opcua_securechannel_types.h>

#include "id_opcua_comm.h"
#include "id_opcua_db_def.h"
#include "id_opcua_db_private.h"
#include "id_opcua_serverinfo.h"
#include "id_opcua_addressspace.h"
#include "id_opcua_addressspace_ids.h"
#include "id_opcua_addressspace_functions.h"

#endif