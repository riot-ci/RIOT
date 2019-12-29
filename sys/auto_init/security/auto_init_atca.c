/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       Initializes cryptoauth devices
 *
 * @author      >
 * @}
 */

#ifdef MODULE_CRYPTOAUTHLIB
#include "atca.h"
#include "atca_params.h"

#define ENABLE_DEBUG                (0)
#include "debug.h"

#define ATCA_NUMOF (ARRAY_SIZE(atca_params))

void auto_init_atca(void) {        
        for (unsigned i = 0; i < ATCA_NUMOF; i++)
        {
                if (atcab_init((ATCAIfaceCfg*)&atca_params[i]) != ATCA_SUCCESS)
                {
                        continue;
                }
        }
}
#else
typedef int dont_be_pedantic;
#endif
