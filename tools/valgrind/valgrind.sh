#!/bin/sh

export GTEST_DEATH_TEST_USE_FORK=1
valgrind --gen-suppressions=yes \
	--suppressions=`dirname $0`/suppressions.txt \
	--leak-check=full \
	--track-origins=yes \
	--show-reachable=yes \
	$@
