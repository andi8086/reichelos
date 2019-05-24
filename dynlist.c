#include "dynlist.h"

void dynlist_append(dynlist **list, dynlist *new)
{
    dynlist *prev;
    while(*list) {
        prev = *list;
        list = &(*list)->next;
    }
    *list = new; 
    (*list)->prev = prev;
}

void dynlist_prepend(dynlist **list, dynlist *new)
{
    if (!*list) {
        *list = new;
        new->next = 0;
        new->prev = 0;
        return;
    }
    dynlist *old = *list;
    dynlist *prev = (*list)->prev;

    if (prev) {
        prev->next = new;
        new->prev = prev;
    }
    new->next = old;
    // update the original pointer pointing to the old element
    *list = new;
}

void dynlist_del(dynlist **list)
{
    dynlist *prev = (*list)->prev, *next = (*list)->next;
    if (prev) {
        prev->next = next;
    }    
    if (next) {
        next->prev = prev;
    }
    (*list) = next;
}
