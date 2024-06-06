#include "libhandler.h"
#include <stdio.h>
#include <stdlib.h>

LH_DEFINE_EFFECT1(sieve, prime);
LH_DEFINE_OP1(sieve, prime, bool, int);

typedef struct primes_params_t {
  int64_t i;
  int64_t n;
  int64_t a;
} primes_params_t;

static lh_value handle_sieve_prime(lh_resume r, lh_value local, lh_value arg) {
  bool res;
  if (lh_long_value(arg) % lh_long_value(local) == 0) res = false;
  else res = lh_bool_value(sieve_prime(arg));
  return lh_tail_resume(r, local, lh_value_bool(res));
}

static const lh_operation _sieve_ops[] = {
  { LH_OP_TAIL, LH_OPTAG(sieve, prime), &handle_sieve_prime},
  { LH_OP_NULL, lh_value_null, NULL}
};

static const lh_handlerdef _sieve_def = {
  LH_EFFECT(sieve), NULL, NULL, NULL, _sieve_ops
};

static lh_value handle_top_sieve_prime(lh_resume r, lh_value local, lh_value arg) {
  return lh_tail_resume(r, local, lh_value_bool(true));
}

static const lh_operation _top_sieve_ops[] = {
  { LH_OP_TAIL_NOOP, LH_OPTAG(sieve, prime), &handle_top_sieve_prime},
  { LH_OP_NULL, lh_value_null, NULL}
};

static const lh_handlerdef _top_sieve_def = {
  LH_EFFECT(sieve), NULL, NULL, NULL, _top_sieve_ops
};

static lh_value primes(lh_value parameter) {
  primes_params_t* params = (primes_params_t*) lh_ptr_value(parameter);
  if (params->i >= params->n) return lh_value_long(params->a);

  int64_t i = params->i;
  if (sieve_prime(params->i)) {
    params->a += i;
    params->i++;
    return lh_handle(&_sieve_def, 
                     lh_value_long(i), 
                     &primes, 
                     lh_value_ptr(params));
  }
  else {
    params->i++;
    return primes(lh_value_ptr(params));
  }
}

static int64_t run(int64_t n) {
  primes_params_t params = {
    .i = 2,
    .n = n,
    .a = 0
  };
  return lh_long_value(lh_handle(&_top_sieve_def, 
                                 lh_value_null, 
                                 &primes, 
                                 lh_value_ptr(&params)));
}

int main(int argc, char** argv) { 
  int64_t n = argc != 2 ? 10 : atoi(argv[1]);
  int64_t r = run(n);

  // Increase output buffer size to increase performance
  char buffer[8192];
  setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
  printf("%ld\n", r);
  return 0;   
}