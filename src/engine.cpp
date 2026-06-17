#include <fstream>
#include <ctime>

#include "engine.h"

void NitipEngine::set(const std::string &key, const std::string &value)
{
    db[key] = {
        value,
        0};
    save();
}

std::string NitipEngine::get(const std::string &key)
{
    auto it = db.find(key);

    if (it == db.end())
    {
        return "{nil}";
    }

    if (
        it->second.expire_at > 0 &&
        std::time(nullptr) > it->second.expire_at)
    {
        del(key);
        return "{nil}";
    }

    return it->second.value;
}

void NitipEngine::expire(const std::string &key, int seconds)
{
    auto it = db.find(key);

    if (it == db.end())
    {
        return;
    }

    it->second.expire_at = std::time(nullptr) + seconds;
    save();
}

void NitipEngine::del(const std::string &key)
{
    db.erase(key);
    save();
}

void NitipEngine::save()
{
    std::ofstream file("nitip.db");

    for (const auto &pair : db)
    {
        file << pair.first << "=" << pair.second.value << "|" << pair.second.expire_at << "\n";
    }
}

void NitipEngine::load()
{
    std::ifstream file("nitip.db");
    std::string line;

    while (std::getline(file, line))
    {
        size_t eqPos = line.find("=");
        size_t pipePos = line.find("|");

        if (eqPos == std::string::npos || pipePos == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1, pipePos - eqPos - 1);
        long long expireAt = std::stoi(line.substr(pipePos + 1));

        if (expireAt > 0 && std::time(nullptr) > expireAt)
        {
            continue;
        }

        db[key] = {
            value,
            expireAt};
    }
}