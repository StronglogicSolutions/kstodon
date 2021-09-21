#pragma once
#include "kstodon/config.hpp"
#include "news.hpp"

namespace technews {
const std::string NAME   {"stronglogicsolutions"};
const std::string SOURCES{"hacker-news,ars-technica,"
                          "crypto-coins-news,"
                          "recode,techradar,"
                          "the-next-web,wired"};
const std::string DB_PATH{"src/applications/technews/posts.json"};
const std::string HASHTAGS{"#StronglogicSolutions #TechNews #News #Technology"};

/**
 * @brief GenerateStatus
 *
 * @returns [out] {Status}
 */
kstodon::Status GenerateStatus()
{
  kstodon::Status status{};
  status.content = GetNews();
  status.visibility = "public";

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
kstodon::Status ReplyToStatus(kstodon::Status received_status)
{
  using namespace conversation;

  std::vector<Token> tokens = SplitTokens(TokenizeText(received_status.content));

  kstodon::Status status{};

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
