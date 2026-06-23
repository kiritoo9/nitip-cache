#include "server.h"
#include "auth.h"
#include <iostream>
#include <string>

void printUsage(const char *prog)
{
    std::cout << "Nitip Cache Server v0.0.11\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << prog << "                              Start server (port 6379)\n";
    std::cout << "  " << prog << " <port>                        Start server (custom port)\n";
    std::cout << "  " << prog << " gen-token <tenant> [--ttl <s>]  Generate auth token\n";
    std::cout << "  " << prog << " revoke-token <tenant>           Revoke all tokens for tenant\n";
    std::cout << "  " << prog << " list-tokens                     List active tokens\n";
    std::cout << "  " << prog << " -h, --help                      Show this help\n";
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help")
        {
            printUsage(argv[0]);
            return 0;
        }
        if (arg == "list-tokens")
        {
            AuthManager::listTokens("nitip.auth");
            return 0;
        }
        if (arg == "gen-token" || arg == "revoke-token")
        {
            std::cerr << "Usage: " << argv[0] << " " << arg << " <tenant_id>\n";
            return 1;
        }
    }

    if (argc >= 3)
    {
        std::string cmd = argv[1];

        if (cmd == "gen-token")
        {
            std::string tenant = argv[2];
            long long ttl = 0;

            for (int i = 3; i < argc - 1; ++i)
            {
                if (std::string(argv[i]) == "--ttl")
                {
                    try { ttl = std::stoll(argv[i + 1]); } catch (...) { ttl = 0; }
                    i++;
                }
            }

            std::string token = AuthManager::generateToken(tenant, "nitip.auth", ttl);
            std::cout << "Token for tenant '" << tenant << "':\n";
            std::cout << token << "\n\n";
            std::cout << "Save this token! It will not be shown again.\n";
            std::cout << "Token stored in: nitip.auth\n";
            return 0;
        }

        if (cmd == "revoke-token")
        {
            std::string tenant = argv[2];
            AuthManager mgr;
            mgr.load("nitip.auth");
            int revoked = mgr.revokeTokens(tenant, "nitip.auth");
            std::cout << "Revoked " << revoked << " token(s) for tenant '" << tenant << "'\n";
            return 0;
        }
    }

    int port = 6379;
    if (argc == 2)
        port = std::stoi(argv[1]);

    NitipServer server;
    server.start(port);

    return 0;
}
