#!/bin/sh
#
# get the maximum length line in the file.
#  max: >80
#


min_len=80
space_len=8

for i in "$@"; do
	awk -F '\n' '
{
	s = $0;
	n = gsub(/\t/, "", s);
	len = length(s) + (n * '$space_len');
	if (len > '$min_len') {
		printf "file: %s, line: %d, width: %d\n",
			FILENAME, NR, len;
	}
}
' "$i"
done
