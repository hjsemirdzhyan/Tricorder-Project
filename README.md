# Tricorder

![star_trek_voyager_tricorder](https://github.com/hjsemirdzhyan/Tricorder-Project/assets/70782656/36f0a92a-f253-48a6-81a4-35d2d03b48c5)
![image](https://user-images.githubusercontent.com/70782656/236332382-bcf56a1f-6522-419e-805b-ac4f0d44fce2.png)

End goal is to have a functional sensor device

Resources
* https://store-usa.arduino.cc/collections/sensors-environment?page=3&grid_list=list-view
* https://randomnerdtutorials.com/21-arduino-modules-you-can-buy-for-less-than-2/
* https://misclab.umeoce.maine.edu/boss/Arduino/bensguides/Arduino_Sensors_Beginners_Guide.pdf
* Improving An Already Phenomenal Star Trek Prop | Hackaday
* https://imgur.com/gallery/Ul3FW0y

Sensors to Add
* Temp and humidity sensor
* Distance meter
    * Using ultrasonic sensor
        * V2, integrate with temp and humidity sensor
        * https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/
* Magnetic sensor
* Sound sensor
    * frequency
    * direction
    * Strength
* Accelerometer/Gyro
    * https://store-usa.arduino.cc/collections/sensors-environment/products/grove-6-axis-accelerometer-gyroscope
* Radio frequency detector
    * Specific frequencies
    * Broad bands
    * Locator
* Voltage detector
* Heartbeat meter
* Thermal sensor
    * V1, measure temp
    * V2, see temp
        * https://www.adafruit.com/product/4469
* Bluetooth detector
* Wifi detector
* GPS coordinates
    * Location
    * Speed
    * Number of satellites
* IR detector
    * V1, detects
    * V2, measures
    * V3, locates
        * https://store-usa.arduino.cc/collections/sensors-environment/products/grove-80cm-infrared-proximity-sensor
* Light sensor
* Air pressure/altitude sensor
* Speed reader
* Atmospheric composition sensor
    * V1, air quality
    * V2, co2, o2
    * V3, other gases
* Water purity detector
* NFC reader
* RFID reader
* Sub-ghz reader
* pH sensor
* Strain gauge
    * https://www.adafruit.com/product/1070
* Force sensor
    * https://www.adafruit.com/product/1075
* UV Sensor
    * https://www.adafruit.com/product/1918
* Wind speed sensor
    * https://www.adafruit.com/product/1733
* Moisture sensor
    * https://randomnerdtutorials.com/guide-for-soil-moisture-sensor-yl-69-or-hl-69-with-the-arduino/
* Stud Finder
* Level
* AM/FM radio
* Walkie talkie
* Microwave sensor
    * https://lastminuteengineers.com/rcwl0516-microwave-radar-motion-sensor-arduino-tutorial/
* EMF Sensor

Plan
* Setup Arduino to be easy to plug stuff into. Really want to work with the nano but the uno might have more versatility
* Add buttons and knobs first for controlling whatever sensor I plug in. No doubt I’ll need to do manual calibrations so might as well have those ready for me. Add a buzzer as well, for audio feedback. And an RGB LED that flashes different colors to mean good or bad stuff. 
* Add an LCD. I’ll need to eventually and could be really useful if using the serial monitor just isn’t an option. Just set it up to display hard coded text for now. Just to show it’s up and running. Can utilize it as needed. 
* Preplan how to deal with peripherals that need contact in order to measure. Maybe design a second breakout board that plugs into the firsts as an extension. Would also need to program it so that it detects when the second board is attached. Something simple like if voltage is high, assume peripheral boards are attached and start reading from there as well. Eventually will need to make this wireless using an RF 433MHz transmitter/reader.https://randomnerdtutorials.com/rf-433mhz-transmitter-receiver-module-with-arduino/
* Will need to build out code in two categories. At least. First being passive sensors which would be built into the primary board. Second would be the active sensors requiring a peripheral. Like the force, strain and pH sensors. There should be a third category too where I keep all my house keeping code. Like the code to check if the peripheral board connected. 
* Build touchscreen UI

Sensor Categories
* Environmental
    * Temp, humidity
    * Barometric pressure
    * Altitude
    * Location
    * Magnetic fields
    * Atmospheric composition
    * Wind speed
* Motion
    * Accelerometer
    * Gyroscope
    * Self speed
    * Others speed
* Light spectrum
    * UV sensor
    * Light sensor
    * IR sensor
* Frequency spectrum
    * Radio detector
    * Wifi detector
    * Bluetooth sensor
    * Nfc reader
    * Raid reader
    * Sub-ghz reader
* Sound
    * Sound strength sensor
    * Sound frequency sensor
* Physical
    * Heartbeat meter
    * Thermal sensor
    * pH sensor
    * Staring gauge
    * Force Sensor
    * Distance

Functionality
* Lock onto particular sound and measure only its db
* Sound direction finder
* Frequency direction finder
* IR direction finder
* Visible light direction finder
* Compass
* Calculator
