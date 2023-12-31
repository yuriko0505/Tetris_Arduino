#include <Arduino.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "components.h"

#define BAUD_RATE 9600
#define FALL_SPEED 1500  // [ms]
#define FLASH_SPEED 150  // [ms]  half cycle of flashing
#define SCROLL_SPEED 100  // [ms]  the bigger, the slower
#define TEXT_FONT Font_5x7

ArduinoLEDMatrix matrix;
unsigned long lastTime;

void generate_block();
void rotate_block();
bool move_block(int8_t dir_x, int8_t dir_y);
void erase_rows();
void update_field(uint8_t on);
bool isGameOver();
void opening();
void ending();

void setup() {
  Serial.begin(BAUD_RATE);
  matrix.begin();

  // opening
  opening();
  delay(10);
  
  randomSeed(analogRead(A0));
  generate_block();
  update_field(1);
  matrix.renderBitmap(field, 8, 12);
  lastTime = millis();
}

bool advanceField = false;
bool gameOver = false;

int8_t block_x;
int8_t block_y;
uint8_t current_block[BLOCK_WIDTH][BLOCK_HEIGHT];
uint16_t score = 0;
char input;

void loop() {
  if (gameOver) {
    ending();
  }
  if (millis() - lastTime > FALL_SPEED || advanceField) {
    if (advanceField || move_block(0, 1) == false) {
      erase_rows();
      if (isGameOver()) {
        gameOver = true;
        return;
      }
      delay(500);
      generate_block();
      update_field(1);
    }
    matrix.renderBitmap(field, 8, 12);
    advanceField = false;
    lastTime = millis();
  }
  if (Serial.available() > 0) {
    input = Serial.read();
    switch(input) {
      case 'w':
        rotate_block();
        break;
      case 'a':
        move_block(1, 0);
        break;
      case 'd':
        move_block(-1, 0);
        break;
      case 's':
        while(move_block(0, 1));
        advanceField = true;
        break;
      default:
        break;
    }
    matrix.renderBitmap(field, 8, 12);
  }
}

void generate_block() {
  uint8_t block_type = random(7);
  memcpy(current_block, block[block_type], sizeof(current_block));
  block_x = 2;
  block_y = 0;
}

// rotate clockwise
void rotate_block() {
  uint8_t temp[BLOCK_WIDTH][BLOCK_HEIGHT];
  for (uint8_t x = 0; x < BLOCK_WIDTH; x++) {
    for (uint8_t y = 0; y < BLOCK_HEIGHT; y++) {
      temp[x][y] = current_block[3-y][x];
    }
  }
  update_field(0);
  memcpy(current_block, temp, sizeof(temp));
  update_field(1);
}

// left: dir_x++, down: dir_y++
bool move_block(int8_t dir_x, int8_t dir_y) {
  update_field(0);
  for (int8_t x = 0; x < BLOCK_WIDTH; x++) {
    for (int8_t y = 0; y < BLOCK_HEIGHT; y++) {
      if (current_block[x][y] == 1) {
        int8_t temp_x = x + block_x + dir_x;
        int8_t temp_y = y + block_y + dir_y;
        if (temp_x < 0 || temp_x >= FIELD_WIDTH
              || temp_y < 0 || temp_y >= FIELD_HEIGHT) {
          update_field(1);
          return false;
        }
        if (field[temp_x][temp_y] == 1) {
          update_field(1);
          return false;
        }
      }
    }
  }
  block_x += dir_x;
  block_y += dir_y;
  update_field(1);
  return true;
}

void erase_rows() {
  bool isFilledRow[FIELD_HEIGHT] = {false};
  uint8_t count = 0;
  for (int8_t y = FIELD_HEIGHT-1; y >= 0; y--) {
    bool isFilled = true;
    for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
      if (field[x][y] == 0) {
        isFilled = false;
        break;
      }
    }
    if (isFilled) {
      isFilledRow[y] = true;
      count++;
    }
  }
  if (count == 0) return;

  score += 10 * count;
  // 消える行をチカチカさせる
  for (uint8_t i = 0; i < 3; i++) {
    for (int8_t y = FIELD_HEIGHT-1; y >= 0; y--) {
      if (isFilledRow[y]) {
        for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
          field[x][y] = 0;
        }
      }
    }
    matrix.renderBitmap(field, 8, 12);
    delay(FLASH_SPEED);
    for (int8_t y = FIELD_HEIGHT-1; y >= 0; y--) {
      if (isFilledRow[y]) {
        for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
          field[x][y] = 1;
        }
      }
    }
    matrix.renderBitmap(field, 8, 12);
    delay(FLASH_SPEED);
  }
  delay(200);
  // 行を消し、詰める
  int8_t ny = FIELD_HEIGHT - 1;
  for (int8_t y = FIELD_HEIGHT-1; y >= 0; y--) {
    if (isFilledRow[y]) continue;
    if (ny != y) {
      for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
        field[x][ny] = field[x][y];
        field[x][y] = 0;
      }
    }
    ny--;
  }
  matrix.renderBitmap(field, 8, 12);
}

void update_field(uint8_t on) {
  for (int8_t x = 0; x < BLOCK_WIDTH; x++) {
    for (int8_t y = 0; y < BLOCK_HEIGHT; y++) {
      if (current_block[x][y] == 1) {
        field[x + block_x][y + block_y] = on;
      }
    }
  }
}

bool isGameOver() {
  bool flag = false;
  for (int8_t x = 2; x <= 4; x++) {
    if (field[x][0] == 1) flag = true;
  }
  if (flag) {
    uint8_t noLED[FIELD_WIDTH][FIELD_HEIGHT] = {0};
    for (uint8_t i = 0; i < 3; i++) {
      matrix.renderBitmap(noLED, 8, 12);
      delay(FLASH_SPEED);
      matrix.renderBitmap(field, 8, 12);
      delay(FLASH_SPEED);
    }
    delay(500);
  }
  return flag;
}

void opening() {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(SCROLL_SPEED);
  matrix.textFont(TEXT_FONT);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println("TETRIS   ");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}

void ending() {
  char message[40];
  sprintf(message, "Thx 4 Playing!  score: %d   ", score);
  while(true) {
    matrix.beginDraw();
    matrix.stroke(0xFFFFFFFF);
    matrix.textScrollSpeed(SCROLL_SPEED);
    matrix.textFont(TEXT_FONT);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.println(message);
    matrix.endText(SCROLL_LEFT);
    matrix.endDraw();
  }
}