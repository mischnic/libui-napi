#ifndef LIBUI_NODE_VALUES_H__
#define LIBUI_NODE_VALUES_H__

#include "napi_utils.h"

napi_value make_size(napi_env env, uint32_t width, uint32_t height);
napi_value make_color(napi_env env, double r, double g, double b, double a);
#endif
