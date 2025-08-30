#pragma once

#include "edcolony/domain/models.hpp"
#include "edcolony/storage.hpp"

namespace edcolony {

bool persistProject(Storage& storage, const Project& p);
bool persistFleetCarrier(Storage& storage, const FleetCarrier& fc);

}


