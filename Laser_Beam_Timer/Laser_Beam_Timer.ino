//Laser Beam Timer
//Using Laser aimed at LED as sensor
//Can be used in 3 modes, select with select button
//Lap Mode: time between beam breaks
//Break Time: time while beam is broken (eg measure speed of falling objects)
//Make Time: as above, but while beam is unbroken
//Calibrate by pressing right
//Can view current analog value (beam aiming)
//Can edit threshold value (up/down) to fine tune
//Left button to exit calibration

#include <LiquidCrystal.h>
LiquidCrystal lcd(8,9,4,5,6,7);

char m[3][11]={       //display mode on LCD
  "Lap Timer ",
  "Break Time",
  "Make Time ",
};

int mode=0;         //as per above
int phase=0;        //0=ready,1=running
int oldstate=0;     //to detect changes
int threshold=145;  //starting default threshold

unsigned long t;    //for keeping track of running time.

void setup() {
  lcd.begin(16, 2); //initialise LCD
  doscreen();       //draw default screen
}

void loop() {
  int k,a;
  int state;
  unsigned long tt; 
  k=lcdkey();
  if(k==1){         //select button to change modes
    mode=(mode+1)%3;
    state=0;
    doscreen();
  }
  if(k==5){          //right=calibrate
    docalibrate();
  }
  a=analogRead(1);  //test input
  tt=millis();
  if(a<threshold){
    state=0;
  }else{
    state=1;
  }
  switch(mode){     //test for different conditions
    case 0:
      if((!state)&&(oldstate)&&(phase==0)){phase=1;t=tt;}                     //start on beam break
      if((!state)&&(oldstate)&&(phase==1)){phase=1;showtime(tt-t);t=tt;}      //lap done on beam break
      break;
    case 1:
      if((!state)&&(oldstate)&&(phase==0)){phase=1;t=tt;}                     //start on beam break
      if((state)&&(!oldstate)&&(phase==1)){phase=0;showtime(tt-t);t=tt;}      //lap done on beam make
      break;
    case 2:
      if((state)&&(!oldstate)&&(phase==0)){phase=1;t=tt;}                     //start on beam make
      if((!state)&&(oldstate)&&(phase==1)){phase=0;showtime(tt-t);t=tt;}      //lap done on beam break
      break;
  }
  oldstate=state;   //record old state for next cycle
}

void docalibrate(){ //jump to calibration mode
  int a;
  while(lcdkey()>0){}   //wait until keys released
  delay(200);           //debounce
  lcd.clear();          //set up calibration screen
  lcd.setCursor(0,0);
  lcd.print("Analog  :");
  lcd.setCursor(0,1);
  lcd.print("Setpoint:");
  while(lcdkey()!=2){     //left to exit
    if(lcdkey()==3){threshold--;if(threshold<0){threshold=0;}}        //down
    if(lcdkey()==4){threshold++;if(threshold>1023){threshold=1023;}}  //up
    a=analogRead(1);
    lcd.setCursor(8,0);           //display analog value
    lcd.write(a/1000+'0');
    lcd.write((a/100)%10+'0');
    lcd.write((a/10)%10+'0');
    lcd.write(a%10+'0');
    lcd.setCursor(8,1);           //display threshold value
    lcd.write(threshold/1000+'0');
    lcd.write((threshold/100)%10+'0');
    lcd.write((threshold/10)%10+'0');
    lcd.write(threshold%10+'0');    
  }
  doscreen();             //get ready for normal operation
}

void showtime(unsigned long mm){     //take time in ms and show as hh:mm:ss.sss
  unsigned long h,m,s,ss;
  s=mm/1000;
  m=s/60;
  h=m/60;
  mm=mm%1000;
  s=s%60;
  m=m%60;
  h=h%100;
  lcd.write(h/10+'0');
  lcd.write(h%10+'0');
  lcd.write(':');
  lcd.write(m/10+'0');
  lcd.write(m%10+'0');
  lcd.write(':');
  lcd.write(s/10+'0');
  lcd.write(s%10+'0');
  lcd.write('.');
  lcd.write(mm/100+'0');
  lcd.write((mm/10)%10+'0');
  lcd.write(mm%10+'0');
  lcd.setCursor(0,1);     //move cursor to be ready for number
}

void doscreen(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(m[mode]);
  lcd.setCursor(11,0);
  lcd.print("Ready");
  lcd.setCursor(0,1);     //move cursor to be ready for number
  delay(300);             //debounce
}

int keystate(){     //read key value
  int a=analogRead(0);
  if(a>831){return 0;}  //no key
  if(a>523){return 1;}  //select
  if(a>331){return 2;}  //left
  if(a>177){return 3;}  //down
  if(a>50){return 4;}   //up
  return 5;             //right
}

int lcdkey(){
  int a=keystate();               //check
  delay(5);                       //wait a bit
  if(a==keystate()){return a;}    //if there's no bounce key is pressed properly
  return 0;                       //otherwise nothing
}

