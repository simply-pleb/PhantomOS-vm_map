#include <stdio.h>
#include <assert.h>

#include <vm_map_map.h>

// setting/getting page

void push_vm_page() {    
    printf("page init...\n");
    vm_page* p = vm_page_init((void*)1);
    p->phys_addr = 123;

    printf("adding page...\n");
    set_page(p);
    
    printf("getting page...\n");
    vm_page p2 = get_page((void*)1);

    assert(p->phys_addr == p2.phys_addr);
    assert((void*)&p != (void*)&p2);
    printf("page init test is completed\n\n");
}


// getting page that does not exist


void test2() {
    printf("checking empty page...\n");
    vm_page p3 = get_page((void*)123);
    assert(p3.exists == 0);
    assert(p3.virt_addr == 0);
    assert(p3.phys_addr == 0);
    printf("empty page test is completed\n\n");
    
}


void test3(int total) {
    printf("counting hits/total rate...\n");
    int hits = 0;
    for (u_int64_t v_addr = 0; v_addr < total; v_addr++) {
        vm_page* p = vm_page_init((void*)v_addr);
        p->phys_addr = v_addr ^ 123;
        set_page(p);
    }
    for (u_int64_t v_addr = 0; v_addr < total; v_addr++) {
        vm_page p = get_page((void*)v_addr);
        if (p.exists == 1 && (u_int64_t)p.virt_addr == v_addr) {
            assert(p.phys_addr == v_addr ^ 123);
            hits++;
        }
    }
    printf("hits/total %d/%d\n\n", hits, total);
}

int main() {
    printf("Testing started...\n\n");

    printf("vm_map init...\n");
    vm_map_map_init();

    push_vm_page();
    test2();
    test3(2000);


    printf("Tests are complited.\n");
    
}