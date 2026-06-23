#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <random>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "logger.h"

class AuthManager
{
public:
    bool load(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            Logger::warn("Auth file not found: " + path + " (multi-tenant disabled)");
            return false;
        }

        std::string line;
        int count = 0;
        std::time_t now = std::time(nullptr);

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;
            std::vector<std::string> parts = split(line, ':');
            if (parts.size() < 2)
                continue;

            std::string tenant = parts[0];
            std::string token = parts[1];
            std::time_t expiresAt = 0;

            if (parts.size() >= 3)
            {
                try
                {
                    expiresAt = std::stoll(parts[2]);
                }
                catch (...)
                {
                    expiresAt = 0;
                }
            }

            if (!tenant.empty() && !token.empty())
            {
                if (expiresAt > 0 && now > expiresAt)
                {
                    Logger::info("Token expired for tenant: " + tenant);
                    continue;
                }
                tokenToTenant[token] = tenant;
                tokenExpires[token] = expiresAt;
                count++;
            }
        }
        authPath = path;
        Logger::info("Loaded " + std::to_string(count) + " auth token(s)");
        return true;
    }

    std::string authenticate(const std::string &token) const
    {
        auto it = tokenToTenant.find(token);
        if (it == tokenToTenant.end())
            return "";

        auto expIt = tokenExpires.find(token);
        if (expIt != tokenExpires.end() && expIt->second > 0)
        {
            if (std::time(nullptr) > expIt->second)
            {
                Logger::info("Token expired for tenant: " + it->second);
                return "";
            }
        }
        return it->second;
    }

    std::string getTenant(const std::string &token) const
    {
        auto it = tokenToTenant.find(token);
        return (it != tokenToTenant.end()) ? it->second : "";
    }

    static std::string generateToken(const std::string &tenantId, const std::string &path, long long ttlSeconds = 0)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        std::stringstream hex;
        hex << "nitip_";
        for (int i = 0; i < 32; ++i)
        {
            hex << std::hex << std::setfill('0') << std::setw(2) << dis(gen);
        }
        std::string token = hex.str();

        std::time_t expiresAt = 0;
        if (ttlSeconds > 0)
            expiresAt = std::time(nullptr) + ttlSeconds;

        std::ofstream file(path, std::ios::app);
        if (file.is_open())
        {
            file << tenantId << ":" << token << ":" << expiresAt << "\n";
            file.close();
            Logger::info("Token generated for tenant: " + tenantId +
                         (ttlSeconds > 0 ? " (expires in " + std::to_string(ttlSeconds) + "s)" : " (no expiry)"));
        }
        else
        {
            Logger::error("Failed to write auth file: " + path);
        }

        return token;
    }

    int revokeTokens(const std::string &tenantId, const std::string &path)
    {
        std::vector<std::string> remaining;
        std::ifstream file(path);
        std::string line;
        int revoked = 0;

        if (file.is_open())
        {
            while (std::getline(file, line))
            {
                if (line.empty() || line[0] == '#')
                {
                    remaining.push_back(line);
                    continue;
                }
                std::vector<std::string> parts = split(line, ':');
                if (parts.size() >= 1 && parts[0] == tenantId)
                {
                    revoked++;
                    if (parts.size() >= 2)
                        Logger::info("Revoked token for tenant: " + tenantId);
                    continue;
                }
                remaining.push_back(line);
            }
            file.close();
        }

        std::ofstream out(path);
        if (out.is_open())
        {
            for (const auto &l : remaining)
                out << l << "\n";
            out.close();
        }

        Logger::info("Revoked " + std::to_string(revoked) + " token(s) for tenant: " + tenantId);
        return revoked;
    }

    static void listTokens(const std::string &path)
    {
        std::ifstream file(path);
        std::string line;
        std::time_t now = std::time(nullptr);
        int count = 0;

        std::cout << "Active tokens:\n";
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;
            std::vector<std::string> parts = split(line, ':');
            if (parts.size() < 2)
                continue;

            std::string tenant = parts[0];
            std::string token = parts[1];
            std::time_t expiresAt = 0;

            if (parts.size() >= 3)
            {
                try { expiresAt = std::stoll(parts[2]); } catch (...) { expiresAt = 0; }
            }

            if (expiresAt > 0 && now > expiresAt)
                continue;

            std::cout << "  Tenant: " << tenant << "\n";
            std::cout << "  Token:  " << token << "\n";
            if (expiresAt > 0)
                std::cout << "  Expires: " << expiresAt << " (" << (expiresAt - now) << "s remaining)\n";
            else
                std::cout << "  Expires: never\n";
            std::cout << "\n";
            count++;
        }
        std::cout << "Total: " << count << " active token(s)\n";
    }

private:
    std::string authPath;
    std::unordered_map<std::string, std::string> tokenToTenant;
    std::unordered_map<std::string, std::time_t> tokenExpires;

    static std::vector<std::string> split(const std::string &s, char delim)
    {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim))
            result.push_back(item);
        return result;
    }
};
