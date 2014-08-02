StatsD client cpp [![Build Status](https://travis-ci.org/euskadi31/statsd-cpp.png)](https://travis-ci.org/euskadi31/statsd-cpp)
=================

A cpp client for the Etsy StatsD server.

Install
-------

~~~shell
$ make && make install
~~~

Test
----

~~~shell
$ make test
~~~

Usage
-----

~~~cpp
#include <statsd.hpp>

int main(int argc, char const *argv[])
{
    statsd::open("127.0.0.1", 8125);

    statsd::timing("test.site.homepage.load", 1500);

    statsd::increment("test.server.http.error.404");

    statsd::decrement("test.server.http.error.404");

    statsd::count("test.site.auth.success", 4);

    statsd::gauge("a.gauge.node", 8);

    statsd::set("a.graphite.set", 12);

    statsd::close();

    return 0;
}
~~~

Documentation
-------------

* statsd::open(std::string `host`[, int16_t `port`])
* statsd::timing(std::string `key`, int64_t `value`[, float `sample_rate` = `1.0`])
* statsd::increment(std::string `key`[, float `sample_rate` = `1.0`])
* statsd::decrement(std::string `key`[, float `sample_rate` = `1.0`])
* statsd::count(std::string `key`, int64_t `value`[, float `sample_rate` = `1.0`])
* statsd::gauge(std::string `key`, int64_t `value`[, float `sample_rate` = `1.0`])
* statsd::set(std::string `key`, int64_t `value`[, float `sample_rate` = `1.0`])
* statsd::close()

License
-------

statsd-cpp is licensed under [the MIT license](LICENSE.md).
