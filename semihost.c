/*!****************************************************************************
 * @file
 * semihost.c
 *
 * @brief
 * Arm Semihosting support
 *
 * @date  30.07.2025
 ******************************************************************************/

/*- Header files -------------------------------------------------------------*/
#include <string.h>
#include "semihost.h"


/*- Macros -------------------------------------------------------------------*/
/*! @brief Semihosting command numbers as defined by Arm
 *  @{                                                                        */
#define SYS_OPEN                      0x01uL  ///< Open file on host
#define SYS_CLOSE                     0x02uL  ///< Close file on host
#define SYS_WRITEC                    0x03uL  ///< Write char to debug console
#define SYS_WRITE0                    0x04uL  ///< Write string to dbg console
#define SYS_WRITE                     0x05uL  ///< Write data to file
#define SYS_READ                      0x06uL  ///< Read data from file
#define SYS_READC                     0x07uL  ///< Read char from dbg console
#define SYS_ISERROR                   0x08uL  ///< Check if ret code is error
#define SYS_ISTTY                     0x09uL  ///< Check if file is TTY device
#define SYS_SEEK                      0x0AuL  ///< Seek in file
#define SYS_FLEN                      0x0CuL  ///< Get file size
#define SYS_TMPNAM                    0x0DuL  ///< Get tmp file name
#define SYS_REMOVE                    0x0EuL  ///< Delete file on host
#define SYS_RENAME                    0x0FuL  ///< Rename file on host
#define SYS_CLOCK                     0x10uL  ///< Get duration since start
#define SYS_TIME                      0x11uL  ///< Get sys timestamp from host
#define SYS_SYSTEM                    0x12uL  ///< Exec shell cmd on host
#define SYS_ERRNO                     0x13uL  ///< Get errno value from host
#define SYS_GET_CMDLINE               0x15uL  ///< Get cmdline string (args)
#define SYS_HEAPINFO                  0x16uL  ///< Send stack/heap info to host
#define SYS_EXIT                      0x18uL  ///< Send application exit code
#define SYS_ELAPSED                   0x30uL  ///< Get ticks since start
#define SYS_TICKFREQ                  0x31uL  ///< Get tick freq (ticks/sec)
/*! @}                                                                        */


/*- Public interface ---------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Request semihosting operation from host
 *
 * Triggers a service call exception using the semihosting "magic number" to
 * signal the command to a connected debugger using the BKPT instruction [1].
 * This function assumes that @c ulCmd is passed in register r0, and @c pArgs
 * is passed in register r1.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/about-semihosting/the-semihosting-interface
 * [2] https://developer.arm.com/documentation/107656/0101/Getting-started-with-Armv8-M-based-systems/Procedure-Call-Standard-for-Arm-Architecture--AAPCS-
 *
 * @param[in] ulCmd   Semihosting command code "SYS_..." (r0)
 * @param[in] ulArg   Argument, if any (r1)
 * @return  (uint64_t)  Command return value (r0), optional args (r1)
 * @date  30.07.2025
 ******************************************************************************/
__attribute__((naked)) uint64_t ullSemihostReqOp(uint32_t ulCmd, uint32_t ulArg)
{
  // Suppress warnings - args read by debugger
  (void)ulCmd; (void)ulArg;

  // Trigger service call to debugger
  __asm__ volatile (
    "bkpt   0xAB                      \r\n" // 0xAB = semihosting "magic number"
    "bx     lr                        \r\n" // function epilogue
  );
}

/*!****************************************************************************
 * @brief
 * Write character to connected debug console
 *
 * Uses the "SYS_WRITEC" [1] command to transmit a single character to a con-
 * nected debugger console.
 *
 * @note This function call blocks until the character is received by the de-
 * bugger.
 *
 * Reference:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-writec--0x03-
 *
 * @param[in] cChar Character to be transmitted
 * @date  30.07.2025
 ******************************************************************************/
void vSemihostWriteC(char cChar)
{
  uint32_t aulArgs[1] = { (uint32_t)cChar };
  (void)ullSemihostReqOp(SYS_WRITEC, (uint32_t)aulArgs);
}

/*!****************************************************************************
 * @brief
 * Write null-terminated string to connected debug console
 *
 * Uses the "SYS_WRITE0" [1] command to transmit a null-terminated string to a
 * connected debugger console.
 *
 * @note This function call blocks until the entire string is received by the
 * debugger.
 *
 * Reference:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-write0--0x04-
 *
 * @param[in] pszStr  Null-terminated string to be transmitted
 * @date  30.07.2025
 ******************************************************************************/
void vSemihostWrite0(const char* pszStr)
{
  (void)ullSemihostReqOp(SYS_WRITE0, (uint32_t)pszStr);
}

/*!****************************************************************************
 * @brief
 * Read character from connected debug console
 *
 * Blocks and reads a single character from a connected debug console using the
 * "SYS_READC" [1] command.
 *
 * @note This function call blocks until a character is received.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-readc--0x07-
 *
 * @return  (uint32_t)  Character received from the console
 * @date  30.07.2025
 ******************************************************************************/
char cSemihostReadC(void)
{
  return (char)ullSemihostReqOp(SYS_READC, 0uL);
}

/*!****************************************************************************
 * @brief
 * Get commandline arguments from host
 *
 * Receives the command line string from the host using the "SYS_GET_CMDLINE"
 * [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-get-cmdline--0x15-
 *
 * @param[out] pBuf   Destination buffer
 * @param[in] ulSize  Buffer size in bytes
 * @return  (uint32_t)  Command line string length
 * @date  03.08.2025
 ******************************************************************************/
uint32_t ulSemihostGetCmdline(void* pBuf, uint32_t ulSize)
{
  uint32_t aulArgs[2] = { (uint32_t)pBuf, ulSize };
  uint64_t ullResult = ullSemihostReqOp(SYS_GET_CMDLINE, (uint32_t)aulArgs);
  return ((uint32_t)ullResult == 0uL) ? (ullResult >> 32) : 0uL;
}

/*!****************************************************************************
 * @brief
 * Run command on host shell
 *
 * Executes the specified command string in a host shell session using the
 * "SYS_SYSTEM" [1] command.
 *
 * @warning The command passed to the host is executed on the host. Ensure that
 * any command passed has no unintended consequences.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-system--0x12-
 *
 * @param[in] pszCmd  Command line string
 * @return  (int32_t) Exit code
 * @date  03.08.2025
 ******************************************************************************/
int32_t lSemihostSystem(const char* pszCmd)
{
  uint32_t aulArgs[2] = { (uint32_t)pszCmd, (uint32_t)strlen(pszCmd) };
  return (int32_t)ullSemihostReqOp(SYS_SYSTEM, (uint32_t)aulArgs);
}

/*!****************************************************************************
 * @brief
 * Open a file on the host system
 *
 * Opens a file on the host system using the "SYS_OPEN" [1] command. See [1]
 * for supported file access flags.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-open--0x01-
 *
 * @param[in] pszPath Null terminated file or device path
 * @param[in] pszMode File access flags
 * @return  (int32_t) File handle
 * @retval  -1  Open failed
 * @date  03.08.2025
 ******************************************************************************/
int32_t lSemihostOpen(const char* pszPath, uint32_t ulMode)
{
  // Determine string length
  uint32_t aulArgs[3] = { (uint32_t)pszPath, ulMode, (uint32_t)strlen(pszPath) };
  return (int32_t)ullSemihostReqOp(SYS_OPEN, (uint32_t)aulArgs);
}

/*!****************************************************************************
 * @brief
 * Close a previously opened file on the host system
 *
 * Closes the file specified by the given handle using the "SYS_CLOSE" [1] com-
 * mand.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-close--0x02-
 *
 * @param[in] lFile File handle
 * @return  (bool) Operation successful
 * @date  03.08.2025
 ******************************************************************************/
bool bSemihostClose(int32_t lFile)
{
  uint32_t aulArgs[1] = { (uint32_t)lFile };
  return (uint32_t)ullSemihostReqOp(SYS_CLOSE, (uint32_t)aulArgs) == 0uL;
}

/*!****************************************************************************
 * @brief
 * Write data to file
 *
 * Writes data from a buffer on the target to an open file on the host, using
 * the "SYS_WRITE" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-write--0x05-
 *
 * @param[in] lFile File handle
 * @param[in] pData Source data buffer
 * @param[in] ulLen Number of bytes to be written
 * @return  (int32_t) Remaining bytes not written to file
 * @date  03.08.2025
 ******************************************************************************/
int32_t lSemihostWrite(int32_t lFile, const void* pData, uint32_t ulLen)
{
  uint32_t aulArgs[3] = { (uint32_t)lFile, (uint32_t)pData, ulLen };
  return (int32_t)ullSemihostReqOp(SYS_WRITE, (uint32_t)aulArgs);
}

/*!****************************************************************************
 * @brief
 * Read data from file into buffer
 *
 * Reads data from an open file on the host into a buffer on the target, using
 * the "SYS_READ" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-read--0x06-
 *
 * @param[in] lFile File handle
 * @param[out] pBuf Destination buffer
 * @param[in] ulLen Buffer size or number of bytes to be read
 * @return  (int32_t) Remaining number of bytes not read
 * @date  03.08.2025
*******************************************************************************/
int32_t lSemihostRead(int32_t lFile, void* pBuf, uint32_t ulLen)
{
  uint32_t aulArgs[3] = { (uint32_t)lFile, (uint32_t)pBuf, ulLen };
  return (int32_t)ullSemihostReqOp(SYS_READ, (uint32_t)aulArgs);
}

/*!****************************************************************************
 * @brief
 * Seek to position in file
 *
 * Moves file pointer to absolute position @c ulPos given in bytes from the
 * start of the file, using the "SYS_SEEK" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-seek--0x0a-
 *
 * @param[in] lFile File handle
 * @param[in] ulPos Absolute position, in bytes
 * @return  (bool) Operation successful
 * @date  03.08.2025
 ******************************************************************************/
bool bSemihostSeek(int32_t lFile, uint32_t ulPos)
{
  uint32_t aulArgs[2] = { (uint32_t)lFile, ulPos };
  return (uint32_t)ullSemihostReqOp(SYS_SEEK, (uint32_t)aulArgs) == 0uL;
}

/*!****************************************************************************
 * @brief
 * Get file size
 *
 * Retrieves the file size for the opened file using the "SYS_FLEN" [1] com-
 * mand.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-flen--0x0c-
 *
 * @param[in] lFile File handle
 * @return  (int32_t) File size in bytes or error indication
 * @retval  -1  Failure
 * @date  03.08.2025
 ******************************************************************************/
int32_t lSemihostGetFLen(int32_t lFile)
{
  uint32_t aulArgs[1] = { (uint32_t)lFile };
  return (int32_t)ullSemihostReqOp(SYS_FLEN, (uint32_t)aulArgs);
}

/*!****************************************************************************
 * @brief
 * Check file type
 *
 * Checks if the file is connected to an interactive device (console etc) using
 * the "SYS_ISTTY" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-istty--0x09-
 *
 * @param[in] lFile File handle
 * @return  (bool)  File is an interactive device
 * @date  03.08.2025
 ******************************************************************************/
bool bSemihostIsTTY(int32_t lFile)
{
  uint32_t aulArgs[1] = { (uint32_t)lFile };
  return (uint32_t)ullSemihostReqOp(SYS_ISTTY, (uint32_t)aulArgs) == 1uL;
}

/*!****************************************************************************
 * @brief
 * Get errno from host
 *
 * Retrieves the "errno" value from the host using the "SYS_ERRNO" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-errno--0x13-
 *
 * @return  (int32_t) Errno value
 * @date  03.08.2025
 ******************************************************************************/
int32_t lSemihostGetErrno(void)
{
  return (int32_t)ullSemihostReqOp(SYS_ERRNO, 0uL);
}

/*!****************************************************************************
 * @brief
 * Delete file on the host filesystem
 *
 * Deletes a file on the host filesystem using the "SYS_REMOVE" [1] command.
 *
 * @warning Deletes a specified file on the host filing system.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-remove--0x0e-
 *
 * @param[in] pszPath File path
 * @return  (int32_t) Host-specific status code, see @c bSemihostIsError
 * @date  03.08.2025
*******************************************************************************/
int32_t lSemihostRemove(const char* pszPath)
{
  uint32_t aulArgs[2] = { (uint32_t)pszPath, (uint32_t)strlen(pszPath) };
  return (int32_t)ullSemihostReqOp(SYS_REMOVE, (uint32_t)aulArgs);
}

/*!****************************************************************************
 * @brief
 * Rename file on the host filesystem
 *
 * Renames a file on the host filesystem using the "SYS_RENAME" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-rename--0x0f-
 *
 * @param[in] pszFrom Old file name
 * @param[in] pszTo   New file name
 * @return  (int32_t) Host-specific status code, see @c bSemihostIsError
 ******************************************************************************/
int32_t lSemihostRename(const char* pszFrom, const char* pszTo)
{
  uint32_t aulArgs[4] = {
    (uint32_t)pszFrom, (uint32_t)strlen(pszFrom),
    (uint32_t)pszTo,   (uint32_t)strlen(pszTo)
  };
  return (int32_t)ullSemihostReqOp(SYS_RENAME, (uint32_t)aulArgs);
}

/*!****************************************************************************
 * @brief
 * Generate a temporary filename
 *
 * Generates a filename to a temporary file on the host file system using the
 * "SYS_TMPNAM" [1] command. The provided buffer @c pszBuf must fit at least
 * @c L_tmpnam bytes.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-tmpnam--0x0d-
 *
 * @param[out] pszBuf File name buffer
 * @param[in] ucId    Unique target identifier
 * @param[in] ulSize  Buffer size in bytes
 * @return  (bool)  Operation successful
 * @date  03.08.2025
 ******************************************************************************/
bool bSemihostGetTmpnam(char* pszBuf, uint8_t ucId, uint32_t ulSize)
{
  uint32_t aulArgs[3] = { (uint32_t)pszBuf, (uint32_t)ucId, ulSize };
  return (uint32_t)ullSemihostReqOp(SYS_TMPNAM, (uint32_t)aulArgs) == 0uL;
}

/*!****************************************************************************
 * @brief
 * Test error indication
 *
 * Checks whether the given host-specific status code is an error indication,
 * using the "SYS_ISERROR" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-iserror--0x08-
 *
 * @param[in] lStatus Host-specific status code
 * @return  (bool)  Status code is an error indication
 * @date  03.08.2025
 ******************************************************************************/
bool bSemihostIsError(int32_t lStatus)
{
  uint32_t aulArgs[1] = { (uint32_t)lStatus };
  return (uint32_t)ullSemihostReqOp(SYS_ISERROR, (uint32_t)aulArgs) != 0uL;
}

/*!****************************************************************************
 * @brief
 * Get execution duration in centiseconds
 *
 * Get the number of centiseconds since execution started using the "SYS_CLOCK"
 * [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-clock--0x10-
 *
 * @return  (uint32_t)  Number of centiseconds since execution start
 * @retval  -1  Command error
 * @date  03.08.2025
 ******************************************************************************/
uint32_t ulSemihostGetClock(void)
{
  return (uint32_t)ullSemihostReqOp(SYS_CLOCK, 0uL);
}

/*!****************************************************************************
 * @brief
 * Get time
 *
 * Get system timestamp in seconds from 00:00 January 1st 1970 (Unit Timestamp)
 * using the "SYS_TIME" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-time--0x11-
 *
 * @return  (uint32_t)  Timestamp
 * @date  03.08.2025
 ******************************************************************************/
uint32_t ulSemihostGetTime(void)
{
  return (uint32_t)ullSemihostReqOp(SYS_TIME, 0uL);
}

/*!****************************************************************************
 * @brief
 * Get execution duration in number of target ticks
 *
 * Retrieves the number of target ticks since execution started using the
 * "SYS_ELAPSED" [1] command. Use @c ulSemihostGetTickFreq to determine the
 * tick frequency.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-elapsed--0x30-
 *
 * @param[out] pullElapsed  Number of elapsed ticks since execution start
 * @return  (bool)  Operation successful
 * @date  03.08.2025
 ******************************************************************************/
bool bSemihostGetElapsed(uint64_t* pullElapsed)
{
  return (uint32_t)ullSemihostReqOp(SYS_ELAPSED, (uint32_t)pullElapsed) == 0uL;
}

/*!****************************************************************************
 * @brief
 * Get tick frequency
 *
 * Retrieves the tick frequency for use with @c vSemihostGetElapsed using the
 * "SYS_TICKFREQ" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-tickfreq--0x31-
 *
 * @return  (uint32_t)  Number of ticks per second
 * @retval  -1  Error occurred
 * @date  03.08.2025
 ******************************************************************************/
uint32_t ulSemihostGetTickFreq(void)
{
  return (uint32_t)ullSemihostReqOp(SYS_TICKFREQ, 0uL);
}

/*!****************************************************************************
 * @brief
 * Get heap and stack configuration
 *
 * Retrieves system heap and stack information from the debug host using the
 * "SYS_HEAPINFO" [1] command.
 *
 * References:
 * [1] https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/sys-heapinfo--0x16-
 *
 * @param[out] psInfo Heap info data structure
 * @return  (bool)  Operation successful
 * @date  03.08.2025
 ******************************************************************************/
bool bSemihostGetHeapInfo(Semihost_HeapInfo* psInfo)
{
  uint32_t aulArgs[1] = { (uint32_t)psInfo };
  (void)ullSemihostReqOp(SYS_HEAPINFO, (uint32_t)aulArgs);
  return ((psInfo->pHeapBase  != NULL) && (psInfo->pHeapLimit  != NULL) && \
          (psInfo->pStackBase != NULL) && (psInfo->pStackLimit != NULL));
}
