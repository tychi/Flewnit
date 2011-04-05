  
#ifndef HELPERS_GUARD
#define HELPERS_GUARD

  //will execute faster than de "generic" modulo operator
  #define BASE_2_MODULO (val, base2Val) ( (val) & ( (base2val) -1 ) )
  
#endif //HELPERS_GUARD
