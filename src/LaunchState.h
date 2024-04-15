#ifndef LaunchState_h
#define LaunchState_h

#include <Arduino.h>
#include "CommonIncludes.h"

namespace LaunchStateNamespace {

    enum class LaunchState{
        PreIgnition,
        Ignition_to_Apogee,
        Thousand_ft,
        Descent,
        Touchdown,
    };

    extern LaunchState current_state;
    void HalyaStateMachine(SixDOF &_6DOF, PHT &alt, MPU9250 &mpu, GPS &gps2);

} // namespace LaunchStateNamespace

#endif
