#ifndef SIMDJSON_WESTMERE_H
#define SIMDJSON_WESTMERE_H

#include "simdjson/portability.h"

#if SIMDJSON_IMPLEMENTATION_WESTMERE

#define SIMDJSON_TARGET_WESTMERE SIMDJSON_TARGET_REGION("sse4.2,pclmul")

//
// These two need to be included outside SIMDJSON_TARGET_REGION
//
#include "simdjson/westmere/implementation.h"
#include "simdjson/westmere/intrinsics.h"

//
// The rest need to be inside the region
//
#include "simdjson/westmere/begin.h"

// Declarations
#include "simdjson/generic/dom_parser_implementation.h"
#include "simdjson/westmere/bitmanipulation.h"
#include "simdjson/westmere/bitmask.h"
#include "simdjson/westmere/simd.h"
#include "simdjson/generic/jsoncharutils.h"
#include "simdjson/generic/atomparsing.h"
#include "simdjson/westmere/stringparsing.h"
#include "simdjson/westmere/numberparsing.h"
#include "simdjson/generic/implementation_simdjson_result_base.h"
#include "simdjson/generic/ondemand.h"

// Inline definitions
#include "simdjson/generic/implementation_simdjson_result_base-inl.h"
#include "simdjson/generic/ondemand-inl.h"

#include "simdjson/westmere/end.h"

#endif // SIMDJSON_IMPLEMENTATION_WESTMERE
#endif // SIMDJSON_WESTMERE_COMMON_H
