/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"

#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
extern VOID _tx_execution_initialize(VOID);
#endif

/* Define any port-specific scheduling data structures.  */

TX_PORT_SPECIFIC_DATA


#ifdef TX_SAFETY_CRITICAL
TX_SAFETY_CRITICAL_EXCEPTION_HANDLER
#endif


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_kernel_enter                         PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the first ThreadX function called during           */
/*    initialization.  It is called from the application's "main()"       */
/*    function.  It is important to note that this routine never          */
/*    returns.  The processing of this function is relatively simple:     */
/*    it calls several ThreadX initialization functions (if needed),      */
/*    calls the application define function, and then invokes the         */
/*    scheduler.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_initialize_low_level          Low-level initialization          */
/*    _tx_initialize_high_level         High-level initialization         */
/*    tx_application_define             Application define function       */
/*    _tx_thread_scheduler              ThreadX scheduling loop           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    main                              Application main program          */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020      William E. Lamie        Initial Version 6.0           */
/*  09-30-2020      Yuxin Zhou              Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  04-25-2022      Scott Larson            Modified comment(s),          */
/*                                            added EPK initialization,   */
/*                                            resulting in version 6.1.11 */
/*  10-31-2023      Xiuwen Cai              Modified comment(s),          */
/*                                            added random generator      */
/*                                            initialization,             */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
VOID  _tx_initialize_kernel_enter(VOID)
{
}

