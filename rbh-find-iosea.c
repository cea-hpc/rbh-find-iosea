/* This file is part of rbh-find-iosea
 * Copyright (C) 2022 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <error.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>

#include <robinhood.h>
#include <robinhood/utils.h>

#include <rbh-find/actions.h>
#include <rbh-find/core.h>
#include <rbh-find/find_cb.h>

#include "filters.h"
#include "parser.h"

static struct find_context ctx;

static void __attribute__((destructor))
on_find_exit(void)
{
    ctx_finish(&ctx);
}

enum command_line_token
iosea_predicate_or_action(const char *string)
{
    switch (string[0]) {
    case '-':
        switch (string[1]) {
        case 'd':
            if (!strcmp(&string[2], "ataset"))
                return CLT_PREDICATE;
            break;
        case 'f':
            if (!strcmp(&string[2], "requency"))
                return CLT_PREDICATE;
            break;
        case 'h':
            if (!strcmp(&string[2], "sm-hint"))
                return CLT_PREDICATE;
            break;
        case 'l':
            if (!strcmp(&string[2], "ifetime"))
                return CLT_PREDICATE;
            break;
        case 'p':
            if (!strcmp(&string[2], "olicy"))
                return CLT_PREDICATE;
            break;
        case 's':
            if (!strcmp(&string[2], "imilarity"))
                return CLT_PREDICATE;
            break;
        }
        break;
    }

    return find_predicate_or_action(string);
}

static struct rbh_filter *
iosea_parse_predicate(struct find_context *ctx, int *arg_idx)
{
    struct rbh_filter *filter;
    int i = *arg_idx;
    int predicate;

    predicate = str2iosea_predicate(ctx->argv[i]);

    if (i + 1 >= ctx->argc)
        error(EX_USAGE, 0, "missing argument to '%s'", ctx->argv[i]);

    /* In the following block, functions should call error() themselves rather
     * than returning.
     *
     * Errors are most likely fatal (not recoverable, and this allows for
     * precise and meaningful error messages.
     */
    switch (predicate) {
    case IPRED_DATASET:
    case IPRED_FREQUENCY:
    case IPRED_HSM_HINT:
    case IPRED_LIFETIME:
    case IPRED_POLICY:
    case IPRED_SIMILARITY:
        error(EXIT_FAILURE, ENOTSUP, "%s", iosea_predicate2str(predicate));
        /* clang: -Wsometimes-unitialized: `filter` */
        __builtin_unreachable();
    default:
        filter = find_parse_predicate(ctx, &i);
        break;
    }
    assert(filter != NULL);

    *arg_idx = i;
    return filter;
}

int
main(int _argc, char *_argv[])
{
    struct rbh_filter_sort *sorts = NULL;
    struct rbh_filter *filter;
    size_t sorts_count = 0;
    int index;

    /* Discard the program's name */
    ctx.argc = _argc - 1;
    ctx.argv = &_argv[1];

    ctx.pre_action_callback = &find_pre_action;
    ctx.exec_action_callback = &find_exec_action;
    ctx.post_action_callback = &find_post_action;
    ctx.parse_predicate_callback = &iosea_parse_predicate;
    ctx.pred_or_action_callback = &iosea_predicate_or_action;

    /* Parse the command line */
    for (index = 0; index < ctx.argc; index++) {
        if (str2command_line_token(&ctx, ctx.argv[index]) != CLT_URI)
            break;
    }
    if (index == 0)
        error(EX_USAGE, 0, "missing at least one robinhood URI");

    ctx.backends = malloc(index * sizeof(*ctx.backends));
    if (ctx.backends == NULL)
        error(EXIT_FAILURE, errno, "malloc");

    for (int i = 0; i < index; i++) {
        ctx.backends[i] = rbh_backend_from_uri(ctx.argv[i]);
        ctx.backend_count++;
    }

    filter = parse_expression(&ctx, &index, NULL, &sorts, &sorts_count);
    if (index != ctx.argc)
        error(EX_USAGE, 0, "you have too many ')'");

    if (!ctx.action_done)
        find(&ctx, ACT_PRINT, &index, filter, sorts, sorts_count);
    free(filter);

    return EXIT_SUCCESS;
}
