#!/usr/bin/env python3

import sympy as sp
import numpy as np

def integrate(function, a, b):
    x = sp.Symbol('x', real=True)
    f = function
    res = sp.integrate(f, (x, a, b))
    print(res)
    return res

def plot(function, a, b):
    x = sp.Symbol('x', real=True)
    f = function
    sp.plot(f, (x, a, b), title="Plot of f(x)")

def f(x):
    return 4*x**4 - 3*x**3 + 2*x**2 - x + 5

def createTestData(randomGenerator, min, max, size, dtype):
    
    array1 = randomGenerator(min, max, size=size, dtype=dtype)
    array2 = randomGenerator(min, max, size=size, dtype=dtype)

    # Sum still stays in float32
    add_array = array1 + array2
    subtract_array = array1 - array2
    multiply_array = array1 * array2
    divide_array = array1 / array2
    scalar_array = 2 * array1
    dot_product = np.dot(array1, array2)
    sum_array = np.sum(array1)

    

    print("Array 1: "    + ",".join(map(str, array1)))
    print("Array 2: "    + ",".join(map(str, array2)))
    print("Sum: "        + ",".join(map(str, add_array)))
    print("Subtract: "   + ",".join(map(str, subtract_array)))
    print("Multiply: "   + ",".join(map(str, multiply_array)))
    print("Divide: "     + ",".join(map(str, divide_array)))
    print("Scalar: "     + ",".join(map(str, scalar_array)))
    print("Dot Product: " + str(dot_product))
    print("Sum of Array: " + str(sum_array))

def main():
    # x = np.linspace(0, 100, 101)
    # print(x)



    # x = sp.symbols('x', real=True)
    # f_poly = 4*x**4 - 3*x**3 + 2*x**2 - x + 5
    # integrate(f_poly, 1, 2)
    # plot(f_poly, 1, 2)

    # g_poly = 20*x**5 + sp.Rational(1, 12)*x**4 - 3*x**3 + sp.Rational(1, 2)*x**2 + 3*x + 100
    # integrate(g_poly, -10, 10)
    # plot(g_poly, -10, 10)

    # x = np.linspace(0, 2, 5)
    # y = f(x)
    # print(y)


    # x = sp.Symbol('x', real=True)
    # f = 4*x**4 - 3*x**3 + 2*x**2 - x + 5
    # res = sp.integrate(f, (x, 1, 2))
    # print(res)

    # g = 20*x**5 + sp.Rational(1,12)*x**4 - 3*x**3 + sp.Rational(1,2)*x**2 + 3*x + 100
    # sp.plot(g, (x, -10, 10), title="Plot of g(x)")



    # array1 = np.array([1, 2, 3, 4, 5, -6, -7, -8, -9, -10])
    # array2 = np.array([1, 2, 3, 4, 5, -6, -7, -8, -9, -10])
    # scalar1 = 2
    # scalar2 = 3
    # array1 = array1 * scalar1
    # array2 = array2 * scalar2
    # print(np.dot(array1, array2))

    # array3 = np.array([1, 2, 3, 4, 5, -6, -7, -8, -9, -10])
    # print(np.sum(array3))

    # array4 = np.array([0.15176857, 0.05467812, 0.77583873, 0.98366106, 0.48010835,
    #                    0.92273104, 0.50269300, 0.73995620, 0.96952440, 0.00317768], dtype=np.float32)
    # array5 = np.array([0.08099137, 0.55122465, 0.98421484, 0.29718230, 0.29520285,
    #                    0.20751731, 0.31523730, 0.19125506, 0.90119886, 0.43635517], dtype=np.float32)

    # Array 1: 330,500,691,-267,342,531,-771,211,-992,807
    # Array 2: 731,748,831,962,-881,297,656,-983,607,-883
    # array4 = np.array([330, 500, 691, -267, 342, 531, -771, 211, -992, 807], dtype=np.int32)
    # array5 = np.array([731, 748, 831, 962, -881, 297, 656, -983, 607, -883], dtype=np.int32)

    # Array 1: 994,967,645,961,190,707,308,109,865,401
    # Array 2: 130,804,275,291,379,41,37,385,263,131
    # array4 = np.array([994, 967, 645, 961, 190, 707, 308, 109, 865, 401], dtype=np.uint32)
    # array5 = np.array([130, 804, 275, 291, 379, 41, 37, 385, 263, 131], dtype=np.uint32)


    # generate non-zero random integer arrays to avoid division by zero
    # createTestData(-128, 127, 10, np.int8)
    # automatically generate test data for all integer types



    for tname in ['int8','int16','uint8','uint16']:
        dt   = np.dtype(tname)         # create a dtype object :contentReference[oaicite:3]{index=3}
        info = np.iinfo(dt)
        print(f"{dt.name:>8s} → min={info.min}, max={info.max}")

        if dt.kind == 'u':
            min = 0
        else:
            min = info.min
        max = info.max
        size = 10
        dtype = dt
        createTestData(np.random.randint, min, max, size, dtype)
        
        

if __name__ == '__main__':
    main()