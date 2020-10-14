#include <Vector.h>
#include <Adafruit_TFTLCD.h> 
#include <Adafruit_GFX.h>    
#include <TouchScreen.h>

#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 
#define LCD_RESET A4 

#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 70
#define TS_MAXY 920

#define YP A2  
#define XM A3  
#define YM 8  
#define XP 9  

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xA233

#define SEA     0x04FF
#define BRICK   0xF800
#define STICK   0x07E0
#define SHEEP   0x07FF
#define WHEAT   0xFFE0
#define DESERT  0xA233
#define ORE     0x8410
#define GOLD    0xA508

#define BUTTON_COL1  60
#define BUTTON_COL2 165
#define BUTTONsCOL1 112

#define BUTTON_ROW1  50
#define BUTTON_ROW2 100
#define BUTTON_ROW3 180

#define BUTTON_W     95
#define BUTTONsW     42
#define BUTTON_H     40

#define REDO_X       0
#define REDO_LW      20

#define BOARD_XE     10
#define BOARD_YE     10
#define BOARD_XO      7
#define BOARD_YO     18
#define HEX_WH       16         

//Setup TFT shield and touchscreen
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Master button control
boolean scenarioEnabled = false;
boolean playersEnabled = false;
// Button select



/*********************************
 * Create a 7x4x3 matrix of uint16_t for the main island
 * rows - resources
 * cols - scenarios
 * height - players
 *                    base | sea base | Island | desert // 3 on first layer,4,then 5+
 *  uint8_t deserts   
 *  uint8_t Forest 
 *  uint8_t Hills
 *  uint8_t Fields
 *  uint8_t Mountains
 *  uint8_t Pastures
 */
 
uint8_t resources[3][6][4] = {
  {
    {1,0,0,0}, 
    {4,3,4,4},
    {3,2,4,3},
    {4,3,4,2},
    {3,2,4,2},
    {4,4,4,3}
  }, 
  {
    {1,1,0,0}, 
    {4,4,5,5},
    {3,3,4,4},
    {4,4,5,2},
    {3,3,4,2},
    {4,4,5,4}
  },
  {
    {2,0,0,0}, 
    {6,0,0,0},
    {5,0,0,0},
    {6,0,0,0},
    {5,0,0,0},
    {6,0,0,0}
  }
};

//for islands (sea base, desert)
// except top slot is sea instead of desert!
// last one is gold mines
/*********************************
 * Create a 7x2x3 matrix of uint16_t for the islands
 * rows - resources
 * cols - scenarios
 * height - players
 *                   new shores | thru desert // 3,4,and5+ players
 *  uint8_t sea  
 *  uint8_t Forest 
 *  uint8_t Hills
 *  uint8_t Fields
 *  uint8_t Mountains
 *  uint8_t Pastures
 *  uint8_t gold
 */
uint8_t sea_islands[3][7][2] = {
  { // 3 Players
    {4,2}, 
    {0,1},
    {2,0},
    {1,2},
    {2,2},
    {1,1},
    {2,2}
  }, 
  { // 4 Players
    {4,2}, 
    {1,0},
    {2,1},
    {1,3},
    {2,3},
    {1,1},
    {2,2}
  },
  { // 5/6 Players
    {0,0}, 
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}
  }
};

/*********************************************
 * Now we need matrices indicating number distributions
 * We will use uint8_t matrix of 12x4x3 with same qualifiers as others 
 * for the main board
 * 
 * Numbers    
 *           base | sea base | Island | desert // 3 | 4 | 5+ 
 * 2
 * 3
 * 4
 * 5
 * 6
 * 8
 * 9
 * 10
 * 11
 * 12
 */
uint8_t numbers[3][12][4] {
{ // 3 Players
   {0,0,0,0}, //1 (not used but for simplicity)
   {1,1,1,1}, //2
   {2,1,2,1},
   {2,1,2,2},
   {2,2,3,1},
   {2,2,2,2}, 
   {0,0,0,0}, //7 (not used but for simplicity)
   {2,2,2,2}, 
   {2,1,3,2}, 
   {2,2,2,2}, 
   {2,2,2,1}, 
   {1,0,1,0}  //12
}, 
{ // 4 Players
   {0,0,0,0}, 
   {1,1,1,0},
   {2,2,2,2},
   {2,2,3,2},
   {2,2,3,2},
   {2,2,2,2}, 
   {0,0,0,0}, 
   {2,2,2,2}, 
   {2,2,3,2}, 
   {2,2,3,2}, 
   {2,2,3,2}, 
   {1,1,1,1}
},
{ // 5 Players
   {0,0,0,0}, 
   {1,0,0,0},
   {3,0,0,0},
   {3,0,0,0},
   {3,0,0,0},
   {3,0,0,0}, 
   {0,0,0,0}, 
   {3,0,0,0}, 
   {3,0,0,0}, 
   {3,0,0,0}, 
   {3,0,0,0}, 
   {3,0,0,0}
 }
};

//Island numbers for new shores and desert
// 3,4,5
uint8_t sea_numbers[3][12][2] {
{
   {0,0}, 
   {1,0},
   {2,1},
   {1,1},
   {1,1},
   {0,2}, 
   {0,0}, 
   {1,2}, 
   {1,2}, 
   {1,2}, 
   {0,1}, 
   {1,0}
}, 
{
   {0,0}, 
   {1,1},
   {1,1},
   {1,1},
   {1,1},
   {1,1}, 
   {0,0}, 
   {1,1}, 
   {1,1}, 
   {1,1}, 
   {1,1}, 
   {0,1}
},
{
   {0,0}, 
   {0,0},
   {0,0},
   {0,0},
   {0,0},
   {0,0}, 
   {0,0}, 
   {0,0}, 
   {0,0}, 
   {0,0}, 
   {0,0}, 
   {0,0}
}};
//Random Seed
long randNumber;

/**
 * Draws setup board and buttons
 */
void setup() {
  Serial.begin(9600);

  tft.reset(); 
  tft.begin(0x9341);
  tft.setRotation(3);
  tft.fillScreen(BLACK); // Background screen
  tft.drawRect(0,0,319,240,WHITE);
  tft.setCursor(40,20);
  tft.setTextColor(WHITE); // Text color
  tft.setTextSize(2);
  tft.print("  Catan Randomizer"); // Text above the button

  tft.setCursor(40,150);
  tft.print(" Number of Players?");
  
  // Base button
  tft.fillRect(BUTTON_COL1,BUTTON_ROW1,BUTTON_W,BUTTON_H,RED);
  tft.drawRect(BUTTON_COL1,BUTTON_ROW1,BUTTON_W,BUTTON_H,WHITE);
  tft.setCursor(BUTTON_COL1+10,60);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("Base"); // Button Text
  
  // Desert player button
  tft.fillRect(BUTTON_COL1,BUTTON_ROW2,BUTTON_W,BUTTON_H,BLUE);
  tft.drawRect(BUTTON_COL1,BUTTON_ROW2,BUTTON_W,BUTTON_H,WHITE);
  tft.setCursor(BUTTON_COL1+10,112);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Desert"); // Button Text

  // Sea Base button
  tft.fillRect(BUTTON_COL2,BUTTON_ROW1,BUTTON_W,BUTTON_H,BLUE);
  tft.drawRect(BUTTON_COL2,BUTTON_ROW1,BUTTON_W,BUTTON_H,WHITE);
  tft.setCursor(BUTTON_COL2+10,60);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("Base"); // Button Text
  
  // Island player button
  tft.fillRect(BUTTON_COL2,BUTTON_ROW2,BUTTON_W,BUTTON_H,BLUE);
  tft.drawRect(BUTTON_COL2,BUTTON_ROW2,BUTTON_W,BUTTON_H,WHITE);
  tft.setCursor(BUTTON_COL2+10,112);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Island"); // Button Text
  //-------------------------------/
  // 3 player button
  tft.fillRect(BUTTON_COL1,BUTTON_ROW3,BUTTONsW,BUTTON_H,RED);
  tft.drawRect(BUTTON_COL1,BUTTON_ROW3,BUTTONsW,BUTTON_H,WHITE);
  tft.setCursor(BUTTON_COL1+12,190);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("3"); // Button Text
  
  // 4 player button
  tft.fillRect(BUTTONsCOL1,BUTTON_ROW3,BUTTONsW,BUTTON_H,RED);
  tft.drawRect(BUTTONsCOL1,BUTTON_ROW3,BUTTONsW,BUTTON_H,WHITE);
  tft.setCursor(BUTTONsCOL1+12,190);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("4"); // Button Text
  
  // 5/6 player button
  tft.fillRect(BUTTON_COL2,BUTTON_ROW3,BUTTON_W,BUTTON_H,RED);
  tft.drawRect(BUTTON_COL2,BUTTON_ROW3,BUTTON_W,BUTTON_H,WHITE);
  tft.setCursor(BUTTON_COL2+10,190);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("5/6"); // Button Text
  

  
}
uint8_t scenario_select = 0; //1 for Base, 2 for Sea base, 3 for thru, 4 for island
uint8_t players_select = 0;  //actual number of players
void loop() {
  TSPoint p = ts.getPoint(); 
  
  if (scenarioEnabled && playersEnabled) {
    //drawboard
    draw_board(players_select,scenario_select);
    delay(100000);
  }
  if (p.z > ts.pressureThreshhold) {
    p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
    p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);
       
    if (!scenarioEnabled) {
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      if (p.x > BUTTON_COL1 && p.x < (BUTTON_COL1+BUTTON_W) &&
          p.y > BUTTON_ROW1 && p.y < (BUTTON_ROW1+BUTTON_H)) {
        // Base game select
        tft.fillRect(BUTTON_COL1,BUTTON_ROW1,BUTTON_W,BUTTON_H,YELLOW);
        tft.drawRect(BUTTON_COL1,BUTTON_ROW1,BUTTON_W,BUTTON_H,WHITE);
        scenarioEnabled = true;
        scenario_select = 1;
      }if(p.x > BUTTON_COL1 && p.x < (BUTTON_COL1+BUTTON_W) &&
          p.y > BUTTON_ROW2 && p.y < (BUTTON_ROW2+BUTTON_H)) {
        // Desert game select
        tft.fillRect(BUTTON_COL1,BUTTON_ROW2,BUTTON_W,BUTTON_H,YELLOW);
        tft.drawRect(BUTTON_COL1,BUTTON_ROW2,BUTTON_W,BUTTON_H,WHITE);
        scenarioEnabled = true;
        scenario_select = 3;
      }if(p.x > BUTTON_COL2 && p.x < (BUTTON_COL2+BUTTON_W) &&
          p.y > BUTTON_ROW1 && p.y < (BUTTON_ROW1+BUTTON_H)) {
        // Sea base game select
        tft.fillRect(BUTTON_COL2,BUTTON_ROW1,BUTTON_W,BUTTON_H,YELLOW);
        tft.drawRect(BUTTON_COL2,BUTTON_ROW1,BUTTON_W,BUTTON_H,WHITE);
        scenarioEnabled = true;
        scenario_select = 2;
      }if(p.x > BUTTON_COL2 && p.x < (BUTTON_COL2+BUTTON_W) &&
          p.y > BUTTON_ROW2 && p.y < (BUTTON_ROW2+BUTTON_H)) {
        // Island base game select
        tft.fillRect(BUTTON_COL2,BUTTON_ROW2,BUTTON_W,BUTTON_H,YELLOW);
        tft.drawRect(BUTTON_COL2,BUTTON_ROW2,BUTTON_W,BUTTON_H,WHITE);
        scenarioEnabled = true;
        scenario_select = 4;
      }
      
    } if (!playersEnabled) {
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      if (p.x > BUTTON_COL1 && p.x < (BUTTON_COL1+BUTTONsW) &&
          p.y > BUTTON_ROW3 && p.y < (BUTTON_ROW3+BUTTON_H)) {
        // 3 player select
        tft.fillRect(BUTTON_COL1,BUTTON_ROW3,BUTTONsW,BUTTON_H,YELLOW);
        tft.drawRect(BUTTON_COL1,BUTTON_ROW3,BUTTONsW,BUTTON_H,WHITE);
        playersEnabled = true;
        players_select = 3;
      }if (p.x > BUTTONsCOL1 && p.x < (BUTTONsCOL1+BUTTONsW) &&
           p.y > BUTTON_ROW3 && p.y < (BUTTON_ROW3+BUTTON_H)) {
        // 4 player select
        tft.fillRect(BUTTONsCOL1,BUTTON_ROW3,BUTTONsW,BUTTON_H,YELLOW);
        tft.drawRect(BUTTONsCOL1,BUTTON_ROW3,BUTTONsW,BUTTON_H,WHITE);
        playersEnabled = true;
        players_select = 4;
      }if (p.x > BUTTON_COL2 && p.x < (BUTTON_COL2+BUTTON_W) &&
           p.y > BUTTON_ROW3 && p.y < (BUTTON_ROW3+BUTTON_H)) {
        // 5/6 player select
        tft.fillRect(BUTTON_COL2,BUTTON_ROW3,BUTTON_W,BUTTON_H,YELLOW);
        tft.drawRect(BUTTON_COL2,BUTTON_ROW3,BUTTON_W,BUTTON_H,WHITE);
        playersEnabled = true;
        players_select = 5;
      }
    }
    delay(10);  
  }
}

/*
 * draw_board()
 * 
 * INPUT
 *      uint8_t num_players - Number of players in game
 *      uint8_t scenario - type of scenario 
 * OUTPUT
 *      none
 */
boolean draw_board(uint8_t num_players, uint8_t scenario) {
  // Erase board
  randomSeed(millis()); //set random seed
  tft.fillScreen(BLACK); // Background screen
  tft.drawRect(0,0,319,240,WHITE);

  Serial.println("Begin drawing board...");
  //boards will be type first layer / number second layer
  switch (scenario) {
    case 1: // Base Game
      if (num_players < 5) {
 
      } else {

      }
      break;
    case 2: // Sea Base
      if (num_players < 4) {
 
      } else if (num_players == 4) {
        
      } else {

      }
      break;
    case 3: // Thru the Desert
      if (num_players < 4) {
 
      } else if (num_players == 4) {
        
      } else {
        
      }
      break;
    case 4: // Islands
      if (num_players < 4) {
 
      } else if (num_players == 4) {
        
      } else {
        
      }
      break;
    default: // Debug?

      break;
  }
  if (scenario == 1 && num_players < 5) {
    //base game, 3/4 players
    //create stack of resources and numbers to populate the board
    //lets make an array of both then create a literal stack of these cards!
    Serial.println("Base game, 3 or 4 players begin...");
  }
    
}


/*
 * draw_hex()
 * 
 * INPUT
 *      int16_t x - Top left x coordinate of hex 
 *      int16_t y - Top left y coordinate of hex 
 *      uint32_t value - color value
 *      uint16_t number - number of tile to draw
 * OUTPUT
 *      none
 */
void draw_hex(int16_t x, int16_t y, uint32_t value, uint16_t number) { 
  //go row by row
  //row 0
  for(int16_t i=0;i<8;i++){
    tft.drawPixel(x+4+i,y,value);
  }
  //rows 1-8
  for(int16_t i=0;i<4;i++){
    for(int16_t j=0;j<2;j++){
      tft.drawPixel(x+3-i,y+1+(i*2)+j,value);
      tft.drawPixel(x+12+i,y+1+(i*2)+j,value);
    }
  }
  //rows 9-14
  for(int16_t i=0;i<3;i++){
    for(int16_t j=0;j<2;j++){
      tft.drawPixel(x+1+i,y+9+(i*2)+j,value);
      tft.drawPixel(x+14-i,y+9+(i*2)+j,value);
    }
  }
  for(int16_t i=0;i<8;i++){
    tft.drawPixel(x+4+i,y+15,value);
  }
  // draw value
  if (number > 9) {
    // draw the tens place
    for (int16_t j = 3; j < 13; j++) {
      tft.drawPixel(x+5,y+j,value);
    }
    tft.drawPixel(x+4,y+4,value);
  }
  switch (number%10) {
    case 0:
      for (int16_t j=4;j<12;j++){
        tft.drawPixel(x+7,y+j,value);
        tft.drawPixel(x+10,y+j,value);
      }
      tft.drawPixel(x+8,y+3,value);
      tft.drawPixel(x+9,y+3,value);
      tft.drawPixel(x+8,y+12,value);
      tft.drawPixel(x+9,y+12,value);
      break;
    case 1:
      // draw the ones place
      for (int16_t j = 3; j < 13; j++) {
        tft.drawPixel(x+9,y+j,value);
      }
      tft.drawPixel(x+8,y+4,value);
      break;
    case 2:
      for (int16_t i=7;i<11;i++) {
        tft.drawPixel(x+7,y+12,value);
      }
      tft.drawPixel(x+7,y+11,value);
      tft.drawPixel(x+7,y+10,value);
      tft.drawPixel(x+7,y+5,value);
      tft.drawPixel(x+7,y+4,value);
      
      tft.drawPixel(x+8,y+3,value);
      tft.drawPixel(x+8,y+9,value);
      
      tft.drawPixel(x+9,y+3,value);
      tft.drawPixel(x+9,y+8,value);
      
      tft.drawPixel(x+10,y+4,value);
      tft.drawPixel(x+10,y+5,value);
      tft.drawPixel(x+10,y+6,value);
      tft.drawPixel(x+10,y+7,value);
      break;
    case 3:
      tft.drawPixel(x+7,y+4,value);
      tft.drawPixel(x+7,y+11,value);
      
      tft.drawPixel(x+8,y+3,value);
      tft.drawPixel(x+8,y+8,value);
      tft.drawPixel(x+8,y+12,value);
      
      tft.drawPixel(x+9,y+3,value);
      tft.drawPixel(x+9,y+8,value);
      tft.drawPixel(x+9,y+12,value);
      
      tft.drawPixel(x+10,y+4,value);
      tft.drawPixel(x+10,y+5,value);
      tft.drawPixel(x+10,y+6,value);
      tft.drawPixel(x+10,y+7,value);
      tft.drawPixel(x+10,y+9,value);
      tft.drawPixel(x+10,y+10,value);
      tft.drawPixel(x+10,y+11,value);
      break;
    case 4:
      for (int16_t j=3;j<13;j++){
        tft.drawPixel(x+10,y+j,value);
      }
      tft.drawPixel(x+11,y+7,value);

      tft.drawPixel(x+7,y+5,value);
      tft.drawPixel(x+7,y+6,value);
      tft.drawPixel(x+7,y+7,value);
      
      tft.drawPixel(x+8,y+3,value);
      tft.drawPixel(x+8,y+4,value);
      tft.drawPixel(x+8,y+7,value);
      
      tft.drawPixel(x+9,y+7,value);
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    default:
      break;
  }
}

