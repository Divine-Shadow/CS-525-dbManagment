/************************************************************
 *                    interface                             *
 ************************************************************/
/* manipulating page files */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "dberror.h"
#include "storage_mgr.h"
#define metaDataSize 50


void initStorageManager (void){}
RC createPageFile (char *fileName){
  
  /*
   * Function Name: createPageFile
   *
   * Description:
   *      Creates a file on the hard-disk with the specified name
   * Precondition:
   *	 fileName points to a valid name for the host system
   * Postcondition:
   * 	 A one page file with the given name exists,
   * 	 with the one page populated with '\0'
   * Parameters:
   *      char *fileName: File name for new pageFile
   * Return:
   * 	RC:RC_OK
   *
   * History:
   *  Date        Name            Content
   *  ----------  --------------  ---------------------
   *  03/14/2016  Bill Molchan    Initialization
   */
//create and open file
  FILE* fp;
  fp =fopen(fileName,"w");

  //metadata
  //Use base 10 log to determine the length the string needs to be
  char strOne[2]="1";
  fputs(strOne,fp);

  //create default page
  //TODO abstract and merge with append empty
  char defaultPage[PAGE_SIZE];
  fseek(fp,metaDataSize,SEEK_SET);
  char defaultChar='\0';
  int i=0;
  while(i<PAGE_SIZE){
    fputc(defaultChar,fp);
    i++;
  }

  fclose(fp);
  return RC_OK;
}
RC openPageFile (char *fileName, SM_FileHandle *fHandle){
  /*
   * Function Name: openPageFile
   *
   * Description:
   *      Opens a file on the hard-disk with the specified name
   * 	  That file is then loaded into the file handler struct
   * 	  File remains open until closed by closePageFile
   *
   * Precondition:
   *	 fileName points to an existing pagefile on host system
   * Postcondition:
   * 	 fileName is open for read/write
   * 	 fHandle has all the meta-data stored in the file
   * Parameters:
   *      char *fileName: File name for target pagefile
   *      SM_FileHandle *fHandle:Encapsulation of page read/write
   * Return:
   * 	RC:RC_OK if successful
   * 	RC:RC_FILE_NOT_FOUND if error opening file
   *
   * History:
   *  Date        Name            Content
   *  ----------  --------------  ---------------------
   *  03/14/2016  Bill Molchan    Initialization
   */

  //Try to open file
  FILE *fp;
  fp = fopen(fileName,"r+");

  //Check if file exists, return error if not found
  if (fp == NULL){return RC_FILE_NOT_FOUND;}

  //Read the metaData for the number of pages TODO abstract into new function
  char strSize[metaDataSize];
  fgets(strSize, metaDataSize,fp);//Destructive update on strSize

  //write to the file handler TODO clarify if this method is to close the file
  fHandle->curPagePos=0;
  fHandle->totalNumPages=atoi(strSize);
  fHandle->fileName=fileName;
  /*fclose(fp); //This is sanitary, but makes closePageFile obsolete
   * Either the file should be closed here, or the pointer should be added to mgmtInfo
   * I have a strong preference for closing the file here and reopening it as needed...
   */
  fHandle->mgmtInfo=fp;
  return RC_OK;}
RC closePageFile (SM_FileHandle *fHandle){
  //update the number of pages
  //set head to start
  fseek(fHandle->mgmtInfo,0,SEEK_SET);
  //create metadata string with length needed to represent number of pages
  char newMeta[metaDataSize];
  //fill string with number of pages in string form
  sprintf(newMeta,"#d",fHandle->totalNumPages);
  //write string to file
  fputs(newMeta,fHandle->mgmtInfo);
  fclose(fHandle->mgmtInfo);
  return RC_OK;
}
RC destroyPageFile (char *fileName){
   /*
   * Function Name:destroyPageFile
   *
   * Description:
   * 	unalloctes memmory for pagefile on host system  
   * Precondition:
   * 	fileName points to the name of an existing file
   * Postcondition:
   * 	there is no more file name fileName in the current dircetory
   * Parameters:
   * 	char *fileName    The name of the file to be destroyed
   * Return:
   * 	RC:RC_OK if successful
   *
   * History:
   *  Date        Name            Content
   *  ----------  --------------  ---------------------
   *  03/14/2016  Bill Molchan    Initialization
   */


 
 remove(fileName);
  return RC_OK;
}

/* reading blocks from disc */
RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){/*
   * Function Name:readBlock
   *
   * Description:
   * 	Reads a block from disk into memory  
   * Precondition:
   * 	There is enough space at the pagehandler to store the page
   * Postcondition:
   * 	The memory location contains the contents of the stored page
   * Parameters:
   * 	fHandle the handler for the pagefile on disk
   *    pHandle the location in memory for the contents to go
   *    pageNum the location of the page in the file.
   * Return:
   * 	RC:RC_OK if successful
   * 	RC:RC_NON_EXISTING_PAGE if the page is not in the file
   * History:
   *  Date        Name            Content
   *  ----------  --------------  ---------------------
   *  03/14/2016  Bill Molchan    Initialization
   */
  //Check to make sure that the requested page is contained within the file
  if(pageNum<0||pageNum>=fHandle->totalNumPages){return RC_READ_NON_EXISTING_PAGE;}



 /* //debugging tools
  fseek(fHandle->mgmtInfo,0,SEEK_SET);
  char curMeta[metaDataSize];
  char thePage[PAGE_SIZE+4];
  fgets(curMeta,metaDataSize,fHandle->mgmtInfo);
  fgets(thePage,PAGE_SIZE,fHandle->mgmtInfo);
*/


  //set the location in the file to the beginning of the current block
  fseek(fHandle->mgmtInfo,metaDataSize+((pageNum)*PAGE_SIZE),SEEK_SET);

  //Clobber memPage with the contents of the block
  fgets(memPage,PAGE_SIZE+1,fHandle->mgmtInfo);

  //adjust the current position
  fHandle->curPagePos=pageNum;
  return RC_OK;
}

int getBlockPos (SM_FileHandle *fHandle){
  /* Function Name:gotBlockPos
   *
   * Description:
   * 	Reads the block position from the filehandler  
   * Precondition:
   * 	The fileHandler was properly initialized
   * Postcondition:
   *	The return value contains the current page pointer from the file handler.
   *
   * Parameters:
   * 	fHandle the handler for the pagefile on disk
   * Return:
   *	The location of the last accesed page in the page-file
   * History:
   *  Date        Name            Content
   *  ----------  --------------  ---------------------
   *  03/14/2016  Bill Molchan    Initialization
   */
    
return fHandle->curPagePos;}

/*
 * The following functions all follow the general form:
 * 
 * Parameters: file handler and memmory for page contents
 * Return    : RC_OK if success, RC_NON_EXISTING_PAGE on outofbounds
 * Which Page is determined by the file name
 * * History:
   *  Date        Name            Content
   *  ----------  --------------  ---------------------
   *  03/14/2016  Bill Molchan    Initialization
 */
  
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  fHandle->curPagePos=0;
  return readBlock(0,fHandle,memPage);
}
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  fHandle->curPagePos--;
  return readBlock(getBlockPos(fHandle),fHandle,memPage);
}
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  return readBlock(getBlockPos(fHandle),fHandle,memPage);
}
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  fHandle->curPagePos++;
  return readBlock(getBlockPos(fHandle),fHandle,memPage);
}
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  fHandle->curPagePos=fHandle->totalNumPages-1;
  return readBlock(fHandle->totalNumPages-1,fHandle,memPage);
}

/* writing blocks to a page file */
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
  //Check to make sure that the requested page is contained within the file
  if(pageNum<0||pageNum>=fHandle->totalNumPages){return RC_READ_NON_EXISTING_PAGE;}

  //set the location in the file to the beginning of the current block TODO abstract and merge with read
  fseek(fHandle->mgmtInfo,metaDataSize+((pageNum)*PAGE_SIZE),SEEK_SET);

  //write to the page in fHandle, at the new location, the contents of memPage
  fputs(memPage,fHandle->mgmtInfo);
  return RC_OK;
}
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  return writeBlock(getBlockPos(fHandle),fHandle,memPage);
}
RC appendEmptyBlock (SM_FileHandle *fHandle){
  //Move write head
  fseek(fHandle->mgmtInfo,metaDataSize+PAGE_SIZE*(fHandle->totalNumPages),SEEK_SET);
  //inc page count
  fHandle->totalNumPages++;
  //add page
  char defaultChar='\0';
  int i=0;
  while(i<PAGE_SIZE){
    fputc(defaultChar,fHandle->mgmtInfo);
    i++;
  }
  //increment the current block pos
  fHandle->curPagePos++;
  //finish with the writeBlock method
  return RC_OK;
}
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle){
  /*Loop Declaration
   * PostCondition: File is at least number of pages long
   * Termination Function: number of remaining pages to be written
   * Termination Lower Bound: 0 pages remaining
   * Termination Decrement: appendEmptyBlock
   * * History:
   *  Date        Name            Content
   *  ----------  --------------  ---------------------
   *  03/14/2016  Bill Molchan    Initialization
   */
  while((numberOfPages - fHandle->totalNumPages)>0){
    appendEmptyBlock(fHandle);
  }
  return RC_OK;
}
