#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#include <string>

namespace constants {
const bool        JSON_PARSE_NO_THROW{false};
const std::string DEFAULT_CONFIG_PATH{"config/config.ini"};
const std::string CONFIG_JSON_PATH{"config/credentials.json"};
const std::string TOKEN_JSON_PATH{"config/token.json"};

const uint8_t HEADER_AUTH_INDEX = 0x00;

const std::vector<std::string> HEADER_NAMES{
"Authorization"
};

namespace MastodonOnline {
const uint8_t TOKEN_INDEX        = 0x00;
const uint8_t TOKEN_VERIFY_INDEX = 0x01;
const uint8_t STATUSES_INDEX     = 0x02;

const std::string BASE_URL{"https://mastodon.online"};

const std::vector<std::string> PATH{
  "/oauth/token",
  "/api/v1/accounts/verify_credentials",
  "/api/v1/statuses"
};
} // namespace MastodonOnline
} // namespace constants

#endif // __CONSTANTS_HPP__
