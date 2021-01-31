#include "kstodon/config.hpp"

namespace technews {
const std::string NAME{"technews"};

/**
 * @brief GenerateStatus
 *
 * @returns [out] {Status}
 */
Status GenerateStatus()
{
  Status status{};
  status.content = "Excited to bring you news";

  return status;
}

/**
 * @brief ReplyToStatus
 *
 * // TODO: Perform analysis using KNLP
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
    status.content += "You mentioned: ";
    for (const Token& token : tokens) status.content += token.value + '(' + TOKEN_TYPES.at(token.type) + '\n';
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

  for (const auto& conversation : bot.FindReplies())
  {
    stats.rx_msg++;
    (bot.ReplyToStatus(conversation.status)) ?  stats.tx_msg++ : stats.tx_err++;
  }

  // (bot.PostStatus()) ?                // Calling with no parameter invokes our GenerateStatus function
  //   stats.tx_msg++ : stats.tx_err++;

  log(stats.to_string());

  return 0;
}
