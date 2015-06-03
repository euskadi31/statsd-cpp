/**
 * StatsD Client
 *
 * Copyright (c) 2012-2014 Axel Etcheverry
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <version.hpp>
#include <statsd.hpp>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>

statsd::statsd_t statsd::info;

void statsd::open(const std::string& host, int16_t port)
{
    if (info.sock == -1)
    {

        if ((info.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        {
            statsd_error("StatsD: fail socket");
            return;
        }

        memset(&info.server, 0, sizeof(info.server));

        info.server.sin_family = AF_INET;
        info.server.sin_port = htons(port);

        struct addrinfo *result = NULL, hints;

        memset(&hints, 0, sizeof(hints));

        hints.ai_family     = AF_INET;
        hints.ai_socktype   = SOCK_DGRAM;

        int error;

        if ((error = getaddrinfo(host.c_str(), nullptr, &hints, &result)))
        {
            statsd_error("StatsD: " + gai_strerror(error));
            return;
        }

        memcpy(
            &(info.server).sin_addr,
            &((struct sockaddr_in*)result->ai_addr)->sin_addr,
            sizeof(struct in_addr)
        );

        freeaddrinfo(result);

        if (inet_aton(host.c_str(), &(info.server).sin_addr) == 0)
        {
            statsd_error("StatsD: fail inet_aton");
            return;
        }

        srandom(static_cast<unsigned int>(time(nullptr)));
    }
}

void statsd::timing(const std::string& key, const int64_t value, const float sample_rate)
{
    send(key, value, sample_rate, "ms");
}

void statsd::increment(const std::string& key, const float sample_rate)
{
    count(key, 1, sample_rate);
}

void statsd::decrement(const std::string& key, const float sample_rate)
{
    count(key, -1, sample_rate);
}

void statsd::count(const std::string& key, const int64_t value, const float sample_rate)
{
    send(key, value, sample_rate, "c");
}

void statsd::gauge(const std::string& key, const int64_t value, const float sample_rate)
{
    send(key, value, sample_rate, "g");
}

void statsd::set(const std::string& key, const int64_t value, const float sample_rate)
{
    send(key, value, sample_rate, "s");
}

void statsd::close()
{
    if (info.sock != -1)
    {
        ::close(info.sock);
        info.sock = -1;
    }
}

void statsd::send(
    const std::string& key,
    const int64_t value,
    const float sample_rate,
    const std::string& unit
)
{
    if (info.sock == -1)
    {
        return;
    }

    if (should_send(sample_rate) == false)
    {
        return;
    }

    std::string message = prepare(key, value, sample_rate, unit);

    if (sendto(
        info.sock,
        message.c_str(),
        message.length(),
        0,
        (struct sockaddr *)&info.server,
        sizeof(info.server)
    ) == -1)
    {
        statsd_error("StatsD: fail sendto");
    }
}

bool statsd::should_send(const float sample_rate)
{
    if (sample_rate < 1.0)
    {
        return (sample_rate > static_cast<float>(random() / RAND_MAX));
    }
    else
    {
        return true;
    }
}

std::string statsd::normalize(const std::string& key)
{
    std::string retval;

    for (std::size_t i = 0; i < key.length(); ++i)
    {
        char chr = key[i];

        if (chr == ':' || chr == '|' || chr == '@')
        {
            chr = '.';
        }

        retval += chr;
    }

    return retval;
}

std::string statsd::prepare(
    const std::string& key,
    const int64_t value,
    const float sample_rate,
    const std::string& unit
)
{
    std::ostringstream out;
    out << normalize(key) << ":" << value << "|" << unit;

    if (sample_rate != 1.0)
    {
        out << "|@" << std::setprecision(1) << sample_rate;
    }

    return out.str();
}

const char* statsd::version()
{
    return STATSD_VERSION;
}
