#!/bin/sh

# Delete old coverage data
find . -name "*.gcda" -delete
find . -name "coverage_report.*" -delete

# Deserialize "coverage.txt" file; generate notes/data files and post-process HTML coverage report
arm-none-eabi-gcov-tool merge-stream coverage.bin --verbose
find . -name "*.gcno" -exec sh -c 'for f in $@; do arm-none-eabi-gcov "${f%.gcno}.obj"; done' {} +
gcovr -r .. -g --html-details coverage_report.html --html-theme github.green
tar -czf coverage_report.tar.gz coverage_report.*