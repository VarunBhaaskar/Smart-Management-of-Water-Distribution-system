#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial gprsSerial(9,10);

float temp=0;
const int trigPin = 7; 
const int echoPin = 6;
int distance();
  
#define tank_height 1000      //constant in cm   
#define tank_volume 20000      //constant 
long avg_consumption=??;  //replace ?? according to seasons //it shows the average water consumption of a family per day
long family=??;           //replace ?? with no of families in the area
double timehigh=0;
double timelow=0;
double fillrate=0;
double dischargerate=0;
double expected_consumption_rate;
double expected_consumption;
void setup()
{
    pinMode(trigPin, OUTPUT); //ultrasonic sensor pins 
    pinMode(echoPin, INPUT);
     avg_consumption=avg_consumption/24; //per hour avg consumption
    Serial.begin(9600);
    gprsSerial.begin(9600);

    // attach or detach from GPRS service 
    gprsSerial.println("AT+CGATT?");  // Attach to GPRS network or not?
    delay(100);
    toSerial();

    // bearer settings
    gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");   //Set the connection type to GPRS
    delay(2000);
    toSerial();

    // bearer settings
    gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"Internet\"");  //Set the APN to airtel sim
    delay(2000);
    toSerial();

    // bearer settings
    gprsSerial.println("AT+SAPBR=1,1");     //Enable the GPRS
    delay(2000);
    toSerial();
}
void loop()
{
    
 // put your main code here, to run repeatedly:
  delay(1000);
 int a=distance();
 if (a <= 0.1*tank_height && a>= 0.5*tank_height){
  Serial.println("motor off");
   timehigh= millis();
 }
 else if(a >= 0.8*tank_height && a<= 0.75*tank_height){
  Serial.println("motor on");
   timelow= millis();
 }
  if(timehigh-timelow>0){
     fillrate=timehigh-timelow;  
  }
  else{
    dischargerate=timelow-timehigh;
    dischargerate= dischargerate/(1000*60*60);
  }
  expected_consumption_rate = avg_consumption*family/24;
  expected_consumption= expected_consumption_rate*dischargerate;

 

  if  (0.7*tank_volume - expected_consumption > 100){
    SendMessage();
  }
 else{ 
  //everything is fine by the hour
 }
  
    
}


void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}

 void SendMessage()
{
  gprsSerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  gprsSerial.println("AT+CMGS=\"+91xxxxxxxxxx\"\r"); // Replace x with mobile number
  delay(1000);
  gprsSerial.println("Something is not right");// The SMS text you want to send
  delay(100);
   gprsSerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

int distance(){          
  // to measure distance in cm
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH);
  int distance= duration*0.034/2;
  return(distance);
}

