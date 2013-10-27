#!/bin/bash
#
# Copyright (c) 2012 Cristian Patrasciuc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Script used to generate code coverage reports for the entire project.
# Useful link:
#   http://www.slideshare.net/maguschen/using-gcov-and-lcov

# Check if the necessary tools are installed
check_if_installed() {
  which $1 >/dev/null 2>&1 || { echo "$1 cannot be found." && exit 1; }
}
check_if_installed lcov
check_if_installed genhtml

MY_DIR=$(dirname $0)
SRC_DIR=${MY_DIR}/../src/
COVERAGE_DIR=${SRC_DIR}/__coverage__
LCOV_INFO_FILE=${COVERAGE_DIR}/lcov.info

# Clean everything
(cd ${SRC_DIR} && make clean)
find ${SRC_DIR} -name "*gcda" -delete
find ${SRC_DIR} -name "*gcno" -delete
rm -rf ${COVERAGE_DIR}

# Build from scratch
# TODO(coverage): Only enable the coverage compile flags here
(cd ${SRC_DIR} && make)

# Run all tests
${SRC_DIR}/base/base_unittests
${SRC_DIR}/game/game_unittests
${SRC_DIR}/ai/ai_unittests
${SRC_DIR}/console_game/console_game_unittests
${SRC_DIR}/console_game/functional_tests
(cd ${SRC_DIR}/graphics && ./in_game_tests)

# Generate coverage reports
mkdir ${COVERAGE_DIR}
find ${SRC_DIR} -name "*gcda" -exec mv '{}' ${COVERAGE_DIR} \;
find ${SRC_DIR} -name "*gcno" -exec mv '{}' ${COVERAGE_DIR} \;
lcov -t "Code coverage report" -o ${LCOV_INFO_FILE} -c -d ${COVERAGE_DIR}
lcov -e ${LCOV_INFO_FILE} -o ${LCOV_INFO_FILE} "*src/base/*" "*src/game/*" \
    "*src/console_game/*" "*src/ai/*" "*src/graphics/*" 
lcov -r ${LCOV_INFO_FILE} -o ${LCOV_INFO_FILE} "*test.cc"

HTML_REPORT_DIR=${COVERAGE_DIR}/html_report
genhtml -o ${HTML_REPORT_DIR} ${LCOV_INFO_FILE}

echo "Code coverage statistics generated succesfully."
echo "Please copy and paste the following URL into your browser:"

# We need the absolute path for the HTML report dir since we want to generate
# the URL that can be copied and pasted into the browser.
echo "  file://$(cd ${HTML_REPORT_DIR} && pwd)/index.html"




