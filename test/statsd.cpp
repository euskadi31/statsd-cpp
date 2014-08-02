/**
 * StatsD Client
 *
 * Copyright (c) 2012-2014 Axel Etcheverry
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */
#include <statsd.hpp>
#include <cassert>

int main(int argc, char const *argv[])
{
    statsd::open("127.0.0.1");

    assert(statsd::normalize("test.server@http:error|404") == "test.server.http.error.404");

    assert(statsd::prepare("test.server@http:error|404", 1, 1, "c") == "test.server.http.error.404:1|c");
    assert(statsd::prepare("test.server@http:error|404", -1, 1, "c") == "test.server.http.error.404:-1|c");
    assert(statsd::prepare("test.server@http:error|404", 1, 0.5, "c") == "test.server.http.error.404:1|c|@0.5");

    statsd::increment("test.server.http.error.404");

    statsd::close();

    return 0;
}
