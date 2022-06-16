#include <stdlib.h>

// #include <string.h>
// #include <stdio.h>

#include "vm_map_map.h"

static vm_new_page*            vm_map_begin;        // array of pages
static vm_new_page*            vm_map_end;          // a byte after map

static hal_mutex_t         vm_map_mutex;

// to allocate 2^20 * 4kb = 4gb
// #define LEN 1048576


void vm_map_map_init() 
{
    int mapsize = LEN * sizeof(vm_new_page);
    vm_map_begin = (vm_new_page*) malloc(mapsize);
    memset(vm_map_begin, 0, mapsize);
    vm_map_end = vm_map_begin + LEN;

    hal_mutex_init(&vm_map_mutex, "VM Map");
}

vm_new_page* vm_new_page_init(void *v_addr)
{
    vm_new_page* p = (vm_new_page*) malloc(sizeof(vm_new_page));
    memset(p, 0, sizeof(vm_new_page));
    p->virt_addr = v_addr;
    p->exists = 1;

    hal_mutex_init(&p->lock, "VM Page");
    return p;
}


inline u_int64_t hash(u_int64_t val) 
{
    return val % LEN;
}


// updates vm_new_page value in vm_map_map structure by val->virt_addr

void set_new_page(vm_new_page* val) 
{
    int page_idx = hash((u_int64_t)val->virt_addr);
    vm_new_page* p = vm_map_begin + page_idx;
    // printf("address: %d\n", page_idx);

    hal_mutex_lock(&vm_map_mutex);
    if (p->exists) {
        hal_mutex_lock(&p->lock);
    }
    *p = *val;
    hal_mutex_unlock(&p->lock);
    hal_mutex_unlock(&vm_map_mutex);
}


// returns vm_new_page value from vm_map_map structure by its v_addr

vm_new_page get_new_page(void* v_addr) 
{
    int page_idx = hash((u_int64_t)(v_addr));   
    if (vm_map_begin[page_idx].virt_addr != v_addr)
        return (const struct vm_new_page){0};

    vm_new_page* p = vm_map_begin + page_idx;
    
    hal_mutex_lock(&p->lock);
    vm_new_page res = *(p); 
    hal_mutex_unlock(&p->lock);
    return res;
}





// Used to show progress
static int vm_map_map_do_for_percentage = 0;

void vm_map_map_do_for_all(vmem_new_page_func_t func, int lock)
{
    u_int64_t total = vm_map_end - vm_map_begin;
    vm_new_page *i;

    hal_mutex_lock(&vm_map_mutex);
    for (i = vm_map_begin; i < vm_map_end; i++)
    {
        if (!i->exists)
            continue;

        if (lock)
            hal_mutex_lock(&i->lock);
        // else
            // assert(!hal_mutex_is_locked(&i->lock));
        
        // should be a copy ?
        func( i );
        
        if (lock)
            hal_mutex_unlock(&i->lock);
        vm_map_map_do_for_percentage = (100L*(i-vm_map_begin))/total;
    }
    vm_map_map_do_for_percentage = 100;
    hal_mutex_unlock(&vm_map_mutex);
}

