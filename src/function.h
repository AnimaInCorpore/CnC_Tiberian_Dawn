#pragma once

/*
 * Legacy "function.h" was the umbrella include for most Win32/Watcom-era
 * modules. For the portable build we keep it as a thin wrapper that pulls in
 * the current compatibility layer and standard headers needed by the ports.
 */

#include "legacy_compat.h"

#include <cstdarg>
