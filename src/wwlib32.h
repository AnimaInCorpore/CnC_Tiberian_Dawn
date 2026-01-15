/* Minimal shim for wwlib32.h used by legacy sources.
 * This file intentionally provides lightweight stubs for types and
 * declarations expected by the port while the full wwlib is not
 * yet available. Expand as compile errors expose missing symbols.
 */
#pragma once

#include <cstddef>

class BufferClass {};

/* Common macros that legacy code might expect. */
#ifndef FAR
#define FAR
#endif
