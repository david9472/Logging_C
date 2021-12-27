
//Includes
#include "logging.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

//Global Variables
static FILE* fp_log_file = NULL; ///< FILE* to the log-file
static bool bErrorOccurred = 0; ///< Flag if an error occurred while creating the log-file

//Defines
#define BUFFER_SIZE 1024 ///< Buffer size for the log-message. If longer messages should be writte to the log-file,
/// this define must be edited
#define LOG_FILE_NAME 26 ///< length of the log-file name
#define LEN_HOUR 2 ///< length of the hour part in an formatted time string
#define LEN_MIN 2 ///< length of the minute part in an formatted time string
#define LEN_SEC 2 ///< length of the second part in an formatted time string
#define LEN_SEP 1 ///< length of the separator between time parts
#define LEN_H_M_S LEN_HOUR + LEN_SEP + LEN_MIN + LEN_SEP + LEN_SEC + LEN_SEP ///< total length of an formatted time
/// string (without milliseconds)
#define PRECISION 3 ///<digits of ms which should be displayed in the log-file (can be increased up to 6)

//Constants
static const char* PC_ERROR_MESSAGE_CONSOLE = "%s did not result in true!\n";
static const char* PC_ERROR_MESSAGE_LOG = "%s did not result in true! FILE: <%s> FUNC: <%s> LINE: <%d>\n";
static const char* PC_ASSERT = "ASSERT";
static const char* PC_EXPECT = "EXPECT";

//-----------------------------------------------------------------------------
/// Determines the log-file name based on the current time. name has following format yymmdd_hhmmss_log.txt
///
/// @param[out] pcName name of the log file
/// @param[in] lMaxLen Max length of the log file name
///
/// @return != 0 Error
/// @return == 0 Ok
//----------------------------------------------------------------------------
static void vGetLogFileName(char* pcName, long lMaxLen)
{
  time_t sTimer;
  struct tm* psTmInfo;
  sTimer = time(NULL);
  psTmInfo = localtime(&sTimer);

  const char* pcLogFilePostFix = "_log.txt";
  const long lLengthPostFix = strlen(pcLogFilePostFix);
  const long lLen_datetime = lMaxLen - lLengthPostFix;
  strftime(pcName, lLen_datetime, "%y%m%d_%H%M%S", psTmInfo);
  strcat(pcName, pcLogFilePostFix);
  pcName[lMaxLen] = '\0';
}

//-----------------------------------------------------------------------------
/// Creates a new log file. If the creation fails, an error message is displayed in the terminal. All future attempts
/// to create a log-file get immediately aborted.
///
/// @return != 0 Error
/// @return == 0 Ok
//----------------------------------------------------------------------------
static short nOpenLogFile()
{
  if (fp_log_file)
    return 0;

  char acLogFileName[LOG_FILE_NAME] = {0};
  vGetLogFileName(acLogFileName, sizeof(acLogFileName) -1);

  fp_log_file = fopen(acLogFileName, "w+");
  if (!fp_log_file)
  {
    printf("ERROR! could not create logfile!\n");
    return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------------
/// Truncates an integer value so only a certain number of digits remain
///
/// @param[in] lInteger Integer which should be truncated
/// @param[in] lLength desired length of the integer. (i.e. 1000 means the number is max. 999)
///
/// @return truncated number
//----------------------------------------------------------------------------
short nGetFirstDigitsOfInteger(long long lInteger, const long lLength)
{
  while (lInteger > lLength)
    lInteger = lInteger / 10;

  return lInteger;
}

//-----------------------------------------------------------------------------
/// Gets the formatted time for each log file entry. Format: hh:mm:ss:us (length of us is determined by the
/// define PRECISION)
///
/// @param[out] pcFormatedTime Formated time string with current time. NOTE: make sure pcFormatedTime is large enough!
///
/// @return formated time
//----------------------------------------------------------------------------
static char* pcGetFormatedTime(char* pcFormatedTime)
{
  struct timeval sTimeVal;
  gettimeofday(&sTimeVal, NULL);
  time_t sCurrentTime = sTimeVal.tv_sec;
  struct tm* ps_TimeInfo = localtime(&sCurrentTime);

  sprintf(pcFormatedTime, "%02d:%02d:%02d.%d", ps_TimeInfo->tm_hour, ps_TimeInfo->tm_min, ps_TimeInfo->tm_sec,
          nGetFirstDigitsOfInteger(sTimeVal.tv_usec, pow(10, PRECISION)));
  return pcFormatedTime;
}

//-----------------------------------------------------------------------------
/// Writes a new logfile entry
///
/// @param[in] Tag for the log message. (i.e. INPUT, FUNC_ENTER, ERROR, etc.)
/// @param[in] format format specifier for printf
/// @param[in] ... arguments for printf
//----------------------------------------------------------------------------
void vLogMessage(const char* pcTag, const char* pcFormat, ...)
{
  if (!fp_log_file)
  {
    if (bErrorOccurred)
      return;
    else if (nOpenLogFile())
      return;
  }
  va_list sArgs;
  va_start (sArgs, pcFormat);
  char acBuffer[BUFFER_SIZE] = {};
  char acTime[LEN_H_M_S + PRECISION +1] = {0};

  sprintf(acBuffer, "%s [%10s]: ", pcGetFormatedTime(acTime), pcTag);

  const int len = strlen(acBuffer);
  vsprintf (&acBuffer[len], pcFormat, sArgs);
  va_end (sArgs);

  strcpy(&acBuffer[BUFFER_SIZE -2], "\n\0");
  fprintf(fp_log_file,"%s", acBuffer);
}

//-----------------------------------------------------------------------------
/// Logs a call of a function
///
/// @param[in] pcFunc name of the function which was called. NOTE: macro __func__ can be used
/// @param[in] lLineNumber line number in which the function call happened
//----------------------------------------------------------------------------
void vLogFuncEnter(const char* pcFunc, const long lLineNumber)
{
  vLogMessage("ENTERING", "Function <%s> at line <%ld>\n", pcFunc, lLineNumber);
}

//-----------------------------------------------------------------------------
/// Logs the return of a function
///
/// @param[in] pcFunc name of the function which was left. NOTE: macro __func__ can be used
/// @param[in] lLineNumber line number in which the function was left
//----------------------------------------------------------------------------
void vLogFuncLeave(const char* pcFunc, const long lLineNumber)
{
  vLogMessage("LEAVING", "Function <%s> at line <%ld>\n", pcFunc, lLineNumber);
}

//-----------------------------------------------------------------------------
/// Logs the return of a function with its return value
///
/// @param[in] pcFunc name of the function which was left. NOTE: macro __func__ can be used
/// @param[in] lLineNumber line number in which the function was left
/// @param[in] lReturnValue return value of the function
//----------------------------------------------------------------------------
void vLogFuncLeaveReturn(const char* pcFunc, const long lLineNumber, long lReturnValue)
{
  vLogMessage("LEAVING", "Function <%s> at line <%ld> with return value <%ld>\n", pcFunc, lLineNumber, lReturnValue);
}

//-----------------------------------------------------------------------------
/// Asserts a certain condition. If the condition does not result in true, the *application is immediately ended.*
/// !!All allocated memory is lost!! Assertion gets written into the log-file
///
/// @param[in] bCondition condition which should be asserted
/// @param[in] pcFile File in which the assertion is performed. Note: macro __FILE__ can be used
/// @param[in] pcFunc Function in which the assertion is performed. Note: macro __func__ can be used
/// @param[in] lLine Line in which the assertion is performed. Note: macro __LINE__ can used
//----------------------------------------------------------------------------
void vAssert(bool bCondition, const char* pcFile, const char* pcFunc, const long lLine)
{
  if (bCondition)
    return;

  vLogMessage(PC_ASSERT, PC_ERROR_MESSAGE_LOG, PC_ASSERT, pcFile, pcFunc, lLine);
  printf(PC_ERROR_MESSAGE_CONSOLE, PC_ASSERT);
  fclose(fp_log_file);
  exit(0);
}

//-----------------------------------------------------------------------------
/// Expects a certain condition. If the condition does not result in true, an error message is written into the log-file
/// and onto the terminal.
///
/// @param[in] bCondition condition which should be expected
/// @param[in] pcFile File in which the expectation is performed. Note: macro __FILE__ can be used
/// @param[in] pcFunc Function in which the expectation is performed. Note: macro __func__ can be used
/// @param[in] lLine Line in which the expectation is performed. Note: macro __LINE__ can used
//----------------------------------------------------------------------------
void vExpect(bool bCondition, const char* pcFile, const char* pcFunc, const long lLine)
{
  if (bCondition)
    return;

  vLogMessage(PC_EXPECT, PC_ERROR_MESSAGE_LOG, PC_EXPECT, pcFile, pcFunc, lLine);
  printf(PC_ERROR_MESSAGE_CONSOLE, PC_EXPECT);
}
