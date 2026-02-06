#!/bin/sh
#
# change C #define number order.
#


n=${1:-0}

awk -F '\n' \
'
BEGIN {
	n = '$n';
}

{
	if ($0 ~ /^\s*#\s*define\s*[A-Za-z0-9_]*\s+[0-9]*[0-9]/) {
		sub(/\<[0-9]*[0-9]\>/, n);
		print;
		n++;
	} else {
		print;
	}
}
'
