#include "vm_map.h"

// typedef u_int64_t physaddr_t;

// typedef struct vm_page
// {
//     void *                 virt_addr;     	    // where phys_addr is mapped
//     physaddr_t             phys_addr; 
//     unsigned char          exists;
//     hal_mutex_t        lock;
// }vm_page;


// to allocate 2^20 * 4kb = 4gb
// #define LEN 1048576

#define LEN 1024

void vm_map_map_init();

vm_page* vm_map_page_init(void *v_addr);

u_int64_t hash(u_int64_t val);

void set_page(vm_page* val);

vm_page get_page(void* v_addr);


typedef void (*vmem_page_func_t)(vm_page *);

void vm_map_map_do_for_all(vmem_page_func_t func, int lock);
