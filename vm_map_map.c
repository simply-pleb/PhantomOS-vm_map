#include <stdlib.h>

// #include <string.h>
// #include <stdio.h>

#include "vm_map_map.h"

static vm_page**            vm_map_begin;        // array of arrays of pages
static vm_page**            vm_map_end;          // a byte after map
static int*                 vm_map_index;
static hal_mutex_t         vm_map_mutex;


void vm_map_map_init() 
{
    int mapsize = MAP_SIZE * sizeof(vm_page*);
    vm_map_begin = (vm_page**) malloc(mapsize);
    vm_map_end = vm_map_begin + MAP_SIZE;

    vm_map_index = (int*) malloc(MAP_SIZE * sizeof(int));
    memset(vm_map_index, 0, MAP_SIZE * sizeof(int));

    // initialize arrays
    int bufsize = MAP_BUF_SIZE * sizeof(vm_page);
    vm_page** i;
    for (i = vm_map_begin; i < vm_map_end; i++)
    {
        *i = (vm_page*) malloc(bufsize);
        memset(*i, 0, bufsize);
    }
    hal_mutex_init(&vm_map_mutex, "VM Map");
}

vm_page* vm_map_page_init(void *v_addr)
{
    vm_page* p = (vm_page*) malloc(sizeof(vm_page));
    memset(p, 0, sizeof(vm_page));
    p->virt_addr = v_addr;
    p->exists = 1;

    hal_mutex_init(&p->lock, "VM Page");
    return p;
}


inline u_int64_t hash(u_int64_t val) 
{
    return val % MAP_SIZE;
}


// updates vm_page value in vm_map_map structure by val->virt_addr

vm_page* find_page(void* v_addr) {
    int page_idx = hash((u_int64_t)v_addr);
    // printf("address: %d\n", page_idx);

    vm_page* buf = vm_map_begin[page_idx];
    vm_page* page = buf + vm_map_index[page_idx];
    vm_page* i;
    for (i = buf; i < buf + MAP_BUF_SIZE; i++) {
        if (i->exists && i->virt_addr == v_addr)
        {
            page = i;
            break;
        }
    }
    return page;
}
void set_page(vm_page* val) 
{
    vm_page* page = find_page(val->virt_addr);
    
    hal_mutex_lock(&vm_map_mutex);
    if (page->exists) {
        hal_mutex_lock(&page->lock);
    } else {
        int page_idx = hash((u_int64_t)val->virt_addr);
        vm_map_index[page_idx] = (vm_map_index[page_idx] + 1) % MAP_BUF_SIZE;
    }
    *page = *val;
    hal_mutex_unlock(&page->lock);
    hal_mutex_unlock(&vm_map_mutex);
}


// returns vm_page value from vm_map_map structure by its v_addr

vm_page get_page(void* v_addr) 
{
    
    vm_page* page = find_page(v_addr);
    if (page->virt_addr != v_addr)
        return (const struct vm_page){0};

    hal_mutex_lock(&page->lock);
    vm_page res = *(page); 
    hal_mutex_unlock(&page->lock);
    return res;
}





// Used to show progress
static int vm_map_map_do_for_percentage = 0;

void vm_map_map_do_for_all(vmem_page_func_t func, int lock)
{
    u_int64_t total = (vm_map_end - vm_map_begin) * MAP_BUF_SIZE;
    vm_page** i;
    int j;
    hal_mutex_lock(&vm_map_mutex);
    for (i = vm_map_begin; i < vm_map_end; i++)
    {
        for (j = 0; j < MAP_BUF_SIZE; j++) {
            if (!((*i)+j)->exists)
                continue;

            if (lock)
                hal_mutex_lock(&((*i)+j)->lock);
            // else
                // assert(!hal_mutex_is_locked(&i->lock));
            
            func( ((*i)+j) );
            
            if (lock)
                hal_mutex_unlock(&((*i)+j)->lock);
            vm_map_map_do_for_percentage = (100L*(i-vm_map_begin))/total;
        }
    }
    vm_map_map_do_for_percentage = 100;
    hal_mutex_unlock(&vm_map_mutex);
}
