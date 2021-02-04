#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#include <string>


namespace constants {
const bool        JSON_PARSE_NO_THROW{false};
const std::string DEFAULT_CONFIG_PATH{"config/config.ini"};
const std::string CONFIG_JSON_PATH   {"config/credentials.json"};
const std::string TOKEN_JSON_PATH    {"config/token.json"};
const std::string DB_JSON_PATH       {"config/db.json"};
const std::string KSTODON_SECTION    {"kstodon"};
const std::string TECH_NEWS_SECTION  {"technews"};
const std::string KOREAN_NEWS_SECTION{"koreannews"};
const std::string USER_CONFIG_KEY    {"user"};
const std::string NEWSAPI_CONFIG_KEY {"news_api"};
const std::string DEFAULT_STATUS_MSG {"This is the response. Take it or leave it."};

const uint32_t MASTODON_CHAR_LIMIT = 0x1F4;
const uint8_t  HEADER_AUTH_INDEX   = 0x00;

const std::vector<std::string> HEADER_NAMES{
"Authorization"
};

namespace MastodonOnline {
const uint8_t TOKEN_INDEX        = 0x00;
const uint8_t TOKEN_VERIFY_INDEX = 0x01;
const uint8_t STATUSES_INDEX     = 0x02;
const uint8_t ACCOUNTS_INDEX     = 0x03;
const uint8_t MEDIA_INDEX        = 0x04;
const uint8_t CONVERSATION_INDEX = 0x05;

const std::string BASE_URL{"https://mastodon.online"};

const std::vector<std::string> PATH{
  "/oauth/token",
  "/api/v1/accounts/verify_credentials",
  "/api/v1/statuses",
  "/api/v1/accounts",
  "/api/v1/media",
  "/api/v1/conversations"
};

inline const std::string STATUS_CONTEXT_URL(const std::string& base_url, uint64_t status_id) {
  return std::string{
    base_url + PATH.at(STATUSES_INDEX) + '/' + std::to_string(status_id) + "/context"
  };
}

} // namespace MastodonOnline
} // namespace constants

#endif // __CONSTANTS_HPP__
