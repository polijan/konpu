#ifndef  BACKEND_SOKOL_PLATFORM_H_
#define  BACKEND_SOKOL_PLATFORM_H_

#include "backend-common/backend.h"

// Macros to control Soko headers
#define  SOKOL_UNREACHABLE()  unreachable()

// Sokol Headers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#  include "sokol/sokol_app.h"
#  include "sokol/sokol_log.h"
#pragma GCC diagnostic pop

#endif //include guard