# Burn-In-Sofware-Suite

GUI Interface and control hardware/firmware designed for semconductor burn-in(quick stress test).  Stresses up to 3 items at a set current and controlled temperature. 

Hardware Components:
* Computer: Raspberry pi 4 2gb version
* Station Controller: Arduino ATMega2560
* Stations: Competly custom. In general heating pads, temp sensors, current drivers, etc
  

Burn-In Interface:
* Interfaces with burn-in stations.  Displays voltage, pad temperatures, test times, and controller responses.


BurnInFirmware:
* In general waits for interface to send start command then runs test autonomously.

Commands:
* Start: 'S', Start test for specified time
* Reset: 'RR', Resets controller
* Switch Current: 'C', Toggles drive current if specific station switching is enabled
* Pause: 'P', pauses and continues test
* Toggle Heating: 'H', starts heating copper plate to desired temperature
* Update Settigns:'UXXXXXX', Updates EEPROM with designates settings


Images Bof hardware below:

Computers:

![Alt Text](https://drive.google.com/uc?export=view&id=1yNvc7ysCoZdkcSIgdfUg6DGberSTTMQe)

Inside Burn-in Station:

![Alt Text](https://drive.google.com/uc?export=view&id=1yZDV98d_3FQboH2jqW3ySjqN3xS40JXA)


All Stations:

![Alt Text](https://drive.google.com/uc?export=view&id=1Dee1hDZKWdVWZ_a8W_dhX4WP5uc4MGdj)

