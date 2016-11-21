#pragma once

#include <vector>

#ifdef _WIN64
typedef unsigned __int64 size_t;
#else
typedef unsigned int     size_t;
#endif

template <typename T>
class ModCurrentsBase
{
public:
    void integrate(T dt);

    size_t getNumCurrents() const;
    T getCurrent(size_t idx) const;

protected:
    std::vector<T*> _currents;
    std::vector<T*> _states;
    std::vector<T*> _derivatives;
};