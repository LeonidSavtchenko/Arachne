#include "ModCurrentsBase.h"

template <typename T>
void ModCurrentsBase<T>::integrate(T dt)
{
    for (size_t i = 0; i < _states.size(); i++)
    {
        *_states[i] += *_derivatives[i] * dt;
    }
}

template <typename T>
size_t ModCurrentsBase<T>::getNumCurrents() const
{
    return _currents.size();
}

template <typename T>
T ModCurrentsBase<T>::getCurrent(size_t idx) const
{
    return *_currents[idx];
}

template
class ModCurrentsBase<float>;

template
class ModCurrentsBase<double>;