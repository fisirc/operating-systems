# Task 2: Lazy loading

## Data structures

```c
// Archivo: vm/page.h

struct spte
  {
    struct file *file; 
    off_t ofs; 
    uint32_t read_bytes, zero_bytes;  
    bool writable;  
  };
```
La idea es que durante la ejecución de un proceso, el código ejecutable no se cargue completamente en la memoria, sino que se gestione dividiéndolo en páginas, entonces poder cargarlas según sea necesario usamos un struct de entrada del SPT (supplemental page table), que almacena información adicional sobre las páginas y permite mapearlas.

## Algorithms

Se crea y devuelve una entrada de la spt usando la información del archivo:

### 1. init_file_spte()

```c
struct spte *
init_file_spte (struct list *spt, struct file *file, off_t ofs, uint32_t read_bytes, uint32_t zero_bytes, bool writable)
{
  struct spte *e;
  
  e = (struct spte *)malloc (sizeof (struct spte));
  
  e->status = PAGE_FILE;
  
  return e;
}
```

### 1.1. load_segment()

Y en lugar de cargar una página de memoria, se crea una entrada de frame en la spt y se inserta en esta.

```c
// Archivo: userprog/process.c
static bool
load_segment (struct file *file, off_t ofs, uint8_t *upage,
              uint32_t read_bytes, uint32_t zero_bytes, bool writable) 
{
  // ...

  file_seek (file, ofs);
  while (read_bytes > 0 || zero_bytes > 0) 
    {
      size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
      size_t page_zero_bytes = PGSIZE - page_read_bytes;
    
      init_file_spte (spt, upage, kpage, read_bytes, zero_bytes, writable);

      read_bytes -= page_read_bytes;
      zero_bytes -= page_zero_bytes;
      upage += PGSIZE;
      ofs += page_read_bytes;
    }
  return true;
}
```

### 1.2. page_fault()

Cuando ocurre un page fault en user mode, el proceso no se termina inmediatamente, solo si la fault address pertenece a la memoria virtual del kernel o si not_present es false. En otros casos, el fallo podría resolverse mediante lazy loading: la cual se realiza usando load_page. Si el lazy loading se realiza con éxito, el proceso regresa a su funcionamiento normal.

```c
static void
page_fault (struct intr_frame *f) 
{
  bool not_present;  /* True: not-present page, false: writing r/o page. */
  bool write;        /* True: access was write, false: access was read. */
  bool user;         /* True: access by user, false: access by kernel. */
  void *fault_addr;  /* Fault address. */

  struct hash *spt;
  void *upage;
  
  // ... 
  upage = pg_round_down(fault_addr);
  
  if (is_kernel_vaddr (fault_addr) || !not_present)
    sys_exit (-1);
  
  if (load_page (spt, upage))
    return;

  /* Determine cause. */
  not_present = (f->error_code & PF_P) == 0;
  write = (f->error_code & PF_W) != 0;
  user = (f->error_code & PF_U) != 0;

  /* To implement virtual memory, delete the rest of the function
     body, and replace it with code that brings in the page to
     which fault_addr refers. */
  printf ("Page fault at %p: %s error %s page in %s context.\n",
          fault_addr,
          not_present ? "not present" : "rights violation",
          write ? "writing" : "reading",
          user ? "user" : "kernel");
  kill (f);
}
```
