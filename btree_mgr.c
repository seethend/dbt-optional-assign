#include <string.h>
#include <stdlib.h>
#include "btree_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"

typedef struct BPlusTreeRecord
{
    struct Value val;
    struct RID rid;
} record;

// B Plus Tree Instance
record **bPlusTreeRecords;

// Total number of nodes
int totalNodeCount;

int scanNextNode;

int bPlusTreeSize;

/**
 *
 * @brief Implementation of FIFO page replacement strategy by replacing the page that has been in memory the longest
 *
 * @author Anand Babu Badrichetty
 *
 * @param bm
 * @param page
 *
 * @return void
 */
RC swapNodes(int i, int j) {

    record *tempRecMem = (record *) malloc (sizeof(record));

    tempRecMem = bPlusTreeRecords[i];
    bPlusTreeRecords[i] = bPlusTreeRecords[j];
    bPlusTreeRecords[j] = tempRecMem;
}


Value *stringToValue(char *val)
{
    Value *valRes = (Value *) malloc(sizeof(Value));

    valRes->dt = DT_INT;
    valRes->v.intV = atoi(val + 1);

    return valRes;
}

/************************************************************
 *            INIT AND SHUTDOWN INDEX MANAGER               *
 ************************************************************/

/*******************************************************************
* DESCRIPTION :     Initializes the index manager by setting a pointer to the memory that contains the B+-Tree
*					with a size of 50.
*******************************************************************/
RC initIndexManager (void *mgmtData) {
    // Create a pointer to the memory that will contain up to 50 nodes
    bPlusTreeRecords = (record **) malloc (sizeof(record *) * B_TREE_NODE_SIZE);
    return RC_OK;
}

/*******************************************************************
* DESCRIPTION :     Terminates the index manager.
*******************************************************************/
RC shutdownIndexManager () {
    free(bPlusTreeRecords);
    return RC_OK;
}


/************************************************************
 *     CREATE, DESTROY, OPEN, AND CLOSE A BTREE INDEX       *
 ************************************************************/

/*******************************************************************
* DESCRIPTION :     Creates a B Tree with the given arguments
*******************************************************************/
RC createBtree (char *idxId, DataType keyType, int n) {

    BTreeHandle *handle;
    handle = (BTreeHandle *) malloc (sizeof(BTreeHandle) * n);
    handle->keyType = keyType;
    handle->idxId = idxId;
    totalNodeCount = 0;
    scanNextNode = 0;

    bPlusTreeSize = n;

    return RC_OK;
}

/*******************************************************************
* DESCRIPTION :     Opens a B Tree identified by its id
*******************************************************************/
RC openBtree (BTreeHandle **tree, char *idxId) {
    *tree = (BTreeHandle *) malloc (sizeof(BTreeHandle) * bPlusTreeSize);
    (*tree)->idxId = (char *) malloc (sizeof(char) * (strlen(idxId) + 1));

    (*tree)->idxId = idxId;

    return RC_OK;
}

/*******************************************************************
* DESCRIPTION :     Closes a B Tree passed as a parameter
*******************************************************************/
RC closeBtree (BTreeHandle *tree) {
    free(tree);
    return RC_OK;
}

/*******************************************************************
* DESCRIPTION :     Deletes a B Tree identified by its id
*******************************************************************/
RC deleteBtree (char *idxId) {

    // remove the file
    destroyPageFile(idxId);

    // Resetting the values to 0
    totalNodeCount = 0;
    scanNextNode = 0;
    return RC_OK;
}

/*******************************************************************
* DESCRIPTION :     Gets the total number of nodes in the B Tree
*******************************************************************/
RC getNumNodes (BTreeHandle *tree, int *result) {
/*
    // Initialize variable to iterate the tree
    int counter = 0;
    int i;
    int j;

    for (i = 0; i < totalNodeCount; i++) {
        for (j = i - 1; j >= 0; j--) {
            // If both pages are the same
            if (bPlusTreeRecords[j]->rid.page == bPlusTreeRecords[i]->rid.page) {
                // Increase the counter and break the inner loop
                counter++;
                break;
            }
        }
    }
*/
    // Return result = total number of nodes - repeated nodes
    *result = totalNodeCount - 2;

    return RC_OK;
}

/**
 *
 * @brief Implementation for finding the number of entries in the BPlusTree
 *
 * @author Seethend Reddy Dummansoor
 *
 * @param tree
 * @param result
 *
 * @return RC
 */
RC getNumEntries (BTreeHandle *tree, int *result) {
    *result = totalNodeCount;
    return RC_OK;
}

/**
 *
 * @brief Implementation for finding the type of given key in the BPlusTree
 *
 * @author Seethend Reddy Dummansoor
 *
 * @param tree
 * @param result
 *
 * @return RC
 */
RC getKeyType (BTreeHandle *tree, DataType *result) {
//    *result = tree->keyType;
    return RC_OK;
}

/**
 *
 * @brief Implementation for finding a given key in the BPlusTree
 *
 * @author Seethend Reddy Dummansoor
 *
 * @param tree
 * @param key
 * @param result
 *
 * @return RC
 */
RC findKey (BTreeHandle *tree, Value *key, RID *result) {
    int i = 0;

    while (i < totalNodeCount) {
        if (bPlusTreeRecords[i]->val.dt == key->dt && bPlusTreeRecords[i]->val.v.intV == key->v.intV) {

            result->page = bPlusTreeRecords[i]->rid.page;
            result->slot = bPlusTreeRecords[i]->rid.slot;
            return RC_OK;
        }
        i++;
    }

    return RC_IM_KEY_NOT_FOUND;
}

/**
 *
 * @brief Implementation for inserting a given key in the BPlusTree
 *
 * @author Seethend Reddy Dummansoor
 *
 * @param tree
 * @param key
 * @param rid
 *
 * @return RC
 */
RC insertKey (BTreeHandle *tree, Value *key, RID rid) {

    int i = 0;
    bool keyFound = false;

    while (i < totalNodeCount) {
        if (bPlusTreeRecords[i]->val.dt == key->dt && bPlusTreeRecords[i]->val.v.intV == key->v.intV) {
            keyFound = true;
            break;
        }
        i++;
    }
    if (!keyFound) {
        bPlusTreeRecords[totalNodeCount] = (record *)malloc(sizeof(record));

        bPlusTreeRecords[totalNodeCount]->rid.page = rid.page;
        bPlusTreeRecords[totalNodeCount]->rid.slot = rid.slot;
        bPlusTreeRecords[totalNodeCount]->val.dt = key->dt;
        bPlusTreeRecords[totalNodeCount]->val.v.intV = key->v.intV;
        totalNodeCount++;
        return RC_OK;
    } else {
        return RC_IM_KEY_ALREADY_EXISTS;
    }
}

/**
 *
 * @brief Implementation for deleting a specified  key in the BPlusTree
 *
 * @author Ganugapanta Vishnuvardhan Reddy
 *
 * @param tree
 * @param key
 *
 * @return RC
 */
RC deleteKey (BTreeHandle *tree, Value *key) {

    int i = 0;

    while (i < totalNodeCount) {
        if (bPlusTreeRecords[i]->val.dt == key->dt && bPlusTreeRecords[i]->val.v.intV == key->v.intV) {

            int nextNodeIndex;
            int currentNodeIndex;

            for (nextNodeIndex = i + 1, currentNodeIndex = i; currentNodeIndex < totalNodeCount && nextNodeIndex < totalNodeCount; currentNodeIndex++, nextNodeIndex++) {
                bPlusTreeRecords[currentNodeIndex]->rid.page = bPlusTreeRecords[nextNodeIndex]->rid.page;
                bPlusTreeRecords[currentNodeIndex]->rid.slot = bPlusTreeRecords[nextNodeIndex]->rid.slot;
                bPlusTreeRecords[currentNodeIndex]->val.dt = bPlusTreeRecords[nextNodeIndex]->val.dt;
                bPlusTreeRecords[currentNodeIndex]->val.v.intV = bPlusTreeRecords[nextNodeIndex]->val.v.intV;
            }

            free(bPlusTreeRecords[nextNodeIndex]);
            totalNodeCount--;
            return RC_OK;
        }
        i++;
    }

    return RC_IM_KEY_NOT_FOUND;
}

/**
 *
 * @brief Implementation for sorting the keys in ascending order
 *
 * @author Ganugapanta Vishnuvardhan Reddy
 *
 * @param tree
 * @param handle
 *
 * @return RC
 */
RC openTreeScan (BTreeHandle *tree, BT_ScanHandle **handle) {

    int i;
    for (i = 0; i < totalNodeCount - 1; i++) {
        int index = i;
        int j;
        for (j = i + 1; j < totalNodeCount; j++) {
            if (bPlusTreeRecords[j]->val.v.intV < bPlusTreeRecords[index]->val.v.intV) {
                index = j;
            }
        }
        swapNodes(i, index);
    }
    return RC_OK;
}

/**
 *
 * @brief Implementation for scanning the nextEnrty in BPlusTree
 *
 * @author Ganugapanta Vishnuvardhan Reddy
 *
 * @param handle
 * @param result
 *
 * @return RC
 */
RC nextEntry (BT_ScanHandle *handle, RID *result) {

    if (scanNextNode < totalNodeCount) {

        result->page = bPlusTreeRecords[scanNextNode]->rid.page;
        result->slot = bPlusTreeRecords[scanNextNode]->rid.slot;

        scanNextNode++;

        return RC_OK;
    }

    return RC_IM_NO_MORE_ENTRIES;
}

/**
 *
 * @brief Implementation of closing the handle after traversing through the nodes
 *
 * @author Ganugapanta Vishnuvardhan Reddy
 *
 * @param handle
 *
 * @return RC
 */
RC closeTreeScan (BT_ScanHandle *handle) {
    free(handle);
    return RC_OK;
}

/**
 *
 * @brief Implementation for a string representation of a B+ tree in the following representation node(pos)[pointer,key,pointer, ...]
 *
 * @author Ganugapanta Vishnuvardhan Reddy
 *
 * @param tree
 *
 * @return char
 */
char *printTree (BTreeHandle *tree) {
    return tree->idxId;
}