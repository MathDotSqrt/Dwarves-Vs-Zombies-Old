#include <stdlib.h>
#include <stdio.h>

#include "example.h"
#include "entt.hpp"
#include <openssl/pem.h> 

int main(){
  printf("Number one: %d\n", calc_number(12));
  printf("Number t: %zu\n", sizeof(void*));
  return 0;
}
