#include "kstodon/kstodon.hpp"

namespace technews {
const std::string NAME{"technews"};

/**
 * @brief GenerateStatus
 *
 * @returns [out] {Status}
 */
Status GenerateStatus() {
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
Status ReplyToStatus(Status received_status) {
  return GenerateStatus();
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

  for (const auto& conversation : bot.FindReplies())
    bot.Reply(conversation.status);

  bot.PostStatus(); // Calling with no parameter invokes our GenerateStatus function

  return 0;
}
