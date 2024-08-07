# Quadruped Robot Project

This Repository contains the code developed for the Quadruped robot Project for [Introduction to Robotics (ECE_ΔK703)](https://www.ece.upatras.gr/index.php/el/undergraduate/under-courses/ece-dk703-eisagogi-sti-rompotiki.html) course.

The robot used is a version of "Spot Mini", a quadruped robot with 12 DOF. Developed code 
contains forward and inverse kinematics for each leg, trajectory generator, inverse differential kinematics algorithms and gaits for walking with variable speed.
Also printed all the parts, assembled them and wired up the electronics.

The trajectories have been implemented with cubic splines and for each leg we have the following:

![](resources/trajectory.gif)

Timing such trajectories right we get a walking gait:

![](resources/walk.gif)

and a troting gait:

![](resources/trot.gif)


This project was implemented by the following students:
- [Christos Athanasakis](https://github.com/athanasac)
- [Christos Kiokakis](https://github.com/ChristosK17)
- [Alexandros Ntagas](https://github.com/NtagkasAlex)
- [Vangelis Tsiatsianas](https://github.com/VagTsiats)
