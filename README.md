# hw3

## Setup the Program 

1.Create a new Mbed project.
2. gesture UI
3. tilt angle detection

## Run the Program 

  1. Compile and run

  ` $ sudo mbed compile --source . --source ~/ee2405/mbed-os-build/ -m B_L4S5I_IOT01A -t GCC_ARM -f ` 
  
  ### - gesture UI mode
  2. Using RPC over serial to send a command to call gesture UI mode.
  3. User use gesture to select from threshold angles.
  4. PC/Python will show the selection on screen.
  ### - tilt angle detection mode
  5. Using RPC over serial to send a command to call tilt angle detection mode.
  6. If tilt angle is over the selected threshold angle, mbed will publish the event and angle through WiFi/MQTT to a broker. 
  7. After 10 tilt events, mbed is back to RPC loop.
  8. PC/Python will show all tilt events on screen.
  ` $ sudo mbed compile --source . --source ~/ee2405/mbed-os-build/ -m B_L4S5I_IOT01A -t GCC_ARM -f ` 

## Result
