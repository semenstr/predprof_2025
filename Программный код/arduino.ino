#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Stepper.h>

LiquidCrystal_I2C lcd(0x27,20,4);

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10 
const byte ROWS = 4; // 4 строки
const byte COLS = 4; // 4 столбца
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
}; 
byte rowPins[ROWS] = {39, 37, 35, 33}; 
byte colPins[COLS] = {31, 29, 27, 25}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const int stepsPerRevolution =100;
Stepper mystepper(stepsPerRevolution, 4, 5, 6, 7);

MFRC522 mfrc522(SS_PIN, RST_PIN);

String password = "7123";
String sigmapass = "";

#include <Servo.h>
Servo servoDver;
void setup() {
  Serial.begin(9600);
  servoDver.attach(11);
  pinMode(8,INPUT);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(A0,INPUT);
  mystepper.setSpeed(60);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Attach your card");
  lcd.setCursor(0, 1);
  lcd.print("or enter PIN.");
  servoDver.write(0); // сообщаем микро серво угол поворота
}

void getdostup(){
  delay(100);
    if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
      if (data == "Access allowed") {
        String data2 = Serial.readStringUntil('\n');
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Please, wait...");
        delay(500);
        lcd.clear();
        lcd.print("Hello, " + data2);
        delay(500);
        for (int i = 0; i < sizeof(data2) + 3; i++) {
          delay(300);
          lcd.scrollDisplayLeft();
        }
        for (int angle = 0; angle <= 11; angle++) {
          mystepper.step(stepsPerRevolution);
        }
        
        delay(5000);
        for (int angle = 0; angle <= 11;) {
          unsigned int dist = analogRead(0);
          if (dist <= 450){
            mystepper.step(-stepsPerRevolution);
            angle++;
          }

        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Attach your card");
        lcd.setCursor(0, 1);
        lcd.print("or enter PIN.");
      }
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Please, wait...");
        delay(500);
        lcd.clear();
        lcd.print("Access denied!" );
        delay(5000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Attach your card");
        lcd.setCursor(0, 1);
        lcd.print("or enter PIN.");
      }
    }
  }

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void loop() {
  char key = keypad.getKey();
  if(key){
    sigmapass += key;
    if (key == 'C'){
      sigmapass = "";
    }
    if (sigmapass == password){
      lcd.clear();
      lcd.print("Access trued!");
      for (int angle = 0; angle <= 120; angle++) {
        servoDver.write(angle); // сообщаем микро серво угол поворота
        delay(20); // ждем 20 мс, пока серво не сделает поворот
      }
      delay(5000);
      for (int angle = 120; angle >= 0; angle--) {
        servoDver.write(angle); // сообщаем микро серво угол поворота
        delay(2); // ждем 20 мс, пока серво не сделает поворот
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Attach your card");
      lcd.setCursor(0, 1);
      lcd.print("or enter PIN.");
      sigmapass = "";
      return;
    }
    if (sigmapass.length() >= 4){
      lcd.clear();
      lcd.print("Access achjhgdj!");
      delay(5000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Attach your card");
      lcd.setCursor(0, 1);
      lcd.print("or enter PIN.");
      sigmapass = "";
      return;
    }
  }

  
  // Ожидание прикладывания новой RFID-метки:
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return; // выход, если не приложена новая карта
  }
  // Считываем серийный номер:
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    
    return; // выход, если невозможно считать сер. номер
  }
  // }
  printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  getdostup();
    
}
