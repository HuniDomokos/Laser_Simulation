# Laser_Simulation
This is a program that simulates the interaction of lasers with mirrors and lenses.
It uses the ALLEGRO5 library and is written purely in C++.

# Contained files
This repository contains all the files neccessary to build the program, but the one which probably interests you the most is the main.exe file, with which you can run it and try the simulation for yourself.
In the main.cpp file you'll find the main logic, whereas the LaserMirror.h file the objects (laser, mirror, lense) are declared and ray-collision is handled.

# Features
There are 2 seperate laser sources you can turn and move around. To add more you will need to modify the main file and rebuild the executable.
There are also 2 mirrors (turnable and moveable), which reflect the rays, and 2 lenses (turnable and moveable with different refraction indices), which refract the rays.

# Preview

<img width="804" height="632" alt="Screenshot 2026-01-30 224202" src="https://github.com/user-attachments/assets/34197b99-1aae-4ce9-af1d-cbdf6678adc9" />

# Controls
You can pick up an object by holding the left mouse button and move it around. Additionally, while having picked up an object, you can turn it by pressing either the left or right arrow key.
