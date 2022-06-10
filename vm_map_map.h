#include <pthread.h>

// typedef	unsigned long long  u_int64_t;
typedef u_int64_t physaddr_t;

typedef struct vm_page
{
    void *                 virt_addr;     	    // where phys_addr is mapped
    physaddr_t             phys_addr; 
    unsigned char          exists;
    pthread_mutex_t        lock;

    int                    some_value;
}vm_page;



static void vm_map_map_init();

static inline u_int64_t hash(u_int64_t val);

static void set_page(vm_page* val);

static vm_page get_page(void* v_addr);


typedef void (*vmem_page_func_t)(vm_page *);

static void vm_map_do_for_all(vmem_page_func_t func, int lock);