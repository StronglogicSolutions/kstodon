#pragma once

#include "kstodon/config.hpp"

namespace technews {
const std::string NAME   {"stronglogicsolutions"};
const std::string SOURCES{"hacker-news,ars-technica,"
                          "crypto-coins-news,"
                          "recode,techradar,"
                          "the-next-web,wired"};
const std::string DB_PATH{"src/applications/technews/posts.json"};
const std::string HASHTAGS{"#StronglogicSolutions #TechNews #News #Technology"};

/**
 * Helper Functions
 */
std::string GetAPIKey() {
  return GetConfigReader().GetString(constants::TECH_NEWS_SECTION, constants::NEWSAPI_CONFIG_KEY, "");
}

std::string GetURL() {
  return "https://newsapi.org/v2/top-headlines?sources=" + SOURCES + "&apiKey=" + GetAPIKey();
}

nlohmann::json GetDatabase() {
   return LoadJSONFile(DB_PATH);
}

void SaveDatabase(nlohmann::json data) {
  SaveToFile(data, DB_PATH);
}

bool AlreadySaved(std::string id) {
  auto db = GetDatabase();

  if (db.is_discarded() || db.is_null() || !db.contains("posted"))
    return false;

  auto posted = db["posted"];

  return std::find(posted.cbegin(), posted.cend(), id) != posted.cend();
}

void SavePostID(std::string id) {
  auto db = GetDatabase();

  if (db.is_discarded() || db.is_null())
    db = {"posted", {id}};
  else
    if (!db.contains("posted"))
      db["posted"] = {id};
    else
      db["posted"].emplace_back(id);

  SaveDatabase(db);
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
  nlohmann::json news_json = GetNewsJSON();
  auto string = news_json.dump();

  if (!news_json.is_null()            &&
       news_json.is_object()          &&
       news_json.contains("articles") &&
      !news_json["articles"].is_null())
  {
    for (const nlohmann::json& article : news_json["articles"])
    {
      if (!technews::AlreadySaved(article["url"]))
      {
        technews::SavePostID(article["url"]);
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
