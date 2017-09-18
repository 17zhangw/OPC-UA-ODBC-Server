/* 
 * Copyright (C) 2017 iData Inc. - All Rights Reserved
 * Developed by William Zhang
 */

#ifndef INDEXER_H
#define INDEXER_H

#include "../Definitions/id_opcua.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef void * ID_IndexerHandle;
    
    // Create Indexer Object
    void ID_Indexer_Create(ID_IndexerHandle * Indexer);

    // Recycle Index to be used later on requisition
    void ID_Indexer_RecyleIndex(ID_IndexerHandle Indexer, OpcUa_UInt32 Index);

    // Request another index
    OpcUa_UInt32 ID_Indexer_GetNextIndex(ID_IndexerHandle Indexer);

    // Add index to reserve that way cannot be selected
    void ID_Indexer_AddToReserved(ID_IndexerHandle Indexer, OpcUa_UInt32 Index);

    // Destroy Indexer
    void ID_Indexer_Delete(ID_IndexerHandle * Indexer);

#ifdef __cplusplus
}
#endif

#endif /* INDEXER_H */
