/*!****************************************************************************
 * @file
 * coverage.h
 *
 * @brief
 * gcov coverage data processing
 *
 * @date  31.10.2025
 ******************************************************************************/

#ifndef COVERAGE_H_
#define COVERAGE_H_

/*- Public interface ---------------------------------------------------------*/
void Coverage_vInit(void);
void Coverage_vDump(const char* pszFilename);

#endif // COVERAGE_H_
