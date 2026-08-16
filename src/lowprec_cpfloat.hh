#ifndef LOWPREC_CP_FLOAT_HH
#define LOWPREC_CP_FLOAT_HH

#ifdef HDNUM_HAS_CPFLOAT

extern "C" {
#include "cpfloat.h"
}
#include <iostream>

namespace hdnum {

// Singleton pattern
template <int m, int e>
class PrecisionSetter {
public:
    static PrecisionSetter *getInstance() {
        static PrecisionSetter instance;
        return &instance;
    }
    optstruct *getFpopts() const { return fpopts; }

    optstruct * get_initialized_opstruct ()
    {
        optstruct *p = init_optstruct();
        setParameters(p);
        return p;
    }

    void setParameters (optstruct *p) const {
        p->precision = m;                 // Bits in the significand + 1.
        p->emax = (1 << (e-1))-1;    // The maximum exponent value.
        p->emin = -(p->emax) +1;                     // The minimum exponent value .
        std::cout << "CPFloatSingleton<" << m << "," << e << "> precision=" << p->precision << " emax=" << p->emax << " emin=" << p->emin << std::endl;
        p->subnormal = CPFLOAT_SUBN_USE;  // Support for subnormals is on.
        p->round = CPFLOAT_RND_NE;  // Round toward round to nearest with ties to even
        p->flip = CPFLOAT_SOFTERR_NO;  // Bit flips are off.
        p->p = 0;                      // Bit flip probability (not used).
        p->explim = CPFLOAT_EXPRANGE_TARG;  // Limited exponent in target format.
        int retval = cpfloat_validate_optstruct(p);
        if (retval != 0)
        {
            std::cout << "cpfloat_validate_optstruct returned " << retval << std::endl;
            return exit(retval);
        }
    }
    ~PrecisionSetter() { free_optstruct(fpopts); }
    PrecisionSetter(PrecisionSetter const &) = delete;
    void operator=(PrecisionSetter const &) = delete;

private:
    PrecisionSetter() {}
    optstruct *fpopts = get_initialized_opstruct();
};

// This is the CPFloat number type wrapped in a C++ interface
// Based on the bachelor thesis of Kevin Heibel from June 2024
template <int m, int e>  // select mantissa and exponent
class CPFloat {
public:

    // constructor without arguments, initializes to zero
    CPFloat () : number(0) {}

    // construction from double
    CPFloat(const double &n) {
        number = n;
        cpfloat(&number, &number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
    }

    // construction from another CPFloat with possibly other precision
    template<int mm, int ee>
    CPFloat (const CPFloat<mm,ee> &n) {
        number = n.getNumber();
        cpfloat(&number, &number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
    }

    // assignment
    template <int mm, int ee>
    void operator=(const CPFloat<mm, ee> &n) {
        number = n.getNumber();
        cpfloat(&number, &number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
    }

    // access to the wrapped double
    double getNumber() const 
    { 
        return number; 
    }

    // addition
    CPFloat operator+(const CPFloat &n) const {
        double r;
        cpf_add(&r, &n.number, &number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
        return CPFloat(r);
    }

    // in place addition
    void operator+=(const CPFloat &n) {
        cpf_add(&number, &n.number, &number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
    }

    // subtraction
    CPFloat operator-(const CPFloat &n) const {
        double r;
        cpf_sub(&r, &number, &n.number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
        return CPFloat(r);
    }

    // in place subtraction
    void operator-=(const CPFloat &n) {
        cpf_sub(&number, &number, &n.number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
    }

    // multiplication
    CPFloat operator*(const CPFloat &n) const {
        double r;
        cpf_mul(&r, &n.number, &number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
        return CPFloat(r);
    }

    // in place multiplication
    void operator*=(const CPFloat &n) {
        cpf_mul(&number, &n.number, &number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
    }

    // division
    CPFloat operator/(const CPFloat &n) const {
        double r;
        cpf_div(&r, &number, &n.number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
        return CPFloat(r);
    }

    // in place division
    void operator/=(const CPFloat &n) {
        cpf_div(&number, &number, &n.number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
    }

    // square root
    CPFloat sqrt() const {
        double r;
        cpf_sqrt(&r, &number, 1, PrecisionSetter<m,e>::getInstance()->getFpopts());
        return CPFloat(r);
    }

    // conversion to double
    operator double() const { return this->getNumber(); }

private:
    // stores a double to represent the low precision number
    double number;
};

template <int m, int e>
std::ostream &operator<<(std::ostream &os, CPFloat<m, e> const &a) {
    os << a.getNumber();
    return os;
}
}  // namespace hdnum

#endif
#endif
