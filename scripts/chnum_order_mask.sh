#!/bin/sh
#
# change C #define number order.
#


n=${1:-1}

awk -F '\n' \
'
BEGIN {
	n = '$n';
}

{
	if ($0 ~ /^\s*#\s*define\s*[A-Za-z0-9_]*\s+0x[0-9a-f]*[0-9a-f]/) {
		s = sprintf("0x%02x", n);
		sub(/\<0x[0-9a-f]*[0-9a-f]\>/, s);
		print;
		n = lshift(n, 1);
	} else {
		print;
	}
}
'
