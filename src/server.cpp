#include "server.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <vector>

void NitipServer::start(int port)
{
    nitip.load();

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(
        server_fd,
        (sockaddr *)&address,
        sizeof(address));

    listen(server_fd, 1);

    std::cout << "Nitip is listening on port :" << port << std::endl;

    int client_fd = accept(server_fd, nullptr, nullptr);

    std::cout << "Client connected" << std::endl;

    char buffer[1024];

    while (true)
    {
        int bytes = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes <= 0)
        {
            break;
        }
        buffer[bytes] = '\0';

        std::string line(buffer);
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
        {
            line.pop_back();
        }
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (ss >> token)
        {
            tokens.push_back(token);
        }

        // prepare for response
        std::string response;
        if (tokens.empty())
        {
            response = "ERR\n";
        }
        else if (tokens[0] == "set")
        {
            nitip.set(tokens[1], tokens[2]);
            response = "OK\n";
        }
        else if (tokens[0] == "get")
        {
            response = nitip.get(tokens[1]) + "\n";
        }
        else if (tokens[0] == "del")
        {
            nitip.del(tokens[1]);
            response = "OK\n";
        }
        else if (tokens[0] == "expire")
        {
            nitip.expire(tokens[1], std::stoi(tokens[2]));
            response = "OK\n";
        }
        else
        {
            response = "UNKNOWN COMMAND\n";
        }

        write(
            client_fd,
            response.c_str(),
            response.size());
    }

    close(client_fd);
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
        {
            tokens.push_back(token);
        }

        if (tokens.empty())
        {
            continue;
        }

        if (tokens[0] == "set")
        {
            nitip.set(tokens[1], tokens[2]);
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
        else if (tokens[0] == "exit")
        {
            break;
        }
        else
        {
            std::cout << "Write 'exit' to close app\n";
        }
    };
}