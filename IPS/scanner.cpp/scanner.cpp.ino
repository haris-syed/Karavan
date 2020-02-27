#include <math.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <string.h>

const int pin27 = 27;
const int pin26 = 26;
const int pin25 = 25;
const int pin33 = 33;
const int pin32 = 32;

struct Coordinates {
  double x;
  double y;
};

void encode(int n){
    int binaryNum[5];
    for (int i=0;i<5;i++){
      binaryNum[i]=0; 
    }
    
    int i = 0;
    while (n > 0) {  
        binaryNum[i] = n % 2; 
        n = n / 2; 
        i++;
    }
    (binaryNum[0] == 0)? digitalWrite(pin32,LOW): digitalWrite(pin32,HIGH);
    (binaryNum[1] == 0)? digitalWrite(pin33,LOW): digitalWrite(pin33,HIGH);
    (binaryNum[2] == 0)? digitalWrite(pin25,LOW): digitalWrite(pin25,HIGH);
    (binaryNum[3] == 0)? digitalWrite(pin26,LOW): digitalWrite(pin26,HIGH);
    (binaryNum[4] == 0)? digitalWrite(pin27,LOW): digitalWrite(pin27,HIGH);
    Serial.println("\nbinary\n ");
    for (int j = 4; j >= 0; j--) 
         Serial.print(binaryNum[j]);
    Serial.println(" ");
}

Coordinates calculateCoordinates(double x1, double y1, double x2, double y2, double x3, double y3, double r1, double r2, double r3) {
  double A = -2*x1 + 2*x2;
  double B = -2*y1+2*y2;
  double C = r1*r1-r2*r2-x1*x1+x2*x2-y1*y1+y2*y2;
  double D = -2*x2+2*x3;
  double E = -2*y2+2*y3;
  double F = r2*r2-r3*r3-x2*x2+x3*x3-y2*y2+y3*y3;
  double x = (C*E-F*B)/(E*A-B*D);
  double y = (C*D-A*F)/(B*D-A*E);
  
  Coordinates loc;
  loc.x = x;
  loc.y = y;
  return loc;
  
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


double _x1 = 2;
double _y1 = 3;
double _x2 = 3;
double _y2 = 0;
double _x3 = 0;
double _y3 = 0;

int LED_BUILTIN = 2;
int scanTime = 1; //In seconds
BLEScan* pBLEScan;
char const * b1 = "24:0a:c4:30:ef:e6";
char const * b2 = "24:0a:c4:30:f5:be";
char const * b3 = "24:0a:c4:30:e4:5a";

int* ar;

double calculateDistance(int RSSI){
  double distance = std::pow(10,(-76-RSSI)/(10*2.519));
  return distance;
}


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
  pinMode(pin33, OUTPUT);
  pinMode(pin32, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  double r1 = 0;
  double r2 = 0;
  double r3 = 0;
  for (int i=0; i<foundDevices.getCount(); i++){
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    if(strcmp(device.getAddress().toString().c_str(),b1)==0 || strcmp(device.getAddress().toString().c_str(),b2)==0 || strcmp(device.getAddress().toString().c_str(),b3)==0){ 
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
      
      double distance = calculateDistance(movingAverageFilter(ar,device.getTimesFound()));
      Serial.print("\nDistance:");
      Serial.println(distance);
      Serial.println("\n\n --------------------------------------------");
      if(strcmp(device.getAddress().toString().c_str(),b1)==0){
        r1=distance;
      }
      else if (strcmp(device.getAddress().toString().c_str(),b2)==0){
        r2=distance;
      }
      else if (strcmp(device.getAddress().toString().c_str(),b3)==0){
        r3=distance;
      }
    }
  }
  
  Coordinates result = calculateCoordinates(_x1, _y1, _x2, _y2, _x3, _y3, r1, r2, r3);
  Serial.print("\nResult");
  Serial.print(result.x);
  Serial.print("\n");
  Serial.print(result.y);
  Serial.print("\n");
  int cell = floor(result.x) + 4*floor(result.y);
  encode(cell);

  
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  digitalWrite(LED_BUILTIN, LOW);
}
