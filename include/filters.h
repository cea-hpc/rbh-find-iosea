/* This file is part of rbh-find-iosea
 * Copyright (C) 2022 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#ifndef RBH_FIND_IOSEA_FILTERS_H
#define RBH_FIND_IOSEA_FILTERS_H

#include "parser.h"

#include <rbh-find/filters.h>

/**
 * Build a filter for the -tier-index predicate
 *
 * @param tier         a string representing a tier index
 *
 * @return             a pointer to a newly allocated struct rbh_filter
 *
 * Exit on error
 */
struct rbh_filter *
tier_index2filter(const char *tier_index);

#endif
