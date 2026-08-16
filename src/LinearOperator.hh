#ifndef LINEAR_OPERATOR_HH
#define LINEAR_OPERATOR_HH

namespace hdnum {

    // Template class for a linear operator
    template<class T>
    class LinearOperator {
    public:
        // Pure virtual function to apply the linear operator
        virtual void apply(const Vector<T>& in, Vector<T>& out) const = 0;

        // Virtual destructor for proper cleanup of derived classes
        virtual ~LinearOperator() {}
    };
}

#endif // LINEAR_OPERATOR_HH

