# Heidelberg Educational Numerics Library (HDNUM)

A simple to use and yet efficient C++ library for teaching numerical methods.

## License

HDNUM is licensed under version 2 of the GNU
General Public License, with the so-called "runtime exception", as
follows:

>   As a special exception, you may use the HDNUM source files as part
>   of a software library or application without restriction.
>   Specifically, if other files instantiate templates or use macros or
>   inline functions from one or more of the HDNUM source files, or you
>   compile one or more of the HDNUM source files and link them with
>   other files to produce an executable, this does not by itself cause
>   the resulting executable to be covered by the GNU General Public
>   License.  This exception does not however invalidate any other
>   reasons why the executable file might be covered by the GNU General
>   Public License.

This licence clones the one of the libstdc++ library. For further
implications of this library please see their [license page][0].

See the file [COPYING.md][] for full copying permissions.

## Installation

There is no installation. Just include the header file `hdnum.hh` and that is it. 

In order to work with hdnum you need a C++ compiler

Now wait a minute. There are things you have to prepare when you want to use 
other number types than the standard ones.

### CPFloat 

CPFloat is a library that simulates floating point numbers with lower precision. 
Within certain bounds you can specify the number of bits in mantissa and exponent.
Internally it stores a double and then rounds after each operation to the required 
accuracy and range. In order to use cpfloat simply copy the cpfloat from GitHub
Into the top level hdnum directory by typing

    cd <your hdnum directory>
    git clone https://github.com/north-numerical-computing/cpfloat.git

Now the directory should look something like this:

    COPYING.md       benchmark/       make.def         tutorial/
    Doxyfile         cpfloat/         programmierkurs/
    README.md        examples/        src/
    TODO.md          hdnum.hh         test/

Now go into the cpfloat directory and type "make lib":

    cd cpfloat
    make lib

This will compile the library. HDNUM needs to find the header file
cpfloat.h in cpfloat/build/include and the library
libcpfloat.a in cpfloat/build/lib


### GNU multiprecision library 

The GNU multiprecision library supports floating point computations with very high precision.
It should be installed with the package manager on your system and the package is often called "gmp".


## Using the Makefiles

Since hdnum is a header only library you can easily compile your code using the command line.
When your code uses cpfloat or gmp you need to specify additional header files and link to appropriate libraries.
In this case makefiles are a convenient way to reduce the typing load. On the other hand these makefiles need to be adapted to your specific installation, i.e. name of the compiler, location of libraries and header files etc.
For that edit the file make.def in the top level directory to define your compiler command,
compilation flags, linker flags, and flags related to the GNU multiprecision library.
Compiler should be C++17 at least.

If you have set up the makefiles properly you can simply type "make" in examples/num 0 type "make" to compile all programs that do not need GMP support.
Write "make gmp" to build all programs needing GMP support and write "make all" to make them all.
"make clean" removes all executables.

## Building the documentation

In the `hdnum` top-level directory just run `doxygen`. This will build
the Doxygen documentation into the directory `doc/html` or `doc/latex`,
respectively.

## History

-    Version 0.11 Revision 1620
-    Version 0.12 from November, 5 2009.
-    Version 0.20 from April, 21 2011.
-    Version 0.22 from May, 11 2011. (Add more methods and documentation.)
-    Version 0.23 from June, 14 2011. (Do not pass by reference for element-wise operations!)
-    Version 0.24 from September, 9 2011. (import methods to solve odes/pdes)
-    Version 0.25 from October, 20 2013. (add exceptions in linear algebra)
-    Version 0.26 from October, 24 2013. delete countingptr and arrays
-    no version numbers .. reworked makefiles April, 30 2020
-    Version 0.27 from Marc, 15 2021. add the result of a practical (QR Methods / SparseMatrix)

## Links

[0]: https://gcc.gnu.org/onlinedocs/libstdc++/faq.html#faq.license
[COPYING.md]: COPYING.md
