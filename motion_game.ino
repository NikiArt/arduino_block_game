/* 
Тестировалось на Arduino IDE 1.8.0
Дата тестирования 29.08.2017г.
*/ 

#include <SD.h>
#include <TFT.h>                 // Подключаем библиотеку TFT  
#include <SPI.h>                 // Подключаем библиотеку SPI
 
#define CS   10                  // Указываем пины cs
#define DC   9                   // Указываем пины dc (A0)
#define RESET  8                   // Указываем пины reset

#define pinX    A2  // ось X джойстика
#define pinY    A1  // ось Y джойстика
#define swPin    2  // кнопка джойстика
 
TFT myScreen = TFT(CS, DC, RESET);

// initial position of the point is the middle of the screen
// initial position of the point is the middle of the screen
int xPos = 64;
int yPos = 64;

// direction and speed
int xDir;
int yDir;

// variables to keep track of the point's location
int xPrev = xPos;
int yPrev = yPos;

int topBorder = 17;
int bottomBorder = 121;
int gameSpeed = 40;

int scores = 0;
int topScores = 0;

char scoreText[6];
char topText[6];
String scoreVal;
String topVal;

int countDelay = 30;
int scoreCount = 0;
int cooldown = 100;
int cooldownCount = 0;

int dotSize = 5;

bool isCollized;

bool isPreviewShown = false;

class Block {
  public:
    bool isStarted = false;
    
    Block() {
      mBlockWidth = random(2, 80);
      mBlockHeight = random(10, 50);
      blockSpeed = random(1, 4);
      startYpos = random(topBorder, bottomBorder-mBlockHeight);
    }

    void startBlock() {
      currentXpos = 128;
      myScreen.stroke(255,255,255);
      myScreen.noFill();
      myScreen.rect(currentXpos, startYpos, mBlockWidth, mBlockHeight);
      isStarted = true;
    }

    void moveBlock() {
      cleanPos();
           
      currentXpos -= blockSpeed;
      myScreen.stroke(255,255,255);
      myScreen.noFill();
      myScreen.rect(currentXpos, startYpos, mBlockWidth, mBlockHeight);
      isCollized = (isCollized) ? isCollized : checkCollise();
    }

    void cleanPos() {
      myScreen.stroke(0,0,0);
      myScreen.noFill();
      myScreen.rect(currentXpos, startYpos, mBlockWidth, mBlockHeight);
    }

    bool isGone() {
      return currentXpos + mBlockWidth < 0;
    }

    bool checkCollise() {
      //Serial.println(String("Курсор: Х = ") + xPos + ", У = " + yPos + ". Блок (Х " + currentXpos + ", У " + startYpos + ", Ш " + mBlockWidth + ", В " + mBlockHeight + ")");
      return yPos + dotSize >= startYpos && yPos <= startYpos + mBlockHeight && xPos + dotSize >= currentXpos && xPos <= currentXpos + mBlockWidth;
    }

  private:
    int mBlockWidth;
    int mBlockHeight;
    int startYpos;
    int currentXpos = 128;
    int blockSpeed;
  
};

Block enemyObj1;
Block enemyObj2;
Block enemyObj3;
Block enemyObj4;

void initGame() {
  isCollized = false;
  isPreviewShown = false;
  xDir = 0;
  yDir = 0;
  myScreen.background(0,0,0); // clear the screen
  myScreen.noFill();

  scores = 0;
  myScreen.setTextSize(1); 
  myScreen.stroke(250,220,0);
  myScreen.text("scores: ", 5, 8);

  scoreVal = String(scores);
  scoreVal.toCharArray(scoreText, 6);
  myScreen.stroke(250,220,0);
  myScreen.text(scoreText, 46, 8);

  myScreen.stroke(250,220,0);
  myScreen.text("top: ", 80, 8);

  topVal = String(topScores);
  topVal.toCharArray(topText, 6);
  myScreen.stroke(250,220,0);
  myScreen.text(topText, 104, 8);

  myScreen.stroke(255,255,255);
  myScreen.line(0, topBorder, 128, topBorder);
  myScreen.line(0, bottomBorder, 128, bottomBorder);
  enemyObj1 = Block();
  enemyObj2 = Block();
  enemyObj3 = Block();
  enemyObj4 = Block();
  enemyObj1.startBlock();
}

void showPreview(bool newGame = false) {
  if (!isPreviewShown) {
    myScreen.stroke(0,0,0); // set the stroke color to black   
    myScreen.rect(xPrev, yPrev, dotSize, dotSize);
    enemyObj1.cleanPos();
    enemyObj2.cleanPos();
    enemyObj3.cleanPos();
    enemyObj4.cleanPos();
    
    myScreen.background(0,0,0);
    myScreen.setTextSize(3);
    myScreen.stroke(255,255,255);
    myScreen.fill(230,140,0);
    myScreen.rect(10, 20, 112, 100);
    if (newGame) {
      myScreen.text("MEGA", 32, 30);
      myScreen.text("BLOCKS", 13, 60);
  
      myScreen.setTextSize(1);
      myScreen.text("Press Button", 30, 90);
      myScreen.text("to start", 43, 100);
    } else {
      myScreen.text("Game", 20, 30);
      myScreen.text("Over", 40, 55);
  
      myScreen.setTextSize(1);
      myScreen.text("Press Button", 30, 90);
      myScreen.text("to try again", 30, 100);
    }
    isPreviewShown = true;
    isCollized = true;
  }
}

void setup(){
  Serial.begin(9600);
  pinMode(swPin, INPUT_PULLUP);

  myScreen.begin();
  showPreview(true);
}
 
void loop(){
  if (!isCollized) {
    // block one 
    if (!enemyObj1.isStarted) {
      if (cooldownCount < cooldown) {
        cooldownCount++;
      } else {
          int tryNew = random(5);
          if (tryNew == 4) {
            enemyObj1.startBlock();
            cooldownCount = 0;
          }
      }
    } else {  
      if (enemyObj1.isGone()) {
        enemyObj1.cleanPos();
        enemyObj1 = Block();
        if (cooldownCount < cooldown) {
          cooldownCount++;
        } else {
          int tryNew = random(10);
          if (tryNew == 4) {
            enemyObj1.startBlock();
            cooldownCount = 0;
          }
        }
      } else {
        enemyObj1.moveBlock();
      }
    }

    // block two
    if (!enemyObj2.isStarted) {
      if (cooldownCount < cooldown) {
        cooldownCount++;
      } else {
          int tryNew = random(10);
          if (tryNew == 4) {
            enemyObj2.startBlock();
            cooldownCount = 0;
          }
      }
    } else {  
      if (enemyObj2.isGone()) {
        enemyObj2.cleanPos();
        enemyObj2 = Block();
        if (cooldownCount < cooldown) {
          cooldownCount++;
        } else {
          int tryNew = random(10);
          if (tryNew == 4) {
            enemyObj2.startBlock();
            cooldownCount = 0;
          }
        }
      } else {
        enemyObj2.moveBlock();
      }
    }

    // block three
    if (!enemyObj3.isStarted) {
      if (cooldownCount < cooldown) {
        cooldownCount++;
      } else {
          int tryNew = random(10);
          if (tryNew == 4) {
            enemyObj3.startBlock();
            cooldownCount = 0;
          }
      }
    } else {  
      if (enemyObj3.isGone()) {
        enemyObj3.cleanPos();
        enemyObj3 = Block();
        if (cooldownCount < cooldown) {
          cooldownCount++;
        } else {
          int tryNew = random(10);
          if (tryNew == 4) {
            enemyObj3.startBlock();
            cooldownCount = 0;
          }
        }
      } else {
        enemyObj3.moveBlock();
      }
    }

    // block four
    if (!enemyObj4.isStarted) {
      if (cooldownCount < cooldown) {
        cooldownCount++;
      } else {
          int tryNew = random(10);
          if (tryNew == 4) {
            enemyObj4.startBlock();
            cooldownCount = 0;
          }
      }
    } else {  
      if (enemyObj4.isGone()) {
        enemyObj4.cleanPos();
        enemyObj4 = Block();
        if (cooldownCount < cooldown) {
          cooldownCount++;
        } else {
          int tryNew = random(5);
          if (tryNew == 4) {
            enemyObj4.startBlock();
            cooldownCount = 0;
          }
        }
      } else {
        enemyObj4.moveBlock();
      }
    }
      
    int X = analogRead(pinX);              // считываем значение оси Х
    int Y = analogRead(pinY);              // считываем значение оси Y
  
    xDir = X/100 - 5;
    yDir = Y/100 - 5;
  
    xPos = xPos + xDir;
    yPos = yPos + yDir;
    
    if (yPos > bottomBorder - dotSize - 1) {
        yPos = bottomBorder - dotSize - 1;
    }
    if (yPos < topBorder + 1) {
        yPos = topBorder + 1;
    }
    if (xPos > 127 - dotSize) {
        xPos = 127 - dotSize;
    }
    if (xPos < 3) {
        xPos = 3;
    }

    if(xPos != xPrev || yPos != yPrev){
      myScreen.stroke(0,0,0); // set the stroke color to black 
      myScreen.fill(0,0,0);  
      myScreen.rect(xPrev, yPrev, dotSize, dotSize); // color in the previous point
    }

    myScreen.stroke(50,250,0);
    myScreen.fill(50,250,0);
    myScreen.rect(xPos, yPos, dotSize, dotSize);
  
    xPrev=xPos;
    yPrev=yPos;
    if (scoreCount < countDelay) {
      scoreCount++;
    } else {
      scoreCount = 0;
      scoreVal = String(scores);
      scoreVal.toCharArray(scoreText, 6);
      myScreen.stroke(0,0,0);
      myScreen.text(scoreText, 46, 8);
      scores++;
      scoreVal = String(scores);
      scoreVal.toCharArray(scoreText, 6);
      myScreen.stroke(250,220,0);
      myScreen.text(scoreText, 46, 8);
    }
    switch (scores) {
      case 1000: 
        gameSpeed = 7;
        break;
      case 700: 
        gameSpeed = 10;
        break;
      case 500: 
        gameSpeed = 14;
        break;
      case 350: 
        gameSpeed = 17;
        break;
        break;
      case 200: 
        gameSpeed = 20;
        break;
      case 100: 
        gameSpeed = 24;
        break;
      case 70: 
        gameSpeed = 27;
        break;
      case 50: 
        gameSpeed = 30;
        break;
      case 30: 
        gameSpeed = 32;
        break;
      case 20: 
        gameSpeed = 35;
        break;
      case 10: 
        gameSpeed = 38;
        break;
      default:
        break;
    }
    delay(gameSpeed);
  } else {
    showPreview();
    if (digitalRead(swPin) == 0) {
      topScores = (topScores < scores) ? scores : topScores;
      initGame();
    }
  }
}
