#include <Arduino.h>

#include "wumpus.h"

#define SHIFT_PIN_SHCP 7
#define SHIFT_PIN_STCP 6
#define SHIFT_PIN_DS   5

typedef void (*stateFn)(unsigned long);
stateFn currentStateFn = &introState;

// north, south, east, west, arrow
const uint8_t buttonPins[5] = {A4, A5, 10, 11, A1};

const uint8_t pinWind = 13; //blue led 
const uint8_t pinLunpus = 12; //green led 

const uint8_t northWall[2] = {0x01, 0x81};
const uint8_t southWall[2] = {0x88, 0x08};
const uint8_t eastWall[2]  = {0x00, 0x06};
const uint8_t westWall[2]  = {0x30, 0x00};

uint8_t buttonStates[5] = {};

const uint8_t mapWidth = 12;
const uint8_t mapHeight = 12;

const uint8_t minWalls = 6;
const uint8_t maxWalls = 20;
const uint8_t minPits = 2;
const uint8_t maxPits = 6;
const uint8_t minBats = 2;
const uint8_t maxBats = 6;

const uint8_t maxArrows = 4;

const uint8_t maxBrightness = 90;

struct room cave[mapWidth][mapHeight];

uint8_t playerX, playerY;
uint8_t wumpusX, wumpusY;
uint8_t arrows;

unsigned long nextNoteTime = 0;
uint8_t currentNote = 0;
uint16_t *currentSong;
uint8_t *currentSongDurations;

unsigned long nextAnimationFrameTime = 0;
uint8_t animationFrameOffset = 0;
uint8_t textOffset = 0;

const uint8_t totalMaskSegments = 7;

SevSegShift sevsegshift(
                  SHIFT_PIN_DS,
                  SHIFT_PIN_SHCP,
                  SHIFT_PIN_STCP,
                  1, /* number of shift registers there is only 1 Shiftregister
                        used for all Segments (digits are on Controller)
                        default value = 2 (see SevSegShift example)
                        */
                  true /* Digits are connected to Arduino directly
                          default value = false (see SevSegShift example)
                        */
                );

bool buttonState(button id) {
  // debounce state
  buttonStates[id] = (buttonStates[id] << 1) | digitalRead(buttonPins[id]);
  return (buttonStates[id] == 0x80);
}

void playNote(uint16_t note) {
  if (note == REST) {
    DDRB &= ~(1 << DDB1); // disable output pin
    return;
  }
  //only reset the counter when the note changes
  if (OCR1A != note) {
    TCNT1 = 0;            // reset the counter
  }
  OCR1A = note;         // set pitch
  DDRB |= (1 << DDB1);  // enable output pin

}

void updateAudio(unsigned long timer) {
  if (nextNoteTime == 0) {
    return;
  }
  if (timer < nextNoteTime) {
    return;
  }
  if (currentNote == pgm_read_byte((currentSongDurations))) {
    stopSong();
    return;
  }
  uint16_t note = pgm_read_word((currentSong + currentNote));
  if (note == REPEAT) {
    currentNote = 0;
    return;
  }
  uint8_t duration = pgm_read_byte((currentSongDurations + currentNote + 1));
  playNote(note);
  nextNoteTime = duration + timer;
  currentNote++;
}

void playSong(uint16_t newSong[], uint8_t newSongDurations[]) {
  currentNote = 0;
  nextNoteTime = 1;
  currentSong = &newSong[0];
  currentSongDurations = &newSongDurations[0];
}

void stopSong() {
  nextNoteTime = 0;
  playNote(REST);
}

void updateCaveDisplay() {
  uint8_t display[2] = {0, 0};
  if (cave[playerX][playerY - 1].wall) {
    display[0] |= northWall[0];
    display[1] |= northWall[1];
  }
  if (cave[playerX][playerY + 1].wall) {
    display[0] |= southWall[0];
    display[1] |= southWall[1];
  }
  if (cave[playerX + 1][playerY].wall) {
    display[0] |= eastWall[0];
    display[1] |= eastWall[1];
  }
  if (cave[playerX - 1][playerY].wall) {
    display[0] |= westWall[0];
    display[1] |= westWall[1];
  }

  sevsegshift.setSegments(display);
}

void displayBatsNearby(unsigned long timer) {
  static unsigned long nextAction = 0;
  if (nextAction > timer) {
    return;
  }
  static uint8_t batFrameOffset = 0;
  static bool batDirection = true;

  updateCaveDisplay();
  uint8_t segments[2];
  sevsegshift.getSegments(segments);
  // wipe out wall dots
  segments[0] &= 0x7F;
  segments[1] &= 0x7F;
  uint8_t frame = pgm_read_byte(batNearbyFrames + batFrameOffset);
  if (batFrameOffset == 0) {
    segments[batDirection] |= frame;
  } else {
    segments[!batDirection] |= frame;
  }
  sevsegshift.setSegments(segments);
  nextAction = timer + 50 + random(100);
  batFrameOffset++;
  if (batFrameOffset == 4) {
    updateCaveDisplay();
    batDirection = timer % 2;
    nextAction = timer + 500 + random(500);
    batFrameOffset = 0;
  }
}

void displayPitNearby(unsigned long timer) {
  const uint16_t *maskSegments;
  uint8_t goingUp;
  if ((cave[playerX][playerY - 1].wall + cave[playerX][playerY + 1].wall) <
      (cave[playerX + 1][playerY].wall + cave[playerX - 1][playerY].wall)) {
    maskSegments = &windNorthSouthMask[0];
    goingUp = playerX % 2;
  } else {
    maskSegments = &windEastWestMask[0];
    goingUp = playerY % 2;
  }

  static unsigned long nextAction = 0;
  static uint8_t windOffset = 0;

  playWind(windOffset);

  if (nextNoteTime == 0) {
    digitalWrite(pinWind, LOW);
  }

  if (nextAction > timer) {
    return;
  }
  updateCaveDisplay();
  if (windOffset == 0xFF) { // wrapped around
    windOffset = totalMaskSegments - 1;
    nextAction = timer + 400 + random(600);
    return;
  }
  if (goingUp && windOffset == totalMaskSegments) {
    windOffset = 0;
    nextAction = timer + 400 + random(600);
    return;
  }
  uint8_t displaySegments[2];
  sevsegshift.getSegments(displaySegments);
  uint16_t word = pgm_read_word(maskSegments + windOffset);
  displaySegments[0] &= (uint8_t)word;
  displaySegments[1] &= (uint8_t)(word >> 8);
  sevsegshift.setSegments(displaySegments);
  nextAction = timer + 75;
  windOffset += (goingUp) ? +1 : -1;
}

void playWind(uint8_t windOffset) {
  // don't play if we're already playing some music
  if (nextNoteTime > 0) {
    return;
  }

  digitalWrite(pinWind, HIGH);

  // clamp to a sane value
  windOffset = (windOffset > 10) ? 0 : windOffset;
  playNote(random(10, 20 + windOffset));
}

void displayWumpusNearby(unsigned long timer) {
  static unsigned long nextAction = 0;
  static int16_t currentBrightness = 0;
  static bool direction = true;
  digitalWrite(pinLunpus, HIGH);
  
  if (nextAction > timer) {
    digitalWrite(pinLunpus, LOW);
    return;
  }
  nextAction = timer + 10;
  
  if (direction) {
    if (currentBrightness == 0) {
      playSong(snoreUp, snoreUpDurations);

    }
    currentBrightness += 1;
    if (currentBrightness >= maxBrightness) {
      direction = false;
      nextAction += 2000;
    }
  } else {
    if (currentBrightness >= maxBrightness) {
      playSong(snoreDown, snoreDownDurations);
    }
    currentBrightness -= 1;
    if (currentBrightness <= 0) {
      direction = true;
      nextAction += 2000;
      stopSong();
    }
  }
  sevsegshift.setBrightness(currentBrightness);

}

bool displayAnimation(unsigned long timer, uint16_t frameTime, const uint8_t frames[]) {
  if (nextAnimationFrameTime > timer) {
    return false;
  }
  uint8_t displaySegments[2];
  uint8_t frameCount = pgm_read_byte(&frames[0]);
  displaySegments[0] = pgm_read_byte(&frames[animationFrameOffset + 1]);
  displaySegments[1] = pgm_read_byte(&frames[animationFrameOffset + 2]);
  sevsegshift.setSegments(displaySegments);
  nextAnimationFrameTime = timer + frameTime;
  animationFrameOffset = (animationFrameOffset + 2) % frameCount;
  return true;
}

void setAdjacentRooms(struct point pt, struct room *adjacentRooms[]) {
  adjacentRooms[0] = &(cave[pt.x - 1][pt.y - 1]);
  adjacentRooms[1] = &(cave[pt.x - 1][pt.y    ]);
  adjacentRooms[2] = &(cave[pt.x - 1][pt.y + 1]);
  adjacentRooms[3] = &(cave[pt.x    ][pt.y - 1]);
  adjacentRooms[4] = &(cave[pt.x    ][pt.y + 1]);
  adjacentRooms[5] = &(cave[pt.x + 1][pt.y - 1]);
  adjacentRooms[6] = &(cave[pt.x + 1][pt.y    ]);
  adjacentRooms[7] = &(cave[pt.x + 1][pt.y + 1]);
}

struct point randomRoom() {
  struct point pt;
  do {
    pt.x = random(mapWidth);
    pt.y = random(mapHeight);
  } while (cave[pt.x][pt.y].wall);
  return pt;
}

void setupMap() {
  uint8_t i, j, count;
  struct point pt;

  for (i = 0; i < mapWidth; i++) {
    for (j = 0; j < mapHeight; j++) {
      cave[i][j] = {0};
    }
  }

  for (i = 0; i < mapWidth; i++) {
    cave[i][0].wall = 1;
    cave[i][mapHeight - 1].wall = 1;
  }
  for (j = 0; j < mapHeight; j++) {
    cave[0][j].wall = 1;
    cave[mapWidth - 1][j].wall = 1;
  }

  count = minWalls + random(maxWalls - minWalls);

  for (i = 0; i < count; i++) {
    pt = randomRoom();
    cave[pt.x][pt.y].wall = 1;
  }

  count = minPits + random(maxPits - minPits);

  struct room *adjacentRooms[8];

  for (i = 0; i < count; i++) {
    pt = randomRoom();
    cave[pt.x][pt.y].pit = 1;
    setAdjacentRooms(pt, adjacentRooms);
    for (j = 0; j < 8; j++) {
      (*adjacentRooms[j]).pitNearby = 1;
    }
  }

  count = minBats + random(maxBats - minBats);

  for (i = 0; i < count; i++) {
    pt = randomRoom();
    cave[pt.x][pt.y].superbat = 1;
    setAdjacentRooms(pt, adjacentRooms);
    for (j = 0; j < 8; j++) {
      (*adjacentRooms[j]).batsNearby = 1;
    }
  }

  do { //preventing overlap 
      pt = randomRoom();
  } while (cave[pt.x][pt.y].pit == 1 || cave[pt.x][pt.y].superbat == 1);

  cave[pt.x][pt.y].wumpus = 1;
  wumpusX = pt.x;
  wumpusY = pt.y;
  setAdjacentRooms(pt, adjacentRooms);
  for (j = 0; j < 8; j++) {
    (*adjacentRooms[j]).wumpusNearby = 1;
  }
}

void setupPlayer() {
  struct room candidate;
  struct point pt;
  do {
    pt = randomRoom();
    candidate = cave[pt.x][pt.y];
  } while (candidate.wumpus || candidate.superbat || candidate.pit);
  playerX = pt.x;
  playerY = pt.y;
  arrows = maxArrows;
}

void setDefaultBrightness() {
  sevsegshift.setBrightness(maxBrightness);
}

void initSpeaker() {
  //reset the registers
    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= (1 << WGM12);  // CTC: Clear Timer on Compare with OCR1A
    TCCR1A |= (1 << COM1A0); // Toggle OC1A on Compare Match
    TCCR1B |= (1 << CS10);   // CPU clock / 8
}

void renderText(unsigned long timer, uint8_t text[], uint8_t length) {
  static unsigned long nextAction = 0;
  if (nextAction > timer) {
    return;
  }
  nextAction = timer + 400;
  uint8_t segments[2];
  segments[0] = pgm_read_byte(&text[textOffset]);
  segments[1] = pgm_read_byte(&text[textOffset + 1]);
  sevsegshift.setSegments(segments);
  textOffset++;
  if (textOffset == length) {
    textOffset = 0;
  }
}

void setup() {
  randomSeed(analogRead(1));

  initSpeaker();

  byte numDigits = 2;
  byte digitPins[] = {3, 4}; // These are the PINS of the ** Arduino **
  byte segmentPins[] = {6, 7, 3, 2, 1, 5, 0, 4}; // these are the PINs of the ** Shift register **
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected

  sevsegshift.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);

  setDefaultBrightness();

  for (uint8_t i = 0; i < 5; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(pinWind, OUTPUT);
  pinMode(pinLunpus, OUTPUT);
  digitalWrite(pinWind, LOW);
  digitalWrite(pinLunpus, LOW);

  playSong(hotmk, hotmkDurations);
}

void loop() {
  static unsigned long timer;
  timer = millis();

  (*currentStateFn)(timer);

  updateAudio(timer);

  sevsegshift.refreshDisplay(); // Must run repeatedly
}

/* #### Game States #### */

void introState(unsigned long timer) {
  renderText(timer, introText, 18);

  if (buttonState(arrow)) {
    currentStateFn = &startState;
  }
}

void startState(unsigned long timer) {
  playSong(fanfare, fanfareDurations);
  setupMap();
  setupPlayer();
  updateCaveDisplay();
  currentStateFn = &playState;
}

void playState(unsigned long timer) {
  uint8_t nextPlayerX = playerX;
  uint8_t nextPlayerY = playerY;

  struct room currentRoom = cave[playerX][playerY];

  if (arrows == 0) {
    currentStateFn = &wumpusEatState;
    return;
  }

  if (currentRoom.wumpus) {
    currentStateFn = &disturbWumpusState;
    return;
  } else if (currentRoom.superbat) {
    currentStateFn = &superbatState;
    return;
  } else if (currentRoom.pit) {
    currentStateFn = &pitfallState;
    return;
  }

  if (currentRoom.batsNearby) {
    displayBatsNearby(timer);
  }
  if (currentRoom.pitNearby) {
    displayPitNearby(timer);
  }
  if (currentRoom.wumpusNearby) {
    displayWumpusNearby(timer);
  }

  if (buttonState(arrow)) {
    stopSong();
    setDefaultBrightness();
    currentStateFn = &arrowStartState;
    return;
  }
  if (buttonState(north)) {
    nextPlayerY--;
  }
  if (buttonState(south)) {
    nextPlayerY++;
  }
  if (buttonState(east)) {
    nextPlayerX++;
  }
  if (buttonState(west)) {
    nextPlayerX--;
  }

  if (nextPlayerX != playerX || nextPlayerY != playerY) {
    currentRoom = cave[nextPlayerX][nextPlayerY];
    nextAnimationFrameTime = 0;
    animationFrameOffset = 0;
    if (currentRoom.wall) {
      // wall beep
      playSong(bonk, bonkDurations);
      return;
    } else {
      stopSong();
      playerX = nextPlayerX;
      playerY = nextPlayerY;
      setDefaultBrightness();
      updateCaveDisplay();
    }
  }
}

void superbatState(unsigned long timer) {
  static uint8_t counter = 0;
  uint8_t animationTimeout = (animationFrameOffset == 0) ? 150 : 100;
  if (displayAnimation(timer, animationTimeout, batFlapFrames) && animationFrameOffset == 0) {
    playSong(batFlap, batFlapDurations);
    counter++;
  }
  if (counter < 6) {
    return;
  }
  counter = 0;
  playSong(batSet, batSetDurations);
  do {
    playerX = random(mapWidth);
    playerY = random(mapHeight);
  } while (cave[playerX][playerY].wall);
  updateCaveDisplay();
  currentStateFn = &playState;
}

uint16_t dropSound = 0;

void pitfallState(unsigned long timer) {
  static unsigned long nextAction = 0;
  displayAnimation(timer, 150, pitfallFrames);
  if (nextAction > timer) {
    return;
  }
  nextAction = timer + 5;
  if (dropSound < 800) {
    playNote(dropSound);
    dropSound += 3;
  } else {
    dropSound = 0;
    stopSong();
    uint8_t segments[2] = {0xFF, 0xFF};
    sevsegshift.setSegments(segments);
    playSong(splat, splatDurations);
    currentStateFn = &youLoseState;
  }
}

void disturbWumpusState(unsigned long timer) {
  if ((timer % 4) == 0) {
    currentStateFn = &wumpusEatState;
  } else {
    playSong(wumpusMove, wumpusMoveDurations);
    currentStateFn = &wumpusBumpState;
  }
}

void wumpusBumpState(unsigned long timer) {
  if (animationFrameOffset < 8) {
    displayAnimation(timer, 100, wumpusBumpFrames);
  } else {
    updateCaveDisplay(); // reset cave view
    currentStateFn = &wumpusMoveState;
  }
}

// move the wumpus and his smells to another location
void wumpusMoveState(unsigned long timer) {
  uint8_t i, j;
  struct point oldWumpusPt, newWumpusPt;
  struct room *adjacentRooms[8];

  do {
    newWumpusPt.x = wumpusX + random(3) - 1;
    newWumpusPt.y = wumpusY + random(3) - 1;
  } while(cave[newWumpusPt.x][newWumpusPt.y].wall);

  cave[wumpusX][wumpusY].wumpus = 0;
  for (i = 0; i < mapWidth; i++) {
    for (j = 0; j < mapHeight; j++) {
      cave[i][j].wumpusNearby = 0;
    }
  }

  setAdjacentRooms(newWumpusPt, adjacentRooms);
  for (i = 0; i < 8; i++) {
    (*adjacentRooms[i]).wumpusNearby = 1;
  }
  wumpusX = newWumpusPt.x;
  wumpusY = newWumpusPt.y;
  cave[wumpusX][wumpusY].wumpus = 1;

  currentStateFn = &playState;
}

void wumpusEatState(unsigned long timer) {
  animationFrameOffset = 0;
  setDefaultBrightness();
  playSong(chopinBlock, chopinBlockDurations);
  currentStateFn = &wumpusEatAnimationState;
}

void wumpusEatAnimationState(unsigned long timer) {
  if (animationFrameOffset < 14) {
    displayAnimation(timer, 180, wumpusBiteFrames);
  }
  if (nextNoteTime == 0) {
    currentStateFn = &youLoseState;
  }
}

enum button selection;

void arrowStartState(unsigned long timer) {
  if (buttonState(arrow)) {
    // cancel
    currentStateFn = &playState;
    updateCaveDisplay();
    return;
  }
  for (uint8_t i = 0; i < 4; i++) {
    if (buttonState(i)) {
      selection = i;
      arrows--;
      currentStateFn = &arrowFireState;
    }
  }

  displayAnimation(timer, 200, arrowSelectFrames[arrows - 1]);
}

void arrowFireState(unsigned long timer) {
  uint8_t x, y;
  x = playerX;
  y = playerY;
  struct room currentRoom;
  do {
    switch(selection) {
      case north:
        y--;
        break;
      case south:
        y++;
        break;
      case east:
        x++;
        break;
      case west:
        x--;
        break;
    }
    currentRoom = cave[x][y];
  } while(!currentRoom.wall && !currentRoom.wumpus);

  updateCaveDisplay(); // reset cave view
  if (currentRoom.wumpus) {
    playSong(youWin, youWinDurations);
    currentStateFn = &youWinState;
  } else if (currentRoom.wumpusNearby) {
    // hit a wall near the wumpus
    playSong(wumpusMove, wumpusMoveDurations);
    currentStateFn = &wumpusMoveState;
  } else {
    // hit a wall
    playSong(ricochet, ricochetDurations);
    currentStateFn = &playState;
  }
}

void youLoseState(unsigned long timer) {
  
  digitalWrite(pinLunpus, LOW);
  digitalWrite(pinWind, LOW);

  if (buttonState(arrow)) {
    currentStateFn = &startState;
  }
}

void youWinState(unsigned long timer) {

  digitalWrite(pinLunpus, LOW);
  digitalWrite(pinWind, LOW);
  
  renderText(timer, youWinText, 18);
  if (buttonState(arrow)) {
    currentStateFn = &startState;
  }
}