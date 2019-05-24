#ifndef __DYNLIST_H__
#define __DYNLIST_H__

typedef struct __attribute__((packed)) _dynlist {
    struct _dynlist *next;
    struct _dynlist *prev;
    void *e;
} dynlist;

void dynlist_append(dynlist **list, dynlist *new);
void dynlist_prepend(dynlist **list, dynlist *new);
void dynlist_del(dynlist **list);

#endif // __DYNLIST_H__
