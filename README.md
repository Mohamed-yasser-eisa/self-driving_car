# self-driving_car

build date of this project: (19 November, 2020)

This project was a task required from me and my teammates (we were 4 members in 
the team) in electronics class.

I was the one who built the android applications in Java using Android Studio IDE.

1- Project description:

    We built a moving vehicle that carries a mobile phone and RFID reader, it moves in a race track has two clear lines (one line on each side), it moves between the two lines autonomously, this is done by a laptop application will depend on the images sent from the mobile camera to guide the vehicle. So, the phone gets the images continuously, and send them to the laptop where OpenCV analyze the images and guide the vehicle.
    Also we can control the vehicle wireless using wifi protocol by another remote-control mobile application, to allow the vehicle to read the RFID tags and send their values to the application.

2- Tools: 

    (web-socket server, ESP chip, Java Android applications, Android Studio IDE, Arduino Uno, DC motors, Ultrasound sensor, buzzer, and RFID reader).

3- Files description:

    3.1- (arduino_code.ino): is the code used to receive orders from ESP chip 
        and  move the motors.
    3.2- (esp_code.ino): is the code used to connect to each of the android
        applications and python code and depending on the received data the ESP 
        sends order to Arduino to move car.
    3.3- (Client_final.py): is the code used to receive image from web socket     
        server (android phone) and do image processing on the recovered image to 
        detect lanes, and sends to the ESP chip the angel of movements.
    3.4- (Auto_pilot_app): is the android application which is used to open the 
        camera and Creates a web socket server on the android phone, and sends the 
        frames whenever the clients Requests it, up to 15 frame per second.
    3.5- (remote-control_app): is the android application which is used to send 
        orders to the ESP to control the car as a remotes control.
    3.6- (client): is a plus android application which is used to receive images 
        which are sent from the "Auto_pilot_mode.rar" application to see the same 
        images which are sent to the python code.

4- Video: (https://youtu.be/p4k6ANMX9f0)

