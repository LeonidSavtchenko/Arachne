// ModTranslatorTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "AutogenCode.h"

template <typename T>
void Test();

void main()
{
    Test<float>();
    Test<double>();

    std::cin.get();
}

template <typename T>
void Test()
{
    ModCurrents<T> modCurrents((T)6.3);

    modCurrents.v = (T)0.1;
    modCurrents.init();

    T dt = (T)0.5;

    // Iteration 1
    modCurrents.v = (T)0.2;
    modCurrents.states();
    modCurrents.integrate(dt);
    modCurrents.currents();

    // Iteration 2
    modCurrents.v = (T)0.3;
    modCurrents.states();
    modCurrents.integrate(dt);
    modCurrents.currents();

    // Print currents
    size_t numCurrents = modCurrents.getNumCurrents();
    for (size_t i = 0; i < numCurrents; i++)
    {
        std::cout << modCurrents.getCurrent(i) << std::endl;
    }
}
