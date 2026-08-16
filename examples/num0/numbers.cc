#include <iostream>  // notwendig zur Ausgabe
#include <experimental/simd>

#include "hdnum.hh"  // hdnum header

int main() 
{
    // Finde größtes i so dass 0.5 + 2^{-i} > 0.5

#ifdef HDNUM_HAS_GMP
    hdnum::FP1024 eps_1024;
    int i_1024 = hdnum::precision(eps_1024);  
    std::cout << "FP1024: " << "2^-" << i_1024 << std::endl;

    hdnum::FP512 eps_512;
    int i_512 = hdnum::precision(eps_512); 
    std::cout << "FP512: " << "2^-" << i_512 << std::endl;

    hdnum::FP256 eps_256;
    int i_256 = hdnum::precision(eps_256); 
    std::cout << "FP256: " << "2^-" << i_256 << std::endl;

    hdnum::FP128 eps_128;
    int i_128 = hdnum::precision(eps_128);  
    std::cout << "FP128: " << "2^-" << i_128 << std::endl;
#endif

    hdnum::FP64 eps_float;
    int i_float = hdnum::precision(eps_float); 
    std::cout << "double(FP64): " << "2^-" << i_float << std::endl; 

    hdnum::FP32 eps_double;
    int i_double = hdnum::precision(eps_double);
    std::cout << "float(FP32): " << "2^-" << i_double << std::endl;

#ifdef HDNUM_HAS_CPFLOAT
    hdnum::FP16 eps_FP16;
    auto i_FP16 = hdnum::precision(eps_FP16);
    std::cout << "FP16: " << "2^-" << i_FP16 << std::endl;

    hdnum::bfloat16 eps_bfloat16;
    auto i_bfloat16 = hdnum::precision(eps_bfloat16);
    std::cout << "bfloat16: " << "2^-" << i_bfloat16 << std::endl;

    hdnum::FP8 eps_FP8;
    int i_FP8 = hdnum::precision(eps_FP8);
    std::cout << "FP8: " << "2^-" << i_FP8 << std::endl;
#endif
}
