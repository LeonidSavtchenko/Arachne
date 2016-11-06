#pragma once

#include <vector>

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