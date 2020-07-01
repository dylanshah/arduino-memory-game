#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <EEPROM.h>
#include <avr/eeprom.h>
#include <TimerOne.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define S_N 1
#define S_M 2
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

uint8_t state = S_N;
unsigned long now;
String uname;
char a;
char b;
char c;
char d;
char e;
char f;
char g;
char h;
char i;

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.setTimeout(10);
  now = millis();
  state = S_N;
  randomSeed(analogRead(0));
}

//This function is shown on startup where user is shown the 2 modes to choose from
void menu() {
  lcd.setBacklight(VIOLET);
  lcd.setCursor(0, 0);
  lcd.print("Practice Mode");
  lcd.setCursor(0, 1);
  lcd.print("Story Mode");
}

char buffer[12];
int lenN = 1, sizM = 1;
int sizeM;
int lengthN;

//This displays the screen where the user inputs the size of M and length N
void display(int lenN, int sizM, int lengthN, int sizeM) {
  lcd.setCursor(0, 0);
  sprintf(buffer, "%02d:%01d", lenN, sizM);
  lcd.print(buffer);
  lcd.setCursor(0, 1);
  lcd.print("LenN:sizM");
  lcd.setCursor(12,0);
  sprintf(buffer, "%02d:%01d", lengthN, sizeM);
  lcd.print(buffer); //Shows the previous selection on the screen
  lcd.setCursor(12,1);
  lcd.print("Prev");
}

uint8_t old_buttons = 0;
bool setinput = true;
bool clearinput = true;
uint8_t buttons, changes;

void select() {
  buttons = lcd.readButtons();
  changes = old_buttons & (~buttons);
  //Calls the display function to show on screen
  display(lenN, sizM, lengthN, sizeM);
  //Switch statement for the the 2 cases to set the size M and length N depending on the buttons pressed
  switch (state) {
    case 1:
      if (changes & BUTTON_UP) {
        lenN = (lenN == 10 ? 1 : lenN + 1);
      } else if (changes & BUTTON_DOWN) {
        lenN = (lenN == 1 ? 10 : lenN - 1);
      } else if (changes & BUTTON_RIGHT) {
        state = S_M;
      } else if (changes & BUTTON_SELECT) {
        lengthN = lenN;
        sizeM = sizM;
        setinput = true;
        clearinput = false;
      }
      break;
    case 2:
      if (changes & BUTTON_UP) {
        sizM = (sizM == 4 ? 1 : sizM + 1);
      } else if (changes & BUTTON_DOWN) {
        sizM = (sizM == 1 ? 4 : sizM - 1);
      } else if (changes & BUTTON_LEFT) {
        state = S_N;
      } else if (changes & BUTTON_SELECT) {
        lengthN = lenN;
        sizeM = sizM;
        setinput = true;
        clearinput = false;
      }
      break;
  }
  old_buttons = buttons;

}

String messagetop = "                Welcome to Practice Mode!       ";
String messagebottom = "Press a button!";
unsigned int tpos = 0;

void practiceScreen() {
  //Scrolls the welcome message accross the screen
  lcd.setCursor(0, 0);
  lcd.print(messagetop.substring(tpos, (tpos + 16)));
  if (millis() - now > 100) {
    now = millis();
    if (++tpos > messagetop.length()) {
      tpos = 0;
    }
  }
  lcd.setCursor(0, 1);
  lcd.print(messagebottom);
}

String messagetop1 = "                Enter username in               ";
String messagebottom1 = "Serial Monitor";
unsigned int tpos1 = 0;

void storyMode() {
  //Scrolls the message accross the screen
  lcd.setCursor(0, 0);
  lcd.print(messagetop1.substring(tpos1, (tpos1 + 16)));
  if (millis() - now > 100) {
    now = millis();
    if (++tpos1 > messagetop1.length()) {
      tpos1 = 0;
    }
  }
  lcd.setCursor(0, 1);
  lcd.print(messagebottom1);
}


//Function to display a loosing message onto the screen
void Lose() {
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  for (int i = 0; i < 4; i++) {
    //Alernates the lighting to display a flashing effect
    lcd.setBacklight(RED);
    delay(500);
    lcd.setBacklight(WHITE);
    delay(500);
  }
}

//Function to display a Winning message after each level
void Win() {
  lcd.setCursor(0, 0);
  lcd.print("Congratulations!");
  for (int i = 0; i < 4; i++) {
    //Flashing effect
    lcd.setBacklight(GREEN);
    delay(500);
    lcd.setBacklight(WHITE);
    delay(500);
  }
}

//Function to display the 2 difficulty levels to choose from
void mode() {
  lcd.setBacklight(BLUE);
  lcd.setCursor(0, 0);
  lcd.print("Beginner");
  lcd.setCursor(0, 1);
  lcd.print("Expert");
}


//Function that randomly generates the button to press in a sequence depending on the size of M chosen by the user
String generatecommand(int siM) {
  String command = "";
  int rancommand = random(1, (siM + 1));
  switch (rancommand) {
    case 1:
      command = "UP";
      break;
    case 2:
      command = "LEFT";
      break;
    case 3:
      command = "RIGHT";
      break;
    case 4:
      command = "DOWN";
  }
  lenN = 1;
  sizM = 1;
  return command;
}

//Function to repeat the generate command to make a sequence of length N, which was chosen by the user
//Also takes the input of the user and checks if it corresponds to the button at that position in the array
//It displays the respective message if the sequence entered is correct or incorrect
void sequence(int lengthN, int sizeM) {
  String line[lengthN];
  for (int i = 0; i < lengthN; i++) {
    //Assigns the button name to be pressed to a array of size N
    line[i] = generatecommand(sizeM);
    lcd.print(line[i]);//Prints the sequence of buttons to be pressed on the screen for 1 second each
    delay(1000);
    lcd.clear();
    delay(1000);
  }
  lcd.print("Enter Sequence");
  delay(500);
  lcd.clear();
  //defins the buttons and allows for user input
  uint8_t input1, input2, change2, change3;
  int j = 0;
  while (j < lengthN) {
    if (line[j] == "UP") { //Only allows UP button if UP is required in the array at that index position 
      input1 = lcd.readButtons();
      change2 = input1;
      if (change2 & BUTTON_UP) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("UP");
        input2 = lcd.readButtons();
        change3 = input2;
        if (!(change3 & BUTTON_UP)) {
          //Moves to the next index position in the array only when button is released
          j++;
        }
      } else if (change2 & BUTTON_LEFT) {
        Lose();
        break;
      } else if (change2 & BUTTON_RIGHT) {
        Lose();
        break;
      } else if (change2 & BUTTON_DOWN) {
        Lose();
        break;
      }
    }
    if (line[j] == "DOWN") {
      input1 = lcd.readButtons();
      change2 = input1;
      if (change2 & BUTTON_DOWN) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("DOWN");
        input2 = lcd.readButtons();
        change3 = input2;
        if (!(change3 & BUTTON_DOWN)) {
          j++;
        }
      } else if (change2 & BUTTON_LEFT) {
        Lose();
        break;
      } else if (change2 & BUTTON_RIGHT) {
        Lose();
        break;
      } else if (change2 & BUTTON_UP) {
        Lose();
        break;
      }
    }
    if (line[j] == "LEFT") {
      input1 = lcd.readButtons();
      change2 = input1;
      if (change2 & BUTTON_LEFT) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("LEFT");
        input2 = lcd.readButtons();
        change3 = input2;
        if (!(change3 & BUTTON_LEFT)) {
          j++;
        }
      } else if (change2 & BUTTON_UP) {
        Lose();
        break;
      } else if (change2 & BUTTON_RIGHT) {
        Lose();
        break;
      } else if (change2 & BUTTON_DOWN) {
        Lose();
        break;
      }
    }
    if (line[j] == "RIGHT") {
      input1 = lcd.readButtons();
      change2 = input1;
      if (change2 & BUTTON_RIGHT) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RIGHT");
        input2 = lcd.readButtons();
        change3 = input2;
        if (!(change3 & BUTTON_RIGHT)) {
          j++;
        }
      } else if (change2 & BUTTON_LEFT) {
        Lose();
        break;
      } else if (change2 & BUTTON_UP) {
        Lose();
        break;
      } else if (change2 & BUTTON_DOWN) {
        Lose();
        break;
      }
    }
  }
  //If the whole sequence is entered correctly, the Win function is called
  if (j == lengthN) {
    Win();
  }
}

int delays;
bool dispstoryexp = true;
bool dispstorybeg = true;
bool clearstorymessage = true;
int level = 1;
int stage = level;

//Function for story mode to display the sequence at random
void storysequence(int lengthN, int sizeM) {
  String line[lengthN];
  for (int i = 0; i < lengthN; i++) {
    line[i] = generatecommand(sizeM);
    lcd.print(line[i]);
    delay(delays); //Shows each command from the array on the screen for a preset amount of time
    lcd.clear();
    delay(delays / 2);
  }
  delay(delays);
  lcd.print("Enter Sequence");
  delay(500);
  lcd.clear();
  uint8_t input1, input2, change2, change3;
  int j = 0;
  while (j < lengthN) {
    if (line[j] == "UP") {
      input1 = lcd.readButtons();
      change2 = input1;
      if (change2 & BUTTON_UP) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("UP");
        input2 = lcd.readButtons();
        change3 = input2;
        if (!(change3 & BUTTON_UP)) {
          //Moves to the next index postion only after the button pressed is released
          j++;
        }
      } else if (change2 & BUTTON_LEFT) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      } else if (change2 & BUTTON_RIGHT) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      } else if (change2 & BUTTON_DOWN) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      }

    }
    if (line[j] == "DOWN") {
      input1 = lcd.readButtons();
      change2 = input1;
      if (change2 & BUTTON_DOWN) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("DOWN");
        input2 = lcd.readButtons();
        change3 = input2;
        if (!(change3 & BUTTON_DOWN)) {
          j++;
        }

      } else if (change2 & BUTTON_LEFT) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      } else if (change2 & BUTTON_RIGHT) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      } else if (change2 & BUTTON_UP) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      }
    }
    if (line[j] == "LEFT") {
      input1 = lcd.readButtons();
      change2 = input1;
      if (change2 & BUTTON_LEFT) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("LEFT");
        input2 = lcd.readButtons();
        change3 = input2;
        if (!(change3 & BUTTON_LEFT)) {
          j++;
        }

      } else if (change2 & BUTTON_UP) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      } else if (change2 & BUTTON_RIGHT) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      } else if (change2 & BUTTON_DOWN) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      }
    }
    if (line[j] == "RIGHT") {
      input1 = lcd.readButtons();
      change2 = input1;
      if (change2 & BUTTON_RIGHT) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RIGHT");
        input2 = lcd.readButtons();
        change3 = input2;
        if (!(change3 & BUTTON_RIGHT)) {
          j++;
        }

      } else if (change2 & BUTTON_LEFT) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      } else if (change2 & BUTTON_UP) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      } else if (change2 & BUTTON_DOWN) {
        Lose();
        level = 1;
        dispstoryexp = true;
        dispstorybeg = true;
        clearstorymessage = false;
        break;
      }
    }
  }
  if (j == lengthN) {
    //Calls the WIn function only if the whole sequence is entered correctly
    Win();
  }
}

int high = EEPROM.read(0);
int k = 0;
char first;
char second;
char third;

//Function to set the top 3 high scores into the EEPROM, for later showing
void highscore(int levels, String initial) {
  first = initial[0]; //Gets the first 3 characters from the string input in the Serial Monitor
  second = initial[1];
  third = initial[2];
  if (levels > high) {
    EEPROM.update(8, EEPROM.read(4)); //If a new high score is set it is set to position 0, the rest of the high scores are moved down a postion
    EEPROM.update(9, EEPROM.read(5));
    EEPROM.update(10, EEPROM.read(6));
    EEPROM.update(11, EEPROM.read(7));
    
    EEPROM.update(4, EEPROM.read(0));
    EEPROM.update(5, EEPROM.read(1));
    EEPROM.update(6, EEPROM.read(2));
    EEPROM.update(7, EEPROM.read(3));
    
    EEPROM.update(k, levels);
    EEPROM.update(1, first);
    EEPROM.update(2, second);
    EEPROM.update(3, third);
    
  } else if (levels <= high) {
    k++;    
    if (levels >= EEPROM.read(k)) {
      EEPROM.update(8, EEPROM.read(4));
      EEPROM.update(9, EEPROM.read(5));
      EEPROM.update(10, EEPROM.read(6));
      EEPROM.update(11, EEPROM.read(7));
      
      EEPROM.update(4, levels);
      EEPROM.update(5, first);
      EEPROM.update(6, second);
      EEPROM.update(7, third);
    
    } else if (levels < EEPROM.read(k)) {
      k++;
      if (levels >= EEPROM.read(k)) {
        
      EEPROM.update(8, levels);
      EEPROM.update(9, first);
      EEPROM.update(10, second);
      EEPROM.update(11, third);
      }
    } 
  }
}

//Function to show the top 3 high scores onto the screen
void highscoredisp() {
  a = EEPROM.read(1);
  b = EEPROM.read(2);
  c = EEPROM.read(3);
  d = EEPROM.read(5);
  e = EEPROM.read(6);
  f = EEPROM.read(7);
  g = EEPROM.read(9);
  h = EEPROM.read(10);
  i = EEPROM.read(11);
  lcd.setBacklight(GREEN);
  lcd.setCursor(0,0);
  lcd.print("Top");
  lcd.setCursor(4,0);
  String names; //String to display the usernames with the high scores
  names += "|";
  names += a;
  names += b;
  names += c;
  names += "|";
  names += b;
  names += e;
  names += f;
  names += "|";
  names += g;
  names += h;
  names += i;
  lcd.print(names);
  lcd.setCursor(1,1);
  lcd.print("3");
  lcd.setCursor(4,1);
  String high;
  high += "| ";
  high += EEPROM.read(0); //reads the EEPROM values at the position of the 3 highscores then appends it to a string for display
  high += " | ";
  high += EEPROM.read(4); 
  high += " | ";
  high += EEPROM.read(8);
  lcd.print(high);
}


//Defining boolean variables that are manipulated to move to the different screens
bool menu1 = false;
bool story = true;
bool clearstory = true;
bool clearmenu = false;
bool practice = true;
bool clearpractice = true;
bool clearmessage = true;
bool lose = true;
bool win = true;
bool selectmode = true;
bool dispmessage = true;
bool highsc = true;


void loop() {
  uint8_t input, change1;
  input = lcd.readButtons();

  //Startup menu 
  if (!menu1) {
    menu(); //Calls the menu function
    //Moves to the according pages depending on if UP or DOWN is pressed
    if (input & BUTTON_UP) {
      lcd.clear();
      practice = false;
      menu1 = true;
    }
    if (input & BUTTON_DOWN) {
      lcd.clear();
      story = false;
      menu1 = true;
    }
  }


  //Screens to run Practice mode
  if (!practice) {
    practiceScreen();
    lcd.setBacklight(TEAL);
    if (lcd.readButtons()) {
      practice = true;
      clearpractice = false;
    }
  }
  if (!clearpractice) {
    lcd.clear();
    clearpractice = true;
    setinput = false;
  }
  if (!setinput) {
    select();
    lcd.setBacklight(TEAL);
  }
  if (!clearinput) {
    lcd.clear();
    clearinput = true;
    dispmessage = false;
  }
  if (!dispmessage) {
    lcd.setBacklight(YELLOW);
    sequence(lenN, sizM); //Uses the user input of M and N as entries for the sequence function
    delay(1000);
    dispmessage = true;
    clearpractice = false;
    setinput = true;
    clearinput = true;
  }


  //Screens for Story mode
  if (!story) {
    storyMode();//Activates the welcome screen for story mode
    lcd.setBacklight(BLUE);
    while (Serial.available() > 0) {  //Only continues once the player had written to the serial monitor
      uname = Serial.readString();
      story = true;
      clearstory = false;
    }
  }
  if (!clearstory) {
    lcd.clear();
    clearstory = true;
    selectmode = false;
  }
  if (!selectmode) {
    mode();
    input = lcd.readButtons();
    if (input & BUTTON_UP) {
      //Sets the mode to beginner if UP is pressed
      lcd.clear();
      delays = 1500; //Sets the delay time for beginner difficulty 
      dispstorybeg = false;
      selectmode = true;
    }
    if (input & BUTTON_DOWN) {
      //Sets the mode to expert if DOWN is pressed
      lcd.clear();
      delays = 500; //Sets the delay time for Expert difficulty
      dispstoryexp = false;
      selectmode = true;
    }
    if (input & BUTTON_LEFT) {
      //Goes back to the main screen to select Practice or Story mode if Left is pressed
      lcd.clear();
      selectmode = true;
      menu1 = false;
    }
    if (input & BUTTON_RIGHT) {
      //Goes to the high score screen
      lcd.clear();
      selectmode = true;
      highsc = false;
    }
  }
  //High Score Screen
  if(!highsc) {
    highscoredisp();
    input = lcd.readButtons();
    //To go back to select the modes of difficulty
    if (input && BUTTON_LEFT) {
      lcd.clear();
      highsc = true;
      selectmode = false;
    }
  }
  //Beginner Story mode
  if (!dispstorybeg) {
    lcd.setBacklight(YELLOW);
    //Randomly assigns the size of M and length N
    lenN = random(1, 11);
    sizM = random(1, 4);
    storysequence(lenN,sizM);
    level++; //Moves to the next level with a correct sequence input
    delays = delays-100; //Delay time reduces slowly after each sequence is entered correctly
    delay(2000);
    lcd.clear();
    if(!dispstorybeg) {
      lcd.setCursor(0,0);
      stage = level;  //Stores the level to use to write to EEPROM if a new highscore is set
      lcd.print("Level " + String(level));
      delay(2000); 
      lcd.clear();
      delay(2000); 
    }
    //Finishes the game if the delay goes below 0.5 seconds, and displays the winning message  
    if (delays < 500) {
      lcd.setBacklight(GREEN);
      lcd.print("You Won!");
      lcd.setBacklight(GREEN);
      lcd.print("You Won!");
      delay(2000);
      highscore(stage, uname);
      lcd.clear();
      highscoredisp();
      delay(2000);
      lcd.clear();
      level = 1; //Resets the level
      stage = 1;
      dispstorybeg = true;
    }
  }
  //Expert Story mode
  if (!dispstoryexp) {
    lcd.setBacklight(YELLOW);
    lenN = random(5, 11); //Randomly sets the length N from size 5 and longer
    sizM = 4; //Avails all the 4 buttons to choose from when generating command
    storysequence(lenN, sizM);
    level++; //Increments the level after a full correct sequence input
    delays = delays - 50; //Delay decrement by 50 each time
    lcd.clear();
    if(!dispstoryexp) {
      lcd.setCursor(0,0);
      lcd.setBacklight(TEAL);
      stage = level;  //Sotres the level to write to the high score in EEPROM
      lcd.print("Level " + String(level));
      delay(2000);
      lcd.clear();
      delay(2000); 
      
    }
    if (delays < 200) {  //Terminates the game if the delay goes below 200, displaying the winning message
      lcd.setBacklight(GREEN);
      lcd.print("You Won!");
      delay(2000);
      highscore(stage, uname); //Calls  highscore function to write the high score to the EEPROM
      highscoredisp();  //Displays the top 3 hogh scores          
      delay(2000);
      lcd.clear();
      level = 1;
      dispstoryexp = true;
    }
  }
  if (!clearstorymessage) {
    lcd.clear();
    highscore(stage, uname); //Calls  highscore function to write the high score to the EEPROM
    highscoredisp();  //Displays the top 3 hogh scores
    delay(2000);
    lcd.clear();
    level = 1;  //Resets the level after game ends
    stage = 1;
    selectmode = false; //Goes back to choosing the level of difficulty
    clearstorymessage = true;
  }
  
  
}
