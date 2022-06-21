# PhantomOS port to 64-bit system

One of the core tasks in porting PhantomOS from 32-bit to 64-bit system  is to update the *Page Table* algorithm.
<!-- The 32-bit version uses array of all possible 2^32 virtual addreses to allocate 4.2 Gb. -->

## Interface

Our goal is to develop a structure for keeping a translations of virtual addresses to physical. First lets define its interface.

The functions that we need:

* add new mapping
* update a mapping
* apply function to all mappings (for snapshots)

The interface that we choose is defined in the ```vm_map_map.h```:

```C
void vm_map_map_init();

vm_page* vm_map_page_init(void *v_addr);

void set_page(vm_page* new_page);

vm_page get_page(void* v_addr);

void vm_map_map_do_for_all(vmem_page_func_t func, int lock);
```
For possibility of parallel operations on our structure we decided that ```get_page``` function will return the copy of a page not a pointer to an actual page. That way we can persist to any changes go through our interface.

## Implemetation

We use the idea of an *inverted page table* to impliment the interface. 

To make it more efficient we use *software TLB* by pre-allocating fixed number of PTE's per hashed virtual address. This technique is also used in the PowerPC's page table.

## TODO

- [x] interface
- [x] basic implemetation
- [x] unit tests
- [ ] connect the implemetation with ```vm_map.c```
- [ ] integration tests
- [ ] tests for perfomanse measurement

## Possible updates to increase perfomance

- change hash function
- use different algorithm to remove old mappings
- use spinlock vs mutex


