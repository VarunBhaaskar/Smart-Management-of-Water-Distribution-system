const int trigPin = 7;
  const int echoPin = 6;
  int distance();
  
  #define tank_height 2000  //in cm
  #define tank_volume 20000   // in litres
  long avg_consumption=0;
  long family=0;
   double timehigh=0;
   double timelow=0;
   double fillrate=0;
   double dischargerate=0;
   double expected_consumption_rate;
   double expected_consumption;
    double volume_in_tank;
    int problem=0;


    String a = "GET /update?api_key=aaaaa&field1=";   //replace aaaaa with your api key
    String b = "GET /update?api_key=aaaaa&field2=";   //replace aaaaa with your api key
    String f = " HTTP/1.1 \nHOST: api.thingspeak.com \r\n\r\n";              // web link

    
void setup() {
  // put your setup code here, to run once:
 pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  avg_consumption=avg_consumption/24;
}

void loop() {
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

  if ( (0.7*tank_volume)- expected_consumption > 100){
    //upload to field 2 of cloud as 1
    problem=1;
  }
 else{ 
  //upload to field 2 of cloud as 0
  problem=0;
 }
}
int distance(){
  //to measure distance in cm using ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH);
  int distance= duration*0.034/2;
  
  return(distance);
  
  volume_in_tank = tank_volume/tank_height*distance;
}
void tocloud(double volume_in_tank,int problem){
  //to plot values to thingspeak.com
  //configure the esp8266 before uploading the code
  char volume_buff[16];
  String volumeX = dtostrf(volume_in_tank, 4, 1, volume_buff);
  char problem_buff[16];
  String problemX = dtostrf(volume_in_tank, 4, 1, problem_buff);

  //char *   dtostrf (double __val, signed char __width, unsigned char __prec, char *__s)

  String postStr1 = a + volumeX + f;
  String postStr2 = b + problemX + f;

  /*********   Uploading to ThinkSpeak Cloud    ***********/
  /*********  Sending AT Commands to ESP8266    ***********/
  
    Serial.println("AT");
    delay(2000);
     
    Serial.print("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
    delay(4000);
          
    String ciplength = "AT+CIPSEND=" + String((postStr1+postStr2).length()) + "\r\n";
    Serial.print(ciplength);
    delay(4000);

    Serial.print(postStr1);
    delay(4000);
    Serial.print(postStr2);
    delay(4000);

    Serial.print("AT+RST\r\n");
    delay(16000);  
} 


