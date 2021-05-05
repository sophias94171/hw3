# hw3

## Setup the Program 

1.Create a new Mbed project.
2.import RPC library.

 ` $ mbed add https://gitlab.larc-nthu.net/ee2405_2021/mbed_rpc.git ` 

3. Add "4DGL-uLCD-SE" library to the current project.
 
 ` $ git clone https://gitlab.larc-nthu.net/ee2405_2021/4dgl-ulcd-se.git `
 ` $ rm -rf ./4dgl-ulcd-se/.git `
 
4. gesture UI
5. tilt angle detection

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

## Demonstration
  ### - gesture UI mode
  
  1.LED_ to indicate the start of UI mode.
  2.Use gesture to select from a list of angles, e.g., 30, 35, 40, 45, etc.
  3.Before confirmation, the selection show on uLCD (so a user can see their current selection).
  ### - tilt angle detection mode
  *1.LED3(Blue) to indicate the start of tilt angle detection mode.
  2.measure the reference acceleration vector.LEDs to show this initialization process for a user to place the mbed on table.
  3.LED1(green) to indicate for a user to tilt the mbed.
  4.show the tilt angle on uLCD  every 1 seconds. 
  5.A MQTT message will publish if mbed tilts over the selected threshold degree to the stationary position.
