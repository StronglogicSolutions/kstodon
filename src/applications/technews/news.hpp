#include "kstodon/config.hpp"
#include "psqlorm.hpp"
#include "tinyxml2.h"

namespace technews {
extern kstodon::Language g_language;
/**
 * Helper Functions
 */
void              SetLanguage(kstodon::Language language);
std::string       GetAPIKey();
std::string       GetURL();
std::string       GetRSSURL();
Database::PSQLORM GetDatabase();
void              SavePost(const std::string& url);
bool              AlreadySaved(std::string url);
nlohmann::json    GetNewsJSON();
std::string       GetFormattedNews();

/**
 * @brief Get a trending tech news URL
 *
 * @returns [out] {std::string}
 */
std::string GetNews();

} // namespace technews
