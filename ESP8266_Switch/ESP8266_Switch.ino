#include <ESP8266WiFi.h>
#include <MicroGear.h>

// ----- แก้ค่า config 7 ค่าข้างล่างนี้ --------------------------------------------------------
const char* ssid     = "NSTDA-Project-7013";      // ชื่อ ssid
const char* password = "1q2w3e4r";  // รหัสผ่าน wifi

#define APPID   "armydev"
#define KEY     "5B1qJc8XO4cDVDI"
#define SECRET  "tagjjnUiuLdWRkXt0cAHzPTKZ"

#define ALIAS   "esp8266"           // แทนที่ด้วยหมายเลขของท่าน เช่น "A01"
#define NEIGHBOR "dashboard"             // ชื่ออุปกรณ์ของเพื่อน เช่น "A02"
// --------------------------------------------------------------------------------------

#define LED1     D1                // pin ที่ต่อกับไฟ LED บนบอร์ด NodeMCU
#define LED2     D3 

String m;                 // ตัวแปรเก็บข้อความชนิด string
const char delimiter = '#';   // ใช้สำหรับเช็ครูปแบบข้อความที่คั่นด้วยเครื่องหมาย sharp

String STATE ="0,0";
String STATELED1 = "0";
String STATELED2 = "0";

WiFiClient client;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
    if(*(char *)msg == '?'){
      STATE = STATELED1;
      STATE += ",";
      STATE += STATELED2;
      microgear.chat(NEIGHBOR,STATE);
    }
    else{
      m = (char*)msg;
      if (m.length() > 0){
        if(m.indexOf(delimiter)!=-1){
          String LED = m.substring(0, m.indexOf(delimiter));
          String Logic = m.substring(m.indexOf(delimiter)+1);
          if(LED == "LED1"){
            if(Logic=="0"){
              digitalWrite(LED1, LOW);
            }
            if(Logic=="1"){
              digitalWrite(LED1, HIGH);
            }
            STATELED1 = Logic;
          }
          if(LED == "LED2"){
            if(Logic=="0"){
              digitalWrite(LED2, LOW);
            }
            if(Logic=="1"){
              digitalWrite(LED2, HIGH);
            }
            STATELED2 = Logic;
          }
          STATE = STATELED1;
          STATE += ",";
          STATE += STATELED2;
          microgear.chat(NEIGHBOR,STATE);
        }
      }
    }

    
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

void setup() {
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    // กำหนดชนิดของ PIN (ขาI/O) เช่น INPUT, OUTPUT เป็นต้น
    pinMode(LED1, OUTPUT);          // LED pin mode กำหนดค่า
    pinMode(LED2, OUTPUT);          // LED pin mode กำหนดค่า

    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
    }
    
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);   // กำหนดค่าตันแปรเริ่มต้นให้กับ microgear
    microgear.connect(APPID);           // ฟังก์ชั่นสำหรับเชื่อมต่อ NETPIE
}

void loop() {
    if (microgear.connected()) {
        microgear.loop();
    }
    else {
        Serial.println("connection lost, reconnect...");
        microgear.connect(APPID); 
    }
}

