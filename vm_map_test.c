#include <stdio.h>
#include <assert.h>

#include <vm_map_map.h>

int main() {
    printf("Testing started...\n");
    printf("vm_map init...\n");
    vm_map_map_init();


    // setting/getting page
    printf("page init...\n");
    vm_page* p = vm_page_init((void*)1);
    printf("page address %p\n", p);
    p->some_value = 123;
    printf("page initialized.\n");


    printf("adding page...\n");
    set_page(p);
    
    printf("getting page...\n");
    vm_page p2 = get_page((void*)1);

    assert(p->some_value == p2.some_value);
    assert((void*)&p != (void*)&p2);


    // getting page that do not exist

    vm_page p3 = get_page((void*)123);
    assert(p3.exists == 0);
    assert(p3.virt_addr == 0);
    assert(p3.phys_addr == 0);
    // assert(p3.lock == 0);


    printf("test complited.\n");
    
}