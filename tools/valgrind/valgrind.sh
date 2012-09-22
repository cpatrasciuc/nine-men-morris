#!/bin/sh

valgrind --gen-suppressions=yes \
	--suppressions=`dirname $0`/suppressions.txt \
	--leak-check=full \
	--track-origins=yes \
	--show-reachable=yes \
	$@
