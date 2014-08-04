/**
 * StatsD Client
 *
 * Copyright (c) 2012-2014 Axel Etcheverry
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */
#pragma once

#include <string>
#include <netinet/in.h>

#ifdef DEBUG
# include <iostream>
# define statsd_error(message) \
    std::cerr << "StatsD: " << message << std::endl;
#else
# define statsd_error(message)
#endif

class statsd
{
public:

    /**
     * Open udp socket
     *
     * @param host The host of StatsD server
     * @param port The port of StatsD server
     */
    static void open(const std::string& host, int16_t port = 8125);

    /**
     * Log timing information
     *
     * @param key           The metric to in log timing info for.
     * @param value         The ellapsed time (ms) to log
     * @param sample_rate   The rate (0-1) for sampling.
     */
    static void timing(const std::string& key, const int64_t value, const float sample_rate = 1.0);

    /**
     * Increments one or more stats counters
     *
     * @param key           The metric(s) to increment.
     * @param sample_rate   The rate (0-1) for sampling.
     */
    static void increment(const std::string& key, const float sample_rate = 1.0);

    /**
     * Decrements one or more stats counters.
     *
     * @param key           The metric(s) to decrement.
     * @param sample_rate   The rate (0-1) for sampling.
     */
    static void decrement(const std::string& key, const float sample_rate = 1.0);

    /**
     * Count is the default statsd method for counting
     *
     * @param key           The metric(s) to count
     * @param value         The count value
     * @param sample_rate   The rate (0-1) for sampling.
     */
    static void count(const std::string& key, const int64_t value, const float sample_rate = 1.0);

    /**
     * Gauge
     *
     * @param key           The metric(s) to count
     * @param value         The count value
     * @param sample_rate   The rate (0-1) for sampling.
     */
    static void gauge(const std::string& key, const int64_t value, const float sample_rate = 1.0);

    /**
     * Set
     *
     * @param key           The metric(s) to count
     * @param value         The count value
     * @param sample_rate   The rate (0-1) for sampling.
     */
    static void set(const std::string& key, const int64_t value, const float sample_rate = 1.0);

    /**
     * Close socket
     */
    static void close();

    /**
     * Normalize key
     *
     * @param key   The key of metric(s)
     * @return      The normalized key
     */
    static std::string normalize(const std::string& key);

    /**
     * Prepare message
     *
     * @param key           The grahite node
     * @param value         The count value
     * @param sample_rate   The rate (0-1) for sampling.
     * @param unit          The unit value
     * @return              The message
     */
    static std::string prepare(
        const std::string& key,
        const int64_t value,
        const float sample_rate,
        const std::string& unit
    );

    /**
     * Get version
     *
     * @return The string of version
     */
    static const char* version();

private:

    struct statsd_t
    {
        struct sockaddr_in server;
        int sock = -1;
    };

    static statsd_t info;

    /**
     * Send
     *
     * @param key           The grahite node
     * @param value         The count value
     * @param sample_rate   The rate (0-1) for sampling.
     * @param unit          The unit value
     */
    static void send(
        const std::string& key,
        const int64_t value,
        const float sample_rate,
        const std::string& unit
    );

    /**
     *
     * @param sample_rate   The rate (0-1) for sampling.
     * @return              True if send
     */
    static bool should_send(const float sample_rate);
};
