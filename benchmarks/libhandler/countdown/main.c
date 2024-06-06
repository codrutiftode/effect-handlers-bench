#include "libhandler.h"
#include <stdio.h>
#include <stdlib.h>

LH_DEFINE_EFFECT2(state, get, put);
LH_DEFINE_OP0(state, get, int);
LH_DEFINE_VOIDOP1(state, put, int);

static lh_value handle_state_put(lh_resume r, lh_value local, lh_value arg) {
  return lh_tail_resume(r, arg, lh_value_null);
}

static lh_value handle_state_get(lh_resume r, lh_value local, lh_value arg) {
  return lh_tail_resume(r, local, local);
}

static const lh_operation _state_ops[] = {
  { LH_OP_TAIL, LH_OPTAG(state, get), &handle_state_get },
  { LH_OP_TAIL, LH_OPTAG(state, put), &handle_state_put }
};

static const lh_handlerdef _state_def = {
  LH_EFFECT(state), NULL, NULL, NULL, _state_ops
};

static lh_value countdown(lh_value parameter) {
  int state = state_get();
  while (state > 0) {
    state_put(state - 1);
    state = state_get();
  }
  return state;
}

int main(int argc, char** argv) {
  int64_t n = argc != 2 ? 5 : atoi(argv[1]);
  int64_t result = lh_handle(&_state_def, lh_value_long(n), &countdown, lh_value_null);
  printf("%ld\n", result);
  return 0;
}