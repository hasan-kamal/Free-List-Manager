# Free-List-Manager

### Introduction
- This is a free list manager — i.e. a custom-written `malloc()` and `free()` implementation
- Following commands are supported:
    - `malloc x` allocates space of _x_ bytes
    - `free y` reclaims space that was allocated by _y-th_ malloc call
    - `display` displays the current state of free list (including pointer addresses of chunks)
    - `exit` exits the program
    
### Repository structure
- `src/free_list_manager.c` contains the implementation of free list manager
- `src/main.c` contains the menu-driven sample driver application

### Build
To build project,
- `cd` into the root of this project and run provided script with `./build.sh`

### Example run
```bash
Hasans-MacBook-Pro:Free-List-Manager hasankamal$ ./build.sh 
>display

free_list:
--------------
free chunk (size:4080, address:0x10ae04000)
--------------

allocated_list:
-------------------
empty
-------------------

>malloc 10
10(total=26) bytes allocated successfully
>malloc 20
20(total=36) bytes allocated successfully
>display

free_list:
--------------
free chunk (size:4018, address:0x10ae0403e)
--------------

allocated_list:
-------------------
allocated block (size:20, malloc_number:2, address:0x10ae0401a)
allocated block (size:10, malloc_number:1, address:0x10ae04000)
-------------------

>free 1
block found(size:10, malloc_number:1)
free successful
newly reclaimed space: [start-0x10ae04000,end-0x10ae04019]
>display

free_list:
--------------
free chunk (size:10, address:0x10ae04000)
free chunk (size:4018, address:0x10ae0403e)
--------------

allocated_list:
-------------------
allocated block (size:20, malloc_number:2, address:0x10ae0401a)
-------------------

>exit

```
