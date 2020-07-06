#include "assert.h"
#include "heap_allocator.h"
#include "debug_serial.h"

static HeapAllocator* inst;

HeapAllocator::HeapAllocator() { inst = this; }
HeapAllocator& HeapAllocator::get() { return *inst; }

#define HEAP_BLOCK_ALIGN 64
union alignas(HEAP_BLOCK_ALIGN) HeapAllocator::HeapBlock {
  struct Header {
    HeapBlock* next;
    size_t size;
    bool allocated;
  } header;
  // header align 16
  uint8_t align[16];
  // data implicitly follows
};

void HeapAllocator::init_heap_pages(
    PhysMemAllocator& physMemAlloc, VirtMemAllocator& virtMemAlloc) {
  // grab a 4MiB contiguous region of kernel virtual memory
  heap = (HeapBlock*)virtMemAlloc.reserve_block();

  assert(HEAP_PAGES % 32 == 0, "heap must be allocated in 32-page blocks");
  const unsigned n_chunks = HEAP_PAGES / CHUNK_PAGES;
  for (unsigned i = 0; i < n_chunks; ++i) {
    assert(CHUNK_PAGES == 32, "heap must be allocated in 32-page blocks");
    phys_heap_chunks[i] = physMemAlloc.alloc32();
    assert(phys_heap_chunks[i], "heap chunk alloc failed");
    for (unsigned j = 0; j < CHUNK_PAGES; ++j) {
      void* heap_page = (void*)((uint32_t)phys_heap_chunks[i] + j*PAGE_SIZE);
      void* virt_page = (void*)((uint32_t)heap + (CHUNK_PAGES*i+j)*PAGE_SIZE);
      void* res = virtMemAlloc.map_page(virt_page, heap_page);
      assert(res, "mapping heap page failed");
      // save addresses of suballocators
      if (j == 0) {
        if (i == n_chunks-3) {
          slab8 = (uint8_t*)virt_page;
        }
        else if (i == n_chunks-2) {
          slab16 = (uint16_t*)virt_page;
        }
        else if (i == n_chunks-1) {
          slab32 = (uint32_t*)virt_page;
        }
      }
    }
  }

  // set up heap blocks
  heap->header.next = nullptr;
  heap->header.size = (CHUNK_PAGES-3) * PAGE_SIZE;
  heap->header.allocated = false;
  debug::serial_printf("heap reserved at v %08x\n", (uint32_t)heap);
  debug::serial_printf("slab8 reserved at v %08x\n", (uint32_t)slab8);
  debug::serial_printf("slab16 reserved at v %08x\n", (uint32_t)slab16);
  debug::serial_printf("slab32 reserved at v %08x\n", (uint32_t)slab32);
}

void* HeapAllocator::slab8_malloc() {
  // TODO: abstract this code somehow
  for (unsigned i = 0; i < sizeof(slab8_bitmap); ++i) {
    uint8_t bitmap = slab8_bitmap[i];
    if (bitmap != 0xff) {
      unsigned j = 0;
      while (bitmap & 0x1) {
        bitmap >>= 1;
        j++;
      }
      slab8_bitmap[i] |= 1 << j;
      return &slab8[8*i+j];
    }
  }
  return nullptr;
}

void* HeapAllocator::slab16_malloc() {
  for (unsigned i = 0; i < sizeof(slab16_bitmap); ++i) {
    uint8_t bitmap = slab16_bitmap[i];
    if (bitmap != 0xff) {
      unsigned j = 0;
      while (bitmap & 0x1) {
        bitmap >>= 1;
        j++;
      }
      slab16_bitmap[i] |= 1 << j;
      return &slab16[8*i+j];
    }
  }
  return nullptr;
}

void* HeapAllocator::slab32_malloc() {
  for (unsigned i = 0; i < sizeof(slab32_bitmap); ++i) {
    uint8_t bitmap = slab32_bitmap[i];
    if (bitmap != 0xff) {
      unsigned j = 0;
      while (bitmap & 0x1) {
        bitmap >>= 1;
        j++;
      }
      slab32_bitmap[i] |= 1 << j;
      return &slab32[8*i+j];
    }
  }
  return nullptr;
}

void* HeapAllocator::block_malloc(size_t size) {
  for (HeapBlock* node = heap; node != nullptr; node = node->header.next) {
    HeapBlock::Header& header = node->header;
    if (!header.allocated && header.size - sizeof(HeapBlock) >= size) {
      header.allocated = true;
      size_t leftover = header.size - sizeof(HeapBlock) - size;
      leftover -= leftover % HEAP_BLOCK_ALIGN;
      if (leftover > 0) {
        header.size -= leftover;
        HeapBlock* next = (HeapBlock*)((uint32_t)node + header.size);
        HeapBlock::Header& next_header = next->header;
        next_header.next = header.next;
        next_header.size = leftover;
        next_header.allocated = false;
        header.next = next;
      }
      return (void*)(node+1); // data is just after header
    }
  }
  return nullptr;
}

void* HeapAllocator::malloc(size_t size) {
  if (size > HEAP_PAGES * PAGE_SIZE) {
    return nullptr;
  }
  // attempt slab allocators first, but fall through if needed
  if (size <= 8) {
    void* out = slab8_malloc();
    if (out) return out;
  }
  if (size <= 16) {
    void* out = slab16_malloc();
    if (out) return out;
  }
  if (size <= 32) {
    void* out = slab32_malloc();
    if (out) return out;
  }
  return block_malloc(size);
}
