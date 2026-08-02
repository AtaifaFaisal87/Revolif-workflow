#pragma once

namespace revolif::db {

enum class ConnectionState
{
    Disconnected,
    Connecting,
    Connected,
    Reconnecting,
    Disconnecting
};

} // namespace revolif::db
