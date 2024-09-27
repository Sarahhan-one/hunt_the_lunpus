// // 0th element of the durations is the length of the song

// Hall of the Mountain King by Edvard Grieg
const uint16_t hotmk[] PROGMEM =
  { chord_A3, chord_B3, chord_C4, chord_D4, chord_E4, chord_C4, chord_E4, REST, chord_Dx4, chord_B3, chord_Dx4, REST, chord_D4, chord_Ax3, chord_D4, REST, chord_A3, chord_B3, chord_C4, chord_D4, chord_E4, chord_C4, chord_E4, chord_A4, chord_G4, chord_E4, chord_C4, chord_E4, chord_G4, chord_G4 };
const uint8_t hotmkDurations[] PROGMEM =
  { 30, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 250, 250 };

const uint16_t fanfare[] PROGMEM =
  { chord_D3, chord_G3, chord_B3, REST, chord_D3, chord_G3, chord_B3, REST, chord_D3, chord_G3, chord_B3, chord_B3 };
const uint8_t fanfareDurations[] PROGMEM =
  { 12, 200, 200, 200, 50, 200, 200, 200, 50, 200, 200, 255, 255 };

const uint16_t youWin[] PROGMEM =
  { chord_E4, chord_C4, chord_D4, chord_E4, chord_C4, chord_G4, chord_G4 };
const uint8_t youWinDurations[] PROGMEM =
  { 7, 200, 200, 200, 200, 250, 255, 255 };

// Funeral March by Chopin
const uint16_t chopinBlock[] PROGMEM =
  {chord_A3, chord_A3, REST, chord_A3, chord_A3, REST, chord_A3, REST, chord_A3, chord_A3, chord_C4, chord_C4, chord_B3, REST, chord_B3, chord_B3, chord_A3, REST, chord_A3, chord_A3, REST, chord_A3, REST, chord_A3, chord_A3, chord_A3};
const uint8_t chopinBlockDurations[] PROGMEM =
  { 26, 250, 250, 20, 250, 250, 20, 100, 20, 250, 250, 250, 250, 100, 20, 250, 250, 200, 20, 250, 250, 20, 200, 20, 250, 250, 100 };

const uint16_t bonk[] PROGMEM = { 3000 };
const uint8_t bonkDurations[] PROGMEM  = { 1, 200 };

const uint16_t batFlap[] PROGMEM = { chord_B4, chord_C5, chord_G4 };
const uint8_t batFlapDurations[] PROGMEM = { 3, 50, 50, 50 };

const uint16_t batSet[] PROGMEM = { chord_E4, chord_A3 };
const uint8_t batSetDurations[] PROGMEM = { 2, 44, 100 };

const uint16_t snoreUp[] PROGMEM = { 1500, 3000, REPEAT };
const uint8_t snoreUpDurations[] PROGMEM = { 3, 10, 10, 0 };

const uint16_t snoreDown[] PROGMEM = { 3000, 4000, REPEAT };
const uint8_t snoreDownDurations[] PROGMEM = { 3, 10, 10, 0 };

const uint16_t splat[] PROGMEM = { 4000 };
const uint8_t splatDurations[] PROGMEM = { 1, 255 };

const uint16_t wumpusMove[] PROGMEM = { 5000, REST, 5000, REST, 5000 };
const uint8_t wumpusMoveDurations[] PROGMEM = { 5, 255, 200, 255, 200, 255 };

const uint16_t ricochet[] PROGMEM = { chord_A3, chord_B5 };
const uint8_t ricochetDurations[] PROGMEM = { 2, 100, 150 };
