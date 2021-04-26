#include <string.h>
#include <stdlib.h>
#include "btree_mgr.h"
#include "dberror.h"

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

/*******************************************************************
* DESCRIPTION :     Search for the index of given key in tree
*******************************************************************/
int searchKey (BTreeHandle *tree, Value *key) {
    int i = 0;

    while (i < totalNodeCount) {
        if (bPlusTreeRecords[i]->val.dt == key->dt
            && ((bPlusTreeRecords[i]->val.dt == DT_INT && bPlusTreeRecords[i]->val.v.intV == key->v.intV))) {
            return i;
        }
        i++;
    }

    return -1;
}

/************************************************************
 *            INIT AND SHUTDOWN INDEX MANAGER               *
 ************************************************************/

/*******************************************************************
* NAME :            RC initIndexManager (void *mgmtData)
*
* DESCRIPTION :     Initializes the index manager by setting a pointer to the memory that contains the B+-Tree
*					with a size of 50.
*
* PARAMETERS:
*            	void *mgmtData					Pointer to memory reserved for management data
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Index manager initialized succesfully
*
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-15     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC initIndexManager (void *mgmtData) {
    // Create a pointer to the memory that will contain up to 50 nodes
    bPlusTreeRecords = (record **) malloc (sizeof(record *) * B_TREE_NODE_SIZE);
    return RC_OK;
}

/*******************************************************************
* NAME :            RC shutdownIndexManager ()
*
* DESCRIPTION :     Terminates the index manager.
*
* PARAMETERS:
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Index manager terminated succesfully
*
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-15     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC shutdownIndexManager () {
    free(bPlusTreeRecords);
    return RC_OK;
}


/************************************************************
 *     CREATE, DESTROY, OPEN, AND CLOSE A BTREE INDEX       *
 ************************************************************/

/*******************************************************************
* NAME :            RC createBtree (char *idxId, DataType keyType, int n)
*
* DESCRIPTION :     Creates a B Tree with the given arguments
*
* PARAMETERS:
*            	char *idxId 					B Tree identifier
*				DataType keyType 				Data type of the B Tree
*				int n 							Node number
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	B Tree created succesfully
*
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-15     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC createBtree (char *idxId, DataType keyType, int n) {
    // Initialize the B Tree handle pointer
    BTreeHandle *btHandle;
    btHandle = (BTreeHandle *) malloc (sizeof(BTreeHandle) * 3);

    // Fill in the handle information
    btHandle->keyType = keyType;
    btHandle->idxId = idxId;

    // Set the global variables to 0
    totalNodeCount = 0;
    scanNextNode = 0;

    return RC_OK;
}

/*******************************************************************
* NAME :            RC openBtree (BTreeHandle **tree, char *idxId)
*
* DESCRIPTION :     Opens a B Tree identified by its id
*
* PARAMETERS:
*				BTreeHandle **tree 				Pointer to the B Tree array
*            	char *idxId 					B Tree identifier
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	B Tree opened succesfully
*
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-15     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC openBtree (BTreeHandle **tree, char *idxId) {
    // Aññpcate memory
    *tree = (BTreeHandle *) malloc (sizeof(BTreeHandle) * 3);
    (*tree)->idxId = (char *) malloc (sizeof(char) * (strlen(idxId) + 1));

    // Set the idxId
    (*tree)->idxId = idxId;

    return RC_OK;
}

/*******************************************************************
* NAME :            RC closeBtree (BTreeHandle *tree)
*
* DESCRIPTION :     Closes a B Tree passed as a parameter
*
* PARAMETERS:
*				BTreeHandle *tree 				Pointer to the B Tree to be closed
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	B Tree closed succesfully
*
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-15     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC closeBtree (BTreeHandle *tree) {
    free(tree);
    return RC_OK;
}

/*******************************************************************
* NAME :            RC deleteBtree (char *idxId)
*
* DESCRIPTION :     Deletes a B Tree identified by its id
*
* PARAMETERS:
*				char *idxId 					B Tree identifier
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	B Tree deleted succesfully
*
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-15     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC deleteBtree (char *idxId) {
    // Set global variables to 0
    totalNodeCount = 0;
    scanNextNode = 0;
    return RC_OK;
}

/************************************************************
 *            ACCESS INFORMATION ABOUT A B-TREE             *
 ************************************************************/

/*******************************************************************
* NAME :            RC getNumNodes (BTreeHandle *tree, int *result)
*
* DESCRIPTION :     Gets the total number of nodes in the B Tree
*
* PARAMETERS:
*				BTreeHandle *tree 				Pointer to the B Tree
*				int *result						Pointer to the integer containing the result
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Number returned successfully
*
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-15     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC getNumNodes (BTreeHandle *tree, int *result) {
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

    // Return result = total number of nodes - repeated nodes
    *result = totalNodeCount - counter;

    return RC_OK;
}

/*******************************************************************
* NAME :            RC getNumEntries (BTreeHandle *tree, int *result)
*
* DESCRIPTION :     Get the number of nodes
*
* PARAMETERS:
*            	BTreeHandle *tree					Pointer to tree
* 				int *result							Pointer to result variable
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Number returned successfully
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-18     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC getNumEntries (BTreeHandle *tree, int *result) {
    *result = totalNodeCount;
    return RC_OK;
}

/*******************************************************************
* NAME :            RC getKeyType (BTreeHandle *tree, DataType *result)
*
* DESCRIPTION :     Get data type of tree
*
* PARAMETERS:
*            	BTreeHandle *tree					Pointer to tree
* 				DataType *result							Pointer to result variable
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	DataType returned successfully
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-18     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC getKeyType (BTreeHandle *tree, DataType *result) {
    *result = tree->keyType;
    return RC_OK;
}

/************************************************************
 *                    INDEX ACCESS                          *
 ************************************************************/

/*******************************************************************
* NAME :            RC findKey (BTreeHandle *tree, Value *key, RID *result)
*
* DESCRIPTION :     Look for a given key in tree
*
* PARAMETERS:
*            	BTreeHandle *tree					Pointer to tree
* 				Value *key							Pointer to search key
* 				RID *result							Pointer to result variable
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Key found and returned successfully
* 						RC_IM_KEY_NOT_FOUND		Key not found
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-18     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC findKey (BTreeHandle *tree, Value *key, RID *result) {
    int i = searchKey(tree, key);
    if (i != -1) {
        result->page = bPlusTreeRecords[i]->rid.page;
        result->slot = bPlusTreeRecords[i]->rid.slot;
        return RC_OK;
    }
    return RC_IM_KEY_NOT_FOUND;
}

/*******************************************************************
* NAME :            RC insertKey (BTreeHandle *tree, Value *key, RID rid)
*
* DESCRIPTION :     Insert new node to tree
*
* PARAMETERS:
*            	BTreeHandle *tree					Pointer to tree
* 				Value *key							Pointer to key to be added
* 				RID rid							RID of node to be added
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Node added successfully
* 						RC_IM_KEY_ALREADY_EXISTS		Key already exists
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-18     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC insertKey (BTreeHandle *tree, Value *key, RID rid) {
    bPlusTreeRecords[totalNodeCount] = (record *)malloc(sizeof(record));

    if (searchKey(tree, key) == -1) {
        bPlusTreeRecords[totalNodeCount]->rid.page = rid.page;
        bPlusTreeRecords[totalNodeCount]->rid.slot = rid.slot;
        bPlusTreeRecords[totalNodeCount]->val.dt = key->dt;
        bPlusTreeRecords[totalNodeCount]->val.v.intV = key->v.intV;
        totalNodeCount++;
        return RC_OK;
    }
    else {
        return RC_IM_KEY_ALREADY_EXISTS;
    }
}

/*******************************************************************
* NAME :            RC deleteKey (BTreeHandle *tree, Value *key)
*
* DESCRIPTION :     Delete key from tree
*
* PARAMETERS:
*            	BTreeHandle *tree					Pointer to tree
* 				Value *key							Pointer to key needed to be delete
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Node deleted successfully
* 						RC_IM_KEY_NOT_FOUND		No key in given tree
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-18     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC deleteKey (BTreeHandle *tree, Value *key) {
    int index = searchKey(tree, key);
    if (index != -1) {
        int i = index;
        index++;
        for (; i < totalNodeCount && index < totalNodeCount; i++, index++) {
            bPlusTreeRecords[i]->rid.page = bPlusTreeRecords[index]->rid.page;
            bPlusTreeRecords[i]->rid.slot = bPlusTreeRecords[index]->rid.slot;
            bPlusTreeRecords[i]->val.dt = bPlusTreeRecords[index]->val.dt;
            bPlusTreeRecords[i]->val.v.intV = bPlusTreeRecords[index]->val.v.intV;
        }
        free(bPlusTreeRecords[i + 1]);
        totalNodeCount--;
        return RC_OK;
    }
    return RC_IM_KEY_NOT_FOUND;
}

/*******************************************************************
* NAME :            RC openTreeScan (BTreeHandle *tree, BT_ScanHandle **handle)
*
* DESCRIPTION :     Sort key in tree
*
* PARAMETERS:
*            	BTreeHandle *tree					Pointer to tree
* 				BT_ScanHandle **handle				TBD
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Sorted successfully
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-18     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
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
        record *tmpBTree = (record *) malloc (sizeof(record));

        tmpBTree = bPlusTreeRecords[i];
        bPlusTreeRecords[i] = bPlusTreeRecords[index];
        bPlusTreeRecords[index] = tmpBTree;
    }
    return RC_OK;
}

/*******************************************************************
* NAME :            RC nextEntry (BT_ScanHandle *handle, RID *result)
*
* DESCRIPTION :     Calculates the next entry in the given scan
*
* PARAMETERS:
*				BT_ScanHandle *handle 			Pointer to the information about the scan
*				RID *result 					Pointer to the result RID (page and slot)
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	Next entry returned successfully
*						RC_IM_NO_MORE_ENTRIES	No more entries to scan
*
* AUTHOR :
*			 	Adrian Tirados <atirados@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Adrian Tirados <atirados@hawk.iit.edu>   Initialization
*            2016-02-15     Adrian Tirados <atirados@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC nextEntry (BT_ScanHandle *handle, RID *result) {
    // Check if the next node to scan is still in bounds
    if (scanNextNode < totalNodeCount) {
        // Return the next node information
        result->page = bPlusTreeRecords[scanNextNode]->rid.page;
        result->slot = bPlusTreeRecords[scanNextNode]->rid.slot;
        // Increase the next node counter
        scanNextNode++;
        return RC_OK;
    }

    // If out of bounds, there are no more entries to scan
    return RC_IM_NO_MORE_ENTRIES;
}


/*******************************************************************
* NAME :            RC closeTreeScan (BT_ScanHandle *handle)
*
* DESCRIPTION :     free allocated memory from handle
*
* PARAMETERS:
* 				BT_ScanHandle **handle				handle needed to be free
*
* RETURN :
*            	Type:   RC                   	Returned code:
*            	Values: RC_OK                  	free successfully
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-18     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
RC closeTreeScan (BT_ScanHandle *handle) {
    free(handle);
    return RC_OK;
}


/************************************************************
 *                DEBUG AND TEST FUNCTIONS                  *
 ************************************************************/

/*******************************************************************
* NAME :            *printTree (BTreeHandle *tree)
*
* DESCRIPTION :     Debug method
*
* PARAMETERS:
* 				BTreeHandle *tree			tree wanted to debug
*
* RETURN :
*            	Type:   char*
*            	Values: idxId of the given tree
*
* AUTHOR :
*			 	Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>
*
* HISTORY :
*            DATE       	WHO     				                 DETAIL
*            -----------    ---------------------------------------  ---------------------------------
*            2016-04-13	    Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Initialization
*            2016-02-18     Patipat Duangchalomnin <pduangchalomnin@hawk.iit.edu>   Added comments and header comment
*
*******************************************************************/
char *printTree (BTreeHandle *tree) {
    return tree->idxId;
}

Value *stringToValue(char *val)
{
    Value *valRes = (Value *) malloc(sizeof(Value));

    valRes->dt = DT_INT;
    valRes->v.intV = atoi(val + 1);

    return valRes;
}