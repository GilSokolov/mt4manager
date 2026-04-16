#pragma once

#include <string>

class CManagerInterface;

void ThrowMt4Error(
    const char *action,
    int code,
    CManagerInterface *manager = nullptr);

bool IsMt4Success(int code);