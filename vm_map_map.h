#include "vm_map.h"

// to allocate 2^20 * 4kb = 4gb
// #define LEN 1048576
#define MAP_SIZE 1024
#define MAP_BUF_SIZE 8

void vm_map_map_init();

vm_page* vm_map_page_init(void *v_addr);

u_int64_t hash(u_int64_t val);

void set_page(vm_page* val);

vm_page get_page(void* v_addr);


typedef void (*vmem_page_func_t)(vm_page *);

void vm_map_map_do_for_all(vmem_page_func_t func, int lock);
