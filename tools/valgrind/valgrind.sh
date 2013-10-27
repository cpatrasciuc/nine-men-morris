#!/bin/sh

export GTEST_DEATH_TEST_USE_FORK=1
export G_SLICE=always-malloc
valgrind --gen-suppressions=yes \
	--suppressions=`dirname $0`/suppressions.txt \
	--leak-check=full \
	--show-reachable=yes \
	$@
