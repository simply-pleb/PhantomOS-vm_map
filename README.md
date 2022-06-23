# PhantomOS's Page Table port to 64-bit

The 32-bit virsion of PhantomOS uses an array to store the *page table*. When porting it to the 64-bit the virtual address space is highly increased and having an array of all mappings is not efficient.

## Goal

Our goal is to develop a structure for quick translating of the virtual addresses to the physical. First, lets define its interface.

## Interface

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

For the possibility of parallel operations on our structure, we decided that ```get_page``` function will return the copy of a page, not a pointer, to an actual page. This way we can persist that any modifications go through our interface.

## Implementation

We use the idea of an *inverted page table* to implement the interface.

To make it more efficient, we use *software TLB* by pre-allocating a fixed number of mappings per hashed virtual address. This technique is also used in the PowerPC's page table.

<!-- mutexes are very important in the page table -->


## TODO

* [x] interface
* [x] basic implementation
* [x] unit tests
* [x] connect the implementation with ```vm_map.c```
* [ ] integration tests
* [ ] tests for performance measurement

## Possible updates to increase the performance

* change hash function
* use different algorithm to remove old mappings
* use spinlock vs mutex

## Next steps

Updating the *page table* is an important step in porting PhantomOS to 64-bit, but there are other things that need to be changed. Here are some suggestions:

### Page table

After a meeting it became clear that arrays for the page table TLB is efficient but won't work. Instead, we should use structure without miss possibilities. Thus the basic implementation will be a hashtable with linked lists.

* make linked lists for hashtable buffer

### Snapshots

The current snapshot algorithm copies the whole page table, but it will be a very heavy operation with 64-bit address space and is not necessary. Instead, we can use a tree-like structure that grows with snapshots of the modified pages or page mappings.

Then we will get queries from this structure to check if any mapping of some virtual address range was modified. To answer the query we have at least 2 options:

* go through all mappings in the hashtable
* make a multilevel page table to mirror the search in our tree

The first solution for now seems more suitable, since snapshots are already slow and happen rarely.
