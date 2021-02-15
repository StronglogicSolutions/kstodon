#include "kstodon/config.hpp"
#include "psqlorm.hpp"
#include "constants.hpp"
#include "tinyxml2.h"

namespace koreannews {
extern kstodon::Language g_language;
namespace xml {
struct XMLNewsItem
{
std::string title;
std::string description;
std::string url;

const std::string to_string() const
{
  return std::string{
    koreannews::HASHTAGS + '\n' +
    url                  + '\n' +
    kstodon::SanitizeOutput(title)                + '\n' +
    description
  };
}
};

std::vector<XMLNewsItem> ReadRSS(std::string s);
} // namespace xml

/**
 * Helper Functions
 */

void        SetLanguage(kstodon::Language language);

std::string GetAPIKey();

std::string GetURL();

std::string GetRSSURL();

Database::PSQLORM GetDatabase();

void SavePost(const std::string& url);

bool AlreadySaved(std::string url);


nlohmann::json GetNewsJSON();

std::string GetNewsXML();

std::string GetNewsRSS();

std::string GetKoreanNews();

std::string GetEnglishNews();

/**
 * @brief Get a trending tech news URL
 *
 * @returns [out] {std::string}
 */
std::string GetNews();

} // namespace koreannews
