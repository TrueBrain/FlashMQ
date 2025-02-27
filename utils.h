/*
This file is part of FlashMQ (https://www.flashmq.org)
Copyright (C) 2021 Wiebe Cazemier

FlashMQ is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, version 3.

FlashMQ is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public
License along with FlashMQ. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <errno.h>
#include <string>
#include <list>
#include <limits>
#include <vector>
#include <algorithm>
#include <openssl/evp.h>
#include <memory>
#include <arpa/inet.h>
#include "unistd.h"
#include "sys/stat.h"

#include "cirbuf.h"
#include "bindaddr.h"
#include "types.h"
#include "flashmq_plugin.h"

#define UNUSED(expr) do { (void)(expr); } while (0)

template<typename T> int check(int rc)
{
    if (rc < 0)
    {
        char *err = strerror(errno);
        std::string msg(err);
        throw T(msg);
    }

    return rc;
}

std::list<std::string> split(const std::string &input, const char sep, size_t max = std::numeric_limits<int>::max(), bool keep_empty_parts = true);
const std::vector<std::string> splitToVector(const std::string &input, const char sep, size_t max = std::numeric_limits<int>::max(), bool keep_empty_parts = true);
void splitToVector(const std::string &input, std::vector<std::string> &output, const char sep, size_t max = std::numeric_limits<int>::max(), bool keep_empty_parts = true);
void splitTopic(const std::string &topic, std::vector<std::string> &output);

bool topicsMatch(const std::string &subscribeTopic, const std::string &publishTopic);

bool isValidUtf8Generic(const std::string &s, bool alsoCheckInvalidPublishChars = false);
bool isValidUtf8(const std::string &s, bool alsoCheckInvalidPublishChars = false);

bool strContains(const std::string &s, const std::string &needle);

bool isValidPublishPath(const std::string &s);
bool isValidSubscribePath(const std::string &s);
bool containsDangerousCharacters(const std::string &s);

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);
bool startsWith(const std::string &s, const std::string &needle);
std::string &rtrim(std::string &s, unsigned char c);

std::string getSecureRandomString(const ssize_t len);
std::string str_tolower(std::string s);
bool stringTruthiness(const std::string &val);
bool isPowerOfTwo(int val);

bool parseHttpHeader(CirBuf &buf, std::string &websocket_key, int &websocket_version, std::string &subprotocol);

std::vector<char> base64Decode(const std::string &s);
std::string base64Encode(const unsigned char *input, const int length);
std::string generateWebsocketAcceptString(const std::string &websocketKey);

std::string generateInvalidWebsocketVersionHttpHeaders(const int wantedVersion);
std::string generateBadHttpRequestReponse(const std::string &msg);
std::string generateWebsocketAnswer(const std::string &acceptString, const std::string &subprotocol);

void testSsl(const std::string &fullchain, const std::string &privkey);

std::string formatString(const std::string str, ...);

std::string dirnameOf(const std::string& fname);

BindAddr getBindAddr(int family, const std::string &bindAddress, int port);

ssize_t getFileSize(const std::string &path);

std::string sockaddrToString(struct sockaddr *addr);

template<typename ex> void checkWritableDir(const std::string &path)
{
    if (path.empty())
        throw ex("Dir path to check is an empty string.");

    if (access(path.c_str(), W_OK) != 0)
    {
        std::string msg = formatString("Path '%s' is not there or not writable", path.c_str());
        throw ex(msg);
    }

    struct stat statbuf;
    memset(&statbuf, 0, sizeof(struct stat));
    if (stat(path.c_str(), &statbuf) < 0)
    {
        // We checked for W_OK above, so this shouldn't happen.
        std::string msg = formatString("Error getting information about '%s'.", path.c_str());
        throw ex(msg);
    }

    if (!S_ISDIR(statbuf.st_mode))
    {
        std::string msg = formatString("Path '%s' is not a directory.", path.c_str());
        throw ex(msg);
    }
}

const std::string websocketCloseCodeToString(uint16_t code);

const std::string protocolVersionString(ProtocolVersion p);

uint32_t ageFromTimePoint(const std::chrono::time_point<std::chrono::steady_clock> &point);
std::chrono::time_point<std::chrono::steady_clock> timepointFromAge(const uint32_t age);

ReasonCodes authResultToReasonCode(AuthResult authResult);


#endif // UTILS_H
