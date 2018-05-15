#include <ui.h>
#include "napi_utils.h"
#include "control.h"
#include "events.h"

static const char *MODULE = "AreaBrush";

static void free_brush_solid(napi_env env, void *finalize_data, void *finalize_hint) {
	uiDrawBrush *brush = (uiDrawBrush *)finalize_data;
	free(brush);
}

napi_ref AreaDrawBrushGradient;

LIBUI_FUNCTION(createSolid) {
	INIT_ARGS(4);

	ARG_DOUBLE(r, 0);
	ARG_DOUBLE(g, 1);
	ARG_DOUBLE(b, 2);
	ARG_DOUBLE(a, 3);

	uiDrawBrush *brush = malloc(sizeof(uiDrawBrush));

	brush->R = r;
	brush->G = g;
	brush->B = b;
	brush->A = a;

	brush->Type = uiDrawBrushTypeSolid;

	napi_value external;
	napi_status status = napi_create_external(env, brush, free_brush_solid, NULL, &external);
	CHECK_STATUS_THROW(status, napi_create_external);

	return external;
}

static void free_brush_gradient(napi_env env, void *finalize_data, void *finalize_hint) {
	uiDrawBrush *brush = (uiDrawBrush *)finalize_data;
	if (brush->NumStops > 0) {
		free(brush->Stops);
	}
	free(brush);
}

LIBUI_FUNCTION(createGradient) {
	INIT_ARGS(1);

	ARG_INT32(type, 0);

	uiDrawBrush *brush = calloc(1, sizeof(uiDrawBrush));

	brush->Type = type;

	napi_value external;
	napi_status status = napi_create_external(env, brush, free_brush_gradient, NULL, &external);
	CHECK_STATUS_THROW(status, napi_create_external);

	return external;
}

LIBUI_FUNCTION(setStart) {
	INIT_ARGS(3);

	ARG_POINTER(uiDrawBrush, brush, 0);
	ARG_DOUBLE(x, 1);
	ARG_DOUBLE(y, 2);

	brush->X0 = x;
	brush->Y0 = y;

	return NULL;
}

LIBUI_FUNCTION(getStart) {
	INIT_ARGS(1);

	ARG_POINTER(uiDrawBrush, brush, 0);

	napi_value result;

	napi_handle_scope handle_scope;
	napi_status status = napi_open_handle_scope(env, &handle_scope);
	CHECK_STATUS_THROW(status, napi_open_handle_scope);

	napi_create_object(env, &result);
	CHECK_STATUS_THROW(status, napi_create_object);

	napi_set_named_property(env, result, "x", make_double(env, brush->X0));
	CHECK_STATUS_THROW(status, napi_set_named_property);
	napi_set_named_property(env, result, "y", make_double(env, brush->Y0));
	CHECK_STATUS_THROW(status, napi_set_named_property);

	status = napi_close_handle_scope(env, handle_scope);
	CHECK_STATUS_THROW(status, napi_close_handle_scope);

	return result;
}

LIBUI_FUNCTION(setEnd) {
	INIT_ARGS(3);

	ARG_POINTER(uiDrawBrush, brush, 0);
	ARG_DOUBLE(x, 1);
	ARG_DOUBLE(y, 2);

	brush->X1 = x;
	brush->Y1 = y;

	return NULL;
}

LIBUI_FUNCTION(getEnd) {
	INIT_ARGS(1);

	ARG_POINTER(uiDrawBrush, brush, 0);

	napi_value result;

	napi_handle_scope handle_scope;
	napi_status status = napi_open_handle_scope(env, &handle_scope);
	CHECK_STATUS_THROW(status, napi_open_handle_scope);

	napi_create_object(env, &result);
	CHECK_STATUS_THROW(status, napi_create_object);

	napi_set_named_property(env, result, "x", make_double(env, brush->X1));
	CHECK_STATUS_THROW(status, napi_set_named_property);
	napi_set_named_property(env, result, "y", make_double(env, brush->Y1));
	CHECK_STATUS_THROW(status, napi_set_named_property);

	status = napi_close_handle_scope(env, handle_scope);
	CHECK_STATUS_THROW(status, napi_close_handle_scope);

	return result;
}

LIBUI_FUNCTION(setOuterRadius) {
	INIT_ARGS(2);

	ARG_POINTER(uiDrawBrush, brush, 0);
	ARG_DOUBLE(r, 1);

	brush->OuterRadius = r;

	return NULL;
}

LIBUI_FUNCTION(getOuterRadius) {
	INIT_ARGS(1);

	ARG_POINTER(uiDrawBrush, brush, 0);

	return make_double(env, brush->OuterRadius);
}

LIBUI_FUNCTION(setStops) {
	INIT_ARGS(2);

	ARG_POINTER(uiDrawBrush, brush, 0);
	napi_value array = argv[1];

	uint32_t numStops;
	napi_status status = napi_get_array_length(env, array, &numStops);
	CHECK_STATUS_THROW(status, napi_array_elgnth);

	if (brush->NumStops > 0) {
		free(brush->Stops);
	}

	brush->NumStops = numStops;
	brush->Stops = malloc(numStops * sizeof(uiDrawBrushGradientStop));

	for (uint32_t i = 0; i < numStops; i++) {
		napi_value v;
		status = napi_get_element(env, array, i, &v);
		CHECK_STATUS_THROW(status, napi_get_element);

		uiDrawBrushGradientStop *s;
		status = napi_get_value_external(env, v, (void **)&s);
		CHECK_STATUS_THROW(status, napi_get_value_external);

		brush->Stops[i].Pos = s->Pos;
		brush->Stops[i].R = s->R;
		brush->Stops[i].G = s->G;
		brush->Stops[i].B = s->B;
		brush->Stops[i].A = s->A;
	}

	return NULL;
}

LIBUI_FUNCTION(getStops) {
	INIT_ARGS(1);

	ARG_POINTER(uiDrawBrush, brush, 0);

	napi_value constructor, array;

	napi_status status = napi_get_reference_value(env, AreaDrawBrushGradient, &constructor);
	CHECK_STATUS_THROW(status, napi_get_reference_value);

	status = napi_create_array_with_length(env, brush->NumStops, &array);
	CHECK_STATUS_THROW(status, napi_create_array_with_length);

	napi_value args[5];

	for (uint32_t i = 0; i < brush->NumStops; i++) {
		napi_value v;
		args[0] = make_double(env, brush->Stops[i].Pos);
		args[1] = make_double(env, brush->Stops[i].R);
		args[2] = make_double(env, brush->Stops[i].G);
		args[3] = make_double(env, brush->Stops[i].B);
		args[4] = make_double(env, brush->Stops[i].A);
		status = napi_new_instance(env, constructor, 5, args, &v);
		CHECK_STATUS_THROW(status, napi_new_instance);

		status = napi_set_element(env, array, i, v);
		CHECK_STATUS_THROW(status, napi_set_element);
	}

	return array;
}

static void free_brush_gradient_stop(napi_env env, void *finalize_data, void *finalize_hint) {
	uiDrawBrushGradientStop *stop = (uiDrawBrushGradientStop *)finalize_data;
	free(stop);
}

LIBUI_FUNCTION(stop_create) {
	INIT_ARGS(5);

	ARG_DOUBLE(pos, 0);
	ARG_DOUBLE(r, 1);
	ARG_DOUBLE(g, 2);
	ARG_DOUBLE(b, 3);
	ARG_DOUBLE(a, 4);

	uiDrawBrushGradientStop *stop = malloc(sizeof(uiDrawBrushGradientStop));

	stop->Pos = pos;
	stop->R = r;
	stop->G = g;
	stop->B = b;
	stop->A = a;

	napi_value external;
	napi_status status = napi_create_external(env, stop, free_brush_gradient_stop, NULL, &external);
	CHECK_STATUS_THROW(status, napi_create_external);

	return external;
}

LIBUI_FUNCTION(stop_setPos) {
	INIT_ARGS(2);

	ARG_POINTER(uiDrawBrushGradientStop, stop, 0);
	ARG_DOUBLE(p, 1);

	stop->Pos = p;

	return NULL;
}

LIBUI_FUNCTION(stop_getPos) {
	INIT_ARGS(1);

	ARG_POINTER(uiDrawBrushGradientStop, stop, 0);

	return make_double(env, stop->Pos);
}

LIBUI_FUNCTION(stop_setColor) {
	INIT_ARGS(5);

	ARG_POINTER(uiDrawBrushGradientStop, stop, 0);
	ARG_DOUBLE(r, 1);
	ARG_DOUBLE(g, 2);
	ARG_DOUBLE(b, 3);
	ARG_DOUBLE(a, 4);

	stop->R = r;
	stop->G = g;
	stop->B = b;
	stop->A = a;

	return NULL;
}

LIBUI_FUNCTION(stop_getColor) {
	INIT_ARGS(1);

	ARG_POINTER(uiDrawBrushGradientStop, stop, 0);

	napi_value result;

	napi_handle_scope handle_scope;
	napi_status status = napi_open_handle_scope(env, &handle_scope);
	CHECK_STATUS_THROW(status, napi_open_handle_scope);

	napi_create_object(env, &result);
	CHECK_STATUS_THROW(status, napi_create_object);

	napi_set_named_property(env, result, "r", make_double(env, stop->R));
	CHECK_STATUS_THROW(status, napi_set_named_property);
	napi_set_named_property(env, result, "g", make_double(env, stop->G));
	CHECK_STATUS_THROW(status, napi_set_named_property);
	napi_set_named_property(env, result, "b", make_double(env, stop->B));
	CHECK_STATUS_THROW(status, napi_set_named_property);
	napi_set_named_property(env, result, "a", make_double(env, stop->A));
	CHECK_STATUS_THROW(status, napi_set_named_property);

	status = napi_close_handle_scope(env, handle_scope);
	CHECK_STATUS_THROW(status, napi_close_handle_scope);

	return result;
}

LIBUI_FUNCTION(init) {
	INIT_ARGS(1);

	ARG_CB_REF(gradient, 0);
	AreaDrawBrushGradient = gradient;

	return NULL;
}

napi_value _libui_init_area_brush(napi_env env, napi_value exports) {
	DEFINE_MODULE();
	LIBUI_EXPORT(createSolid);
	LIBUI_EXPORT(createGradient);
	LIBUI_EXPORT(setStart);
	LIBUI_EXPORT(getStart);
	LIBUI_EXPORT(setEnd);
	LIBUI_EXPORT(getEnd);
	LIBUI_EXPORT(setOuterRadius);
	LIBUI_EXPORT(getOuterRadius);
	LIBUI_EXPORT(setStops);
	LIBUI_EXPORT(getStops);
	LIBUI_EXPORT(stop_create);
	LIBUI_EXPORT(stop_setPos);
	LIBUI_EXPORT(stop_getPos);
	LIBUI_EXPORT(stop_setColor);
	LIBUI_EXPORT(stop_getColor);
	LIBUI_EXPORT(init);

	return module;
}