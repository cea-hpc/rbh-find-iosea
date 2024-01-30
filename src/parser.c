/* This file is part of rbh-find-iosea
 * Copyright (C) 2022 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <sysexits.h>

#include <rbh-find/parser.h>

#include "parser.h"

int
str2iosea_predicate(const char *string)
{
    assert(string[0] == '-');

    switch (string[1]) {
    case 't':
        if (strcmp(&string[2], "ier") == 0)
            return IPRED_TIER;
        break;
    }

    return str2predicate(string);
}

#define LOCAL(X) (X) - IPRED_MIN
static const char *__iosea_predicate2str[] = {
    [LOCAL(IPRED_TIER)]    = "tier",
};

const char *
iosea_predicate2str(int predicate)
{
    if (IPRED_MIN <= predicate && predicate < IPRED_LAST)
        return __iosea_predicate2str[LOCAL(predicate)];

    return predicate2str(predicate);
}
