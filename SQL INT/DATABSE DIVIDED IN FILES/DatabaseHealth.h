#pragma once

#include <string>
#include <chrono>
#include <ctime>

namespace revolif::db {

struct DatabaseHealth
{
    bool connected = false;
    bool transactionActive = false;
    std::string serverVersion;
    std::string databaseName;
    std::string charset;
    std::chrono::milliseconds ping{0};
    std::time_t connectedSince = 0;
    std::time_t lastReconnect = 0;
};

} // namespace revolif::db
