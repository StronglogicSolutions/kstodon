#pragma once

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
bool PostStatus(Status status = Status{}, std::vector<File> files = std::vector<File>{}) {
  return m_client.PostStatus(
    status.is_valid() ?
      status :
      m_gen_fn_ptr(),
    files
  );
}

/**
 * @brief ReplyTostatus
 *
 * @param   [in]  {Status}      status
 * @param   [in]  {std::string} message
 * @param   [in]  {bool}        remove_id
 * @returns [out] {bool}
 */
bool ReplyToStatus(Status status, std::string message = "", bool remove_id = true)
{
  Status reply;

  if (message.empty())
    reply = m_rep_fn_ptr(status);
  else
  {
    reply = Status{};
    reply.content = message;
  }

  reply.replying_to_id = std::to_string(status.id);
  reply.content        = MakeMention(status) + reply.content;
  reply.visibility     = status.visibility;

  bool result = m_client.PostStatus(reply);

  if (remove_id && result)
    return RemoveStatusID(m_client.GetUsername(), string_to_uint64(status.replying_to_id));

  return result;
}

private:
Client           m_client;
GenerateFunction m_gen_fn_ptr;
ReplyFunction    m_rep_fn_ptr;
};

} // namespace kstodon
