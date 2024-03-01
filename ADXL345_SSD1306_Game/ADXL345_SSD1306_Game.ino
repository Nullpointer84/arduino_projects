#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int screenWidth = 128;
const int screenHeight = 64;
const int wallWidth = 2;
const int wallHeight = 2;
const int unitWidth = 4;
const int unitHeight = 4;
const int goalWidth = 8;
const int goalHeight = 8;

const int start_point_x = 0;
const int start_point_y = 0;

int xUnit = 0;
int yUnit = 0;

struct Barrier {
    float x, y, width, height;
    bool collidesWithUnit(float unitX, float unitY) const {
        return (x < unitX + unitWidth &&
                x + width > unitX &&
                y < unitY + unitHeight &&
                y + height > unitY);
    }
    Barrier* next;
};

struct Level {
  Barrier* barriers;
  Barrier goal[17] = {};
  int goal_x;
  int goal_y;
};

void addElement(Level& level, float x, float y, float width, float height) {
    Barrier* newBarrier = new Barrier{ x, y, width, height, nullptr };
    if (level.barriers == nullptr) {
        level.barriers = newBarrier;
    } else {
      newBarrier->next = level.barriers;
      level.barriers = newBarrier;
    }
}

Level level_one;
Level level_two;
Level level_three;

Level* current_level;
int current_level_counter = 1;

void generateGoalArray(float base_x, float base_y, Barrier goal[]) {
  goal[0] = {base_x, base_y, 2, 1};
  goal[1] = {base_x+2, base_y, 2, 1};
  goal[2] = {base_x+4, base_y, 2, 1};
  goal[3] = {base_x+6, base_y, 2, 1};

  goal[4] = {base_x, base_y, 1, 2};
  goal[5] = {base_x, base_y+2, 1, 2};
  goal[6] = {base_x, base_y+4, 1, 2};
  goal[7] = {base_x, base_y+6, 1, 2};

  goal[8] = {base_x, base_y+8, 2, 1};
  goal[9] = {base_x+2, base_y+8, 2, 1};
  goal[10] = {base_x+4, base_y+8, 2, 1};
  goal[11] = {base_x+6, base_y+8, 2, 1};

  goal[12] = {base_x+8, base_y, 1, 2};
  goal[13] = {base_x+8, base_y+2, 1, 2};
  goal[14] = {base_x+8, base_y+4, 1, 2};
  goal[15] = {base_x+8, base_y+6, 1, 2};
  goal[16] = {0, 0, 0, 0};
}

void nextLevel(void) {
  current_level_counter++;
  switch(current_level_counter) {
    case 1: 
      current_level = &level_one;
    break;
    case 2:
      current_level = &level_two;
    break;
    case 3:
      current_level = &level_three;
    break;
    default:
      current_level = &level_one;
      current_level_counter = 1;
    break;
  }
}

void setup(void) 
{
  Wire.begin(32, 33);
  Serial.begin(115200);
  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  createLevelOne();
  createLevelTwo();
  createLevelThree();
  current_level = &level_one;
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
}

void createLevelThree(void) 
{
  addElement(level_three, 0, 8, screenWidth-8, wallHeight);
  addElement(level_three, 8, 24, screenWidth-8, 12);
  addElement(level_three, 0, 50, screenWidth-8, wallHeight);
  level_three.goal_x = 2;
  level_three.goal_y = screenHeight-10;
  generateGoalArray(level_three.goal_x, level_three.goal_y, level_three.goal);
}

void createLevelTwo(void) 
{
  addElement(level_two, 0, 2*8, 14*8, wallHeight);
  addElement(level_two, 14*8, 2*8, wallWidth, 4*8+2);
  addElement(level_two, 8, 6*8, 13*8, wallHeight);
  addElement(level_two, 8, 4*8, wallWidth, 2*8);
  addElement(level_two, 8, 4*8, 10*8, wallHeight);
  level_two.goal_x = 12;
  level_two.goal_y = 4*8+4;
  generateGoalArray(level_two.goal_x, level_two.goal_y, level_two.goal);
}

void createLevelOne(void) 
{
  u_int yUpper = 16;
  u_int yLower = 64-yUpper;
  addElement(level_one, 16*1, 0, wallWidth, yLower);
  addElement(level_one, 16*2, yUpper, wallWidth, yLower);
  addElement(level_one, 16*3, 0, wallWidth, yLower);
  addElement(level_one, 16*4, yUpper, wallWidth, yLower);
  addElement(level_one, 16*5, 0, wallWidth, yLower);
  addElement(level_one, 16*6, yUpper, wallWidth, yLower);
  addElement(level_one, 16*7, 0, wallWidth, yLower);
  level_one.goal_x = 16*7+6;
  level_one.goal_y = 2;
  generateGoalArray(level_one.goal_x, level_one.goal_y, level_one.goal);
}

float roundToOneDecimal(float num) {
    return round(num * 10.0) / 10.0;
}

int goal_color = SSD1306_WHITE;
void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  accel.getEvent(&event);

  float roll = roundToOneDecimal(event.acceleration.roll);
  float pitch = roundToOneDecimal(event.acceleration.pitch);

  int xNew = xUnit;
  int yNew = yUnit;

  xNew += roll;
  yNew -= pitch;

  if(xNew>screenWidth-unitWidth) {
    xNew = screenWidth-unitWidth;
  } else if(xNew<0) {
    xNew = 0;
  }

  if(yNew>screenHeight-unitHeight) {
    yNew = screenHeight-unitHeight;
  } else if(yNew<0) {
    yNew = 0;
  }
  
  display.clearDisplay();

  bool restart_collision = false;
  Level& currentLevelStruct = *current_level;
  Barrier* head = currentLevelStruct.barriers;
  while(head  != nullptr) {
    Barrier* barrier = head;
    display.fillRect(barrier->x, barrier->y, barrier->width, barrier->height, SSD1306_WHITE);
    if(barrier->collidesWithUnit(xNew, yUnit) || barrier->collidesWithUnit(xUnit, yNew)) {
      restart_collision = true;
      break;
    }
    head = barrier->next;
  }
  
  bool goal_collision = false;
  for (int i = 0; i < 17; i++) {
    Barrier currentBarrier = currentLevelStruct.goal[i];
    display.fillRect(currentBarrier.x, currentBarrier.y, currentBarrier.width, currentBarrier.height, goal_color);
    if(goal_color==SSD1306_WHITE)  {
      goal_color = SSD1306_BLACK;
    } else {
      goal_color = SSD1306_WHITE;
    }
    if(currentBarrier.collidesWithUnit(xNew, yNew)) {
      goal_collision = true;
      break;
    }
  }

  xUnit = xNew;
  yUnit = yNew;
  if(restart_collision) {
    xUnit = start_point_x;
    yUnit = start_point_y;
  }

  if(goal_collision) {
    xUnit = start_point_x;
    yUnit = start_point_y;
    nextLevel();
  }

  // Draw unit
  display.fillRect(xUnit, yUnit, 1, 1, SSD1306_WHITE);
  display.fillRect(xUnit+3, yUnit, 1, 1, SSD1306_WHITE);
  display.fillRect(xUnit, yUnit+3, 1, 1, SSD1306_WHITE);
  display.fillRect(xUnit+3, yUnit+3, 1, 1, SSD1306_WHITE);
  display.fillRect(xUnit+1, yUnit+1, 2, 2, SSD1306_WHITE);

  display.display();

  delay(50);
}
