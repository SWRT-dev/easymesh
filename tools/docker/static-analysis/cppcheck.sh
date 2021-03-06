#!/bin/bash

scriptdir="$(cd "${0%/*}" && pwd)"
rootdir="${scriptdir%/*/*/*}"

# shellcheck source=functions.sh
. "${rootdir}/tools/functions.sh"

OUTPUT_FILE="$rootdir/cppcheck_results.txt"
INCLUDES=()
while IFS='' read -r line; do INCLUDES+=("-I$line"); done < <(find -L "$rootdir/common" "$rootdir/framework" -type d -name include)

usage() {
    echo "usage: $(basename "$0") <source> [source]"
    echo "run cpp check on <source> and output number of issues per severity"
}

run_cppcheck() {
    # --force is needed to check more than 12 ifdefs
    cppcheck --template='{file}:{line}:{column}: {severity}: {message} [{id}]\n{code}' \
             --force \
             --error-exitcode=1 \
             --enable=warning,style,information \
             -i"$rootdir/framework/platform/nbapi/unit_tests" \
             -i"$rootdir/framework/external" \
             -i"$rootdir/common/beerocks/bwl/unit_tests" \
             -i"$rootdir/common/beerocks/bcl/unit_tests" \
             -i"$rootdir/controller/src/beerocks/master/db/unit_tests" \
             -i"$rootdir/build" \
             --inline-suppr \
             --suppressions-list="$rootdir"/tools/docker/static-analysis/suppressions.txt \
             -rp="$rootdir" \
             -j"$(nproc)" \
             -q \
             "${INCLUDES[@]}" \
             "$@" 2>&1 | sed "s|$rootdir/||g" | tee "$OUTPUT_FILE"
}

colorize_severity() {
    local severity msg
    severity="$1"
    msg="$2"
    case "$severity" in
        error)
            printf '\033[1;31m%s\033[0m\n' "$msg"
            ;;
        warning)
            printf '\033[1;33m%s\033[0m\n' "$msg"
            ;;
        style)
            printf '\033[1;34m%s\033[0m\n' "$msg"
            ;;
        performance)
            printf '\033[1;35m%s\033[0m\n' "$msg"
            ;;
        portability)
            printf '\033[1;36m%s\033[0m\n' "$msg"
            ;;
        ok)
            printf '\033[1;90m%s\033[0m\n' "$msg"
            ;;

        *)
            echo unknown severity "$severity"
            exit 1
            ;;
    esac
}

output_results() {
    local severities status
    severities="portability performance style warning error"
    status=0
    for s in $severities; do
        nb_issues="$(grep -Ec "^[^ ]+ $s:" "$OUTPUT_FILE")"
        if [ "$nb_issues" -gt 0 ] ; then
            colorize_severity "$s" "$nb_issues issues with severity $s"
            case "$s" in
                # report the following severities in exit code:
                error|warning)
                    status=1
                    ;;
                # ignore all others:
                *);;
            esac
        else
            colorize_severity "ok" "$nb_issues issues with severity $s"
        fi
    done
    return $status
}

main() {
    if ! command -v cppcheck > /dev/null ; then
        err "This script requires cppcheck! Please install it and try again."
        exit 1
    fi

    run_cppcheck "$@"
    output_results
}

main "$@"
