#include <math.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <string.h>


const int pin27 = 27;
const int pin26 = 26;
const int pin25 = 25;


void encode(int n){
    int binaryNum[3];
    for (int i=0;i<3;i++){
      binaryNum[i]=0; 
    }
    
    int i = 0;
    while (n > 0) {  
        binaryNum[i] = n % 2; 
        n = n / 2; 
        i++;
    }
    (binaryNum[0] == 0)? digitalWrite(pin25,LOW): digitalWrite(pin25,HIGH);
    (binaryNum[1] == 0)? digitalWrite(pin26,LOW): digitalWrite(pin26,HIGH);
    (binaryNum[2] == 0)? digitalWrite(pin27,LOW): digitalWrite(pin27,HIGH);
    Serial.println("\nbinary\n ");
    for (int j = 2; j >= 0; j--) 
         Serial.print(binaryNum[j]);
    Serial.println(" ");
}

int movingAverageFilter(int * ar, int n){
  int j=0;
  if(n%2==0) {
    j=n-1;
  }
  else {
    j=n;
  }
  
  for (int i=1;i<j-1;i++){
    ar[i]=(ar[i-1]+ar[i+1])/2;
  }
  int sum = 0;
  for (int i=0;i<j;i++){
    sum+=ar[i];
  }
  return sum/j;
}

int LED_BUILTIN = 2;
int scanTime = 1; //In seconds
BLEScan* pBLEScan;
char const * b1 = "24:0a:c4:30:ef:e6";
char const * b2 = "24:0a:c4:30:f5:be";
char const * b3 = "24:0a:c4:30:d6:4a";
char const * b4 = "24:0a:c4:30:d5:42";

int* ar;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");
  pinMode (LED_BUILTIN, OUTPUT);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), false);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  pinMode(pin27, OUTPUT);
  pinMode(pin26, OUTPUT);
  pinMode(pin25, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  int maxRSSI = -1000;
  int position = 0;
  for (int i=0; i<foundDevices.getCount(); i++){
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    if(strcmp(device.getAddress().toString().c_str(),b1)==0 || strcmp(device.getAddress().toString().c_str(),b2)==0 || strcmp(device.getAddress().toString().c_str(),b3)==0 || strcmp(device.getAddress().toString().c_str(),b4)==0){ 
      Serial.println(device.getAddress().toString().c_str());
      Serial.print("RSSI: ");
      Serial.print(device.getRSSI());
      Serial.print("\nTX Power:");
      Serial.println(device.getTXPower());
      Serial.print("\nTimesfound:");
      Serial.println(device.getTimesFound()); //array size
      ar = device.getValues();
      Serial.print("\nMAR:");
      Serial.println(movingAverageFilter(ar,device.getTimesFound()));
      int averageRSSI = movingAverageFilter(ar,device.getTimesFound()); 
      if(averageRSSI > maxRSSI){
        maxRSSI = averageRSSI;
        if(strcmp(device.getAddress().toString().c_str(),b1)==0){
          position = 1;
        }
        if(strcmp(device.getAddress().toString().c_str(),b2)==0){
          position = 2;
        }
        if(strcmp(device.getAddress().toString().c_str(),b3)==0){
          position = 3;
        }
        if(strcmp(device.getAddress().toString().c_str(),b4)==0){
          position = 4;
        }
      }
      
      Serial.println("\n\n --------------------------------------------");
    }
  }
  
  
  encode(position);
  Serial.print("\n\n I am at position: ");
  Serial.println(position);

  
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  digitalWrite(LED_BUILTIN, LOW);
}
