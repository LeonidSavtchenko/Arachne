#pragma once

// Synaptic Conductance Matrix Type
enum ScmType
{
    // Types AllZeros, AllEqual: the matrix is not generated explicitly.
    AllZeros = 1,           // All elements in a matrix are zeros
    AllEqual = 2,           // All elements in a matrix are equal
    
    // Types HstDense, HstSparse: the matrix is generated in Matlab and saved to input MAT-file with other data.
    HstDense = 3,           // A matrix is populated with custom elements and has dense structure
    HstSparse = 4,          // A matrix is populated with custom elements and has sparse structure

    // Types KrnDense, KrnSparse, KrnOneBit: the matrix is generated in HPC kernel before 1st iteration already in distributed form.
    KrnDense = 5,           // An element of the matrix is stored in floating-point-number format, the matrix has dense structure
    KrnSparse = 6,          // An element of the matrix is stored in floating-point-number format, the matrix has sparse structure
    KrnOneBit = 7,          // An element of the matrix is stored in one bit, the matrix has dense structure

    // Type KrnInPlace: the matrix is re-generated on each iteration.
    KrnInPlace = 8          // Each element is generated each time when it's used
};