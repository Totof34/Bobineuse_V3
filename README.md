Bobineuse_V3
============

The project describes a stand-alone coil winder

If you would have more information about this project please visit 
www.audiyofan.org

A link to the dedicated post 
http://www.audiyofan.org/forum/viewtopic.php?f=73&t=7176&start=105


PCB & schema are designed with Kicad

the motor part uses a DC motor with a home made supply described below

![](schematics/Moteur.gif)

The prototype of the display and the buttons

![](images/Facade_bobineuse_V3_montee.JPG)

Everything that goes around the Leonardo's board

![](schematics/Bobineuse_schema_petit.png)

Here the function than my program include and which seems to me appropriate

 1° automatic counting and uncounting
 
 2° counting from zero or down from a specified number of turns
 
 3° initial point or initial carcass start recordable and reminder 
 
 4° guide wire enslaved on the up / down counting
 
 5° automatic reversing guide wire depending on the number of turns per layer and change side
 
 6° manual correction of errors in advance of the guide wire, losses step essentially
 
 7° Manual correction of counting errors
 
 8° management parameters by a "management" menu and a second separate part for the counting itself
 
 9° recordings of 10 different configurations to facilitate the winding of several transformers,
    by recording the primary and secondary sections separately and recall stored data in memory
    
10° display menus on 16x2 characters LCD

Of course my Leonardo's board also allows PC control if you wanted to, but I prefer a
standalone machine

See in the arduino folder for the sketch

Work in progress

![](images/Reconstruction_bobineuse_V3_5.JPG)

Detail of the guidewire

![](images/Chariot_guide_fil_1.JPG)

![](images/Ecrou_guide_fil.JPG)

Implementation of electronic boards in progress

![](images/Implantation_carte_arduino_1.JPG)

![](images/Implantation_carte_arduino_2.JPG)


All comments are welcome

Totof
