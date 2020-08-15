#include "randlib.h"
#include <stdio.h>
#include <stdlib.h>

static void rand64_init (void) __attribute__((constructor));
static void rand64_fini (void) __attribute__((destructor));

static FILE *urandstream;

static void rand64_init (void) {
  urandstream = fopen ("/dev/urandom", "r");
  if (! urandstream)
    abort ();
}

static void rand64_fini (void) {
  fclose (urandstream);
}

extern unsigned long long rand64 (void) {
  unsigned long long int x;
  if (fread (&x, sizeof x, 1, urandstream) != 1)
    abort ();
  return x;
}
