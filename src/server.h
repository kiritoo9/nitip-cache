#pragma once

#include "engine.h"

class NitipServer
{
public:
    void start(int port);
    void cli();

private:
    NitipEngine nitip;
};