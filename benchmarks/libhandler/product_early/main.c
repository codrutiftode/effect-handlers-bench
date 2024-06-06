#include "libhandler.h"
#include <stdio.h>
#include <stdlib.h>

#define ENUMERATE_SIZE 1000

LH_DEFINE_EFFECT1(early, done);
LH_DEFINE_VOIDOP1(early, done, int);

// Using a linked list to mirror the other languages
typedef struct list {
  int64_t head;
  struct list* next;
} list_t;

static lh_value handle_early_done(lh_resume r, lh_value local, lh_value arg) {
  return arg;
}

static const lh_operation _early_ops[] = {
  { LH_OP_NORESUME, LH_OPTAG(early, done), &handle_early_done},
};

static const lh_handlerdef _early_def = {
  LH_EFFECT(early), NULL, NULL, NULL, _early_ops
};

static lh_value product(lh_value parameter) {
  list_t list = *(list_t*) lh_ptr_value(parameter);
  if (list.head == 0) {
    early_done(0);
    return lh_value_null;
  }
  else {
    int64_t res = lh_long_value(product(lh_value_ptr(list.next))); 
    return lh_value_long(list.head * res);
  }
}

static int run_product(list_t* xs) {
  return lh_handle(&_early_def, lh_value_null, &product, lh_value_ptr(xs));
}

// Allocate a linked list on the heap
static list_t* enumerate(size_t n) {
  list_t* top = malloc(sizeof(list_t));
  list_t* current = top;

  for (size_t i = 0; i < n; i++) {
    current->head = (n - 1) - i; 
    if (i == n - 1) current->next = NULL;
    else current->next = malloc(sizeof(list_t));
    current = current->next;
  }
  return top;
}

// Free linked list
static void free_list(list_t* xs) {
  while (xs != NULL) {
    list_t* next = xs->next;
    free(xs);
    xs = next;
  }
}

static int64_t run(int64_t n) {
  list_t* xs = enumerate(ENUMERATE_SIZE + 1);

  int64_t a = 0;
  for (int i = 0; i < n; i++) {
    a += run_product(xs);
  }

  free_list(xs);
  return a;
}

int main(int argc, char** argv) {
  int64_t n = argc != 2 ? 5 : atoi(argv[1]);
  int64_t r = run(n);

  // Increase output buffer size to increase performance
  char buffer[8192];
  setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
  printf("%ld\n", r);
  return 0;
}