#!/bin/sh
#
# check for non-ascii character in the file.
#


for i in $@; do
	s=$(od -t x1 -w1 $i |grep ' [8-9a-f][0-9a-f]')
	if test "x$s" != "x"; then
		b=$(printf %d $(echo $s |cut -d ' ' -f1))
		b=$((b + 1))
		n=$(printf %d $(head -c $b $i |cat -n |tail -n 1 |grep -o '^\s*[0-9]\+'))
		c=$(head -c $b $i |tail -n 1 |wc -c)
		echo "file: $i, length: $b, line: $n, col: $c"
	fi
done
