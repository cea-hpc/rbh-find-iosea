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

# Create a number of Hestia objects corresponding to the number of files in the
# current directory, group them in an array, and sort the array alphabetically
create_objects()
{
    local n="$(ls | wc -l)"
    local unsorted_objects=()

    objects=()

    for i in `seq 1 $n`; do
        local tmp="$(hestia object --verbosity 1 create)"
        unsorted_objects+=("$tmp")
    done

    readarray -td '' objects < <(printf '%s\0' "${unsorted_objects[@]}" |
                                    sort -z)
}

test_equal_1K()
{
    touch "empty"
    truncate --size 1K "1K"
    truncate --size 1025 "1K+1"

    create_objects

    hestia object put_data "${objects[0]}" --file "$(realpath "empty")"
    hestia object put_data "${objects[1]}" --file "$(realpath "1K")"
    hestia object put_data "${objects[2]}" --file "$(realpath "1K+1")"

    rbh-sync "rbh:hestia:" "rbh:mongo:$testdb"

    rbh_ifind "rbh:mongo:$testdb" -extent-size 1k | sort |
        difflines "$obj2"
}

test_plus_1K()
{
    touch "empty"
    truncate --size 1K "1K"
    truncate --size 1025 "1K+1"
    truncate --size 1M "1M"

    create_objects

    hestia object put_data "${objects[0]}" --file "$(realpath "empty")"
    hestia object put_data "${objects[1]}" --file "$(realpath "1K")"
    hestia object put_data "${objects[2]}" --file "$(realpath "1K+1")"
    hestia object put_data "${objects[3]}" --file "$(realpath "1M")"

    rbh-sync "rbh:hestia:" "rbh:mongo:$testdb"

    rbh_ifind "rbh:mongo:$testdb" -extent-size +1k | sort |
        difflines "${objects[2]}" "${objects[3]}"
}

test_plus_1K_minus_1M()
{
    touch "empty"
    truncate --size 1K "1K"
    truncate --size 1025 "1K+1"
    truncate --size 1M "1M"

    create_objects

    hestia object put_data "${objects[0]}" --file "$(realpath "empty")"
    hestia object put_data "${objects[1]}" --file "$(realpath "1K")"
    hestia object put_data "${objects[2]}" --file "$(realpath "1K+1")"
    hestia object put_data "${objects[3]}" --file "$(realpath "1M")"

    rbh-sync "rbh:hestia:" "rbh:mongo:$testdb"

    # Filtering on size +1k and size -1M is supposed to match nothing, as
    # +1k ensures we only get files of length 2k and more, while -1M
    # only matches files of length 0.
    rbh_ifind "rbh:mongo:$testdb" -extent-size +1k -extent-size -1M | sort |
        difflines
}

test_equal_1M()
{
    touch "empty"
    truncate --size 1M "1M"
    truncate --size $((1024 * 1024 + 1)) "1M+1"

    create_objects

    hestia object put_data "${objects[0]}" --file "$(realpath "empty")"
    hestia object put_data "${objects[1]}" --file "$(realpath "1M")"
    hestia object put_data "${objects[2]}" --file "$(realpath "1M+1")"

    rbh-sync "rbh:hestia:" "rbh:mongo:$testdb"

    rbh_ifind "rbh:mongo:$testdb" -extent-size 1M | sort |
        difflines "${objects[1]}"
}

test_minus_1M()
{
    touch "empty"
    truncate --size 1M "1M"

    create_objects

    hestia object put_data "${objects[0]}" --file "$(realpath "empty")"
    hestia object put_data "${objects[1]}" --file "$(realpath "1M")"

    rbh-sync "rbh:hestia:" "rbh:mongo:$testdb"

    # Since the object has a size of 0, Hestia doesn't put it on a tier, so it
    # doesn't have an extent, and as such, no extent size to compare
    rbh_ifind "rbh:mongo:$testdb" -extent-size -1M | sort |
        difflines
}

test_plus_3M()
{
    touch "empty"
    truncate --size 3M "3M"
    truncate --size 3M "3M+1"
    echo "a" >> "3M+1"
    truncate --size 4M "4M"

    create_objects

    hestia object put_data "${objects[0]}" --file "$(realpath "empty")"
    hestia object put_data "${objects[1]}" --file "$(realpath "3M")"
    hestia object put_data "${objects[2]}" --file "$(realpath "3M+1")"
    hestia object put_data "${objects[3]}" --file "$(realpath "4M")"

    rbh-sync "rbh:hestia:" "rbh:mongo:$testdb"

    rbh_ifind "rbh:mongo:$testdb" -extent-size +3M | sort |
        difflines "${objects[2]}" "${objects[3]}"
}

test_plus_1M_minus_2G()
{
    touch "empty"
    truncate --size 4M "4M"
    truncate --size 1M "1.xM"
    echo "hello world!" >> "1.xM"

    create_objects

    hestia object put_data "${objects[0]}" --file "$(realpath "empty")"
    hestia object put_data "${objects[1]}" --file "$(realpath "4M")"
    hestia object put_data "${objects[2]}" --file "$(realpath "1.xM")"

    rbh-sync "rbh:hestia:" "rbh:mongo:$testdb"

    rbh_ifind "rbh:mongo:$testdb" -extent-size +1M -extent-size -2G | sort |
        difflines "${objects[1]}" "${objects[2]}"
}

################################################################################
#                                     MAIN                                     #
################################################################################

declare -a tests=(test_equal_1K test_plus_1K test_plus_1K_minus_1M
                  test_equal_1M test_minus_1M test_plus_3M
                  test_plus_1M_minus_2G)

run_tests ${tests[@]}
