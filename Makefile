##
# StatsD Client
#
# Copyright (c) 2012-2014 Axel Etcheverry
#
# For the full copyright and license information, please view the LICENSE
# file that was distributed with this source code.
##
STATSD_COMPILE_MODE ?= Release

build: clean
	@echo "Build ${STATSD_COMPILE_MODE}"
	mkdir -p build/${STATSD_COMPILE_MODE}
	@cd build/${STATSD_COMPILE_MODE}; cmake ../../ -DCMAKE_BUILD_TYPE=${STATSD_COMPILE_MODE}
	@cd build/${STATSD_COMPILE_MODE}; make

test: build
	@cd build/${STATSD_COMPILE_MODE}; make test

install: build
	@cd build/${STATSD_COMPILE_MODE}; make install

clean:
	@rm -rf build

.PHONY: clean test
