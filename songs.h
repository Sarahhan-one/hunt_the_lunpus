// 0th element of the durations is the length of the song

// Hall of the Mountain King by Edvard Grieg
uint16_t hotmk[]  =
  { A2, B2, C3, D3, E3, C3, E3, REST, Dx3, B2, Dx3, REST, D3, Ax2, D3, REST, A2, B2, C3, D3, E3, C3, E3, A3, G3, E3, C3, E3, G3, G3 };

uint8_t hotmkDurations[]  =
  { 30, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 250, 250 };

uint16_t fanfare[]  =
  { D2, G2, B2, REST, D2, G2, B2, REST, D2, G2, B2, B2 };

uint8_t fanfareDurations[]  =
  { 12, 200, 200, 200, 50, 200, 200, 200, 50, 200, 200, 255, 255 };

uint16_t youWin[]  =
  { E3, C3, D3, E3, C3, G3, G3 };

uint8_t youWinDurations[]  =
  { 7, 200, 200, 200, 200, 250, 255, 255 };

// Funeral March by Chopin
uint16_t chopinBlock[]  =
  { A0, A0, REST, A0, A0, REST, A0, REST, A0, A0, C1, C1, B0, REST, B0, B0, A0, REST, A0, A0, REST, A0, REST, A0, A0, A0 };

uint8_t chopinBlockDurations[]  =
  { 26, 250, 250, 20, 250, 250, 20, 100, 20, 250, 250, 250, 250, 100, 20, 250, 250, 200, 20, 250, 250, 20, 200, 20, 250, 250, 100 };

uint16_t bonk[]  = { 3000 };
uint8_t bonkDurations[]  = { 1, 200 };

uint16_t batFlap[]  = { B3, C4, G3 };
uint8_t batFlapDurations[]  = { 3, 50, 50, 50 };

uint16_t batSet[]  = { E3, A2 };
uint8_t batSetDurations[]  = { 2, 44, 100 };

uint16_t snoreUp[]  = { 1500, 3000, REPEAT };
uint8_t snoreUpDurations[]  = { 3, 10, 10, 0 };

uint16_t snoreDown[]  = { 3000, 4000, REPEAT };
uint8_t snoreDownDurations[]  = { 3, 10, 10, 0 };

uint16_t splat[]  = { 4000 };
uint8_t splatDurations[]  = { 1, 255 };

uint16_t wumpusMove[]  = { 5000, REST, 5000, REST, 5000 };
uint8_t wumpusMoveDurations[]  = { 5, 255, 200, 255, 200, 255 };

uint16_t ricochet[]  = { A0, B2 };
uint8_t ricochetDurations[]  = { 2, 100, 150 };
