//
// Created by David Frühwirth on 27.12.21.
//

#ifndef LOGGING_H
#define LOGGING_H
#include <stdbool.h>

#ifdef LOGGING

  ///Asserts a certain condition. If the condition is not fulfilled, the application is immediately terminated
  #define ASSERT(expr) {vAssert((bool)expr, __FILE__, __func__, __LINE__);}
  ///Expects a certain condition. If the condition is not fulfilled, an error messages gets printed and written to
  /// the log-file
  #define EXPECT(expr) {vExpect((bool)expr, __FILE__, __func__, __LINE__);}

  ///Wrapper for vLogFuncEnter, which encapsulates the function call with all required parameter
  #define FUNC_ENTER {vLogFuncEnter(__func__, __LINE__);}
  ///Wrapper for vLogFuncLeave, which encapsulates the function call with all required parameter
  #define FUNC_LEAVE {vLogFuncLeave(__func__, __LINE__);};
  void vLogMessage(const char* pcTag, const char* pcFormat, ...);
  void vLogFuncEnter(const char* pcFunc, const long lLineNumber);
  void vLogFuncLeave(const char* pcFunc, const long lLineNumber);
  void vExpect(bool bCondition, const char* pcFile, const char* pcFunc, const long lLine);
  void vAssert(bool bCondition, const char* pcFile, const char* pcFunc, const long lLine);
#else
  #define FUNC_ENTER {}
  #define FUNC_LEAVE {}
#endif
#endif //LOGGING_H
