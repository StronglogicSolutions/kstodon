#include "news.hpp"


namespace koreannews {
kstodon::Language g_language{kstodon::Language::korean};
namespace xml {
std::vector<XMLNewsItem> ReadRSS(std::string s)
{
  tinyxml2::XMLDocument    doc{};
  std::vector<XMLNewsItem> news_items{};
  doc.Parse(s.c_str());

  auto channel = doc.FirstChildElement("rss")->FirstChildElement("channel");

  for (tinyxml2::XMLNode* e = channel->FirstChildElement("item"); e; e = e->NextSibling())
  {
    news_items.emplace_back(
      XMLNewsItem{
        .title       = e->FirstChildElement("title")->GetText(),
        .description = e->FirstChildElement("description")->GetText(),
        .url         = e->FirstChildElement("link")->GetText()
      }
    );
  }

  return news_items;
}
} // namespace xml

/**
 * Helper Functions
 */
void        SetLanguage(kstodon::Language language) {
  g_language = language;
}

std::string GetAPIKey() {
  return kstodon::GetConfigReader().GetString(kstodon::constants::KOREAN_NEWS_SECTION, kstodon::constants::NEWSAPI_CONFIG_KEY, "");
}

std::string GetURL() {
  return "http://newsapi.org/v2/top-headlines?country=kr&apiKey=" + GetAPIKey();
}

std::string GetRSSURL() {
  return "http://www.koreaherald.com/common/rss_xml.php?ct=102";
}

Database::PSQLORM GetDatabase() {
  return Database::PSQLORM{
    DatabaseConfiguration{
      .credentials = DatabaseCredentials{
        .user     = "", // Load via config
        .password = "",
        .name     = ""
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
    kstodon::log("Could not save to database");
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
    kstodon::log(value.first + " : " + value.second);

  return !values.empty();
}


nlohmann::json GetNewsJSON() {
  kstodon::RequestResponse response{cpr::Get(
    cpr::Url{GetURL()}
  )};

  return (!response.error) ?
    response.json() : nlohmann::json{};
}

std::string GetNewsXML() {
  kstodon::RequestResponse response{cpr::Get(
    cpr::Url{GetRSSURL()}
  )};

  return (!response.error) ?
    response.text() : "";
}

std::string GetEnglishNews() {
  std::vector<xml::XMLNewsItem> news_items = xml::ReadRSS(GetNewsXML());
  for (const auto& item : news_items)
    if (!AlreadySaved(item.url))
    {
      SavePost(item.url);
      return item.to_string();
    }
  return "";
}

std::string GetKoreanNews()
{
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
 * @brief Get a trending tech news URL
 *
 * @returns [out] {std::string}
 */
std::string GetNews() {
  using namespace koreannews;

  return (koreannews::g_language == kstodon::Language::korean) ?
    GetKoreanNews() :
    GetEnglishNews();
}

} // namespace koreannews
