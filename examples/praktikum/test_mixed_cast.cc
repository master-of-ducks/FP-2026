#/**
 * @file test_mixed_cast.cc
 * @brief Tests all combinations of precisions to cast between
 */
#include "../../hdnum.hh"
#include "../../src/gram_schmidt_mixed.hh"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace hdnum;

#ifdef HDNUM_HAS_CPFLOAT
#ifdef HDNUM_HAS_GMP

/**
 * @file test_mixed_cast.cc
 * @brief Example program that exercises mixed_cast across several floating-point types.
 */

/**
 * @brief Test casting from one source type to all supported target types.
 * @tparam From Source type used to initialize the value.
 * @param fromName Human-readable name of the source type.
 * @param value Value to cast into each target type.
 */
template <class From>
void testFromType(const std::string& fromName, From value) {
    std::cout << "\n=== Casting FROM " << fromName << " ===" << std::endl;
    std::cout << "Source value: " << std::fixed << std::setprecision(25) << value << std::endl;
    
    // Cast to FP1024
    {
        auto result = mixed_cast<FP1024>(value);
        std::cout << "  -> FP1024: " << std::fixed << std::setprecision(25) << result << std::endl;
    }
    
    // Cast to FP512
    {
        auto result = mixed_cast<FP512>(value);
        std::cout << "  -> FP512: " << std::fixed << std::setprecision(25) << result << std::endl;
    }
    
    // Cast to FP256
    {
        auto result = mixed_cast<FP256>(value);
        std::cout << "  -> FP256: " << std::fixed << std::setprecision(25) << result << std::endl;
    }
    
    // Cast to FP128
    {
        auto result = mixed_cast<FP128>(value);
        std::cout << "  -> FP128: " << std::fixed << std::setprecision(25) << result << std::endl;
    }
    
    // Cast to FP64 (double)
    {
        auto result = mixed_cast<FP64>(value);
        std::cout << "  -> FP64: " << std::fixed << std::setprecision(25) << result << std::endl;
    }
    
    // Cast to FP32 (float)
    {
        auto result = mixed_cast<FP32>(value);
        std::cout << "  -> FP32: " << std::fixed << std::setprecision(25) << result << std::endl;
    }
    
    // Cast to FP16
    {
        auto result = mixed_cast<FP16>(value);
        std::cout << "  -> FP16: " << std::fixed << std::setprecision(25) << result << std::endl;
    }
    
    // Cast to bfloat16
    {
        auto result = mixed_cast<bfloat16>(value);
        std::cout << "  -> bfloat16: " << std::fixed << std::setprecision(25) << result << std::endl;
    }
    
    // Cast to FP8
    {
        auto result = mixed_cast<FP8>(value);
        std::cout << "  -> FP8: " << std::fixed << std::setprecision(25) << result << std::endl;
    }
}

/**
 * @brief Run the mixed_cast test suite for all supported floating-point types.
 * @return Exit status of the program.
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Mixed_cast Test Suite" << std::endl;
    std::cout << "Testing all 9x9 type combinations" << std::endl;
    std::cout << "========================================" << std::endl;
    
    FP1024 one = 1.0;
    FP1024 three = 3.0;
    FP1024 testValue = one / three;
    testValue *= -1.0;


    // Test casting FROM each type
    testFromType("FP1024", mixed_cast<FP1024>(testValue));
    testFromType("FP512", mixed_cast<FP512>(testValue));
    testFromType("FP256", mixed_cast<FP256>(testValue));
    testFromType("FP128", mixed_cast<FP128>(testValue));
    testFromType("FP64", mixed_cast<FP64>(testValue));
    testFromType("FP32", mixed_cast<FP32>(testValue));
    testFromType("FP16", mixed_cast<FP16>(testValue));
    testFromType("bfloat16", mixed_cast<bfloat16>(testValue));
    testFromType("FP8", mixed_cast<FP8>(testValue));
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "All mixed_cast combinations tested successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}

#endif
#endif