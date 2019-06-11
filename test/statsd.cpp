/**
 * StatsD Client
 *
 * Copyright (c) 2012-2014 Axel Etcheverry
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */
#include <statsd.hpp>
#include <thread>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <iostream>

#define test_assert_equal(A, B) \
    assert(A == B);

int main(int argc, char const *argv[])
{
    /* Sample UDP server */
    std::thread server([]() {
        int sockfd;
        ssize_t n;
        struct sockaddr_in servaddr, cliaddr;
        socklen_t len;
        char mesg[1000];

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(8125);
        bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

        int i = 0;

        while (true)
        {
            len = sizeof(cliaddr);
            n = recvfrom(sockfd, mesg, 1000, 0, (struct sockaddr *)&cliaddr, &len);

            if (n > 0)
            {
                mesg[n] = 0;
                std::cout << mesg << std::endl;

                switch (i)
                {
                    case 0:
                        assert(strcmp(mesg, "test.site.homepage.load:1500|ms") == 0);
                        break;
                    case 1:
                        assert(strcmp(mesg, "test.server.http.error.404:1|c") == 0);
                        break;
                    case 2:
                        assert(strcmp(mesg, "test.server.http.error.404:-1|c") == 0);
                        break;
                    case 3:
                        assert(strcmp(mesg, "test.site.auth.success:4|c") == 0);
                        break;
                    case 4:
                        assert(strcmp(mesg, "a.gauge.node:8|g") == 0);
                        break;
                    case 5:
                        assert(strcmp(mesg, "a.graphite.set:12|s") == 0);
                        break;
                }

                ++i;
            }

            if (i == 5)
            {
                break;
            }
        }

        ::close(sockfd);
    });

    std::cout << "StatsD Version : " << statsd::version() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    statsd::open("127.0.0.1");

    std::vector<std::string> vect{} ;
    vect.push_back( std::string("version=")+statsd::version()); 
    statsd::setGlobalTags(vect);

    test_assert_equal(
        statsd::normalize("test.server@http:error|404"),
        "test.server.http.error.404"
    );

    test_assert_equal(
        statsd::prepare("test.server@http:error|404", 1, 1, "c"),
        "test.server.http.error.404:1|c"
    );

    test_assert_equal(
        statsd::prepare("test.server@http:error|404", -1, 1, "c"),
        "test.server.http.error.404:-1|c"
    );

    test_assert_equal(
        statsd::prepare("test.server@http:error|404", 1, 0.5, "c"),
        "test.server.http.error.404:1|c|@0.5"
    );

    statsd::timing("test.site.homepage.load", 1500);

    statsd::increment("test.server.http.error.404");

    statsd::decrement("test.server.http.error.404");

    statsd::count("test.site.auth.success", 4);

    statsd::gauge("a.gauge.node", 8);

    statsd::set("a.graphite.set", 12);

    statsd::close();

    server.join();

    return 0;
}
