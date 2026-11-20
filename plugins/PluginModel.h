// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "revoice/Parameters.h"
#include "pluginterfaces/base/ibstream.h"
#include <cstdint>
#include <optional>

namespace revoice::plugin {
// Host IDs are hashes of persisted keys, independent of enum order.
std::uint32_t parameterId(Param id);
std::optional<Param> parameterFromId(std::uint32_t id);
bool exposed(Param id);
bool gainParameter(Param id);
double fromNormalized(Param id,double normalized);
double toNormalized(Param id,double value);
bool readState(Steinberg::IBStream* stream,Parameters& parameters);
bool writeState(Steinberg::IBStream* stream,const Parameters& parameters);
}
