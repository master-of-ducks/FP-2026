// -*- tab-width: 4; indent-tabs-mode: nil -*-
#ifndef HDNUM_PRECISION_HH
#define HDNUM_PRECISION_HH

/** @file
 *  @brief find machine precision for given float type
 */

namespace hdnum {

// return largest i such that 0.5 + 2^{-i} > 0.5
template <typename X>
int precision(X& eps) {
    X x(eps), large(eps), largex(eps), two(eps);
    large = 0.5;
    two = 2.0;
    x = 0.5;
    largex = large + x;
    int i(0);
    while (largex > large) {
        eps = x;
        i = i + 1;
        // std::cout << i << " " << std::scientific << std::showpoint
        // 					<< std::setprecision(20) << large+x << " " << x << std::endl;
        x = x * large;
        largex = large + x;
    }
        // std::cout << i << " " << std::scientific << std::showpoint
        // 					<< std::setprecision(20) << large+x << " " << x << std::endl;
    return i;
}

}  // namespace hdnum

#endif
