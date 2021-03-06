#include "control.h"
#include <assert.h>
#include "napi_utils.h"
#include "events.h"

struct ctrl_map controls_map;

#if UI_NODE_DEBUG
static const char *MODULE = "ControlInternal";
#endif

int control_event_cb(void *ctrl, void *data) {
	struct event_t *event = (struct event_t *)data;
	fire_event(event);
	return 0;
}

void control_on_destroy(uiControl *control) {
	LIBUI_NODE_DEBUG("A control is destroying.");

	struct control_handle *handle;
	ctrl_map_get(&controls_map, control, &handle);
	LIBUI_NODE_DEBUG_F("Control %s %p destroying.", handle->ctrl_type_name, handle);

	handle->original_destroy(control);
	ctrl_map_remove(&controls_map, control);
	clear_all_events(handle->events);

	clear_children(handle->env, handle->children);

	LIBUI_NODE_DEBUG_F("Control %s %p destroyed.", handle->ctrl_type_name, handle);
	handle->is_destroyed = true;
}

static void on_control_gc(napi_env env, void *finalize_data, void *finalize_hint) {
	struct control_handle *handle = (struct control_handle *)finalize_data;
	LIBUI_NODE_DEBUG_F("Control %s %p garbage collected.", handle->ctrl_type_name, handle);

	free(handle->children);
	free(handle->events);
	LIBUI_NODE_DEBUG_F("%s %p handle freed.", handle->ctrl_type_name, handle);
	handle->ctrl_type_name = NULL;
	free(handle);
}

napi_value control_handle_new(napi_env env, uiControl *control, const char *ctrl_type_name) {
	// printf("on creation %p\n", control->Parent(control));

	struct control_handle *handle = calloc(1, sizeof(struct control_handle));
	handle->env = env;
	handle->events = calloc(1, sizeof(struct events_list));
	handle->children = create_children_list();
	handle->control = control;
	handle->ctrl_type_name = ctrl_type_name;
	handle->original_destroy = control->Destroy;
	control->Destroy = control_on_destroy;
	ctrl_map_insert(&controls_map, handle, control);
	LIBUI_NODE_DEBUG_F("%s %p created.", handle->ctrl_type_name, handle);

	napi_value handle_external;
	napi_status status = napi_create_external(env, handle, on_control_gc, NULL, &handle_external);
	CHECK_STATUS_THROW(status, napi_create_external);

	napi_ref ctrl_ref = NULL;

	status = napi_create_reference(env, handle_external, 0, &ctrl_ref);
	handle->ctrl_ref = ctrl_ref;
	CHECK_STATUS_THROW(status, napi_create_reference);

	return handle_external;
}

napi_value remove_child(napi_env env, struct children_list *list, struct control_handle *child) {
	if (list->head == NULL) {
		return NULL;
	}

	struct children_node *node = list->head;
	struct children_node *prev_node = NULL;

	while (node != NULL) {
		if (node->handle == child) {
			uint32_t new_ref_count;
			napi_status status = napi_reference_unref(env, node->handle->ctrl_ref, &new_ref_count);
			CHECK_STATUS_THROW(status, napi_reference_unref);
			LIBUI_NODE_DEBUG_F("new reference count for %s %p: %d", node->handle->ctrl_type_name,
							   node->handle, new_ref_count);

			if (node == list->head) {
				// removing first child
				list->head = node->next;
			}

			if (node == list->tail) {
				// removing last child
				list->tail = prev_node;
			}

			if (prev_node != NULL) {
				prev_node->next = node->next;
			}

			free(node);

			return NULL;
		}

		prev_node = node;
		node = node->next;
	}
	return NULL;
}

bool has_child(struct children_list *list, struct control_handle *child) {
	struct children_node *node = list->head;
	while (node != NULL) {
		if (node->handle == child) {
			return true;
		}
		node = node->next;
	}
	return false;
}

napi_status add_child_at(napi_env env, struct children_list *list, struct control_handle *child,
						 int index) {
	assert(env != NULL);
	assert(list != NULL);
	assert(child != NULL);
	if (child->control->Parent != NULL && child->control->Parent(child->control) != NULL) {
		napi_throw_error(env, NULL, "Child control already has parent.");
		return napi_pending_exception;
	}

	struct children_node *new_node = malloc(sizeof(struct children_node));
	new_node->next = NULL;
	new_node->handle = child;

	uint32_t new_ref_count;
	napi_status status = napi_reference_ref(env, child->ctrl_ref, &new_ref_count);
	CHECK_STATUS_PENDING(status, napi_reference_unref);

	if (index == 0) {
		new_node->next = list->head;
		list->head = new_node;

		if (list->tail == NULL) {
			// first child
			list->tail = new_node;
		}
		return napi_ok;
	}

	int i = 0;
	struct children_node *node = list->head;
	struct children_node *prev_node = NULL;
	while (i < index && node != NULL) {
		prev_node = node;
		node = node->next;
		i++;
	}

	prev_node->next = new_node;
	new_node->next = node;

	if (i < index) {
		// we are past end. insert at tail
		list->tail->next = new_node;
		list->tail = new_node;
	}

	return napi_ok;
}

napi_value destroy_all_children(napi_env env, struct children_list *list) {
	if (list->head == NULL) {
		// This control has no children
		return NULL;
	}

	struct children_node *node = list->head;

	while (node != NULL) {
		uiControlDestroy(node->handle->control);
		node = node->next;
	}

	return NULL;
}

napi_value clear_children(napi_env env, struct children_list *list) {
	if (list->head == NULL) {
		// This control has no children
		return NULL;
	}

	struct children_node *node = list->head;
	struct children_node *node_to_free;

	while (node != NULL) {
		uint32_t new_ref_count;
		napi_status status = napi_reference_unref(env, node->handle->ctrl_ref, &new_ref_count);
		CHECK_STATUS_THROW(status, napi_reference_unref);
		LIBUI_NODE_DEBUG_F("new reference count for %s %p: %d", node->handle->ctrl_type_name,
						   node->handle, new_ref_count);

		node_to_free = node;
		node = node->next;
		free(node_to_free);
	}

	list->head = NULL;
	list->tail = NULL;

	return NULL;
}

napi_value remove_child_at(napi_env env, struct children_list *list, int index_to_remove) {
	if (list->head == NULL) {
		return NULL;
	}

	struct children_node *node = list->head;
	struct children_node *prev_node = NULL;
	int i = 0;
	while (node != NULL) {
		if (i == index_to_remove) {
			uint32_t new_ref_count;
			napi_status status = napi_reference_unref(env, node->handle->ctrl_ref, &new_ref_count);
			CHECK_STATUS_THROW(status, napi_reference_unref);
			LIBUI_NODE_DEBUG_F("new reference count for %s %p: %d", node->handle->ctrl_type_name,
							   node->handle, new_ref_count);

			if (node == list->head) {
				// removing first child
				list->head = node->next;
			}

			if (node == list->tail) {
				// removing last child
				list->tail = prev_node;
			}

			if (prev_node != NULL) {
				prev_node->next = node->next;
			}

			free(node);

			return NULL;
		}
		i++;
		prev_node = node;
		node = node->next;
	}
	return NULL;
}

struct children_list *create_children_list() {
	return calloc(1, sizeof(struct children_list));
}

struct children_node *create_node(struct control_handle *child) {
	assert(child != NULL);
	struct children_node *new_node = malloc(sizeof(struct children_node));
	new_node->next = NULL;
	new_node->handle = child;
	return new_node;
}

napi_status add_child(napi_env env, struct children_list *list, struct control_handle *child) {
	assert(env != NULL);
	assert(list != NULL);
	assert(child != NULL);
	if (child->control->Parent != NULL && child->control->Parent(child->control) != NULL) {
		napi_throw_error(env, NULL, "Child control already has parent.");
		return napi_pending_exception;
	}
	struct children_node *new_node = malloc(sizeof(struct children_node));
	new_node->next = NULL;
	new_node->handle = child;
	uint32_t new_ref_count;
	napi_status status = napi_reference_ref(env, child->ctrl_ref, &new_ref_count);
	CHECK_STATUS_PENDING(status, napi_reference_unref);
	if (list->head == NULL) {
		// First child for this control
		list->head = new_node;
		list->tail = new_node;
		return napi_ok;
	}

	// Control already has other children. Append to tail
	list->tail->next = new_node;

	// set this node as the new tail
	list->tail = new_node;

	return napi_ok;
}
