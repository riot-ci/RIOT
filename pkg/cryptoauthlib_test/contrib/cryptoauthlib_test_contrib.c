#include "cryptoauthlib.h"
#include "atca.h"
#include "atca_params.h"

/* Function switches the default cfg in cryptoauthlib test to Riot cfg */
void riot_switch_cfg(ATCAIfaceCfg *cfg)
{
    *cfg = atca_params[0];
}