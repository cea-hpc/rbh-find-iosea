#!/usr/bin/env bash

# This file is part of the rbh-find-iosea project
# Copyright (C) 2024 Commissariat a l'energie atomique et aux energies
#                    alternatives
#
# SPDX-License-Identifer: LGPL-3.0-or-later

if ! command -v rbh-sync &> /dev/null; then
    echo "This test requires rbh-sync to be installed" >&2
    exit 1
fi

test_dir=$(dirname $(readlink -e $0))
. $test_dir/test_utils.bash

################################################################################
#                                    TESTS                                     #
################################################################################

test_syntax_error()
{
    rbh_ifind "rbh:mongo:$testdb" -tier -1 &&
        error "ifind with a negative tier index should have failed"

    rbh_ifind "rbh:mongo:$testdb" -tier $(echo 2^64 | bc) &&
        error "ifind with an tier index too big should have failed"

    rbh_ifind "rbh:mongo:$testdb" -tier 42blob &&
        error "ifind with an invalid tier index should have failed"

    rbh_ifind "rbh:mongo:$testdb" -tier invalid &&
        error "ifind with an invalid tier index should have failed"

    return 0
}

test_unknown_tier()
{
    local obj=$(hestia_obj create)
    hestia_obj put_data $obj --file /etc/hosts
    rbh-sync "rbh:hestia:" "rbh:mongo:$testdb"

    rbh_ifind "rbh:mongo:$testdb" -tier 42 | sort |
        difflines
}

test_known_tier()
{
    local obj1=$(hestia_obj create a79fbb90-bf7a-11ee-ae3e-e43d1aab2666)
    local obj2=$(hestia_obj create b79fbb90-bf7a-11ee-ae3e-e43d1aab2666)
    local obj3=$(hestia_obj create c79fbb90-bf7a-11ee-ae3e-e43d1aab2666)

    hestia_obj put_data $obj1 --file /etc/hosts
    hestia_obj put_data $obj2 --file /etc/hosts
    hestia_obj put_data $obj3 --file /etc/hosts

    hestia_obj copy_data $obj2 --source 0 --target 1
    hestia_obj move_data $obj3 --source 0 --target 1

    rbh-sync "rbh:hestia:" "rbh:mongo:$testdb"

    rbh_ifind "rbh:mongo:$testdb" -tier 0 | sort |
        difflines "$obj1" "$obj2"
    rbh_ifind "rbh:mongo:$testdb" -tier 1 | sort |
        difflines "$obj2" "$obj3"
    rbh_ifind "rbh:mongo:$testdb" -tier 0 -tier 1 | sort |
        difflines "$obj2"
    rbh_ifind "rbh:mongo:$testdb" -tier 0 -or -tier 1 | sort |
        difflines "$obj1" "$obj2" "$obj3"
}

################################################################################
#                                     MAIN                                     #
################################################################################

declare -a tests=(test_syntax_error test_unknown_tier test_known_tier)

run_tests ${tests[@]}
