#include "libhandler.h"
#include <stdio.h>
#include <stdlib.h>

LH_DEFINE_EFFECT1(iterator, emit);
LH_DEFINE_VOIDOP1(iterator, emit, int);

static lh_value handle_iterator_emit(lh_resume r, lh_value local, lh_value arg) {
  return lh_tail_resume(r, local + arg, lh_value_null);
}

static lh_value handle_iterator_result(lh_value local, lh_value arg) {
  return local;
}

static const lh_operation _iterator_ops[] = {
  { LH_OP_TAIL_NOOP, LH_OPTAG(iterator, emit), &handle_iterator_emit },
  { LH_OP_NULL, lh_op_null, NULL}
};

static const lh_handlerdef _iterator_def = {
  LH_EFFECT(iterator), NULL, NULL, &handle_iterator_result, _iterator_ops
};

struct range_args {
  int64_t l;
  int64_t u;
};

static lh_value range(lh_value args) {
  struct range_args* range_args = lh_ptr_value(args);
  for (int l = range_args->l; l <= range_args->u; l++) {
    iterator_emit(l);
  }
  return lh_value_null;
}

static int64_t run(int64_t n) {
  struct range_args args = {
    .l = 0,
    .u = n
  };
  return lh_handle(&_iterator_def, 
                   lh_value_long(0), 
                   &range,
                   lh_value_ptr(&args));
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