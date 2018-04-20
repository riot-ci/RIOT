typedef void (*cbmplex_cb_t)(void *);

typedef struct cbmplex {
    struct cbmplex *next;
    uint8_t flags;
    uint8_t cbid;
    cbmplex_cb_t cb;
    void *arg;
} cbmplex_t;

typedef void (*cbmplex_ud_func_t)(cbmplex_t *, void *);

void cbmplex_add(cbmplex_t *head, cbmplex_t *entry);

void cbmplex_del(cbmplex_t *head, cbmplex_t *entry);

void cbmplex_update(cbmplex_t *head, cbmplex_ud_func_t func, void *arg);

cbmplex_t *cbmplex_find(cbmplex_t *head, uint8_t cbid_val);
