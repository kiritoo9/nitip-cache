#include "server.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include <iostream>
#include <sstream>
#include <vector>

std::string NitipServer::prefixKey(const std::string &tenant, const std::string &key)
{
    return tenant + ":" + key;
}

void NitipServer::handleClient(int client_fd)
{
    char buffer[4096];
    std::string currentTenant;
    bool authenticated = false;

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes <= 0)
            break;

        std::string raw(buffer);

        std::stringstream ss(raw);
        std::string firstLine;
        std::getline(ss, firstLine);
        if (!firstLine.empty() && (firstLine.back() == '\r' || firstLine.back() == '\n'))
            firstLine.pop_back();

        if (firstLine.empty())
            continue;

        std::stringstream lineSS(firstLine);
        std::vector<std::string> tokens;
        std::string token;
        while (lineSS >> token)
            tokens.push_back(token);

        std::string response;

        if (tokens.empty())
        {
            response = "ERR\r\n";
        }
        else if (tokens[0] == "auth")
        {
            if (tokens.size() < 2)
            {
                response = "ERR usage: AUTH <token>\r\n";
            }
            else
            {
                std::string tenant = auth.authenticate(tokens[1]);
                if (!tenant.empty())
                {
                    currentTenant = tenant;
                    authenticated = true;
                    Logger::info("Tenant '" + tenant + "' authenticated from " +
                                 std::to_string(client_fd));
                    response = "+OK\r\n";
                }
                else
                {
                    Logger::warn("Auth failed for token from " +
                                 std::to_string(client_fd));
                    response = "-ERR invalid token\r\n";
                }
            }
        }
        else if (!authenticated)
        {
            response = "-ERR not authenticated\r\n";
        }
        else if (tokens[0] == "set")
        {
            if (tokens.size() >= 3)
            {
                std::string value = tokens[2];
                for (size_t i = 3; i < tokens.size(); ++i)
                    value += " " + tokens[i];
                std::string fullKey = prefixKey(currentTenant, tokens[1]);
                nitip.set(fullKey, value);
                response = "+OK\r\n";
            }
            else
            {
                response = "ERR usage: SET key value\r\n";
            }
        }
        else if (tokens[0] == "get")
        {
            std::string fullKey = prefixKey(currentTenant, tokens[1]);
            response = nitip.get(fullKey) + "\r\n";
        }
        else if (tokens[0] == "del")
        {
            std::string fullKey = prefixKey(currentTenant, tokens[1]);
            nitip.del(fullKey);
            response = "+OK\r\n";
        }
        else if (tokens[0] == "expire")
        {
            std::string fullKey = prefixKey(currentTenant, tokens[1]);
            nitip.expire(fullKey, std::stoi(tokens[2]));
            response = "+OK\r\n";
        }
        else if (tokens[0] == "info")
        {
            response = nitip.info() + "\r\n";
        }
        else if (tokens[0] == "whoami")
        {
            response = currentTenant + "\r\n";
        }
        else if (tokens[0] == "who")
        {
            response = currentTenant + "\r\n";
        }
        else
        {
            response = "ERR unknown command\r\n";
        }

        write(client_fd, response.c_str(), response.size());
    }

    Logger::info("Client disconnected");
    close(client_fd);
}

void NitipServer::start(int port)
{
    nitip.load();
    auth.load("nitip.auth");

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        Logger::error("Failed to create socket");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (sockaddr *)&address, sizeof(address)) < 0)
    {
        Logger::error("Failed to bind to port " + std::to_string(port));
        return;
    }

    if (listen(server_fd, SOMAXCONN) < 0)
    {
        Logger::error("Failed to listen on port " + std::to_string(port));
        return;
    }

    Logger::info("Nitip server started on port " + std::to_string(port));

    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int client_fd = accept(server_fd, (sockaddr *)&clientAddr, &clientLen);

        if (client_fd < 0)
        {
            Logger::error("Failed to accept connection");
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        Logger::info("Client connected from " + std::string(clientIP));

        handleClient(client_fd);
    }

    close(server_fd);
}

void NitipServer::cli()
{
    NitipEngine nitip;
    nitip.load();

    std::string line;

    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, line);

        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (ss >> token)
            tokens.push_back(token);

        if (tokens.empty())
            continue;

        if (tokens[0] == "set")
        {
            std::string value = tokens[2];
            for (size_t i = 3; i < tokens.size(); ++i)
                value += " " + tokens[i];
            nitip.set(tokens[1], value);
            std::cout << "OK\n";
        }
        else if (tokens[0] == "get")
        {
            std::cout << nitip.get(tokens[1]) << "\n";
        }
        else if (tokens[0] == "del")
        {
            nitip.del(tokens[1]);
            std::cout << "OK\n";
        }
        else if (tokens[0] == "expire")
        {
            nitip.expire(tokens[1], std::stoi(tokens[2]));
            std::cout << "OK\n";
        }
        else if (tokens[0] == "info")
        {
            std::cout << nitip.info() << "\n";
        }
        else if (tokens[0] == "exit")
        {
            break;
        }
        else
        {
            std::cout << "Commands: set key value, get key, del key, expire key seconds, info, exit\n";
        }
    };
}
