#pragma once

#include <string>
#include <unordered_map>

struct Entry
{
    std::string value;
    long long expire_at;
};

class NitipEngine
{
public:
    void set(const std::string &key, const std::string &value);
    std::string get(const std::string &key);
    void del(const std::string &key);
    void expire(const std::string &key, int seconds);

    void save();
    void load();

private:
    std::unordered_map<std::string, Entry> db;
};