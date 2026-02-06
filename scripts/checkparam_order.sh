#!/bin/sh
#
# check the parameters number order in the file.
#


for i in "$@"; do
	awk -F '\n' '
BEGIN {
	n = 1;
}

{
	s = $0;
	if (s ~ /^\/\* @func:/)
		n = 1;

	if (s ~ /^ \* #[0-9]:/) {
		sub(/^ \* #/, "", s);
		sub(/:.*/, "", s);
		if (s != n) {
			printf "file: %s, line: %d, %s\n",
				FILENAME, NR, $0;
		} else {
			n++;
		}
	}
}
' "$i"
done
