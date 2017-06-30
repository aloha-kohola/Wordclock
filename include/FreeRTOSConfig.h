/* Wordclock FreeRTOSConfig overrides.

   This is intended as an example of overriding some of the default FreeRTOSConfig settings,
   which are otherwise found in FreeRTOS/Source/include/FreeRTOSConfig.h
*/

/* The serial driver depends on counting semaphores */
#define configTIMER_TASK_PRIORITY 5

/* compatibility to old function names */
#define configENABLE_BACKWARD_COMPATIBILITY 1

/* Use the defaults for everything else */
#include_next<FreeRTOSConfig.h>

