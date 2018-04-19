# HTTP GET Server - COMP30023 Assignment 1

## Required Files:

- server.c

- respond.h

- respond.c

- Makefile

  ## Compilation:

  Compile with:

  ```
  make
  ```

## TestScript

The Test script and sample files linked in the LMS.

## General Comments:

I had intended to replace pretty much all of the `char*` types with the `s_String` type defined in `respond.h`, however I effectively ran out of time and decided that the energy cost to make it work was not worth the positives.

The server app is reasonably primitive but I believe it's generally acceptable. It definitely passes all the given test cases anyway. 