#pragma once

#include "engine.h"
#include "auth.h"

class NitipServer
{
public:
    void start(int port);
    void cli();

private:
    NitipEngine nitip;
    AuthManager auth;
    void handleClient(int client_fd);
    std::string prefixKey(const std::string &tenant, const std::string &key);
};
