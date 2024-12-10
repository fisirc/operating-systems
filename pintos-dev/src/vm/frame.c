#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>
#include "filesys/file.h"
#include "filesys/off_t.h"
#include "vm/stdint.h"

#define PAGE_ZERO 0
#define PAGE_FRAME 1
#define PAGE_FILE 2
#define PAGE_SWAP 3

struct spte
  {
    void *upage;
    void *kpage;
  
    struct hash_elem hash_elem;
  
    int status;
  
    struct file *file;  // file to read
    off_t ofs;  // offset
    uint32_t read_bytes, zero_bytes;  // bytes to read or set to 0
    bool writable;  // is the page writable?
    int swap_id;
  };

void init_spt (struct hash *);
void destroy_spt (struct hash *);
void init_spte (struct hash *, void *, void *);
void init_zero_spte (struct hash *, void *);
void init_frame_spte (struct hash *, void *, void *);
struct spte *init_file_spte (struct hash *, void *, struct file *, off_t, uint32_t, uint32_t, bool);
bool load_page (struct hash *, void *);
struct spte *get_spte (struct hash *, void *);
void page_delete (struct hash *spt, struct spte *entry);

#endif