##
# StatsD Client
#
# Copyright (c) 2012-2014 Axel Etcheverry
#
# For the full copyright and license information, please view the LICENSE
# file that was distributed with this source code.
##
release: clean
	@mkdir -p build/Release
	@cd build/Release; cmake ../../ -DCMAKE_BUILD_TYPE=Release
	@cd build/Release; make

test: clean
	@mkdir -p build/Debug
	@cd build/Debug; cmake ../../ -DCMAKE_BUILD_TYPE=Debug
	@cd build/Debug; make
	@cd build/Debug; make test

install: release
	@cd build/Release; make install

clean:
	@rm -rf build

.PHONY: clean test
