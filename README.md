# PhantomOS's Page Table port to 64-bit

The 32-bit version of PhantomOS uses an array to store the *page table*. When porting it to the 64-bit the virtual address space is highly increased and having an array of all mappings is not efficient.

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

For the possibility of parallel operations on our structure, we decided that `get_page` function will return the copy of a page, not a pointer, to an actual page. This way we can persist that any modifications go through our interface.

## Implementation

We use the idea of an *inverted page table* to implement the interface.

### Structure

To make it more efficient, we use *software TLB* by pre-allocating a fixed number of mappings per hashed virtual address. This technique is also used in the PowerPC's page table.

### Mutexes

Mutexes are very important in the page table algorithm, because the main priority is the speed of get requests.

We have two kinds of mutexes:

* `vm_map_mutex` is locked when we update any map entry
* `vm_page.lock` (in every `vm_page`) is locked when we want to read from or write to the entry

From that we can see the differences between `get_page` and `set_page` commands:

* `get_page` can not be done if any other process is using it
* `set_page` also can not be done if the map is locked (any other process updates map entry or `do_for_all` is in progress)

<!-- maybe `set_page` should not lock the map, but only check if it is locked-->
<!-- but `do_for_all` can do function without locking the pages -->

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

It might appear that arrays for the page table TLB is efficient but won't work, since some data can be lost. Instead, we could use structure without miss possibilities. The basic implementation of it will be a hashtable with linked lists.

### Snapshots

The current snapshot algorithm copies the whole page table, but it will be a very heavy operation with 64-bit address space and is not necessary.

#### Tree structure

Instead, we can use a tree-like structure that grows with snapshots of the modified pages or page mappings.

Then we will get queries from this structure to check if any mapping of some virtual address range was modified. To answer the query we have at least 2 options:

* go through all mappings in the hashtable
* make a multilevel page table to mirror the search in our tree

The first solution for now seems more suitable, since snapshots are already slow and happen rarely.

#### File system algorithm

Another possible solution is to reuse some file system algorithm, since its task seems to be similar.
