#include "vm_map.h"

// to allocate 2^20 * 4kb = 4gb
#define MAP_SIZE 1048576
// #define MAP_SIZE 1024
#define MAP_BUF_SIZE 8


static vm_page**            vm_map_begin;        // 2d array of pages
static vm_page**            vm_map_end;          // a byte after map

// TODO: change vm_map_index to pointer to char
static int*                 vm_map_index;        // indexes of next page to replace
static hal_mutex_t          vm_map_mutex;


void vm_map_map_init();

vm_page* vm_map_page_init(void *v_addr);

void set_page(vm_page* new_page);

vm_page get_page(void* v_addr);

void vm_map_map_do_for_all(vmem_page_func_t func, int lock);
