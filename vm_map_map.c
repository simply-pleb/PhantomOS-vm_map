#include <stdlib.h>

// #include <string.h>
// #include <stdio.h>

#include "vm_map_map.h"


// initialize the data structure
void vm_map_map_init() 
{
    int mapsize = MAP_SIZE * sizeof(vm_page*);
    vm_map_begin = (vm_page**) malloc(mapsize);
    vm_map_end = vm_map_begin + MAP_SIZE;

    vm_map_index = (int*) malloc(MAP_SIZE * sizeof(int));
    memset(vm_map_index, 0, MAP_SIZE * sizeof(int));

    // initialize arrays
    int bufsize = MAP_BUF_SIZE * sizeof(vm_page);
    // vm_page** i;
    // for (i = vm_map_begin; i < vm_map_end; i++)
    // {
    //     *i = (vm_page*) malloc(bufsize);
    //     memset(*i, 0, bufsize);
    // }
    vm_page** ar = vm_map_begin;
    for (size_t i = 0; i < MAP_SIZE; i++)
    {
        ar[i] = (vm_page*) malloc(bufsize);
        memset(ar[i], 0, bufsize);
    }
    hal_mutex_init(&vm_map_mutex, "VM Map");
}

// create a vm_page with a specified virtual address
vm_page* vm_map_page_init(void *v_addr)
{
    vm_page* p = (vm_page*) malloc(sizeof(vm_page));
    memset(p, 0, sizeof(vm_page));
    p->virt_addr = v_addr;
    p->exists = 1;

    hal_mutex_init(&p->lock, "VM Page");
    return p;
}

// returns a hash
inline u_int64_t hash(u_int64_t val) 
{
    return val % MAP_SIZE;
}


// updates vm_page value in vm_map_map structure by val->virt_addr

vm_page* find_page(void* v_addr) {
    int page_idx = hash((u_int64_t)v_addr);
    // printf("address: %d\n", page_idx);

    vm_page* buf = vm_map_begin[page_idx];
    // vm_page* page = buf + vm_map_index[page_idx];
    // vm_page* i;
    // iterate over the array of buf
    // for (i = buf; i < buf + MAP_BUF_SIZE; i++) {
    //     if (i->exists && i->virt_addr == v_addr)
    //     {
    //         page = i;
    //         break;
    //     }
    // }
    int i;
    for (i = 0; i < MAP_BUF_SIZE; i++) {
        if (buf[i].exists && buf[i].virt_addr == v_addr)
        {
            return &buf[i];
        }
    }
    // if the page is not found, return the index of the next page to be replace on this hash value
    return buf + vm_map_index[page_idx];
}


// load a new vm_page into vm_map
// REMARK: page gets replaced, but the state is not saved.
void set_page(vm_page* new_page) 
{
    void* new_page_vaddr = new_page->virt_addr;
    vm_page* page = find_page(new_page_vaddr);
    
    hal_mutex_lock(&vm_map_mutex);
    if (page->exists) {
        hal_mutex_lock(&page->lock);
    } 
    else {
        int page_idx = hash((u_int64_t)new_page_vaddr);
        vm_map_index[page_idx] = (vm_map_index[page_idx] + 1) % MAP_BUF_SIZE;
    }

    // TODO: add saving the state of the old page here.
    // TODO: somehow perform a pageout

    *page = *new_page;
    hal_mutex_unlock(&page->lock);
    hal_mutex_unlock(&vm_map_mutex);
}


// returns vm_page value from vm_map_map structure by its v_addr
vm_page get_page(void* v_addr) 
{
    // if map mutex not locked?
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


