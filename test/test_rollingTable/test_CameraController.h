#include <unity.h>
#include "../../src/rollingTable/CameraController.h"
#include "ArduCAM.h"

using namespace RollingTable;
void TestAll_CameraController();
void Test_SizeOfCameraFIFO();
void Test_CameraTest();
inline const char * const BoolToString(bool b);