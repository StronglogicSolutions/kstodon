#ifndef __BOT_HPP__
#define __BOT_HPP__

#include "kstodon/client/client.hpp"

namespace kstodon {

using GenerateFunction = Status(*)();
using ReplyFunction    = Status(*)(Status status);

class Bot : public ConversationTracker {

public:
Bot(const std::string& username = "",
    GenerateFunction gen_fn_ptr = nullptr,
    ReplyFunction    rep_fn_ptr = nullptr)
: m_gen_fn_ptr(gen_fn_ptr),
  m_rep_fn_ptr(rep_fn_ptr),
  m_client(Client{
    username
  })
{
  if (!m_client.HasAuth()) {
    throw std::invalid_argument{"Client was not able to authenticate"};
  }
}

std::vector<Conversation> FindReplies() override {
  std::vector<uint64_t> status_ids = GetSavedStatusIDs(m_client.GetUsername());
  std::vector<Conversation> conversations;
  std::vector<Conversation> replies{};

  try {
    conversations = m_client.FetchConversations();
  }
  catch (const request_error& e) {
    log(e.what());
  }

  return ParseRepliesFromConversations(conversations, status_ids);
}

/**
 * @brief
 *
 * @param   [in]  {Status}            status
 * @param   [in]  {std::vector<File>} files
 * @returns [out] {bool}
 */
bool PostStatus(Status status, std::vector<File> files) {
  return m_client.PostStatus(status, files);
}

/**
 * @brief ReplyTostatus
 *
 * @param   [in]  {Status}      status
 * @param   [in]  {std::string} message
 * @param   [in]  {bool}        remove_id
 * @returns [out] {bool}
 */
bool ReplyToStatus(Status status, std::string message = constants::DEFAULT_STATUS_MSG, bool remove_id = true) {
  Status placeholder_response{};
  placeholder_response.replying_to_id = std::to_string(status.id);
  placeholder_response.content        = MakeMention(status) + message;
  placeholder_response.visibility     = status.visibility;

  bool result = m_client.PostStatus(placeholder_response);

  if (remove_id && result) {
    return RemoveStatusID(m_client.GetUsername(), string_to_uint64(status.replying_to_id));
  }

  return result;
}

private:
Client           m_client;
GenerateFunction m_gen_fn_ptr;
ReplyFunction    m_rep_fn_ptr;
};

} // namespace kstodon

#endif // __BOT_HPP__
