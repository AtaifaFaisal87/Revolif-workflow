#pragma once

namespace revolif::db::SQL {

inline constexpr auto Ping          = "SELECT 1";
inline constexpr auto Begin         = "START TRANSACTION";
inline constexpr auto Commit        = "COMMIT";
inline constexpr auto Rollback      = "ROLLBACK";
inline constexpr auto SchemaVersion = "SELECT version FROM schema_version WHERE id = 1";

} // namespace revolif::db::SQL
