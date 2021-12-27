# Logging_C

## What does this framework do:
Provides functions and macros to add simple and uncomplicated logging for existing C code. Of course, this framework can also be used for new implementations.


## How to use it:
In order to use this framework, one simply needs to add the .c and .h to the already existing code files, add the .c to the build configuration and you are done. 
To log a function enter/leave, just use the provided macros FUNC_ENTER and FUNC_LEAVE at the beginning and end of a function. If additional information should be written to the log-file the vLogMessage() function can be used. To distinguish between different types of information the 'TAG' parameter provides a simply and straight forward way to differ between log-file entries. By default function enter/leave are tagged with 'ENTERING' and 'LEAVING', but can of course be changed. 

---

To make sure variables or condition are as expected, the provided ASSERT and EXPECT macros can be used. ASSERT prints an error message onto the terminal and writes a new log-file entry, afterwards the application immediatly terminates if a condition does not result in true, whereas EXPECT only prints an error message onto the terminal and writes a new log-file entry. Here the application does not terminate. 

---

Because log files are not wanted in every situation. For example, due to poor performance or due to the disclosure of secret information, log files are only created if define LOGGING is active before including the header file. As soon as this define is removed or commented out, no more log files are created and the macros FUNC_ENTER or FUNC_LEAVE are replaced by an empty code block

### Example:
main.c
```C
#ifdef DEBUG_MODE //logging should only be active in debug-mode
#define LOGGING
#include "logging.h"
#endif


void foo()
{
  FUNC_ENTER

  int someValue = 5;

  ASSERT(someValue > 0)
  
  //funcion foo does what foo function do

  FUNC_LEAVE
}
```

