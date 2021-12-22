#!/bin/bash

scriptdir="$(cd "${0%/*}" && pwd)"
rootdir="${scriptdir%/*/*}"

# shellcheck source=functions.sh
. "${rootdir}/tools/functions.sh"


# generate the list of current issues in the same format as the known issues:
"$rootdir"/tools/docker/static-analysis/cppcheck.sh "$rootdir" "--template={file}: {severity}: {message} [{id}]{code}" -i"$rootdir/framework/tlvf/AutoGenerated" > /dev/null 2>&1

# Remove the caret lines, we don't need them:
sed -i '/^\s*\^\s*$/d' "$rootdir/cppcheck_results.txt"

# Sort the list of issues to have a stable list, easier to compare:
sort "$rootdir/cppcheck_results.txt" > "$rootdir/cppcheck_results_sorted.txt"
mv "$rootdir/cppcheck_results_sorted.txt" "$rootdir/cppcheck_results.txt"

mapfile -t issues < <(grep -E ".+: .+: .+ \[.+\] .*$"  "$rootdir/cppcheck_results.txt")

status=0
for i in "${issues[@]}"
do
    if ! grep -q -F "$i" "$rootdir"/ci/cppcheck/cppcheck_existing_issues.txt ; then
        status=1
        err "New issue:"
        info "$i"
    fi
done

# Generate a new list of existing issues by removing the ones that are now solved
comm -12 <(sort "$rootdir"/cppcheck_results.txt) <(sort "$rootdir"/ci/cppcheck/cppcheck_existing_issues.txt)  > "$rootdir"/cppcheck_existing_issues.txt

mv "$rootdir"/cppcheck_existing_issues.txt "$rootdir"/ci/cppcheck/cppcheck_existing_issues.txt

if ! git diff -U0 --exit-code -- "$rootdir"/ci/cppcheck/cppcheck_existing_issues.txt ; then
    status=1
    err "You solved at least one cppcheck issue! Please download cppcheck_existing_issues.txt from the artifacts and replace it in ci/cppcheck/cppcheck_existing_issues.txt"
    err "If you ran cppcheck-new-issues.sh locally, the changes have already been applied for you."
fi

exit $status