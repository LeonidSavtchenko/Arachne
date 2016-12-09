#pragma once

#include <vector>
#include <memory>
#include <tuple>

#include "Containers/DistVector.h"



template <typename T>
class AllModCurrents
{
    public:
    AllModCurrents() = default;
    
    virtual void SetVoltage(const DistVector<T>& v) { }
    
    virtual DistVector<T> GetSumCurrent(int length)
    {
        DistVector<T> I(length);
        I.AssignZeros();
        
        return I;
    }
    
    virtual DistVector<T> DoOneStepPart1(const DistVector<T>& v, T dt05)
    {
        DistVector<T> I(v.length);
        I.AssignZeros();
        
        return I;
    }
    
    virtual DistVector<T> DoOneStepPart2(const DistVector<T> &v, const DistVector<T> &v_tmp, T dt05)
    {
        DistVector<T> I(v.length);
        I.AssignZeros();
        
        return I;
    }
    
};

template <typename T>
class AllModCurrents_e : public AllModCurrents<T>
{
public:
    AllModCurrents_e(int num_e) { }
};

template <typename T>
class AllModCurrents_i : public AllModCurrents<T>
{
public:
    AllModCurrents_i(int num_i) { }
};
