#include "news.hpp"


namespace technews {
kstodon::Language g_language{kstodon::Language::english};
static const std::string NAME   {"stronglogicsolutions"};
static const std::string SOURCES{"hacker-news,ars-technica,"
                          "crypto-coins-news,"
                          "recode,techradar,"
                          "the-next-web,wired"};
static const std::string DB_PATH{"src/applications/technews/posts.json"};
static const std::string DB_NAME{"posts"};
static const std::string HASHTAGS{"#StronglogicSolutions #TechNews #News #Technology"};
/**
 * Helper Functions
 */
void        SetLanguage(kstodon::Language language) {
  g_language = language;
}

std::string GetAPIKey() {
  return kstodon::GetConfigReader().GetString(kstodon::constants::TECH_NEWS_SECTION, kstodon::constants::NEWSAPI_CONFIG_KEY, "");
}

std::string GetURL() {
  return "https://newsapi.org/v2/top-headlines?sources=" + SOURCES + "&apiKey=" + GetAPIKey();
}

Database::PSQLORM GetDatabase() {
  return Database::PSQLORM{
    DatabaseConfiguration{
      .credentials = DatabaseCredentials{
        .user     = "", // TODO: Load via config
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

  try {
    PSQLORM db = GetDatabase();

    QueryValues values = db.select(
      technews::DB_NAME,
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
  catch (const std::exception& e)
  {
    kstodon::log(e.what());
    return false;
  }
}


nlohmann::json GetNewsJSON() {
  kstodon::RequestResponse response{cpr::Get(
    cpr::Url{GetURL()}
  )};

  return (!response.error) ?
    response.json() : nlohmann::json{};
}

std::string GetFormattedNews()
{
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
        technews::SavePost(article["url"]);
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
  return GetFormattedNews();
}

} // namespace technews
