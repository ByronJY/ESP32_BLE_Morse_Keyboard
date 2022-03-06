#include <BleKeyboard.h>

String morse_Alpha[] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--.."};
String morse_Num[] = {"-----",".----","..---","...--","....-",".....","-....","--...","---..","----."};

const byte buttonPin = 33; // 按鈕 (電鍵)
const byte buzzPin = 27; // 有源蜂鳴器
const byte BLE_status_LED = LED_BUILTIN; // 藍芽狀態指示燈
const int char_Interval = 500; // 字元間隔時間閾值
const int dit_Dur = 200; // dit最大閾值
const int dah_Dur = 500; // dah最大閾值
int buttonState = 0;     // current state of the button
int lastButtonState = 0; // previous state of the button
int startPressed = 0;    // the moment the button was pressed
int endPressed = 0;      // the moment the button was released
int holdTime = 0;        // how long the button was hold
int idleTime = 0;        // how long the button was idle
String temp = "";
boolean caps_lock = 0; // 0=小寫，1=大寫 (初始狀態為小寫)

BleKeyboard bleKeyboard("Morse_Keyboard","Espressif",100); // 其中「Morse_Keyboard」爲鍵盤名稱，「Espressif」爲製造商

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(buzzPin, OUTPUT);
  pinMode(BLE_status_LED, OUTPUT);
  bleKeyboard.begin();
  delay(1000);
  digitalWrite(BLE_status_LED, HIGH);
  Serial.println("Start!");
  
}

void loop() {
  if(bleKeyboard.isConnected()) {
    digitalWrite(BLE_status_LED, LOW); // 藍芽連接上，狀態燈滅
    buttonState = digitalRead(buttonPin); // read the button input
    //Serial.println(temp);
  
    if (buttonState != lastButtonState) { 
       updateState();     // button state changed. It runs only once.
    }
  
    lastButtonState = buttonState;        // save state for next loop
  
        if (millis() - endPressed >= 10 && millis() - endPressed <= char_Interval) {
            //Serial.print("IDLE ");
            //Serial.print(idleTime);
            //Serial.print("  ");
        } else if (millis() - endPressed > char_Interval){
            //Serial.print("　");
            if (temp == "..--"){         // Google摩斯電碼鍵盤的「 」(space)
                Serial.print(" ");
                bleKeyboard.print(' ');
            }else if (temp == ".-.-"){   // Google摩斯電碼鍵盤的「Enter」
                Serial.print("\n");
                bleKeyboard.write(KEY_RETURN);
            }else if (temp == "----"){   // Google摩斯電碼鍵盤的「Backspace」
                bleKeyboard.write(KEY_BACKSPACE);
            }else if (temp == "....-."){ // Caps Lock (Google摩斯電碼鍵盤的「Shift」)，切換大小寫會有「嗶嗶」兩聲提示音
                caps_lock = !caps_lock;
                digitalWrite(buzzPin, HIGH);
                delay(100);
                digitalWrite(buzzPin, LOW);
                delay(50);
                digitalWrite(buzzPin, HIGH);
                delay(100);
                digitalWrite(buzzPin, LOW);
            }else if (temp.length() < 5){          // 字母
                for (size_t i=0 ; temp != morse_Alpha[i], i < sizeof(morse_Alpha)/sizeof(morse_Alpha[0]) ; i++){
                  if (temp == morse_Alpha[i]){
                    if (caps_lock == 0){
                      Serial.print(char( i+'a' )); // i+'a'，i=0~25，對應ASCII的a~z
                      bleKeyboard.print(char( i+'a'));
                    }else if (caps_lock == 1){
                      Serial.print(char( i+'A' )); // i+'A'，i=0~25，對應ASCII的A~Z
                      bleKeyboard.print(char( i+'A'));
                    }
                  }
                }
            } else if (temp.length() == 5){ // 數字及部分符號
              if (temp == "-...-"){         // =
                Serial.print("=");
                bleKeyboard.print('=');
              }else if (temp == "-..-."){   // /
                Serial.print("/");
                bleKeyboard.print('/');
              }else if (temp == "-.--."){   // (
                Serial.print("(");
                bleKeyboard.print('(');
              }else if (temp == ".-..."){   // &
                Serial.print("&");
                bleKeyboard.print('&');
              }else if (temp == ".-.-."){   // +
                Serial.print("+");
                bleKeyboard.print('+');
              }else{
                for (size_t i=0 ; temp != morse_Num[i], i < sizeof(morse_Num)/sizeof(morse_Num[0]) ; i++){
                  if (temp == morse_Num[i]){
                    Serial.print(i); // i對應morse_Num的index
                    bleKeyboard.print(i);
                  }
                }
              }
            }else{ // 其它符號
              if (temp == ".-.-.-"){
                Serial.print(".");
                bleKeyboard.print('.');
              }else if (temp == "---..."){
                Serial.print(":");
                bleKeyboard.print(':');
              }else if (temp == "--..--"){
                Serial.print(",");
                bleKeyboard.print(',');
              }else if (temp == "-.-.-."){
                Serial.print(";");
                bleKeyboard.print(';');
              }else if (temp == "..--.."){
                Serial.print("?");
                bleKeyboard.print('?');
              }else if (temp == ".----."){
                Serial.print("\'");
                bleKeyboard.print('\'');
              }else if (temp == "-.-.--"){
                Serial.print("!");
                bleKeyboard.print('!');
              }else if (temp == "-....-"){
                Serial.print("-");
                bleKeyboard.print('-');
              }else if (temp == "..--.-"){
                Serial.print("_");
                bleKeyboard.print('_');
              }else if (temp == ".-..-."){
                Serial.print("\"");
                bleKeyboard.print('\"');
              }else if (temp == "-.--.-"){
                Serial.print(")");
                bleKeyboard.print(')');
              }else if (temp == "...-..-"){
                Serial.print("$");
                bleKeyboard.print('$');
              }else if (temp == ".--.-."){
                Serial.print("@");
                bleKeyboard.print('@');
              }
            }
            //Serial.println(temp);
            temp = "";
        }
    delay(50);
  }else{
    digitalWrite(BLE_status_LED, HIGH); // 如果藍芽沒連接上，狀態燈常亮
    delay(5000);
  }
  
}

void updateState() {
  // the button has been just pressed
  if (buttonState == LOW) {
      digitalWrite(buzzPin, HIGH);
      startPressed = millis();
      idleTime = startPressed - endPressed;




  // the button has been just released
  } else {
      digitalWrite(buzzPin, LOW);
      endPressed = millis();
      holdTime = endPressed - startPressed;

      if (holdTime >= 10 && holdTime < dit_Dur) {
          //Serial.print("HELD "); 
          //Serial.println(holdTime);
          //Serial.print("．");
          temp += '.';
          //Serial.println(temp);
      } else if (holdTime >= dit_Dur){
          //Serial.print("—");
          temp += '-';
          //Serial.println(temp);
      }
  }

}
