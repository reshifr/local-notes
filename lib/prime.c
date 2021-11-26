#include <stdio.h>
#include <inttypes.h>

int is_prime(uint64_t n) {
  if( n<=3 )
    return n>1;
  if( n%2==0 || n%3==0 )
    return 0;
  uint64_t i = 5;
  while( (i*i)<=n ) {
    if( n%i==0 || n%(i+2)==0 )
      return 0;
    i += 6;
  }
  return 1;
}

uint64_t closest_prime(uint64_t n) {
  uint64_t l, r, nl, nr;
  l = r = nl = nr = 0;
  for(uint64_t i=n; i>0; --i, ++l) {
    if( is_prime(i) ) {
      nl = i;
      break;
    }
  }
  for(uint64_t i=n;; ++i, ++r) {
    if( is_prime(i) ) {
      nr = i;
      break;
    }
  }
  return r<=l ? nr : nl;
}

int main(void) {
  for(int i=3; i<62; ++i) {
    printf("%d = %"PRIu64"\n", i, closest_prime((uint64_t)1<<i));
  }
}
