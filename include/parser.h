/* This file is part of rbh-find-iosea
 * Copyright (C) 2022 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#ifndef RBH_FIND_IOSEA_PARSER_H
#define RBH_FIND_IOSEA_PARSER_H

#include <rbh-find/parser.h>

enum iosea_predicate {
    IPRED_MIN = PRED_LAST,

    IPRED_TIER = IPRED_MIN,
    IPRED_EXTENT_SIZE,

    IPRED_LAST
};

/**
 * str2iosea_predicate - convert a string to an integer corresponding to a
 * predicate or an iosea_predicate
 *
 * @param string    a string representing a valid predicate
 *
 * @return          the predicate that \p string represents
 *
 * This function will exit if \p string is not a valid predicate
 */
int
str2iosea_predicate(const char *string);

/**
 * predicate2str - convert a predicate or iosea_predicate to a string
 *
 * @param predicate a predicate
 *
 * @return          the string that represents \p predicate
 */
const char *
iosea_predicate2str(int predicate);

#endif
