#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void
memory_error()
{
  fprintf(stderr, "Error: failed to allocate memory\n");
  exit(-1);
}

void
file_error(const char * filename)
{
  fprintf(stderr, "Error: failed to open file: %s\n", filename);
  exit(-2);
}
