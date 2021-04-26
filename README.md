Programming Assignment (Optional)
========================================

1. **Project Group Information**
    - **Chandu Bharghav Kasukurthi 		M13990058 		kasukucv@mail.uc.edu**
    - **Seethend Reddy Dummansoor 		M13989903 		dummansy@mail.uc.edu**
    - **Anand Babu Badrichetty 			M14001725 		badricau@mail.uc.edu**
    - **Vishnuvardhan Reddy Ganugapanta	M14026594 		ganugavy@mail.uc.edu**


2. **Project Structure**
    - group9
        - assign2
            - buffer_mgr.c
                - buffer_mgr.h
                - buffer_mgr_stat.c
                - buffer_mgr_stat.h
                - dberror.c
                - dberror.h
                - dt.h
                - Makefile
                - Readme.md
                - storage_mgr.c
                - storage_mgr.h
                - test_assign2_1.c
                - test_helper.h
        - .gitignore


3. **Project file Descriptions**
    - buffer_mgr.c
        - Buffer pool manager to manage fixed number of pages in memory
    - buffer_mgr.h
        - Interface of the Buffer manager
    - buffer_mgr_stat.c
        - Utility module to print or read the page or pool content
    - buffer_mgr_stat.h
        - Interface for print / read the page / pool content module
    - dberror.c
      - This print the error codes to the console
    - dberror.h
      - Here all the error codes of the program are defined
    - storage_mgr.c
      - All the implementations functions of the Storage Manager are defined here
    - storage_mgr.h
      - Interface of the Storage Manager
    - test_assign2_1.c
      - Contains all the test cases of Buffer Manager
    - test_helper.h
      - Helper header file for the test cases


4. **Execution Instructions**

    - Run the below command from the assign2 folder  
      ```
      $ make
      ```   
      ```
      $ make run
      ```
    - To clean up and re-run the code, run the reset command like below  
      ```
      $ make reset
      ```


5. **Storage Manager Descriptions**
    - createPageFile
        - **Author**: _Anand Babu Badrichetty_
        - **Desc**: _This function creates a new Page file with name "fileName". The initial fileSize is assumed as 1 page. This function is expected to fill this one page with '\0' bytes read from a memory block._

    - openPageFile
        - **Author**: _Anand Babu Badrichetty_
        - **Desc**: _Opens an existing page file_

    - closePageFile
        - **Author**: _Swapna Bellam_
        - **Desc**: _Closes an opened page file_

    - destroyPageFile
        - **Author**: _Anand Babu Badrichetty_
        - **Desc**: _destroys page file from disk_

    - readBlock
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _ReadBlock at pageNum from a file_

    - getBlockPos
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _gets the current block position in a file_

    - readFirstBlock
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _read first block of file_

    - readPreviousBlock
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _return previousblock of file into address pointed to memPage_

    - readCurrentBlock
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _return currentblock of file into memPage_

    - readNextBlock
        - **Author**: _Swapna Bellam_
        - **Desc**: _Reads the next block to fHandle->curPagePos_

    - readLastBlock
        - **Author**: _Swapna Bellam_
        - **Desc**: _Read the last block in this file into memo address memPage_

    - writeBlock
        - **Author**: _Seethend Reddy Dummansoor_
        - **Desc**: _Write data from memPage to the file using page number_

    - writeCurrentBlock
        - **Author**: _Seethend Reddy Dummansoor_
        - **Desc**: _Write data into file to the current page number_

    - appendEmptyBlock
        - **Author**: _Seethend Reddy Dummansoor_
        - **Desc**: _Increase the number of pages in the file by one. The new last page should be filled with zero bytes._

    - ensureCapacity
        - **Author**: _Seethend Reddy Dummansoor_
        - **Desc**: _Increases the page size of the file if its below number of pages_


6. **Buffer Manager Descriptions**
    - BF_Page_Frame
        - **Desc**: _Struct to track Buffer frame pages_
    - FIFO
        - **Author**: _Anand Babu Badrichetty_
        - **Desc**: _Implementation of FIFO page replacement strategy by replacing the page that has been in memory the longest._
    - LFU
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _Implementation of LFU page replacement strategy by replacing the page that has been used least frequently._
    - LRU
        - **Author**: _Seethend Reddy Dummansoor_
        - **Desc**: _Implementation of LRU replacement strategy by replacing the page that has been used least recently._
    - CLOCK
        - **Author**: _Swapna Bellam_
        - **Desc**: _Implementation of CLOCK replacement strategy by replacing the page in round robin matter and it has not been accessed since its last consideration._
    - initBufferPool
        - **Author**: _Anand Babu Badrichetty_
        - **Desc**: _Initialize the buffer pool objects, loading the pages into the buffer pool_
    - shutdownBufferPool
        - **Author**: _Anand Babu Badrichetty_
        - **Desc**: _Shutdown buffer pool, thereby removing all the pages from the memory releasing some memory space along with freeing up all resources_
    - forceFlushPool
        - **Author**: _Anand Babu Badrichetty_
        - **Desc**: _This function writes all the dirty_flag pages (having client_count = 0) to disk_
    - markDirty
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _Checks for the empty buffer, if not searches for the page number and sets the buffer as modified by making dirty flag to 1_
    - unpinPage
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _This function removes the page from the memory_
    - forcePage
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _writes the modified content onto the disk then removes the page from the memory and sets the dirty flag to '0'_
    - handleBufferPoolOverflow
        - **Author**: _Seethend Reddy Dummansoor_
        - **Desc**: _function handles the buffer overflow condition by implementing the appropriate replacement strategy and pins the page by incrementing the client_count by 1_
    - handleEmptyBufferPool
        - **Author**: _Seethend Reddy Dummansoor_
        - **Desc**: _Reads page from disk and inscribing page frame's content in the buffer pool and pins the page by incrementing the client_count by 1_
    - handleNonEmptyBufferPool
        - **Author**: _Seethend Reddy Dummansoor_
        - **Desc**: _handles the non empty buffer, finds the page and pins it in the buffer_
    - pinPage
        - **Author**: _Seethend Reddy Dummansoor_
        - **Desc**: _Checks if the buffer pool is empty, or the page exists in the pool and pins the page accordingly_
    - getFrameContents
        - **Author**: _Swapna Bellam_
        - **Desc**: _Returns an array of page numbers_
    - getDirtyFlags
        - **Author**: _Swapna Bellam_
        - **Desc**: _Returns an array of modified pages_
    - getFixCounts
        - **Author**: _Swapna Bellam_
        - **Desc**: _Returns the array of client fix counts_
    - getNumReadIO
        - **Author**: _Chandu Bhargav Kasukurthi_
        - **Desc**: _Returns the number of pages that have been read from disk._
    - getNumWriteIO
        - **Author**: _Anand Babu Badrichetty_
        - **Desc**: _Returns the number of pages written to the page file._


7. **Test Cases**
    - Test cases in test_assign2_1.c
        - testCreatingAndReadingDummyPages
            - createDummyPages
              - Initiating the buffer pool
              - Pins the page in memory
              - Marks the page as dirty
              - Unpins the page
              - Shutdown the buffer pool
            - checkDummyPages
                - Initiating the buffer pool
                - Pins the page in memory
                - Compares the data with expected data
                - Unpins the page
                - Shutdown the buffer pool
        - testReadPage
            - Create a page file
            - Initiating the buffer pool
            - Pins the multiple pages in memory
            - Marks the page as dirty
            - Unpins the pages
            - Force removes the page from memory
            - Shutdown the buffer pool
            - Destroys the created page file
        - testFIFO
            - Test the FIFO page replacement strategy
        - testLRU
            - Test the LRU page replacement strategy
        - testLFU
            - Test the LFU page replacement strategy
        - testClock
            - Test the CLOCK page replacement strategy  
	