/* This file is part of rbh-find-iosea
 * Copyright (C) 2022 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#include <error.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include <robinhood/backend.h>
#include <rbh-find/utils.h>

#include "filters.h"

static const struct rbh_filter_field predicate2filter_field[] = {
    [IPRED_TIER - IPRED_MIN] = {.fsentry = RBH_FP_INODE_XATTRS,
                                .xattr = "tiers.index"},
};

struct rbh_filter *
tier2filter(const char *_tier)
{
    struct rbh_filter *filter;
    uint64_t tier;
    int rc;

    if (_tier[0] == '-')
        error(EX_USAGE, errno, "tier cannot be negative: `%s'", _tier);

    rc = str2uint64_t(_tier, &tier);
    if (rc)
        error(EX_USAGE, errno, "invalid tier: `%s'", _tier);

    filter = rbh_filter_compare_uint32_new(
        RBH_FOP_EQUAL,
        &predicate2filter_field[IPRED_TIER - IPRED_MIN],
        tier
    );
    if (filter == NULL)
        error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
                      "rbh_filter_compare_uint32_new");

    return filter;
}
