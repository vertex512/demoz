#!/bin/sh
#
# get C static functions.
#


for i in "$@"; do
	echo
	echo "/* $i */"

	sed -n '/^\/\* @func:.*(.*static.*).*/,/\(^{$\)\|\( {$\)/p' "$i" \
	|sed '/^\/\*/,/\*\/$/d' \
	|sed 's/\(^{$\)\|\( {$\)/;/'
done
