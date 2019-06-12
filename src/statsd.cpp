/**
 * StatsD Client
 *
 * Copyright (c) 2012-2014 Axel Etcheverry
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */
#include <sys/types.h>

#include <time.h>
#include <statsd.hpp>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <string>

#include <random>
#include <iostream>
#include <sstream> // for ostringstream
#include <version.hpp>

#ifdef _WIN32
#include <ws2tcpip.h>
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif

statsd::statsd_t statsd::info;
std::string statsd::prefix;
std::string statsd::global_tags_str;


static std::random_device rd; // random device engine, usually based on /dev/random on UNIX-like systems  
static std::mt19937 generator(rd()); // initialize Mersennes' twister using rd to generate the seed

int statsd::open(const std::string& host, int16_t port, int mode)
{
    if (info.sock == -1)
    {
        int proto;
        if (mode == SOCK_STREAM){
            proto = IPPROTO_TCP;
			info.type = SOCK_STREAM;
        } else  {
            proto = IPPROTO_UDP;
			info.type = SOCK_DGRAM;
        }
        
		info.sock = socket(AF_INET, info.type, proto);
        if (info.sock == 0)
        {
            statsd_error("fail socket create");
            return 1;
        }

        memset(&info.server, 0, sizeof(info.server));

        info.server.sin_family = AF_INET;
        info.server.sin_port = htons(port);

        struct addrinfo *result = NULL, hints;

        memset(&hints, 0, sizeof(hints));

        hints.ai_family     = AF_INET;
        hints.ai_socktype   = info.type;

        int error = getaddrinfo(host.c_str(), nullptr, &hints, &result);
        if (error != 0 )
        {
            statsd_error(gai_strerror(error));
            return error;
        }

        memcpy(
            &(info.server).sin_addr,
            &((struct sockaddr_in*)result->ai_addr)->sin_addr,
            sizeof(struct in_addr)
        );

        freeaddrinfo(result);

        if (inet_pton(AF_INET, host.c_str(), &(info.server).sin_addr) == 0)
            {
                statsd_error("fail inet_pton");
                return 2;
            } 

        if (mode == SOCK_STREAM){
            if (connect(info.sock, (struct sockaddr *)&info.server, sizeof(info.server)) < 0) 
            { 
                statsd_error("fail connect");
                return 3; 
            } 
        }
    }
	return 4;
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
        #ifdef _WIN32
        closesocket(info.sock);
        #else
        ::close(info.sock);
        #endif
		
        info.sock = -1;
    }
}

void statsd::setPrefix(const std::string& _prefix) {
	prefix = _prefix;
}

void statsd::setGlobalTags(std::vector<std::string> global_tags){
    std::ostringstream tagstream;
    std::vector<std::string>::iterator it = global_tags.begin();
    if (it != global_tags.end()) {
        tagstream << *it++;
    }
    for ( ; it != global_tags.end() ; it++) {
    tagstream << "," << *it;
    }
    global_tags_str = tagstream.str();
}

const std::vector<std::string> empty_tags;

void statsd::send(
    const std::string& key,
    const int64_t value,
    const float sample_rate,
    const std::string& unit
)
{
    if (info.sock == -1)
    {
		statsd_error("fail sendto socket not created");

        return;
    }

    if (should_send(sample_rate) == false)
    {
        return;
    }

    std::string message = prepare(key, value,empty_tags, sample_rate, unit);

	if (info.type == SOCK_STREAM) {
		if (::send(
			info.sock,
			message.c_str(),
			message.length(),
			0) == -1)
		{
			statsd_error("fail send");
		}
	}
	else {
		if (sendto(
			info.sock,
			message.c_str(),
			message.length(),
			0,
			(struct sockaddr *)&info.server,
			sizeof(info.server)
		) == -1)
		{
			statsd_error("fail sendto");
		}
	}

    
}

bool statsd::should_send(const float sample_rate)
{
    if (sample_rate < 1.0)
    {
        return (sample_rate > static_cast<float>(generator() / 4294967295));
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
    const std::vector<std::string> tags,
    const float sample_rate,
    const std::string& unit
)
{
    bool tagging = false;
    std::ostringstream out;

    out << prefix <<  normalize(key) << ":" << value << "|" << unit;
    if (sample_rate != 1.0)
    {
        out << "|@" << std::setprecision(1) << sample_rate;
    }

    if (tags.size()) {
      tagging = true;
      // TODO: remove newlines from the tags.
      auto it = tags.begin();
      out << "|#" << *it++;
      for( ; it != tags.end() ; ++it) {
        out << "," << *it;
      }
    }

    if (!global_tags_str.empty()) {
      if (!tagging) {
        out << "|#" ;
      } else {
        out << "," ;
      }
      out << global_tags_str;
    }

    out << "\n";

    return out.str();
}

const char* statsd::version()
{
    return STATSD_VERSION;
}
