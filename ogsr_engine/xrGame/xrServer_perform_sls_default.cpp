#include "stdafx.h"

#include "xrServer.h"

void xrServer::SLS_Default()
{
    XR_ASSERT(game->custom_sls_default());
    game->sls_default();
}
