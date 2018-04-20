#include "utlist.h"

void cbmplex_add(cbmplex_t *head, cbmplex_t *entry)
{
    LL_PREPEND(head,entry);
}

void cbmplex_del(cbmplex_t *head, cbmplex_t *entry)
{
    LL_DELETE(head,entry);
}

void cbmplex_update(cbmplex_t *head, cbmplex_ud_func_t func, void *arg)
{
    cbmplex_t *entry;

    LL_FOREACH(head,entry) {
        func(arg);
    }
}

cbmplex_t *cbmplex_find(cbmplex_t *head, uint8_t cbid_val)
{
    cmbplex_t *entry;

    LL_SEARCH_SCALAR(head,entry,cbid,cbid_val);

    return entry;
}
