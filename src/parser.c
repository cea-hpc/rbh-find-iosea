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
    case 'd':
        if (strcmp(&string[2], "ataset") == 0)
            return IPRED_DATASET;
        break;
    case 'f':
        if (strcmp(&string[2], "requency") == 0)
            return IPRED_FREQUENCY;
        break;
    case 'h':
        if (strcmp(&string[2], "sm-hint") == 0)
            return IPRED_HSM_HINT;
        break;
    case 'l':
        if (strcmp(&string[2], "lifetime") == 0)
            return IPRED_LIFETIME;
        break;
    case 'p':
        if (strcmp(&string[2], "olicy") == 0)
            return IPRED_POLICY;
        break;
    case 's':
        if (strcmp(&string[2], "imilarity") == 0)
            return IPRED_SIMILARITY;
        break;
    }

    return str2predicate(string);
}

#define LOCAL(X) (X) - IPRED_MIN
static const char *__iosea_predicate2str[] = {
    [LOCAL(IPRED_DATASET)]    = "dataset",
    [LOCAL(IPRED_FREQUENCY)]  = "frequency",
    [LOCAL(IPRED_HSM_HINT)]   = "hsm-hint",
    [LOCAL(IPRED_LIFETIME)]   = "lifetime",
    [LOCAL(IPRED_POLICY)]     = "policy",
    [LOCAL(IPRED_SIMILARITY)] = "similarity",
};

const char *
iosea_predicate2str(int predicate)
{
    if (IPRED_MIN <= predicate && predicate < IPRED_LAST)
        return __iosea_predicate2str[LOCAL(predicate)];

    return predicate2str(predicate);
}
