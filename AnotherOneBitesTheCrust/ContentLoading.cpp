#include "ContentLoading.h"

namespace ContentLoading {

bool loadVehicleData(char* filename, Vehicle* vehicle) {
	FILE* filePointer = fopen(filename, "rb");
	if (filePointer == NULL) {
		printf("Error, vehicle file couldn't load.");
		return 0;
	}
	char readBuffer[10000];
	rapidjson::FileReadStream reader(filePointer, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(reader);
	if (d.HasMember("mass")) {
		vehicle->chassisMass = (float)d["mass"].GetDouble();
	}
	vehicle->updateTuning();

	fclose(filePointer);

	return true;
}

}