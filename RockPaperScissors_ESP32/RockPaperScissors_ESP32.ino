#define TIME_CLOCK_PIN 21 
#define TIME_RESET_CLOCK_PIN 19 
 
#define PLAYER1_ROCK_BUTTON_PIN 32 
#define PLAYER1_PAPER_BUTTON_PIN 33 
#define PLAYER1_SCISSORS_BUTTON_PIN 25 
#define PLAYER1_SCORE_CLOCK_PIN 17 
#define PLAYER1_SCORE_RESET_CLOCK_PIN 16 
#define PLAYER1_WIN_LED_PIN 12  
 
#define PLAYER2_ROCK_BUTTON_PIN 26 
#define PLAYER2_PAPER_BUTTON_PIN 14 
#define PLAYER2_SCISSORS_BUTTON_PIN 27 
#define PLAYER2_SCORE_CLOCK_PIN 18 
#define PLAYER2_SCORE_RESET_CLOCK_PIN 5 
#define PLAYER2_WIN_LED_PIN 13  
 
#define START_RESET_BTN_PIN 22  
#define AI_MODE_BTN_PIN 23      
#define AI_MODE_LED_PIN 2       
 
const int WINNING_SCORE = 5; 
 
enum Move { NONE, ROCK, PAPER, SCISSORS }; 
 
Move p1Move = Move::NONE; 
Move p2Move = Move::NONE; 
 
int timeLeft = 10; 
unsigned long lastTickTime = 0; 
 
bool gameRunning = false;  
bool aiMode = false;       
int p1Score = 0;          
int p2Score = 0; 
 
void sendPulse(int pin) { 
  digitalWrite(pin, HIGH); 
  delay(10); 
  digitalWrite(pin, LOW); 
} 
 
void displayTime(int count) { 
  digitalWrite(TIME_RESET_CLOCK_PIN, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TIME_RESET_CLOCK_PIN, LOW); 
  delayMicroseconds(10); 
 
  for (int i = 0; i < count; i++) { 
    digitalWrite(TIME_CLOCK_PIN, HIGH); 
    delayMicroseconds(10); 
    digitalWrite(TIME_CLOCK_PIN, LOW); 
    delayMicroseconds(10); 
  } 
} 
 
void resetRound() { 
  p1Move = Move::NONE; 
  p2Move = Move::NONE; 
  timeLeft = 10; 
  displayTime(0); 
  lastTickTime = millis(); 
  Serial.println("New round! Start..."); 
} 
 
void resetGame() { 
  digitalWrite(PLAYER1_SCORE_RESET_CLOCK_PIN, HIGH); 
  digitalWrite(PLAYER2_SCORE_RESET_CLOCK_PIN, HIGH); 
  delay(10); 
  digitalWrite(PLAYER1_SCORE_RESET_CLOCK_PIN, LOW); 
  digitalWrite(PLAYER2_SCORE_RESET_CLOCK_PIN, LOW); 
   
  p1Score = 0; 
  p2Score = 0; 
   
  resetRound(); 
} 
 
void celebrateRoundWinner(int ledPin) { 
  for(int i = 0; i < 10; i++) { 
    digitalWrite(ledPin, HIGH); 
    delay(100); 
    digitalWrite(ledPin, LOW); 
    delay(100); 
  } 
} 
 
void celebrateMatchWinner(int ledPin) { 
  Serial.println("MATCH OVER!"); 
  for(int i = 0; i < 25; i++) {  
    digitalWrite(ledPin, HIGH); 
    delay(100); 
    digitalWrite(ledPin, LOW); 
    delay(100); 
  } 
} 
 
void checkButtons() { 
  if (digitalRead(PLAYER1_ROCK_BUTTON_PIN) == LOW) p1Move = Move::ROCK; 
  if (digitalRead(PLAYER1_PAPER_BUTTON_PIN) == LOW) p1Move = Move::PAPER; 
  if (digitalRead(PLAYER1_SCISSORS_BUTTON_PIN) == LOW) p1Move = Move::SCISSORS; 
 
  if (!aiMode) { 
    if (digitalRead(PLAYER2_ROCK_BUTTON_PIN) == LOW) p2Move = Move::ROCK; 
    if (digitalRead(PLAYER2_PAPER_BUTTON_PIN) == LOW) p2Move = Move::PAPER; 
    if (digitalRead(PLAYER2_SCISSORS_BUTTON_PIN) == LOW) p2Move = Move::SCISSORS; 
  } 
} 
 
void determineWinner() { 
  if (aiMode) { 
    p2Move = static_cast<Move>(random(1, 4)); 
    Serial.print("Computer: "); 
    Serial.println(p2Move); 
  } 
 
  if (p1Move == Move::NONE && p2Move == Move::NONE) return;  
 
  if (p1Move == p2Move) { 
    Serial.println("Draw!"); 
    digitalWrite(PLAYER1_WIN_LED_PIN, HIGH); 
    digitalWrite(PLAYER2_WIN_LED_PIN, HIGH); 
    delay(500); 
    digitalWrite(PLAYER1_WIN_LED_PIN, LOW); 
    digitalWrite(PLAYER2_WIN_LED_PIN, LOW); 
  }  
  else if ((p1Move == Move::ROCK && p2Move == Move::SCISSORS) || 
           (p1Move == Move::PAPER && p2Move == Move::ROCK) || 
           (p1Move == Move::SCISSORS && p2Move == Move::PAPER)) { 
      
    p1Score++; 
    Serial.print("P1 Point! Score: "); 
    Serial.print(p1Score); 
    Serial.print(":"); 
    Serial.println(p2Score); 
    sendPulse(PLAYER1_SCORE_CLOCK_PIN);
      
    if (p1Score >= WINNING_SCORE) { 
        celebrateMatchWinner(PLAYER1_WIN_LED_PIN); 
        gameRunning = false;
        resetGame();
        Serial.println("Game ended. Press START."); 
    } else { 
        celebrateRoundWinner(PLAYER1_WIN_LED_PIN); 
    } 
      
  }  
  else { 
    p2Score++; 
    Serial.print("P2 Point! Score: "); 
    Serial.print(p1Score); 
    Serial.print(":"); 
    Serial.println(p2Score); 
    sendPulse(PLAYER2_SCORE_CLOCK_PIN); 
 
    if (p2Score >= WINNING_SCORE) { 
        celebrateMatchWinner(PLAYER2_WIN_LED_PIN); 
        gameRunning = false;
        resetGame();
        Serial.println("Game ended. Press START."); 
    } else { 
        celebrateRoundWinner(PLAYER2_WIN_LED_PIN); 
    } 
  } 
} 
 
void handleSystemButtons() { 
  if (digitalRead(START_RESET_BTN_PIN) == LOW) { 
    delay(50);  
    if (digitalRead(START_RESET_BTN_PIN) == LOW) { 
      if (gameRunning) { 
        gameRunning = false; 
        resetGame(); 
        displayTime(0); 
        Serial.println("Manual reset."); 
      } else { 
        gameRunning = true; 
        resetGame();
        Serial.println("Game START!"); 
      } 
      while(digitalRead(START_RESET_BTN_PIN) == LOW);  
    } 
  } 
 
  if (digitalRead(AI_MODE_BTN_PIN) == LOW) { 
    delay(50); 
    if (digitalRead(AI_MODE_BTN_PIN) == LOW) { 
      aiMode = !aiMode; 
      digitalWrite(AI_MODE_LED_PIN, aiMode ? HIGH : LOW); 
      Serial.print("AI MODE: "); 
      Serial.println(aiMode ? "ON" : "OFF"); 
      while(digitalRead(AI_MODE_BTN_PIN) == LOW); 
    } 
  } 
} 
 
void setup() { 
  Serial.begin(9600); 
  randomSeed(analogRead(0)); 
 
  pinMode(TIME_CLOCK_PIN, OUTPUT); 
  pinMode(TIME_RESET_CLOCK_PIN, OUTPUT); 
 
  pinMode(PLAYER1_ROCK_BUTTON_PIN, INPUT_PULLUP); 
  pinMode(PLAYER1_PAPER_BUTTON_PIN, INPUT_PULLUP); 
  pinMode(PLAYER1_SCISSORS_BUTTON_PIN, INPUT_PULLUP); 
   
  pinMode(PLAYER2_ROCK_BUTTON_PIN, INPUT_PULLUP); 
  pinMode(PLAYER2_PAPER_BUTTON_PIN, INPUT_PULLUP); 
  pinMode(PLAYER2_SCISSORS_BUTTON_PIN, INPUT_PULLUP); 
 
  pinMode(PLAYER1_SCORE_CLOCK_PIN, OUTPUT); 
  pinMode(PLAYER1_SCORE_RESET_CLOCK_PIN, OUTPUT); 
  pinMode(PLAYER1_WIN_LED_PIN, OUTPUT); 
 
  pinMode(PLAYER2_SCORE_CLOCK_PIN, OUTPUT); 
  pinMode(PLAYER2_SCORE_RESET_CLOCK_PIN, OUTPUT); 
  pinMode(PLAYER2_WIN_LED_PIN, OUTPUT); 
 
  pinMode(START_RESET_BTN_PIN, INPUT_PULLUP); 
  pinMode(AI_MODE_BTN_PIN, INPUT_PULLUP); 
  pinMode(AI_MODE_LED_PIN, OUTPUT); 
   
  digitalWrite(AI_MODE_LED_PIN, LOW); 
  resetGame();
} 
 
void loop() { 
  handleSystemButtons();  
 
  if (gameRunning) { 
    unsigned long currentTime = millis(); 
 
    if (timeLeft >= 0) { 
      checkButtons();  
 
      if (currentTime - lastTickTime >= 1000) { 
        displayTime(timeLeft);  
          
        if (timeLeft == 0) { 
          determineWinner(); 
            
          if (gameRunning) { 
             resetRound();
          } 
        } else { 
          timeLeft--; 
        } 
        lastTickTime = currentTime; 
      } 
    } 
  } 
}