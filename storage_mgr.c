//
// Created by seeth on 27-01-2021.
//
#include <stdio.h>
#include <stdlib.h>
#include "storage_mgr.h"

/**
 * @brief Gives user an indication that the storage manager is getting Initiated...
 *
 * @author Anand Babu Badrichetty
 *
 * @returns void
 */
void initStorageManager(void){

    printf("<======= Start of initStorageManager Method=======> \n");
    printf("<=======Initiating Storage manager======> \n");   // indicates that the storageManager is getting initiated by printing message on screen.

    //return;
}

/**
 *
 * @brief This function creates a new Page_Frame file with name "fileName". The initial fileSize is assumed as 1 page. This function is expected to fill this one page with '\0' bytes read from a memory block.
 *
 * @param fileName Name
 *
 * @return RC
 *
 * @author Anand Babu Badrichetty
 *
 */
RC createPageFile(char *fileName){

    FILE *filePointer;
    char page_data[PAGE_SIZE];
    int write_result;

    filePointer = fopen(fileName,"w");  // checks and creates a file with Name passed as parameter if it doesn't exists in Writing Mode

    //filling some data into array which is used later to write into file.
    for(int i=0; i < sizeof(page_data); i++ ){
        page_data[i] = '\0';
    }

    if(filePointer == NULL){    // checking if the file really exists
        fclose(filePointer);
        return RC_WRITE_FAILED;
    }

    write_result = fwrite(page_data, 1, PAGE_SIZE, filePointer);

    if(write_result != PAGE_SIZE){
        fclose(filePointer);
        destroyPageFile(fileName);
        return RC_WRITE_FAILED;
    }

    fclose(filePointer);

    return RC_OK;
}

/**
 *
 * @brief Opens an existing page file
 *
 * @author Anand Babu Badrichetty
 *
 * @param fileName
 * @param fHandle (Pointer to an area in memory storing the data of a block)
 *
 * @return RC
 */
RC openPageFile (char *fileName, SM_FileHandle *fHandle){


    FILE *filePointer;
    int fileSize;
    int read_result;

    filePointer = fopen(fileName, "r");

    if(filePointer == NULL){
        return RC_FILE_NOT_FOUND;
    }

    read_result = fseek(filePointer, 0, SEEK_END);

    if(read_result != 0){
        fclose(filePointer);
        return RC_RM_UNKOWN_DATATYPE;
    }

    fileSize = ftell(filePointer);
    if(fileSize == -1){
        fclose(filePointer);
        return RC_RM_UNKOWN_DATATYPE;
    }

    fHandle->fileName = fileName;
    fHandle->totalNumPages = (int)(fileSize / PAGE_SIZE + 1);
    fHandle->curPagePos = 0;


    fclose(filePointer);
    return RC_OK;
}

/**
 *
 * @brief destroys page file from disk
 *
 * @author Anand Babu badrichetty
 *
 * @param fileName
 *
 * @return RC
 */
RC destroyPageFile (char *fileName){

//    int return_code;
    int remove_result;

    FILE *filepointer;

    filepointer = fopen(fileName, "r");

    if(filepointer == NULL)
        return RC_FILE_NOT_FOUND;

    remove_result = remove(fileName);

//    if(remove_result == 0){
//        return_code = RC_OK;
//    }else{
//        return_code = RC_FILE_NOT_FOUND;
//    }

    return RC_OK;
}

/**
 *
 * @brief Closes an opened page file
 *
 * @author Swapna Bellam
 *
 * @param fHandle
 *
 * @return RC - some defined return code
 */
RC closePageFile (SM_FileHandle *fHandle){

    fHandle->fileName = "";
    fHandle->curPagePos = 0;
    fHandle->totalNumPages = 0;


    return RC_OK;
}

/**
 * @brief ReadBlock at pageNum from a file
 * @author Kasukurthi Chandu Bhargav
 *
 * @param pageNum where to start reading a file
 * @param fHandle pointer to SM_FileHandle struct
 * @param memPage character array of one page size which takes data
 * @return
 */

RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    //Condition to check pagenumber value
    if (pageNum > fHandle->totalNumPages || pageNum < 0)
        return RC_READ_NON_EXISTING_PAGE;

    //opening file in read mode - opens empty file
    FILE *filepointer;
    filepointer = fopen(fHandle->fileName, "r");

    //condition to check fileopen statement
    if(filepointer == NULL)
        return RC_FILE_NOT_FOUND;

    //Adjust the position of the pointer filestream, position is evaluated by pagenumber * PAGE_SIZE
    //seek is complete when it is equal to 0
    int isSeekSuccess = fseek(filepointer, (pageNum * PAGE_SIZE), SEEK_SET);
    if(isSeekSuccess == 0) {
        //Reading the content, storing it to the location set out by memory_page
        if(fread(memPage, sizeof(char), PAGE_SIZE, filepointer) < PAGE_SIZE)
            return RC_ERROR;
    } else {
        return RC_READ_NON_EXISTING_PAGE;
    }

    //Setting the current page position to the pointer
    //After reading 1 block/page what is the current posotion of the pointer in file. It returns the current location in file stream
    fHandle->curPagePos = ftell(filepointer);

    // Closing file stream so that all the buffers are flushed.
    fclose(filepointer);

    return RC_OK;
}

/**
 *
 * @brief gets the current block position in a file
 * @author Kasukurthi Chandu Bhargav
 *
 * @param fHandle pointer to SM_FileHandle struct
 * @return
 */

int getBlockPos(SM_FileHandle *fHandle) {
    return fHandle->curPagePos;
}

/**
 * @brief read first block of file
 * @author Kasukurthi Chandu Bhargav
 *
 * @param fHandle pointer to SM_FileHandle struct
 * @param memPage character array of one page size which takes data
 * @return
 */

RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    FILE *filepointer;
    filepointer = fopen(fHandle->fileName, "r");
    fseek(filepointer, 0, SEEK_SET);
    fread(memPage, sizeof(char), PAGE_SIZE, filepointer);
    fHandle->curPagePos = 0;
    fclose(filepointer);
    return RC_OK;
}

/**
 * @brief return previousblock of file into address pointed to memory_page
 * @author Kasukurthi Chandu Bhargav
 *
 * @param fHandle pointer to SM_FileHandle struct
 * @param memPage character array of one page size which takes data
 * @return
 */

RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle->curPagePos <= 0 || fHandle->curPagePos > fHandle->totalNumPages - 1)
        return RC_READ_NON_EXISTING_PAGE;
    else {
        FILE *filepointer;
        filepointer = fopen(fHandle->fileName, "r");
        int offset = (fHandle->curPagePos - 1) * PAGE_SIZE;
        fseek(filepointer, offset, SEEK_SET);
        fread(memPage, sizeof(char), PAGE_SIZE, filepointer);
        fHandle->curPagePos = fHandle->curPagePos - 1;
        fclose(filepointer);
        return RC_OK;
    }
}

/**
 *
 * @brief return currentblock of file into memory_page
 * @author Swapna Bellam
 *
 * @param fHandle pointer to SM_FileHandle struct
 * @param memPage character array
 * @return
 */

RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle->curPagePos < 0 || fHandle->curPagePos > fHandle->totalNumPages - 1)
        return RC_READ_NON_EXISTING_PAGE;
    else {
        FILE *filepointer;
        filepointer = fopen(fHandle->fileName, "r");
        int offset = fHandle->curPagePos * PAGE_SIZE;
        fseek(filepointer, offset, SEEK_SET);
        fread(memPage, sizeof(char), PAGE_SIZE, filepointer);
        fclose(filepointer);
        return RC_OK;
    }
}

/**
 *
 * @brief Read the last block in this file into memo address memory_page
 *
 * @author Swapna Bellam
 *
 * @param fHandle
 * @param memPage
 * @return RC
 */
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE *filePointer;

    filePointer = fopen(fHandle->fileName,"r");

    int offset = -PAGE_SIZE;

    fseek(filePointer, offset, SEEK_END);
    fread(memPage, sizeof(char), PAGE_SIZE, filePointer);

    fHandle->curPagePos=fHandle->totalNumPages-1;

    fclose(filePointer);

    return RC_OK;
}

/**
 *
 * @brief Write data from memory_page to the file using page number
 *
 * @author Dummansoor Seethend Reddy
 *
 * @param pageNum
 * @param fHandle
 * @param memPage
 * @return RC
 */
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){

    if(fHandle == NULL){
        return RC_FILE_HANDLE_NOT_INIT;
    }

    ensureCapacity (pageNum, fHandle);

    FILE *filePointer;
    RC returnCode;

    filePointer = fopen(fHandle->fileName,"r+");

    // Seek to the last page block and check if it exists
    if(fseek(filePointer,pageNum * PAGE_SIZE, SEEK_SET) != 0) {
        returnCode = RC_READ_NON_EXISTING_PAGE;
    } else if (fwrite(memPage, PAGE_SIZE, 1, filePointer) == 0) {
        returnCode = RC_WRITE_FAILED;
    } else {
        //Success write block, then curPagePos should be changed.
        fHandle->curPagePos = pageNum;
        returnCode = RC_OK;
    }

    fclose(filePointer);

    return returnCode;
}


/**
 *
 * @brief Write data into file to the current page number
 *
 * @author Dummansoor Seethend Reddy
 *
 * @param fHandle
 * @param memPage
 * @return RC
 */
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){

    if(fHandle == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    if(fHandle->curPagePos < 0) {
        return RC_WRITE_FAILED;
    }

    return writeBlock(fHandle->curPagePos, fHandle, memPage);
}

/**
 *
 * @brief Increase the number of pages in the file by one. The new last page should be filled with zero bytes.
 *
 * @author Dummansoor Seethend Reddy
 *
 * @param fHandle
 * @return RC
 */
RC appendEmptyBlock (SM_FileHandle *fHandle){

    if(fHandle == NULL){
        return RC_FILE_HANDLE_NOT_INIT;
    }

    FILE *filePointer;
    char *data;
    RC returnCode;

    data = (char *) calloc(1, PAGE_SIZE);
    filePointer = fopen(fHandle->fileName,"a+");

    if(fwrite(data, PAGE_SIZE, 1, filePointer) == 0) {
        returnCode = RC_WRITE_FAILED;
    } else {
        fHandle -> totalNumPages += 1;
        returnCode = RC_OK;
    }

    free(data);
    fclose(filePointer);

    return  returnCode;
}

/**
 *
 * @brief Increases the page size of the file if its below number of pages
 *
 * @author Dummansoor Seethend Reddy
 *
 * @param numberOfPages
 * @param fHandle
 * @return RC
 */
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle){
    if(fHandle == NULL){
        return RC_FILE_HANDLE_NOT_INIT;
    }
    if(fHandle -> totalNumPages >= numberOfPages){
        return RC_OK;
    }

    FILE *filePointer;
    long updatedCapacity;
    char *data;
    RC returnCode;

    updatedCapacity = (numberOfPages - fHandle -> totalNumPages) * PAGE_SIZE;
    data = (char *) calloc(1, updatedCapacity);

    filePointer = fopen(fHandle->fileName,"a+");

    long insertedFIleData = fwrite(data, updatedCapacity, 1, filePointer);

    if(insertedFIleData == 0) {
        returnCode = RC_WRITE_FAILED;
    } else {
        fHandle -> totalNumPages = numberOfPages;
        returnCode = RC_OK;
    }

    free(data);
    fclose(filePointer);

    return returnCode;
}
