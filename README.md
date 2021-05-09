# hw3

## Setup the Program 

1. Compile

 `$ mkdir -p ~/ee2405new `
 `$ cp -r ~/ee2405/mbed-os ~/ee2405new`
 `$ cd ee2405new `
 `$ mbed compile --library --no-archive -t GCC_ARM -m B_L4S5I_IOT01A --build ~/ee2405new/mbed-os-build2 `
 
2.Create a new Mbed project.
3.import RPC library.

 ` $ mbed add https://gitlab.larc-nthu.net/ee2405_2021/mbed_rpc.git ` 

3. Import "4DGL-uLCD-SE" library to the current project.
 
 ` $ git clone https://gitlab.larc-nthu.net/ee2405_2021/4dgl-ulcd-se.git `
 ` $ rm -rf ./4dgl-ulcd-se/.git `
 
4. Import BSP library for running accelerometer.
 `$ mbed add http://developer.mbed.org/teams/ST/code/BSP_B-L475E-IOT01/#bfe8272ced90`
 `$ rm ~/ee2405/mbed08/src/data_collect/BSP_B-L475E-IOT01/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_qspi.*`
 
5. gesture UI
6. tilt angle detection

## Run the Program 

  1. Compile and run
 `$ sudo mbed compile --source . --source ~/ee2405new/mbed-os-build2/ -m B_L4S5I_IOT01A -t GCC_ARM -f ` 
  
  ### - gesture UI mode
  
  2. Using RPC over serial to send a command to call gesture UI mode.
  3. LED3(blue) to indicate the start of gesture UI mode.
  4. User use gesture to select from threshold angles from a list of angles, 20, 25, 30, 35, 40, 45, 50, 55.
  5. The selected threshold angle is published through WiFi/MQTT to a broker.
  ### - tilt angle detection mode
  
  5. Using RPC over serial to send a command to call tilt angle detection mode.
  6. LED1(green) start blinking to indicate for a user to place the mbed on table.
  7. LED1(green) stop blinking to indicate for a user to tilt the mbed. Screen will show the tilt angle on uLCD  every 1 seconds.
  8. If tilt angle is over the selected threshold angle, mbed will publish the event and angle through WiFi/MQTT to a broker. 
  9. After 10 tilt events, mbed is back to RPC loop.

## Results
  ### - gesture UI mode
  
  1. RPC call
  2. LED3(blue) to indicate the start of gesture UI mode
  3. the selection show on uLCD
  4. MQTT message
  5. Back to RPC loop.
  ### - tilt angle detection mode
  1.RPC call
  2.LED1(green) start blinking to indicate for a user to place the mbed on table.
  3.LED1(green) stop blinking to indicate for a user to tilt the mbed.
  4.Tilt angle show on uLCD  every 1 seconds.
  5.MQTT message 
  6.Back to RPC loop.
