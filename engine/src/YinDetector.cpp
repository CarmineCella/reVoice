// SPDX-License-Identifier: BSD-2-Clause
#include "revoice/YinDetector.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace revoice {
namespace { constexpr double Pi=3.14159265358979323846; }
double YinDetector::Biquad::process(double sample) noexcept {
    const double out=b0*sample+z1;
    z1=b1*sample-a1*out+z2; z2=b2*sample-a2*out;
    return out;
}
YinDetector::YinDetector(double rate,double minimum,double maximum,double rmsGate)
    : minimum_(minimum),maximum_(maximum) {
    if(!std::isfinite(rate) || rate<8000 || rate>192000 || !std::isfinite(minimum) ||
       !std::isfinite(maximum) || minimum<20 || maximum<=minimum || maximum>rate*.45)
        throw std::invalid_argument("Invalid YIN sample rate or frequency range");
    // Bound the difference calculation while retaining at least four samples
    // per highest permitted pitch. Filter BEFORE decimation to avoid aliasing.
    decimation_=std::max(1u,unsigned(rate/std::max(12000.0,4*maximum)));
    analysisRate_=rate/decimation_;
    const double omega=2*Pi*(analysisRate_*.4)/rate,c=std::cos(omega),s=std::sin(omega);
    const double qualities[]={.541196100146197,1.306562964876377};
    for(std::size_t i=0;i<filters_.size();++i) {
        const double alpha=s/(2*qualities[i]),a0=1+alpha;
        auto& f=filters_[i];
        f.b0=f.b2=(1-c)/(2*a0); f.b1=(1-c)/a0;
        f.a1=-2*c/a0; f.a2=(1-alpha)/a0;
    }
    minLag_=std::max<std::size_t>(2,std::size_t(std::floor(analysisRate_/maximum)));
    maxLag_=std::size_t(std::ceil(analysisRate_/minimum));
    window_=maxLag_; hop_=std::max<std::size_t>(1,std::size_t(analysisRate_*.005));
    ring_.resize(window_+maxLag_+1); samples_.resize(ring_.size());
    difference_.resize(maxLag_+2); gate(rmsGate); reset();
}
void YinDetector::gate(double rms) noexcept { gate_=std::isfinite(rms)?std::clamp(rms,0.0,1.0):.003; }
void YinDetector::reset() noexcept {
    std::fill(ring_.begin(),ring_.end(),0); std::fill(samples_.begin(),samples_.end(),0);
    std::fill(difference_.begin(),difference_.end(),0);
    for(auto& f:filters_) f.z1=f.z2=0;
    cursor_=filled_=decimationPhase_=0; untilAnalysis_=hop_; pitch_=confidence_=0;
}
double YinDetector::process(double sample) noexcept {
    sample=std::isfinite(sample)?std::clamp(sample,-4.0,4.0):0;
    for(auto& f:filters_) sample=f.process(sample);
    if(++decimationPhase_<decimation_) return pitch_;
    decimationPhase_=0;
    ring_[cursor_]=sample; cursor_=(cursor_+1)%ring_.size();
    if(filled_<ring_.size()) ++filled_;
    if(--untilAnalysis_==0) {
        untilAnalysis_=hop_;
        if(filled_==ring_.size()) analyze();
    }
    return pitch_;
}
void YinDetector::analyze() noexcept {
    double sum=0,energy=0;
    for(std::size_t i=0;i<samples_.size();++i) {
        const double sample=ring_[(cursor_+i)%ring_.size()];
        samples_[i]=sample; sum+=sample; energy+=sample*sample;
    }
    const double mean=sum/samples_.size();
    pitch_=confidence_=0;
    // AC RMS, so a DC offset cannot open the gate. Unlike a peak-band gate,
    // this threshold is independent of the distribution of vocal harmonics.
    if(energy/samples_.size()-mean*mean<std::max(gate_*gate_,1e-12)) return;
    // YIN squared difference and cumulative mean normalization, equations 6/8
    // in de Cheveigne & Kawahara (2002), doi:10.1121/1.1458024.
    double running=0; difference_[0]=1;
    for(std::size_t lag=1;lag<=maxLag_+1;++lag) {
        double difference=0;
        for(std::size_t j=0;j<window_;++j) {
            const double delta=samples_[j]-samples_[j+lag]; difference+=delta*delta;
        }
        running+=difference;
        difference_[lag]=running>1e-20?difference*lag/running:1;
    }
    const auto interpolated=[&](std::size_t lag) {
        const double left=difference_[lag-1],mid=difference_[lag],right=difference_[lag+1];
        const double curvature=left-2*mid+right;
        const double offset=curvature>1e-12?std::clamp(.5*(left-right)/curvature,-.5,.5):0;
        return std::array<double,2>{{offset,mid+.5*(right-left)*offset+.5*curvature*offset*offset}};
    };
    std::size_t best=0;
    for(std::size_t lag=minLag_;lag<=maxLag_;++lag) {
        // Test the interpolated dip, not only an integer lag. At high pitches
        // a half-sample lag error can otherwise cause an octave-down decision.
        if(difference_[lag]<=difference_[lag-1] && difference_[lag]<=difference_[lag+1] && interpolated(lag)[1]<.15) {
            best=lag; break;
        }
    }
    if(!best) {
        // A relaxed fallback still requires a periodic signal. Never return
        // an arbitrary minimum for noise or an unvoiced consonant.
        best=std::size_t(std::min_element(difference_.begin()+minLag_,
            difference_.begin()+maxLag_+1)-difference_.begin());
        if(interpolated(best)[1]>.2) return;
    }
    const auto dip=interpolated(best);
    const double hz=analysisRate_/(best+dip[0]);
    if(hz>=minimum_*.99 && hz<=maximum_*1.01) {
        pitch_=std::clamp(hz,minimum_,maximum_); confidence_=std::clamp(1-dip[1],0.0,1.0);
    }
}
} // namespace revoice
