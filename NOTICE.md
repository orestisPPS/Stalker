STAΛKER Linear Algebra Library
Copyright 2024-2026 Papas Christoforos Orestis

I. ACKNOWLEDGMENTS

STAΛKER is a high-performance, header-only C++ linear algebra library developed by Papas Christoforos Orestis.

This project's core library (located in the include/ directory) is 
independent and has no external dependencies other than the C++ 
Standard Library (STL).

II. THIRD-PARTY COMPONENTS

The STAΛKER distribution includes a benchmark suite (located in the 
Benchmarks/ directory) which, for comparison and evaluation purposes, 
links against the following third-party software:

1. Eigen (Mozilla Public License v. 2.0)
   Copyright (c) Eigen authors.
   Available at: https://eigen.tuxfamily.org/

2. OpenBLAS (BSD 3-Clause License)
   Copyright (c) The OpenBLAS Project.
   Available at: https://github.com/OpenMathLib/OpenBLAS

3. PAPI - Performance Application Programming Interface (BSD License)
   Copyright (c) Innovative Computing Laboratory, University of Tennessee.
   Available at: https://icl.utk.edu/papi/

The licenses for these third-party components are provided for 
informational purposes and apply only to the benchmark suite and its 
execution. The STAΛKER core library remains governed solely by the 
Apache License, Version 2.0.