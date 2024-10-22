# Hunt the Lunpus 
Hunt the Lunpus is a handheld adaptation of the classic 1970s text-based game _Hunt the Wumpus_, built on the ATmega328p microcontroller. 
It is based on [Douglas McInnes's Lunpus project](https://github.com/dmcinnes/lunpus/tree/main?tab=readme-ov-file), which provided the foundation for this adaptation. 


## Overview
Hunt the Lunpus is a handheld game console where the player explores a dangerous cave system filled with traps and a monstrous creature, the Lunpus. 
The objective is to avoid the deadly hazards, locate the Lunpus, and defeat it before it awakens. 
It uses seven-segment displays to represent the cave walls and hazards. 
This project builds on Douglas's work with additional hardware features - adds visual feedback in the form of LED indicators with ATmega328p microcontroller for its additional capabilities.

## How to Play
* Start the Game: Press the ```ARROW``` button to begin the game.
* Movement: Use the directional buttons (```NORTH```, ```SOUTH```, ```EAST```, ```WEST```) to navigate the cave system. The walls of the cave will appear on the seven-segment display.
* Avoid Hazards:
  * Superbats: Transport you to a random location if encountered.
  * Slime Pits: Instant death upon falling into one.
  * Lunpus: The sleeping monster will eat you if disturbed.
* Hazard Warnings:
  * LED Indicators: The ```Wind LED``` warns of nearby Slime Pits, while the ```Stench LED``` lights up when close to the Lunpus.
  * Sound Alerts: Wind sounds indicate pits, and snoring sounds warn that the Lunpus is near.
* Fire Arrows: Press the ```ARROW``` button to check your remaining arrows. Press a directional button to fire an arrow in that direction.
* Game Over: If you die or run out of arrows, press the ```ARROW``` button to restart.

## PCB & Schematic
The pcb directory includes EasyEDA project files that outline the schematic and PCB design. 



