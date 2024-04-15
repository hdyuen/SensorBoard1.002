#include "LaunchState.h"
#include <Arduino.h>

namespace LaunchStateNamespace{

    LaunchState current_state = LaunchState::PreIgnition;
    const int READINGS_LENGTH = 5;
    double rate_of_change;
    double AltArray[READINGS_LENGTH];
    double SECOND_IGNITION_HEIGHT = 65;
    int count = 0;
    int elems = 0;
    int negatives = 0;

    double returnAverage(double arr[], int len) {
        
        double changes[READINGS_LENGTH-1];
        for(count = 0; count < len-1; count++) {
            double difference = arr[count+1] - arr[count];
            changes[count] = difference;
            
        }
        for (count = 0; count < len - 1; count++)
        {
            if (changes[count] < 0)
            {
                negatives++;
            }
        }
        for (count = 0; count < len; count++)
        {
            Serial.println("Altitude ");
            Serial.print(count);
            Serial.println(AltArray[count]);
        }
        if (negatives >= 3)
        {
            Serial.println("Descending");
            return -1;
        }
        else
        {
            Serial.println("Not Descending");
            negatives = 0;
            return 1;
        }
    }
    //in the main loop, we will get sensor data from 6dof, altimeter, and mpu. 
    //then we will run HalyaStateMachine, which can determine correct state of the flight and run prioritized code for that state
    void HalyaStateMachine(SixDOF &_6DOF, PHT &alt, MPU9250 &mpu, GPS &gps2) {
        switch (current_state) {

            case LaunchState::PreIgnition:
                Serial.println("PreIgnition");
            
                // while(!_6DOF.check_IGNITABLE()){    //ignitable ALSO determined by CAN
                //     delay(200); //long delay to not waste power
                // }
                //Serial.println("halya ignition!");
                if (_6DOF.check_IGNITABLE())
                {
                    Serial.println("Halya Ignition");
                    current_state = LaunchState::Ignition_to_Apogee;
                    
                }
                else
                {
                    delay(200);
                }
                break;


            case LaunchState::Ignition_to_Apogee:
                Serial.println("Ignition to Apogee");
                //fill AltArray and check rate of change
                
                // while (ascent)
                // {
                if (elems < READINGS_LENGTH) {
                    Serial.println("Not filled yet");
                    AltArray[elems] = alt.getAltitude();
                    elems++;
                } 
                if(elems >= READINGS_LENGTH) {
                    Serial.println("Filled");
                    for (int i = 0; i < READINGS_LENGTH - 1; i++){
                        AltArray[i] = AltArray[i+1];
                    }
                    //Serial.println(alt.getAltitude());
                    AltArray[READINGS_LENGTH - 1] = alt.getAltitude(); //does altitude update in the loop? I think that it would just set everything in AltArray to the same value
                    rate_of_change = returnAverage(AltArray, READINGS_LENGTH);
                }
                if(rate_of_change < 0){
                    // digitalWrite(DROGUE_1, HIGH);
                    // delay(300);
                    // digitalWrite(DROGUE_1, LOW);
                    // digitalWrite(DROGUE_2, HIGH);
                    // delay(300);
                    // digitalWrite(DROGUE_2, LOW);
                    Serial.println("Halya has reached apogee!");
                    delay(100); //15 x 100ms means approximately 1.5 seconds after apogee, we should deploy our parachute.
                    current_state = LaunchState::Thousand_ft;
                //}
                }
                delay(100); //15 x 100ms means approximately 1.5 seconds after apogee, we should deploy our parachute.
                //current_state = LaunchState::Thousand_ft;
                
                
                break;


            case LaunchState::Thousand_ft:
                Serial.println("Thousand_ft");

                // while (alt.getAltitude() >= 1000)
                // {
                if(alt.getAltitude() <= SECOND_IGNITION_HEIGHT) {
                    // digitalWrite(igniter2, HIGH);
                    Serial.println("Ignition 2 occured");
                }
                //}
                current_state = LaunchState::Descent;
                break;


            case LaunchState::Descent:
                Serial.println("Descent");
                // print sensor data?
                current_state = LaunchState::Touchdown;
                break;


            case LaunchState::Touchdown :
                Serial.println("Touchdown");
                //print gps readings
                //Serial.println("Touchdown");
                gps2.printInfo();
                break;

            default:
                break;
        }
    }
}