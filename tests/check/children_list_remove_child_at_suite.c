#include <assert.h>
#include <stdlib.h>
#include <ui.h>
#include "control.h"
#include "tests.h"

static struct control_handle *get_child(napi_env env) {
	uiControl *ctrl = malloc(sizeof(uiControl));
	napi_value child_value = control_handle_new(env, ctrl, "name_test");
	struct control_handle *child;
	napi_get_value_external(env, child_value, (void **)&child);
	return child;
}

static void test_clear_remove_child_at_empty_list(napi_env env) {
	struct children_list *list = create_children_list();
	napi_value ret = remove_child_at(env, list, 1);
	assert(ret == NULL);
	free(list);
}

static void test_clear_remove_child_at_one_child_list(napi_env env) {

	struct children_list *list = create_children_list();
	struct control_handle *child = get_child(env);
	add_child(env, list, child);
	remove_child_at(env, list, 0);

	assert(list->head == NULL);
	assert(list->tail == NULL);

	free(child);
	free(list);
}

static void test_clear_remove_child_overflow_at_one_child_list(napi_env env) {

	struct children_list *list = create_children_list();
	struct control_handle *child = get_child(env);
	add_child(env, list, child);
	remove_child_at(env, list, 1);

	assert(list->head->handle == child);
	assert(list->tail == list->head);

	free(child);
	free(list);
}

static void test_clear_remove_child_one_from_two_child_list(napi_env env) {

	struct children_list *list = create_children_list();
	struct control_handle *child1 = get_child(env);
	struct control_handle *child2 = get_child(env);
	add_child(env, list, child1);
	add_child(env, list, child2);
	remove_child_at(env, list, 0);

	assert(list->head->handle == child2);
	assert(list->tail == list->head);

	free(child1);
	free(child2);
	free(list);
}

static void test_clear_remove_child_two_from_two_child_list(napi_env env) {

	struct children_list *list = create_children_list();
	struct control_handle *child1 = get_child(env);
	struct control_handle *child2 = get_child(env);
	add_child(env, list, child1);
	add_child(env, list, child2);
	remove_child_at(env, list, 1);

	assert(list->head->handle == child1);
	assert(list->tail == list->head);

	free(child1);
	free(child2);
	free(list);
}

static void test_clear_remove_middle_child_from_three_child_list(napi_env env) {

	struct children_list *list = create_children_list();
	struct control_handle *child1 = get_child(env);
	struct control_handle *child2 = get_child(env);
	struct control_handle *child3 = get_child(env);

	add_child(env, list, child1);
	add_child(env, list, child2);
	add_child(env, list, child3);
	assert(list->head->handle == child1);
	assert(list->head->next->handle == child2);
	assert(list->tail->handle == child3);

	remove_child_at(env, list, 1);

	assert(list->head->handle == child1);
	assert(list->head->next == list->tail);
	assert(list->tail->handle == child3);

	free(child1);
	free(child2);
	free(child3);
	free(list);
}

void children_list_remove_child_at_suite(napi_env env) {
	RUN_TEST(test_clear_remove_child_at_one_child_list);
	RUN_TEST(test_clear_remove_child_at_empty_list);
	RUN_TEST(test_clear_remove_child_overflow_at_one_child_list);
	RUN_TEST(test_clear_remove_child_one_from_two_child_list);
	RUN_TEST(test_clear_remove_child_two_from_two_child_list);
	RUN_TEST(test_clear_remove_middle_child_from_three_child_list);
}