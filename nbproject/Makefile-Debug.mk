#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AddressSpace/AddressSpace.o \
	${OBJECTDIR}/AddressSpace/AddressSpaceCopier.o \
	${OBJECTDIR}/AddressSpace/AddressSpaceHelper.o \
	${OBJECTDIR}/AddressSpace/OPCDerivations.o \
	${OBJECTDIR}/Functions/Access.o \
	${OBJECTDIR}/Functions/Discovery.o \
	${OBJECTDIR}/Functions/EXECUTE_SQLVariant.o \
	${OBJECTDIR}/Functions/ExposedFunctions.o \
	${OBJECTDIR}/Functions/IUD_SQLVariant.o \
	${OBJECTDIR}/Functions/QUERY_SQLVariant.o \
	${OBJECTDIR}/Functions/RegisterUnregisterNodes.o \
	${OBJECTDIR}/Functions/Session.o \
	${OBJECTDIR}/Functions/UASQLFunctions.o \
	${OBJECTDIR}/Functions/UASQLInternal.o \
	${OBJECTDIR}/Functions/UnimplementedFunctions.o \
	${OBJECTDIR}/Indexer/Aliaser.o \
	${OBJECTDIR}/Indexer/Continuer.o \
	${OBJECTDIR}/Indexer/Indexer.o \
	${OBJECTDIR}/SessionManager/SecurityManager.o \
	${OBJECTDIR}/SessionManager/SessionManager.o \
	${OBJECTDIR}/UASpace/Base64.o \
	${OBJECTDIR}/UASpace/Initialize.o \
	${OBJECTDIR}/UASpace/InitializeCustom.o \
	${OBJECTDIR}/UASpace/InitializeObjectTypes.o \
	${OBJECTDIR}/UASpace/InitializeObjects.o \
	${OBJECTDIR}/UASpace/InitializeOtherTypes.o \
	${OBJECTDIR}/UASpace/InitializeTest.o \
	${OBJECTDIR}/cJSON/cJSON.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=-pthread -g

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-luastack -lssl -lcrypto -ldl -lodbc -lm

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/uaserver

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/uaserver: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/uaserver ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/AddressSpace/AddressSpace.o: AddressSpace/AddressSpace.c
	${MKDIR} -p ${OBJECTDIR}/AddressSpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AddressSpace/AddressSpace.o AddressSpace/AddressSpace.c

${OBJECTDIR}/AddressSpace/AddressSpaceCopier.o: AddressSpace/AddressSpaceCopier.c
	${MKDIR} -p ${OBJECTDIR}/AddressSpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AddressSpace/AddressSpaceCopier.o AddressSpace/AddressSpaceCopier.c

${OBJECTDIR}/AddressSpace/AddressSpaceHelper.o: AddressSpace/AddressSpaceHelper.c
	${MKDIR} -p ${OBJECTDIR}/AddressSpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AddressSpace/AddressSpaceHelper.o AddressSpace/AddressSpaceHelper.c

${OBJECTDIR}/AddressSpace/OPCDerivations.o: AddressSpace/OPCDerivations.c
	${MKDIR} -p ${OBJECTDIR}/AddressSpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AddressSpace/OPCDerivations.o AddressSpace/OPCDerivations.c

${OBJECTDIR}/Functions/Access.o: Functions/Access.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/Access.o Functions/Access.c

${OBJECTDIR}/Functions/Discovery.o: Functions/Discovery.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/Discovery.o Functions/Discovery.c

${OBJECTDIR}/Functions/EXECUTE_SQLVariant.o: Functions/EXECUTE_SQLVariant.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/EXECUTE_SQLVariant.o Functions/EXECUTE_SQLVariant.c

${OBJECTDIR}/Functions/ExposedFunctions.o: Functions/ExposedFunctions.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/ExposedFunctions.o Functions/ExposedFunctions.c

${OBJECTDIR}/Functions/IUD_SQLVariant.o: Functions/IUD_SQLVariant.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/IUD_SQLVariant.o Functions/IUD_SQLVariant.c

${OBJECTDIR}/Functions/QUERY_SQLVariant.o: Functions/QUERY_SQLVariant.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/QUERY_SQLVariant.o Functions/QUERY_SQLVariant.c

${OBJECTDIR}/Functions/RegisterUnregisterNodes.o: Functions/RegisterUnregisterNodes.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/RegisterUnregisterNodes.o Functions/RegisterUnregisterNodes.c

${OBJECTDIR}/Functions/Session.o: Functions/Session.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/Session.o Functions/Session.c

${OBJECTDIR}/Functions/UASQLFunctions.o: Functions/UASQLFunctions.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/UASQLFunctions.o Functions/UASQLFunctions.c

${OBJECTDIR}/Functions/UASQLInternal.o: Functions/UASQLInternal.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/UASQLInternal.o Functions/UASQLInternal.c

${OBJECTDIR}/Functions/UnimplementedFunctions.o: Functions/UnimplementedFunctions.c
	${MKDIR} -p ${OBJECTDIR}/Functions
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Functions/UnimplementedFunctions.o Functions/UnimplementedFunctions.c

${OBJECTDIR}/Indexer/Aliaser.o: Indexer/Aliaser.c
	${MKDIR} -p ${OBJECTDIR}/Indexer
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Indexer/Aliaser.o Indexer/Aliaser.c

${OBJECTDIR}/Indexer/Continuer.o: Indexer/Continuer.c
	${MKDIR} -p ${OBJECTDIR}/Indexer
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Indexer/Continuer.o Indexer/Continuer.c

${OBJECTDIR}/Indexer/Indexer.o: Indexer/Indexer.c
	${MKDIR} -p ${OBJECTDIR}/Indexer
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Indexer/Indexer.o Indexer/Indexer.c

${OBJECTDIR}/SessionManager/SecurityManager.o: SessionManager/SecurityManager.c
	${MKDIR} -p ${OBJECTDIR}/SessionManager
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SessionManager/SecurityManager.o SessionManager/SecurityManager.c

${OBJECTDIR}/SessionManager/SessionManager.o: SessionManager/SessionManager.c
	${MKDIR} -p ${OBJECTDIR}/SessionManager
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SessionManager/SessionManager.o SessionManager/SessionManager.c

${OBJECTDIR}/UASpace/Base64.o: UASpace/Base64.c
	${MKDIR} -p ${OBJECTDIR}/UASpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UASpace/Base64.o UASpace/Base64.c

${OBJECTDIR}/UASpace/Initialize.o: UASpace/Initialize.c
	${MKDIR} -p ${OBJECTDIR}/UASpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UASpace/Initialize.o UASpace/Initialize.c

${OBJECTDIR}/UASpace/InitializeCustom.o: UASpace/InitializeCustom.c
	${MKDIR} -p ${OBJECTDIR}/UASpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UASpace/InitializeCustom.o UASpace/InitializeCustom.c

${OBJECTDIR}/UASpace/InitializeObjectTypes.o: UASpace/InitializeObjectTypes.c
	${MKDIR} -p ${OBJECTDIR}/UASpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UASpace/InitializeObjectTypes.o UASpace/InitializeObjectTypes.c

${OBJECTDIR}/UASpace/InitializeObjects.o: UASpace/InitializeObjects.c
	${MKDIR} -p ${OBJECTDIR}/UASpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UASpace/InitializeObjects.o UASpace/InitializeObjects.c

${OBJECTDIR}/UASpace/InitializeOtherTypes.o: UASpace/InitializeOtherTypes.c
	${MKDIR} -p ${OBJECTDIR}/UASpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UASpace/InitializeOtherTypes.o UASpace/InitializeOtherTypes.c

${OBJECTDIR}/UASpace/InitializeTest.o: UASpace/InitializeTest.c
	${MKDIR} -p ${OBJECTDIR}/UASpace
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UASpace/InitializeTest.o UASpace/InitializeTest.c

${OBJECTDIR}/cJSON/cJSON.o: cJSON/cJSON.c
	${MKDIR} -p ${OBJECTDIR}/cJSON
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cJSON/cJSON.o cJSON/cJSON.c

${OBJECTDIR}/main.o: main.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
