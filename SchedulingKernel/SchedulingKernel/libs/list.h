#pragma once

#include "defs.h"
#include <stdbool.h>
#include <stdlib.h>

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when manipulating
 * whole lists rather than single entries, as sometimes we already know
 * the next/prev entries and we can generate better code by using them
 * directly rather than using the generic single-entry routines.
 */

struct list_ele_t {
	struct list_ele_t *prev;
	struct list_ele_t *next;
};

static inline void list_init(struct list_ele_t *);
static inline void list_add(struct list_ele_t *, struct list_ele_t *);
static inline void list_add_before(struct list_ele_t *, struct list_ele_t *);
static inline struct list_ele_t *list_add_after(struct list_ele_t *, struct list_ele_t *);
static inline void list_del(struct list_ele_t *);
static inline void list_del_init(struct list_ele_t *);
static inline bool list_empty(struct list_ele_t *);
static inline struct list_ele_t *list_next(struct list_ele_t *);
static inline struct list_ele_t *list_prev(struct list_ele_t *);
static inline void __list_add(struct list_ele_t *, struct list_ele_t *, struct list_ele_t *);
static inline void __list_del(struct list_ele_t *, struct list_ele_t *);

/*
 * list_init - initialize a new entry
 * //@ele:        new entry to be initialized
 */
static inline void list_init(struct list_ele_t *ele)
{
	if (ele == NULL) {
		exit(EXIT_FAILURE);
	}
	ele->prev = ele->next = ele;
}

/*
 * list_add - add a new entry
 * @listele:    list head to add after
 * @ele:        new entry to be added
 *
 * Insert the new element @ele *after* the element @listele which
 * is already in the list.
 */
static inline void list_add(struct list_ele_t *listele, struct list_ele_t *ele)
{
	list_add_after(listele, ele);
}

/*
 * list_add_before - add a new entry
 * @listele:    list head to add before
 * @ele:        new entry to be added
 *
 * Insert the new element @ele *before* the element @listele which
 * is already in the list.
 */
static inline void list_add_before(struct list_ele_t *listele,
				   struct list_ele_t *ele)
{
	/* What should you do if the list is NULL? */
	if (listele == NULL || ele == NULL) {
		exit(EXIT_FAILURE);
	}
	__list_add(ele, listele->prev, listele);
}

/*
 * list_add_after - add a new entry
 * @listele:    list head to add after
 * @ele:        new entry to be added
 *
 * Insert the new element @ele *after* the element @listele which
 * is already in the list.
 */
static inline struct list_ele_t *list_add_after(struct list_ele_t *listele,
				  struct list_ele_t *ele)
{
	/* What should you do if the list is NULL? */
	if (listele == NULL || ele == NULL) {
		exit(EXIT_FAILURE);
	}
	__list_add(ele, listele, listele->next);
	return ele;
}

/*
 * list_del - deletes entry from list
 * @listele:    the element to delete from the list
 *
 * Note: list_empty() on @listele does not return true after this, the entry is
 * in an undefined state.
 */
static inline void list_del(struct list_ele_t *listele)
{
	if (list_empty(listele)) {
		exit(EXIT_FAILURE);
	}
	__list_del(listele->prev, listele->next);
}

/*
 * list_del_init - deletes entry from list and reinitialize it.
 * @listele:    the element to delete from the list.
 *
 * Note: list_empty() on @listele returns true after this.
 */
static inline void list_del_init(struct list_ele_t *listele)
{
	list_del(listele);
	list_init(listele);
}

/*
 * list_empty - tests whether a list is empty
 * @list:       the list to test.
 */
static inline bool list_empty(struct list_ele_t *list)
{
	return list->next == list;
}

/*
 * list_next - get the next entry
 * @listele:    the list head
 */
static inline struct list_ele_t *list_next(struct list_ele_t *listele)
{
	return listele->next;
}

/*
 * list_prev - get the previous entry
 * @listele:    the list head
 */
static inline struct list_ele_t *list_prev(struct list_ele_t *listele)
{
	return listele->prev;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_ele_t *ele,
			      struct list_ele_t *prev,
			      struct list_ele_t *next)
{
	prev->next = next->prev = ele;
	ele->next = next;
	ele->prev = prev;
}

/*
 * Delete a list entry by making the prev/next entries point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_ele_t *prev,
			      struct list_ele_t *next)
{
	prev->next = next;
	next->prev = prev;
}