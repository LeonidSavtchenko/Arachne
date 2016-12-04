#include "ModCurrentsBase.h"

template <typename T>
void ModCurrentsBase<T>::integrate(T dt)
{
    for (std::size_t i = 0; i < _states.size(); ++i)
    {
        *_states[i] += *_derivatives[i] * dt;
    }
}

template <typename T>
std::size_t ModCurrentsBase<T>::getNumCurrents() const
{
    return _currents.size();
}

template <typename T>
T ModCurrentsBase<T>::getCurrent(std::size_t idx) const
{
    return *_currents[idx];
}

template <typename T>
T ModCurrentsBase<T>::getResCurrent() const
{
    T resCurrent = (T)0;
    for (std::size_t i = 0; i < _currents.size(); ++i)
    {
        resCurrent += *_currents[i];
    }

    return resCurrent;
}

template
class ModCurrentsBase<float>;

template
class ModCurrentsBase<double>;