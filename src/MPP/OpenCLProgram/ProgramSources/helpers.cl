  
#ifndef HELPERS_GUARD
#define HELPERS_GUARD

  //will execute faster than de "generic" modulo operator
  #define BASE_2_MODULO (val, base2Val) ( (val) & ( (base2val) -1 ) )
  
  #define WARP_SIZE 32
  #define HALF_WARP_SIZE 16
  
#endif //HELPERS_GUARD
