#pragma once

#include <cstdint>

namespace revolif::db {

struct DatabaseStatistics
{
    uint64_t successfulConnections = 0;
    uint64_t failedConnections     = 0;
    uint64_t reconnects            = 0;
    uint64_t executedQueries       = 0;
    uint64_t failedQueries         = 0;
    uint64_t transactionsStarted   = 0;
    uint64_t committedTransactions = 0;
    uint64_t rolledBackTransactions= 0;
};

} // namespace revolif::db
