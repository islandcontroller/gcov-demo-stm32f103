/*!****************************************************************************
 * @file
 * semihost.h
 *
 * @brief
 * Arm Semihosting support
 *
 * @date  30.07.2025
 ******************************************************************************/

#ifndef SEMIHOST_H_
#define SEMIHOST_H_

/*- Header files -------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>


/*- Macros -------------------------------------------------------------------*/
/*! @brief Default file handles
 *  @{                                                                        */
#define SEMIHOST_STDIN                0L  ///< stdin
#define SEMIHOST_STDOUT               1L  ///< stdout
#define SEMIHOST_STDERR               2L  ///< stderr
/*! @}                                                                        */


/*- Type definitions ---------------------------------------------------------*/
/// System heap information returned by @c bSemihostGetHeapInfo
typedef struct Semihost_HeapInfo
{
  void* pHeapBase;                    ///< Heap base address
  void* pHeapLimit;                   ///< Heap limit
  void* pStackBase;                   ///< Stack base address
  void* pStackLimit;                  ///< Stack limit
} Semihost_HeapInfo;


/*- Public interface ---------------------------------------------------------*/
// Request custom operation
uint64_t ullSemihostReqOp(uint32_t ulCmd, uint32_t ulArg);

// Console I/O
void vSemihostWriteC(char cChar);
void vSemihostWrite0(const char* pszStr);
char cSemihostReadC(void);

// Command line
uint32_t ulSemihostGetCmdline(void* pBuf, uint32_t ulSize);
int32_t lSemihostSystem(const char* pszCmd);

// File I/O
int32_t lSemihostOpen(const char* pszPath, uint32_t ulMode);
bool bSemihostClose(int32_t lFile);
int32_t lSemihostWrite(int32_t lFile, const void* pData, uint32_t ulLen);
int32_t lSemihostRead(int32_t lFile, void* pBuf, uint32_t ulLen);
bool bSemihostSeek(int32_t lFile, uint32_t ulPos);
bool bSemihostIsTTY(int32_t lFile);
int32_t lSemihostGetFLen(int32_t lFile);

// Error handling
int32_t lSemihostGetErrno(void);

// Filesystem operations
int32_t lSemihostRemove(const char* pszPath);
int32_t lSemihostRename(const char* pszFrom, const char* pszTo);
bool bSemihostGetTmpnam(char* pszBuf, uint8_t ucId, uint32_t ulSize);
bool bSemihostIsError(int32_t lValue);

// Timing
uint32_t ulSemihostGetClock(void);
uint32_t ulSemihostGetTime(void);
bool bSemihostGetElapsed(uint64_t* pullElapsed);
uint32_t ulSemihostGetTickFreq(void);

// Miscellaneous
bool bSemihostGetHeapInfo(Semihost_HeapInfo* psInfo);

#endif // SEMIHOST_H_
