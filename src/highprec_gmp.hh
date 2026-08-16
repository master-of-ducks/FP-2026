#include <cmath>

#ifndef HIGHPREC_GMP_HH
#define HIGHPREC_GMP_HH

#ifdef HDNUM_HAS_GMP

namespace hdnum {

// This is the GMP number type wrapped in a C++ interface with 
// precision as a template parameter
template <int m>  // select mantissa
class FP {
public:

    // constructor without arguments, initializes to zero
    FP () : number(0.0,m) 
    {
        mpf_set_default_prec(m);
    }

    // construction from double
    FP(const double &n) : number(n,m) 
    {
        mpf_set_default_prec(m);
    }

    // construction from float
    FP(const float &n) : number(n,m) 
    {
        mpf_set_default_prec(m);
    }

    // construction from int
    FP(const int &n) : number((double)n,m) 
    {
        mpf_set_default_prec(m);
    }

    // construction from mpf_class
    FP(const mpf_class &n) : number(n) {}

    // construction from FP type with possibly different accuracy
    template<int mm>
    FP(const FP<mm> &n) : number(n,m) {}

    // assignment from FP type with possibly different accuracy
    template <int mm>
    void operator=(const FP<mm> &n) {
        number = n.number;
    }

    // assignment from float
    void operator=(const float &n) {
        number = n;
    }

    // assignment from double
    void operator=(const double &n) {
        number = n;
    }

    // assignment from int
    void operator=(const int &n) {
        number = (double) n;
    }

    // addition
    FP<m> operator+(const FP<m> &n) const {
        return FP<m>(number+n.number);
    }

    // in place addition
    void operator+=(const FP<m> &n) {
        number += n.number;
    }

    // subtraction
    FP<m> operator-(const FP<m> &n) const {
        return FP<m>(number-n.number);
    }

    // in place subtraction
    void operator-=(const FP<m> &n) {
        number -= n.number;
    }

    // multiplication
    FP<m> operator*(const FP<m> &n) const {
        return FP<m>(number*n.number);
    }

    // unary minus
    FP<m> operator-() const {
        return FP<m>(-number);
    }

    // in place multiplication
    void operator*=(const FP<m> &n) {
        number *= n.number;
    }

    // division
    FP<m> operator/(const FP<m> &n) const {
        return FP<m>(number/n.number);
    }

    // in place division
    void operator/=(const FP<m> &n) {
        number /= n.number;
    }

    // access to the wrapped mpf_class
    mpf_class getNumber() const 
    { 
        return number; 
    }

    // conversion to double
    operator mpf_class() const { return number; }

    bool operator< (const FP<m>& other) const
    {
        return number<other.number;
    }

    bool operator<= (const FP<m>& other) const
    {
        return number<=other.number;
    }

    bool operator> (const FP<m>& other) const
    {
        return number>other.number;
    }

    bool operator>= (const FP<m>& other) const
    {
        return number>other.number;
    }

    bool operator== (const FP<m>& other) const
    {
        return number==other.number;
    }

    bool operator!= (const FP<m>& other) const
    {
        return number!=other.number;
    }

private:
    // stores a double to represent the low precision number
    mpf_class number;
};

template <int m>
FP<m> sqrt (const FP<m>& x)
{
    return FP<m>(sqrt(x.getNumber()));
}

template <int m>
FP<m> log (const FP<m>& x)
{
    return FP<m>(std::log(x.getNumber().get_d()));
}

template <int m>
FP<m> exp (const FP<m>& x)
{
    return FP<m>(std::exp(x.getNumber().get_d()));
}

template <int m>
FP<m> max (const FP<m>& x, const FP<m>& y)
{
    if (x>y)
        return x;
    else
        return y;
}

template <int m>
FP<m> min (const FP<m>& x, const FP<m>& y)
{
    if (x<y)
        return x;
    else
        return y;
}

template <int m>
FP<m> abs (const FP<m>& x)
{
    return FP<m>(abs(x.getNumber()));
}

template <int m>
std::ostream &operator<<(std::ostream &os, FP<m> const &a) {
    os << a.getNumber();
    return os;
}
}  // namespace hdnum

#endif
#endif
