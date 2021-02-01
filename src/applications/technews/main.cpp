#include "kstodon/config.hpp"

namespace technews {
const std::string NAME{"technews"};
std::string GetAPIKey() {
  return GetConfigReader().GetString(constants::TECH_NEWS_SECTION, constants::NEWSAPI_CONFIG_KEY, "");
}

std::string GetURL() {
  return "https://newsapi.org/v2/top-headlines?country=us&category=technology&apiKey=" + GetAPIKey();
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

  if (!news_json.is_null()            &&
       news_json.is_object()          &&
       news_json.contains("articles") &&
      !news_json["articles"].is_null())
  {
    return news_json["articles"].front()["url"];
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
    status.content += "\nYou mentioned:\n";
    for (const Token& token : tokens) status.content += token.value + '(' + TOKEN_TYPES.at(token.type) + ")\n";
  }
  else
    status.content += "Got a story for me?";

  return status;
}

}

/**
 * Tech News Main
 *
 * @param   [in]  {int}    argc
 * @param   [in]  {char**} argv
 * @returns [out] {int}
 */
int main(int argc, char** argv)
{
  kstodon::GenerateFunction gen_status_fn_ptr = &technews::GenerateStatus;
  kstodon::ReplyFunction    rep_status_fn_ptr = &technews::ReplyToStatus;

  kstodon::Bot bot{
    technews::NAME,
    gen_status_fn_ptr,
    rep_status_fn_ptr
  };

  kstodon::BotStats stats{};

  for (const auto& private_conversation : bot.FindReplies())
  {
    stats.rx_msg++;
    (bot.ReplyToStatus(private_conversation.status)) ?  stats.tx_msg++ : stats.tx_err++;
  }

  for (const auto& comment : bot.FindComments())
  {
    stats.rx_msg++;
    (bot.ReplyToStatus(comment)) ? stats.tx_msg++ : stats.tx_err++;
  }

  (bot.PostStatus()) ?                // Calling with no parameter invokes our GenerateStatus function
    stats.tx_msg++ : stats.tx_err++;

  log(stats.to_string());

  return 0;
}
