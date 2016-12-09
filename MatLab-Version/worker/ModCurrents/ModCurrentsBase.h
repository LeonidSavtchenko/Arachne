#pragma once

#include <vector>

template <typename T>
class ModCurrentsBase
{
public:
    void integrate(T dt);

    std::size_t getNumCurrents() const;
    T getCurrent(std::size_t idx) const;
    T getResCurrent() const;

    virtual void init() {}
    virtual void states() {}
    virtual void currents() {}

public:
    const T PI = (T)3.14159265358979323846; 
    const T E = (T)2.71828182845904523536;
    const T GAMMA = (T)0.57721566490153286060;
    const T DEG = (T)57.29577951308232087680;
    const T PHI = (T)1.61803398874989484820;    // golden ratio
    const T FARADAY = (T)96485.309;             // coulombs/mole
    const T R = (T)8.31441;                     // molar gas constant

public:
    T v;
    static T t;

protected:
    std::vector<T*> _currents;
    std::vector<T*> _states;
    std::vector<T*> _derivatives;
};