# Acrobot-Project
The acrobot is a two-link robot manipulator ( it has three links if we counted the base) with only one actuator at the second joint. In this project I designed, and built an acrobot from scratch with the help of my supervisor. we didn't use any motion transfer methods (such as gears and timing belts with pulleys) in this hardware. It was also required to stablize it using LQR control in minimal and maximal coordinates. The mechanical design still needs some modifications for the controller to work in the upright position, but unfortunately I no longer have access to the hardware.  
![single pendulum](https://user-images.githubusercontent.com/71582409/146194395-43920c1b-1a6a-46b7-b8a7-24f519d4942c.PNG)

## The mechanical design and hardware
The design is very simple and the links used are inexpenive and can be obtained easily.To connect the parts together, we designed connections and used 3D printing technology to maufacture them. We designed similar connections that can be produced using milling, but we didn't manufacture them.
### List of components 
this list contains all the mechanical and electronics components that we used (nX) means we used n parts. d = diameter, t= thickness
* Base link: it is a 20mm d X 1mm t X 205mm aluminum tube  https://www.bauhaus.info/metallprofile-kunststoffprofile/kantoflex-rundrohr/p/26683708 (The strength of the tube is good and enough for this application. However, I recommend getting a steel tube for a longer life cycle) 
* Bearings: (2X) 6004-ZZ with width 12mm, Inner diameter 20mm and outer diameter 42mm https://www.bauhaus.info/kugellager/kugellager-6004-zz/p/26538118( you can get other bearings with an inner diameter of 20 mm, but the dimensions of the housing needs to be changed)
* Link1: 1 mm t X 20x20mm X 300mm square Aluminum tube https://www.bauhaus.info/metallprofile-kunststoffprofile/kantoflex-vierkantrohr/p/10508571 
* Link2: it is a 20mm diam* 200mm aluminum tube with a 12mm d X 18 mm steel rod (I recommend getting a 20mm d X 1 mm t X 200mm steel tube or 20mm* 300mm aluminum rod) 
* Motor: Odrive DUAL SHAFT MOTOR D5065 270KV https://eu.odriverobotics.com/shop/odrive-custom-motor-d5065
* Motor driver: ODRIVE V3.6 56volts with connectors https://eu.odriverobotics.com/shop/odrive-v36 ( I may consider getting moteus r4.8 developer kit to build another acrobot but this will require some modifications in the design https://mjbots.com/products/moteus-r4-8-developer-kit )
* Microcontroller: Teensy4.1 development board https://www.conrad.com/p/pjrc-microcontroller-teensy-41-2269230 ( I don't recommend using Arduino microcontroller in this project. You can make it work with arduino however the control code will be more complicated and you will need to use native protocol instead of Ascii for UART communication)
* Encoders: (2X) Incremental capacitive encoder CUI AMT102-V https://eu.odriverobotics.com/shop/cui-amt-102 (I recommend getting absolute encoders as they will reduce the calculations)
* The base:  You can get any wooden or metal base, but you should make sure that it is stable and doesn't move while the acrobot is working)
## Modelling and control
* We used Julia programming language to write the Acrobot parameters, and calculated the LQR controller gains. We also tried to use Julia to control the Acrobot, but it is not a good idea.
(You can also use matlab or any language for calculations)
* We used Arduino IDE to write codes to the teensy. The instructions for installation can be found here https://www.pjrc.com/teensy/td_download.html. You can use both linux and windows operating systems for teensyarduino. 
## Communication 
Since we need to send torque values to the odrive and receive the positions and velocities data from the odrive, we used UART communication and send the commands using ASCII protocol. More information about ASCII protocol can be found here https://docs.odriverobotics.com/ascii-protocol. I tried testing native protocol. Although it is faster than the ASCII, it is more complicated. You can find more info about the native protocol here https://docs.odriverobotics.com/native-protocol. I have posted my native protocol notes in https://github.com/MonaElboughdady/Acrobot-Project/tree/main/Calculations.

