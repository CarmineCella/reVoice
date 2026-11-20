// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "NRev.h"
#include <cmath>

namespace revoice {
// Reuse STK's NRev topology/tick without changing STK's process-global sample
// rate. Delay topology: Perry R. Cook and Gary P. Scavone, STK 1995-2023;
// see the bundled STK license. Each engine owns its rate and decay coefficients.
class InstanceReverb final : public stk::NRev {
    double rate_;
public:
    InstanceReverb(double rate,double seconds) : stk::NRev(seconds),rate_(rate) {
        const int lengths[]={1433,1601,1867,2053,2251,2399,347,113,37,59,53,43,37,29};
        for(int i=0;i<14;++i) {
            unsigned delay=static_cast<unsigned>(std::floor(rate_*lengths[i]/25641.0));
            if((delay&1)==0) ++delay;
            while(!isPrime(delay)) delay+=2;
            auto& line=i<6?combDelays_[i]:allpassDelays_[i-6];
            line.setMaximumDelay(delay); line.setDelay(delay);
        }
        setT60(seconds); clear();
    }
    void setT60(double seconds) noexcept {
        for(int i=0;i<6;++i)
            combCoefficient_[i]=std::pow(10.0,-3.0*combDelays_[i].getDelay()/(seconds*rate_));
    }
};
} // namespace revoice
