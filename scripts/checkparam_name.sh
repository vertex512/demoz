#!/bin/sh
#
# check the parameters name in the file.
#


for f in "$@"; do
	echo $f
	for j in $(grep '^\/\* @func:' $f \
			|grep -o ':\s*[a-zA-Z0-9_]\+' \
			|sed 's/:\s*//'); do
		d=$(sed -n '/^\/\* @func: '$j'/,/{/p' $f \
			|sed '/^\/\* @func:/,/\*\/$/d')
		for i in $(sed -n '/@func:\s*'$j'/,/{/p' $f \
				|grep '#[0-9]\+:' \
				|sed 's/^\s*\* #[0-9]\+:\s*\([A-Za-z0-9_]\+\).*$/\1/'); do
			grep -q "\b$i\b" <<EOF
$d
EOF
test $? != 0 && echo "  $j $i"
		done
	done
done
