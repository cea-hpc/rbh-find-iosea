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
    [IPRED_EXTENT_SIZE - IPRED_MIN] = {.fsentry = RBH_FP_INODE_XATTRS,
                                       .xattr = "tiers.extents"},
    [IPRED_TIER_INDEX - IPRED_MIN]  = {.fsentry = RBH_FP_INODE_XATTRS,
                                       .xattr = "tiers.index"},
};

struct rbh_filter *
tier_index2filter(const char *_tier_index)
{
    struct rbh_filter *filter;
    uint64_t tier_index;
    int rc;

    if (_tier_index[0] == '-')
        error(EX_USAGE, errno, "tier index cannot be negative: `%s'",
              _tier_index);

    rc = str2uint64_t(_tier_index, &tier_index);
    if (rc)
        error(EX_USAGE, errno, "invalid tier index: `%s'", _tier_index);

    filter = rbh_filter_compare_uint32_new(
        RBH_FOP_EQUAL,
        &predicate2filter_field[IPRED_TIER_INDEX - IPRED_MIN],
        tier_index
    );
    if (filter == NULL)
        error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
                      "rbh_filter_compare_uint32_new");

    return filter;
}

static struct rbh_filter *
filter_uint64_range_new(const struct rbh_filter_field *field, uint64_t start,
                        uint64_t end)
{
    struct rbh_filter *low, *high;

    low = rbh_filter_compare_uint64_new(RBH_FOP_STRICTLY_GREATER, field, start);
    if (low == NULL)
        error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
                      "rbh_filter_compare_time");

    high = rbh_filter_compare_uint64_new(RBH_FOP_STRICTLY_LOWER, field, end);
    if (high == NULL)
        error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
                      "rbh_filter_compare_time");

    return filter_and(low, high);
}

struct rbh_filter *
extent_size2filter(const char *_extent_size)
{
    char operator = *_extent_size;
    struct rbh_filter *filter;
    uint64_t extent_size;
    uint64_t unit_size;
    char *suffix;

    switch (operator) {
    case '+':
    case '-':
        _extent_size++;
    }

    extent_size = strtoull(_extent_size, &suffix, 10);
    if (extent_size == 0ULL)
        error(EX_USAGE, 0,
              "arguments to -extent-size should start with at least one digit");
    else if (errno == ERANGE && extent_size == ULLONG_MAX)
        error(EX_USAGE, EOVERFLOW, "invalid argument `%s' to -extent-size",
              _extent_size);

    switch (*suffix++) {
    case 'T':
        unit_size = 1099511627776;
        break;
    case 'G':
        unit_size = 1073741824;
        break;
    case 'M':
        unit_size = 1048576;
        break;
    case 'k':
        unit_size = 1024;
        break;
    case '\0':
        /* default suffix */
        suffix--;
        __attribute__((fallthrough));
    case 'b':
        unit_size = 512;
        break;
    case 'w':
        unit_size = 2;
        break;
    case 'c':
        unit_size = 1;
        break;
    default:
        error(EX_USAGE, 0, "invalid unit `%s' to -extent-size", _extent_size);
    }

    if (*suffix)
        error(EX_USAGE, 0, "invalid argument `%s' to -extent-size",
              _extent_size);

    switch (operator) {
    case '-':
        filter = rbh_filter_compare_uint64_new(
                RBH_FOP_LOWER_OR_EQUAL,
                &predicate2filter_field[IPRED_EXTENT_SIZE - IPRED_MIN],
                (extent_size - 1) * unit_size
                );
        break;
    case '+':
        filter = rbh_filter_compare_uint64_new(
                RBH_FOP_STRICTLY_GREATER,
                &predicate2filter_field[IPRED_EXTENT_SIZE - IPRED_MIN],
                extent_size * unit_size
                );
        break;
    default:
        filter = filter_uint64_range_new(
                &predicate2filter_field[IPRED_EXTENT_SIZE - IPRED_MIN],
                (extent_size - 1) * unit_size,
                extent_size * unit_size + 1
                );
    }

    if (filter == NULL)
        error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__,
                      "filter_compare_integer");

    return filter;
}
