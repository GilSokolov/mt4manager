#pragma once

#include <napi.h>
#include "pumping_options.h"

PumpingOptions ParsePumpingOptions(const Napi::Value &value);