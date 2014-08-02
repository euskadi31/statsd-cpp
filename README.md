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

see the [statsd.hpp](include/statsd.hpp)

License
-------

statsd-cpp is licensed under [the MIT license](LICENSE.md).
