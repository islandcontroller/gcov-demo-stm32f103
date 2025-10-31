/*!****************************************************************************
 * @file
 * coverage.c
 *
 * @brief
 * gcov coverage data processing
 *
 * @date  31.10.2025
 ******************************************************************************/

/*- Header files -------------------------------------------------------------*/
#include <stddef.h>
#include <gcov.h>
#include "semihost.h"
#include "coverage.h"


/*- Global data --------------------------------------------------------------*/
// gcov info structures in FLASH memory, placed by the linker
extern const struct gcov_info* const __gcov_info_start[]; // start marker
extern const struct gcov_info* const __gcov_info_end[]; // end marker


/*- Prototypes ---------------------------------------------------------------*/
static void vDumpCb(const void *pData, unsigned uLength, void *pArg);
static void vFilenameCb(const char *pszFname, void *pArg);
static void* pAllocateCb(unsigned, void *);


/*- Public interface ---------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Initialise coverage data collection
 *
 * @date  31.10.2025
 ******************************************************************************/
void Coverage_vInit(void)
{
  __gcov_reset();
}

/*!****************************************************************************
 * @brief
 * Dump coverage data to a file
 *
 * This will dump all collected coverage data to a file on the host machine,
 * using Semihosting file transfers.
 *
 * @param[in] pszFilename Output file name on host system
 * @date  31.10.2025
 ******************************************************************************/
void Coverage_vDump(const char* pszFilename)
{
  int32_t lFile = lSemihostOpen(pszFilename, 5 /* wb */);
  for (const struct gcov_info* const* pIt = __gcov_info_start; pIt != __gcov_info_end; ++pIt)
  {
    __gcov_info_to_gcda(*pIt, vFilenameCb, vDumpCb, pAllocateCb, &lFile);
  }
  bSemihostClose(lFile);
}


/*- Private functions --------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Callback: Transfer gcov information byte stream to target
 *
 * @param[in] pData   Source data buffer
 * @param[in] uLength Source data length in bytes
 * @param[inout] pArg User-defined argument (here used for fp)
 * @date  31.10.2025
*******************************************************************************/
static void vDumpCb(const void *pData, unsigned uLength, void *pArg)
{
  lSemihostWrite(*(int32_t*)pArg, pData, uLength);
}

/*!****************************************************************************
 * @brief
 * Callback: Serialise filename to gfcn data stream
 *
 * The contained data is deserialised using the "merge-stream" command of the
 * "gcov-tool".
 *
 * @param[in] pszFname  File name of coverage info
 * @param[inout] pArg   User-defined argument (here used for fp)
 * @date  31.10.2025
 ******************************************************************************/
static void vFilenameCb(const char *pszFname, void *pArg)
{
  __gcov_filename_to_gcfn(pszFname, vDumpCb, pArg);
}

/*!****************************************************************************
 * @brief
 * Callback: Allocate data buffer
 *
 * Depending on your system, gcov instrumentation may need to allocate data
 * buffers dynamically. If required, use @c malloc. In this case, dynamic memo-
 * ry allocation can be omitted.
 *
 * @date  31.10.2025
 * @return  (void*) Allocated memory block
 ******************************************************************************/
static void* pAllocateCb(unsigned, void *)
{
  return NULL;
}

