////////////////////////////
// double linked list module
// original idea from linux kernel code
// ( http://lxr.free-electrons.com/source/include/linux/list.h )
// modifyed by Daeyoun Kang

#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include "include/debug-msg.h"

typedef struct list_head list_t;
typedef struct list_head list_head;
struct list_head { list_t *next, *prev; };

#define LIST_HEAD(name) list_head name = { &name, &name }
#define list_init(name) {(name)->next = (name); (name)->prev = (name);}

#define list_entry(entry, type, member) \
	(type *)( (char *)(entry) - (char *) &( ((type *)0)->member ) )

#define list_next_entry(entry, type, member) \
	list_entry((entry)->member.next, type, member)
#define list_next_or_null(head, entry, type, member) \
	(&(entry)->member) != (head)->prev ? list_next_entry(entry, type, member) : (type *)0

#define list_first_or_null(head, type, member) \
	(!list_isempty(head)) ? list_entry((head)->next, type, member) : (type *)0

#define list_for(entry, head)	\
	for (entry = (head)->next; entry != head; entry = (entry)->next)

#define list_foreach(entry, head, type, member) 		\
	for (entry = list_first_or_null(head, type, member);	\
		(entry);					\
		entry = ((entry)->member).next != head ? list_next_entry(entry, type, member) : (type *)0)

inline static void list_add_between(list_t *__new, list_t *prev, list_t *next) {
	next->prev = __new;
	__new->next = next;
	__new->prev = prev;
	prev->next = __new;
}

inline static void list_add(list_t *__new, list_t *head) {
	list_add_between(__new, head, head->next);
}

inline static void list_add_tail(list_t *__new, list_t *head) {
	list_add_between(__new, head->prev, head);
}

inline static void *list_delete(list_t *item) {
	item->next->prev = item->prev;
	item->prev->next = item->next;
	item->prev = 0; item->next = 0;
	return NULL;
}

inline static int list_isempty(list_t *head) {
	return head->next == head;
}

inline static int list_isfirst(list_t *head, list_t *entry) {
	return entry->prev == head;
}

inline static int list_islast(list_t *head, list_t *entry) {
	return entry->next == head;
}

inline static void list_connect(list_t *head, list_t *tail) {
	head->prev->next = tail->next;
	head->next->prev = tail->prev;
	tail->next->prev = head->prev;
	tail->prev->next = head->next;
}

inline static int list_islisted(list_t *item) {
	return item->prev != 0 && item->next != 0;
}

#endif