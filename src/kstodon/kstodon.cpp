#include "kstodon.hpp"

namespace kstodon {
Client::Client()
: m_authenticator(Authenticator{}) {
  if (!m_authenticator.HasValidToken()) {
    m_authenticator.FetchToken();
  }

  if (!m_authenticator.HasValidToken())
    throw std::invalid_argument{"Client was unable to authenticate."};
}

bool Client::has_auth() {
  return m_authenticator.IsAuthenticated();
}


} // namespace kstodon

