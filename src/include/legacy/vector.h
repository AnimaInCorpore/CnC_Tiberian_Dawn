#pragma once

/*
**	The legacy code expects a header named "vector.h" that provides the
**	VectorClass/DynamicVectorClass templates. The modernized implementations
**	live in src/VECTOR.h; include that here so every legacy include picks up
**	the new definitions (with inline method bodies) and avoids missing
**	template symbols at link time.
*/
#include "../../VECTOR.h"
