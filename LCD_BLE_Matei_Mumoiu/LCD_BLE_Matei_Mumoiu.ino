

#include <LiquidCrystal_I2C.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>


LiquidCrystal_I2C lcd(0x27,16,2);

#define bleServerName "radumatei"

bool deviceConnected = false;

#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"
#define CHARACTERISTIC_UUID "ca73b3ba-39f6-4ab3-91ae-186dc9577d99"

BLECharacteristic characteristic(
  CHARACTERISTIC_UUID,
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
);

BLEDescriptor *characteristicDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2902));


class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};


class CharacteristicsCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      std::string data = characteristic->getValue();
      Serial.println(data.c_str());

String input=data.c_str();

DynamicJsonDocument request(15000);

DeserializationError error = deserializeJson(request, input);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}
else{
  
  String action = request["action"].as<String>();
  String teamId= request["teamId"].as<String>();
  Serial.println(action);


      if(action=="getLCDs")
      {
        StaticJsonDocument<128> doc;
    

doc["type"] = "16x2";
doc["interface"] = "Parallel 4-bit";
doc["resolution"] = "16x2";
doc["id"] = 1;
doc["teamId"] = "A30";
  String output1;
serializeJson(doc,output1);

            characteristic->setValue(output1.c_str());
            characteristic->notify();     
          
      }

      if(action=="setText"){

        String text = request["text"].as<String>();
      String output2;

        StaticJsonDocument<96> doc;

doc["id"] = 1;
doc["text"] = "string";
doc["teamId"] = "A30";


serializeJson(doc, output2);
 characteristic->setValue(output2.c_str());
            characteristic->notify();
             lcd.setCursor(0, 0);
             lcd.print(text);
              
      } 



      if(action=="scroll"){

        String direction = request["direction"].as<String>();

        if(direction=="Left")
        {        String output3;
          StaticJsonDocument<96> doc;

doc["id"] = 1;
doc["scrolling"] = "Left";
doc["teamId"] = "A30";

serializeJson(doc, output3);
characteristic->setValue(output3.c_str());
            characteristic->notify();
            for (int position = 0; position < 16; position++) {
    lcd.scrollDisplayLeft();
    delay(100); 
  }
        }


        if(direction=="Right")
        {        String output4;
          StaticJsonDocument<96> doc;

doc["id"] = 1;
doc["scrolling"] = "Right";
doc["teamId"] = "A30";

serializeJson(doc, output4);
characteristic->setValue(output4.c_str());
            characteristic->notify();
            for (int position = 0; position < 16; position++) {
    lcd.scrollDisplayRight();
    delay(100); 
  }

        }

        if(direction=="Off")
        {        String output5;
          StaticJsonDocument<96> doc;

doc["id"] = 1;
doc["scrolling"] = "Off";
doc["teamId"] = "A30";

serializeJson(doc, output5);
characteristic->setValue(output5.c_str());
            characteristic->notify();
            lcd.setCursor(5, 0);
            lcd.clear();
            }
      
               
      
          } 



        
      }




    }    
};


void setup() {
  Serial.begin(115200);


lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  BLEDevice::init(bleServerName);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *bleService = pServer->createService(SERVICE_UUID);

  bleService->addCharacteristic(&characteristic);  
  characteristic.addDescriptor(characteristicDescriptor);
  characteristic.setCallbacks(new CharacteristicsCallbacks());
  
  bleService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

}

void loop() {
  
}