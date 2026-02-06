#!/bin/sh
#
# get C extern functions, compatible with C++.
#


s='\(F_SYMBOL(\)\|\(C_SYMBOL(\)'

cat <<__EOF__

#ifdef __cplusplus
extern "C" {
#endif
__EOF__

for i in "$@"; do
	echo
	echo "/* $i */"

	sed -n '/^\/\* @func: /,/\(^{$\)\|\( {$\)/p' "$i" \
	|sed '/^\/\* @func:.*(.*static.*).*/,/\(^{$\)\|\( {$\)/d;/^\/\*/,/\*\/$/d' \
	|sed 's/\(^{$\)\|\( {$\)/;/;/'"$s"'/s/^/\nextern\n/' \
	|sed 's/\bnew\b/_new/g'
done

cat <<__EOF__

#ifdef __cplusplus
}
#endif

__EOF__
