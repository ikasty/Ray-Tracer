// Queue implementation by double linked-list
#ifndef QUEUE_H
#define QUEUE_H

#define queue_head(head) LIST_HEAD(head)
#define queue_item(head) list_t queue_##head
#define queue_add(item, head) list_add_tail(&(item)->queue_##head, &head)
#define queue_get(head, type) list_first_or_null(&(head), type, queue_##head)
#define queue_del(item, head) ((item) ? list_delete(&(item)->queue_##head), item : item)
#define queue_pop(item, head, type) (item = queue_get(head, type), queue_del(item, head))
#define is_queued(item, head) list_islisted(&(item)->queue_##head)
#define is_queue_empty(head) list_isempty(queue_##head)

#endif