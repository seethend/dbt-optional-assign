#include<stdio.h>
#include<stdlib.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "pthread.h"

static pthread_mutex_t mutex_thread_handle = PTHREAD_MUTEX_INITIALIZER;

/**
 * Struct to track Buffer frame pages
 */
typedef struct BF_Page_Frame
{
    SM_PageHandle memory_page;
    PageNumber page_number;
    int dirty_flag;
    int client_count;
    int lru_count;
    int lfu_count;
} Page_Frame;


/**
 * Represents the size of buffer pool
 */
int buffer_size = 0;

/**
 * Tracks the number of page read from the disk
 */
int read_index = 0;

/**
 * Tracks the number of I/O writes to the disk
 */
int number_of_writes = 0;

/**
 * Tracks when a page is added to buffer pool
 */
int hit_count = 0;

/**
 * Used by CLOCK algorithm to point to the last added page in the buffer pool.
 */
int clock_pointer = 0;

/**
 * Used by LFU algorithm to store the least frequently used page.
 */
int lfu_pointer = 0;

/*==========================================
    -----------------------------------
     Page replacement strategies start
    ------------------------------------
=============================================*/

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
extern void FIFO(BM_BufferPool *const bm, Page_Frame *page)
{
    Page_Frame *page_frame = (Page_Frame *) bm->mgmtData;

    int i, f_Index;
    f_Index = read_index % buffer_size;


    for ( i = 0; i < buffer_size; i++)
    {
        if(page_frame[f_Index].client_count == 0)
        {
            if(page_frame[f_Index].dirty_flag == 1)
            {
                SM_FileHandle fh;
                openPageFile(bm->pageFile, &fh);
                writeBlock(page_frame[f_Index].page_number, &fh, page_frame[f_Index].memory_page);
                number_of_writes++;
            }
            page_frame[f_Index].memory_page = page->memory_page;
            page_frame[f_Index].page_number = page->page_number;
            page_frame[f_Index].dirty_flag = page->dirty_flag;
            page_frame[f_Index].client_count = page->client_count;
            break;
        }
        else {
            f_Index++;
            f_Index = (f_Index % buffer_size == 0) ? 0 : f_Index;
        }
    }
}

/**
 *
 * @brief Implementation of LFU replacement strategy by replacing the page that has been used least frequently
 *
 * @author Chandu Bhargav Kasukurthi
 *
 * @param bm
 * @param page
 *
 * @return void
 */
extern void LFU(BM_BufferPool *const bm, Page_Frame *page)
{
    Page_Frame *page_frame = (Page_Frame *) bm->mgmtData;

    int i;
    int j;
    int least_freq_Index;
    int l_f_Ref;
    least_freq_Index = lfu_pointer;

    for(i = 0; i < buffer_size; i++)
    {
        if(page_frame[least_freq_Index].client_count == 0)
        {
            least_freq_Index = (least_freq_Index + i) % buffer_size;
            l_f_Ref = page_frame[least_freq_Index].lfu_count;
            break;
        }
    }

    i = (least_freq_Index + 1) % buffer_size;

    for(j = 0; j < buffer_size; j++)
    {
        if(page_frame[i].lfu_count < l_f_Ref)
        {
            least_freq_Index = i;
            l_f_Ref = page_frame[i].lfu_count;
        }
        i = (i + 1) % buffer_size;
    }

    if(page_frame[least_freq_Index].dirty_flag == 1)
    {
        SM_FileHandle fh;
        openPageFile(bm->pageFile, &fh);
        writeBlock(page_frame[least_freq_Index].page_number, &fh, page_frame[least_freq_Index].memory_page);

        number_of_writes++;
    }

    page_frame[least_freq_Index].memory_page = page->memory_page;
    page_frame[least_freq_Index].page_number= page->page_number;
    page_frame[least_freq_Index].dirty_flag = page->dirty_flag;
    page_frame[least_freq_Index].client_count = page->client_count;
    lfu_pointer = least_freq_Index + 1;
}

/**
 *
 * @brief Implementation of LRU replacement strategy by replacing the page that has been used least recently.
 *
 * @author Seethend Reddy Dummansoor
 *
 * @param bm
 * @param page
 *
 * @return void
 */
extern void LRU(BM_BufferPool *const bm, Page_Frame *page)
{
    Page_Frame *page_frame = (Page_Frame *) bm->mgmtData;
    int i, least_hit_Record, least_hit_Count;

    for( i = 0; i < buffer_size; i++)
    {
        if(page_frame[i].client_count == 0)
        {
            least_hit_Count=page_frame[i].lru_count;
            least_hit_Record = i;
            break;
        }
    }

    for(i = least_hit_Record + 1; i < buffer_size; i++)
    {
        if(page_frame[i].lru_count < least_hit_Count)
        {
            least_hit_Count = page_frame[i].lru_count;
            least_hit_Record = i;
        }
    }

    if(page_frame[least_hit_Record].dirty_flag == 1)
    {
        SM_FileHandle fh;
        openPageFile(bm->pageFile, &fh);
        writeBlock(page_frame[least_hit_Record].page_number, &fh, page_frame[least_hit_Record].memory_page);

        number_of_writes++;
    }

    page_frame[least_hit_Record].memory_page = page->memory_page;
    page_frame[least_hit_Record].page_number = page->page_number;
    page_frame[least_hit_Record].dirty_flag = page->dirty_flag;
    page_frame[least_hit_Record].client_count = page->client_count;
    page_frame[least_hit_Record].lru_count = page->lru_count;
}

/**
 *
 * @brief Implementation of CLOCK replacement strategy by replacing the page in round robin matter and it has not been
 *          accessed since its last consideration.
 *
 * @author Swapna Bellam
 *
 * @param bm
 * @param page
 *
 * @return void
 */
extern void CLOCK(BM_BufferPool *const bm, Page_Frame *page)
{
    Page_Frame *page_frame = (Page_Frame *) bm->mgmtData;

    while(TRUE)
    {
        clock_pointer = (clock_pointer % buffer_size == 0) ? 0 : clock_pointer;

        if(page_frame[clock_pointer].lru_count != 0) {
            page_frame[clock_pointer++].lru_count = 0;
        } else {
            if(page_frame[clock_pointer].dirty_flag == 1) {
                SM_FileHandle fh;
                openPageFile(bm->pageFile, &fh);
                writeBlock(page_frame[clock_pointer].page_number, &fh, page_frame[clock_pointer].memory_page);
                number_of_writes++;
            }

            page_frame[clock_pointer].memory_page = page->memory_page;
            page_frame[clock_pointer].page_number = page->page_number;
            page_frame[clock_pointer].dirty_flag = page->dirty_flag;
            page_frame[clock_pointer].client_count = page->client_count;
            page_frame[clock_pointer].lru_count = page->lru_count;

            clock_pointer++;

            break;
        }
    }
}

/*========================================
    ---------------------------------
     Page replacement strategies end
    ----------------------------------
==========================================*/

/*======================================================
    ----------------------------------------------
     Buffer Manager Interface Pool Handling start
    ----------------------------------------------
========================================================*/

/**
 *
 * @brief Initialize the buffer pool objects, loading the pages into the buffer pool
 *
 * @author Anand Babu Badrichetty
 *
 * @param bm
 * @param pageFileName
 * @param numPages
 * @param strategy
 * @param stratData
 * @return RC
 */
extern RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)
{
    pthread_mutex_lock(&mutex_thread_handle);

    Page_Frame *page = malloc(sizeof(Page_Frame) * numPages);
    buffer_size = numPages;
    int i;

    for(i = 0; i < buffer_size; i++)
    {
        page[i].memory_page = NULL;
        page[i].page_number = -1;
        page[i].dirty_flag = page[i].client_count = page[i].lru_count = page[i].lfu_count = 0;
    }

    bm->mgmtData = page;
    bm->pageFile = (char *)pageFileName;
    bm->numPages = numPages;
    bm->strategy = strategy;

    number_of_writes = 0;
    clock_pointer = 0;
    lfu_pointer = 0;

    pthread_mutex_unlock(&mutex_thread_handle);
    return RC_OK;
}

/**
 *
 * @brief Shutdown buffer pool, thereby removing all the pages from the memory releasing
 *          some memory space along with freeing up all resources
 *
 * @author Anand Babu Badrichetty
 *
 * @param bm
 * @return RC
 */
extern RC shutdownBufferPool(BM_BufferPool *const bm)
{
    Page_Frame *page = (Page_Frame *)bm->mgmtData;
    forceFlushPool(bm);

    int i;
    for(i = 0; i < buffer_size; i++)
    {
        if(page[i].client_count != 0)
        {
            return RC_BUFFER_HAS_PINNED_PAGES;
        }
    }

    free(page);

    bm->mgmtData = NULL;

    return RC_OK;
}


/**
 *
 * @brief This function writes all the dirty_flag pages (having client_count = 0) to disk
 *
 * @author Anand Babu Badrichetty
 *
 * @param bm
 * @return RC
 */
extern RC forceFlushPool(BM_BufferPool *const bm)
{
    Page_Frame *page = (Page_Frame *)bm->mgmtData;

    int i;
    for(i = 0; i < buffer_size; i++)
    {
        if(page[i].client_count == 0 && page[i].dirty_flag == 1)
        {
            SM_FileHandle fh;
            openPageFile(bm->pageFile, &fh);
            writeBlock(page[i].page_number, &fh, page[i].memory_page);
            page[i].dirty_flag = 0;
            number_of_writes++;
        }
    }

    return RC_OK;
}

/*======================================================
    --------------------------------------------
     Buffer Manager Interface Pool Handling end
    --------------------------------------------
========================================================*/

/*======================================================
    ---------------------------------------------
     Buffer Manager Interface Access Pages start
    ---------------------------------------------
========================================================*/

/**
 *
 * @brief Checks for the empty buffer, if not searches for the page number and sets the buffer as modified
 *          by making dirty flag to 1
 *
 * @author Chandu Bhargav Kasukurthi
 *
 * @param bm
 * @param page
 * @return RC
 */
extern RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    Page_Frame *page_frame = (Page_Frame *)bm->mgmtData;

    int i;
    for( i = 0; i < buffer_size; i++)
    {
        if(page_frame[i].page_number == page->pageNum)
        {
            page_frame[i].dirty_flag = 1;
            break;
        }
    }
    return RC_OK;
}

/**
 *
 * @brief This function removes the page from the memory
 *
 * @author Chandu Bhargav Kasukurthi
 *
 * @param bm
 * @param page
 * @return RC
 */
extern RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    pthread_mutex_lock(&mutex_thread_handle);

    Page_Frame *page_frame = (Page_Frame *)bm->mgmtData;

    int i;
    for( i = 0; i < buffer_size; i++)
    {
        if(page_frame[i].page_number == page->pageNum)
        {
            page_frame[i].client_count--;
            break;
        }
    }

    pthread_mutex_unlock(&mutex_thread_handle);
    return RC_OK;
}

/**
 *
 * @brief writes the modified content onto the disk then removes the page from the memory
 *          and sets the dirty flag to '0'
 *
 * @author Chandu Bhargav Kasukurthi
 *
 * @param bm
 * @param page
 * @return RC
 */
extern RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    Page_Frame *page_frame = (Page_Frame *)bm->mgmtData;

    int i;

    for( i = 0; i < buffer_size; i++)
    {
        if(page_frame[i].page_number == page->pageNum)
        {
            SM_FileHandle fh;
            openPageFile(bm->pageFile, &fh);
            writeBlock(page_frame[i].page_number, &fh, page_frame[i].memory_page);
            page_frame[i].dirty_flag = 0;
            number_of_writes++;
            break;
        }
    }

    return RC_OK;
}

/*================================
    -------------------------
     Utility functions start
    -------------------------
==================================*/

/**
 *
 * @brief function handles the buffer overflow condition by implementing the appropriate replacement strategy
 *          and pins the page by incrementing the client_count by 1
 *
 * @author Seethend Reddy Dummansoor
 *
 * @param bm
 * @param page
 * @param pageNum
 */
extern void handleBufferPoolOverflow(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum) {
    Page_Frame *newPage = (Page_Frame *) malloc(sizeof(Page_Frame));

    SM_FileHandle fh;

    openPageFile(bm->pageFile, &fh);

    newPage->memory_page = (SM_PageHandle) malloc(PAGE_SIZE);

    readBlock(pageNum, &fh, newPage->memory_page);

    newPage->page_number = pageNum;
    newPage->dirty_flag = newPage->lfu_count = 0;
    newPage->client_count = 1;

    read_index++;
    hit_count++;

    if(bm->strategy == RS_LRU)
        newPage->lru_count = hit_count;
    else if(bm->strategy == RS_CLOCK)
        newPage->lru_count = 1;

    page->pageNum = pageNum;
    page->data = newPage->memory_page;

    if(bm->strategy == RS_LFU) {
        LFU(bm, newPage);
    } else if(bm->strategy == RS_FIFO) {
        FIFO(bm, newPage);
    } else if(bm->strategy == RS_CLOCK) {
        CLOCK(bm, newPage);
    } else if(bm->strategy == RS_LRU) {
        LRU(bm, newPage);
    } else {
        printf("%s", "\nAlgorithm Not Implemented\n");
    }
}

/**
 *
 * @brief Reads page from disk and inscribing page frame's content in the buffer pool
 *          and pins the page by incrementing the client_count by 1
 *
 * @author Seethend Reddy Dummansoor
 *
 * @param bm
 * @param page
 * @param pageNum
 * @param page_frame
 * @return RC
 */
extern RC handleEmptyBufferPool(BM_BufferPool *const bm, BM_PageHandle *const page,
                                const PageNumber pageNum, Page_Frame *page_frame) {
    SM_FileHandle fh;

    openPageFile(bm->pageFile, &fh);
    ensureCapacity(pageNum,&fh);

    page_frame[0].memory_page = (SM_PageHandle) malloc(PAGE_SIZE);

    readBlock(pageNum, &fh, page_frame[0].memory_page);

    page_frame[0].page_number = pageNum;
    page_frame[0].client_count++;

    page->pageNum = pageNum;
    page->data = page_frame[0].memory_page;

    read_index = hit_count = page_frame[0].lru_count = page_frame[0].lfu_count = 0;

    return RC_OK;
}

/**
 *
 * @brief handles the non empty buffer, finds the page and pins it in the buffer
 *
 * @author Seethend Reddy Dummansoor
 *
 * @param bm
 * @param page
 * @param pageNum
 * @param page_frame
 * @return RC
 */
extern RC handleNonEmptyBufferPool(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum,
                                   Page_Frame *page_frame) {

    int i;
    bool isBufferFull = true;

    for(i = 0; i < buffer_size; i++)
    {
        if(page_frame[i].page_number != -1)
        {
            if(page_frame[i].page_number == pageNum)
            {
                page_frame[i].client_count++;
                isBufferFull = false;
                hit_count++;

                if(bm->strategy == RS_LRU)
                    page_frame[i].lru_count = hit_count;
                else if(bm->strategy == RS_CLOCK)
                    page_frame[i].lru_count = 1;
                else if(bm->strategy == RS_LFU)
                    page_frame[i].lfu_count++;

                page->pageNum = pageNum;
                page->data = page_frame[i].memory_page;

                clock_pointer++;
                break;
            }
        } else {
            SM_FileHandle fh;

            openPageFile(bm->pageFile, &fh);

            page_frame[i].memory_page = (SM_PageHandle) malloc(PAGE_SIZE);
            readBlock(pageNum, &fh, page_frame[i].memory_page);

            page_frame[i].page_number = pageNum;
            page_frame[i].client_count = 1;
            page_frame[i].lfu_count = 0;

            read_index++;
            hit_count++;

            if(bm->strategy == RS_LRU)
                page_frame[i].lru_count = hit_count;
            else if(bm->strategy == RS_CLOCK)
                page_frame[i].lru_count = 1;

            page->pageNum = pageNum;
            page->data = page_frame[i].memory_page;

            isBufferFull = false;
            break;
        }
    }

    if(isBufferFull == true)
    {
        handleBufferPoolOverflow(bm, page, pageNum);
    }

    return RC_OK;
}

/*================================
    ------------------------
     Utility functions End
    ------------------------
==================================*/

/**
 *
 * @brief Checks if the buffer pool is empty, or the page exists in the pool and pins the page accordingly
 *
 * @author Seethend Reddy Dummansoor
 *
 * @param bm
 * @param page
 * @param pageNum
 * @return
 */
extern RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{
    pthread_mutex_lock(&mutex_thread_handle);
    Page_Frame *page_frame = (Page_Frame *)bm->mgmtData;

    if(page_frame[0].page_number != -1)
    {
        pthread_mutex_unlock(&mutex_thread_handle);
        return handleNonEmptyBufferPool(bm, page, pageNum, page_frame);
    } else {
        pthread_mutex_unlock(&mutex_thread_handle);
        return handleEmptyBufferPool(bm, page, pageNum, page_frame);
    }
}

/*======================================================
    -------------------------------------------
     Buffer Manager Interface Access Pages end
    -------------------------------------------
========================================================*/

/*====================================
    -----------------------------
     Statistical functions start
    -----------------------------
=======================================*/


/**
 *
 * @brief Returns an array of page numbers
 *
 * @author Swapna Bellam
 *
 * @param bm
 * @return PageNumber[]
 */
extern PageNumber *getFrameContents (BM_BufferPool *const bm)
{
    PageNumber *pageNumbers = malloc(sizeof(PageNumber) * buffer_size);

    Page_Frame *page_frame = (Page_Frame *) bm->mgmtData;

    int i;

    for(i = 0; i < buffer_size; i++)
    {
        if(page_frame[i].page_number != -1) {
            pageNumbers[i] = page_frame[i].page_number;
        } else {
            pageNumbers[i] = NO_PAGE;
        }
    }

    return pageNumbers;
}

/**
 *
 * @brief Returns an array of modified pages
 *
 * @author Swapna Bellam
 *
 * @param bm
 * @return bool[]
 */
extern bool *getDirtyFlags (BM_BufferPool *const bm)
{
    bool *dirtyFlags = malloc(sizeof(bool) * buffer_size);
    Page_Frame *page_frame = (Page_Frame *)bm->mgmtData;
    int i=0;
    while(i < buffer_size)
    {
        dirtyFlags[i++] = page_frame[i].dirty_flag == 1;
    }

    return dirtyFlags;
}

/**
 *
 * @brief Returns the array of client fix counts
 *
 * @author Swapna Bellam
 *
 * @param bm
 * @return int[]
 */
extern int *getFixCounts (BM_BufferPool *const bm)
{
    int *fixCounts = malloc(sizeof(int) * buffer_size);
    Page_Frame *page_frame= (Page_Frame *)bm->mgmtData;

    int i = 0;

    while(i < buffer_size)
    {
        fixCounts[i++] = page_frame[i].client_count != -1 ? page_frame[i].client_count : 0;
    }

    return fixCounts;
}

/**
 *
 * @brief Returns the number of pages that have been read from disk.
 *
 * @author Chandu Bhargav Kasukurthi
 *
 * @param bm
 * @return int
 */
extern int getNumReadIO (BM_BufferPool *const bm)
{
    return read_index + 1;
}

/**
 *
 * @brief Returns the number of pages written to the page file.
 *
 * @author Anand Babu Badrichetty
 *
 * @param bm
 * @return int
 */
extern int getNumWriteIO (BM_BufferPool *const bm)
{
    return number_of_writes;
}

/*================================
    ---------------------------
     Statistical functions end
    ---------------------------
==================================*/