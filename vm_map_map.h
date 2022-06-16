#include <queue.h>
#include <kernel/vm.h>

#include "spinlock.h"
#include "pager.h"
#include "hal.h"

// typedef	unsigned long long  u_int64_t;
typedef u_int64_t physaddr_t;

typedef struct vm_new_page
{
    void *                 virt_addr;     	    // where phys_addr is mapped
    physaddr_t             phys_addr; 
    unsigned char          exists;
    hal_mutex_t        lock;
}vm_new_page;

#define LEN 1024

void vm_map_map_init();

vm_new_page* vm_new_page_init(void *v_addr);

u_int64_t hash(u_int64_t val);

void set_new_page(vm_new_page* val);

vm_new_page get_new_page(void* v_addr);


typedef void (*vmem_new_page_func_t)(vm_new_page *);

void vm_map_map_do_for_all(vmem_new_page_func_t func, int lock);
