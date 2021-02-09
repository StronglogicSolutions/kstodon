#pragma once

#include "kstodon/config.hpp"

#include "psqlorm.hpp"

namespace koreannews {
const std::string NAME   {"koreannews"};
const std::string SOURCES{"hacker-news,ars-technica,"
                          "crypto-coins-news,"
                          "recode,techradar,"
                          "the-next-web,wired"};
const std::string DB_PATH{"src/applications/koreannews/posts.json"};
const std::string DB_NAME{"posts"};
const std::string HASHTAGS{"#KoreanNews #KSTYLEYO #KoreanLanguage #LearnKorean #KoreanStudy #KoreanLearning #Topik"};

/**
 * Helper Functions
 */
std::string GetAPIKey() {
  return GetConfigReader().GetString(constants::KOREAN_NEWS_SECTION, constants::NEWSAPI_CONFIG_KEY, "");
}

std::string GetURL() {
  return "http://newsapi.org/v2/top-headlines?country=kr&apiKey=" + GetAPIKey();
}

Database::PSQLORM GetDatabase() {
  return Database::PSQLORM{
    DatabaseConfiguration{
      .credentials = DatabaseCredentials{
        .user     = "knewsadmin",
        .password = "knewsadmin",
        .name     = "knews"
      },
      .address = "127.0.0.1",
      .port    = "5432"
    }
  };
}

void SavePost(const std::string& url) {
  using namespace Database;
  PSQLORM db = GetDatabase();

  if (!db.insert(DB_NAME, {"url"},{url}))
    log("Could not save to database");
}

bool AlreadySaved(std::string url) {
  using namespace Database;
  PSQLORM db = GetDatabase();

  QueryValues values = db.select(
    DB_NAME,
    Fields{
      "id"
    },
    QueryFilter{
      {
        "url", url
      }
    }
  );

  for (const auto& value : values)
    log(value.first + " : " + value.second);

  return !values.empty();
}


nlohmann::json GetNewsJSON() {
  RequestResponse response{cpr::Get(
    cpr::Url{GetURL()}
  )};

  return (!response.error) ?
    response.json() : nlohmann::json{};
}

/**
 * @brief Get a trending tech news URL
 *
 * @returns [out] {std::string}
 */
std::string GetNews() {
  using namespace koreannews;

  nlohmann::json news_json = GetNewsJSON();

  if (!news_json.is_null()            &&
       news_json.is_object()          &&
       news_json.contains("articles") &&
      !news_json["articles"].is_null())
  {
    for (const nlohmann::json& article : news_json["articles"])
    {
      if (!AlreadySaved(article["url"]))
      {
        SavePost(article["url"]);
        return std::string{
          article["title"].get<std::string>() + '\n' +
          HASHTAGS                            + '\n' +
          article["url"].get<std::string>()
        };
      }
    }
  }
  return "";
}

/**
 * @brief GenerateStatus
 *
 * @returns [out] {Status}
 */
Status GenerateStatus()
{
  Status status{};
  status.content = GetNews();

  return status;
}

/**
 * @brief ReplyToStatus
 *
 * TODO: Remove markup before tokenizing
 *
 * @param   [in]  {Status} received_status
 * @returns [out] {Status}
 */
Status ReplyToStatus(Status received_status)
{
  using namespace conversation;

  std::vector<Token> tokens = SplitTokens(TokenizeText(received_status.content));

  Status status{};

  if (!tokens.empty())
  {
    status.content += "\nI am down with";
    std::string delim{};
    for (const Token& token : tokens)
    {
      status.content += delim + token.value;
      delim = " and ";
    }
  }
  else
    status.content += "Greetings from Stronglogic";

  return status;
}

}
