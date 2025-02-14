# Hunt the Lunpus 
Hunt the Lunpus is a handheld adaptation of the classic 1970s text-based game _Hunt the Wumpus_, built on the ATmega328p microcontroller. 
It is based on [Douglas McInnes's Lunpus project](https://github.com/dmcinnes/lunpus/tree/main?tab=readme-ov-file), which provided the foundation for this adaptation. 

![lunpus (2)](https://github.com/user-attachments/assets/47767b1e-2f2c-4b8a-84da-5010ec537dd8)

**[Check out the video in action](https://youtu.be/F6LPQ03PLAs?si=z6rZ3W5Jc2bM02LP)**


## Overview
Hunt the Lunpus is a handheld game console where the player explores a dangerous cave system filled with traps and a monstrous creature, the Lunpus. 
The objective is to avoid the deadly hazards, locate the Lunpus, and defeat it before it awakens. 
It uses seven-segment displays to represent the cave walls and hazards. 
This project builds on Douglas's work with additional hardware features - adds visual feedback in the form of LED indicators with ATmega328p microcontroller for its additional capabilities.

## How to Play
* **Start the Game**: Press the ```ARROW``` button to begin the game.
* **Movement**: Use the directional buttons (```NORTH```, ```SOUTH```, ```EAST```, ```WEST```) to navigate the cave system. The walls of the cave will appear on the seven-segment display.
* **Avoid Hazards**:
  * _Superbats_: Transport you to a random location if encountered.
  * _Slime Pits_: Instant death upon falling into one.
  * _Lunpus_: The sleeping monster will eat you if disturbed.
* **Hazard Warnings**:
  * LED Indicators: The ```Wind LED``` warns of nearby Slime Pits, while the ```Stench LED``` lights up when close to the Lunpus.
  * Sound Alerts: Wind sounds indicate pits, and snoring sounds warn that the Lunpus is near.
* **Fire Arrows**: Press the ```ARROW``` button to check your remaining arrows. Press a directional button to fire an arrow in that direction.
* **Game Over**: If you die or run out of arrows, press the ```ARROW``` button to restart.

## PCB & Schematic
The pcb directory includes EasyEDA project files that outline the schematic and PCB design. 
![image](https://github.com/user-attachments/assets/434eae47-bbb2-49ba-bfbf-b3fdc4b49141)

## Timer-Driven Processes
```cpp
void loop() {
    static unsigned long timer = millis();
    (*currentStateFn)(timer); // State-specific processing

    updateAudio(timer); // Audio management
    sevsegshift.refreshDisplay(); // Display refresh
}
```
### key features
* **Provides non-blocking time tracking**:
  * In the ```loop()``` function, ```timer = millis()``` captures the current time in milliseconds. Various functions then use this ```timer``` to determine when to perform actions without stopping the entire program execution. For example, in ```renderText()```, it uses timer comparisons to manage timing:
```cpp
static unsigned long nextAction = 0;
if (nextAction > timer) {
return;
}
nextAction = timer + 400;
```

  * State functions using timer like ```updateAudio()``` check timer without blocking game progression
```cpp
  void updateAudio(unsigned long timer) {
// Non-blocking audio processing
//- Tracks next note timing
//- Manages song progression dynamically
    if (nextNoteTime == 0) return;
    if (timer < nextNoteTime) return;

    playNote(currentSong[currentNote]);
    nextNoteTime = duration + timer;
}
```
  * No ```delay()``` functions used
    
* **Supports pseudo-random timing variations**:
   * Uses ```random()``` with timer. The combination of timer and random() creates a more natural, preventing monotonous, mechanical-feeling animations or interactions.
```cpp
nextAction = timer + 400 + random(600); // Randomized bat animation
```

* **Enables concurrent process management**:
     * Simultaneously manages:
       * Audio updates
       * Display refreshes
       * Hazard warnings
       * Player input processing

## State Machine Diagram
![Image](https://github.com/user-attachments/assets/0e785b5a-3bd0-4b93-8277-5850491cec6a)
