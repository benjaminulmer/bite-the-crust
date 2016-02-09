#pragma once
#include "Vehicle.h"
#include <iostream>
#include <rapidjson\filereadstream.h>
#include <rapidjson\document.h>

namespace ContentLoading
{
	bool loadVehicleData(char* filename, Vehicle* vehicle);
};

