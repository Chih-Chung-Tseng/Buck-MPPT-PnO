#define Duty_pin 8
#define CurOUT_pin A0
#define CurIN_pin A1
#define volOUT_pin A2//BATTERY
#define DCbus_pin A3
#define volPV_pin A6
#define VCC_pin A4

#define volOUT_gain 0.00435//
#define volOUToffset -0.24363721
//volOUT=raw*gain-offset
#define volPV_gain 0.0065742657 // 1/0.122257807*3.3/4095, assume 0 offset
//volPV=raw*gain
#define volDCbus_gain 0.0065742657
//volDCbus=raw*gain
#define VCC_gain 0.001742
#define CTin_Vgain 0.001624418681//raw*CTin_Vgain=CTin_V
#define CTout_Vgain 0.001624418681//raw*CTout_Vgain=CTout_V
#define CTin_gain 10//10A/V
#define CTout_gain 10//10A/V
#define CTin_ofst 0.0135
#define CTout_ofst 0.0007//-0.003
//#define CurOUT_offset 2.49
//#define CurIN_offset 2.49
//Cur=(raw-offset)*gain
#define duty_max 240
#define duty_min 70
#define startup_vth 11.5
#define duty_step 5
#define Pth 0.1
#define itr 1000
#define Vfchg 14.4
double Duty;
double CurOUT=0;
double CurIN=0;
double volOUT=0;
double VDCbus=0;
double volPV=0;
double VCC=4.995;
double Pin_prv=0;
double Pin_new=0;
double Pout_prv=0;
double Pout_new=0;
short int dir=1;
//double swap=0;
void acquist(void);
void setup() {
  pinMode(Duty_pin,OUTPUT);
  pinMode(CurOUT_pin,INPUT);
  pinMode(CurIN_pin,INPUT);
  pinMode(volOUT_pin,INPUT);
  pinMode(DCbus_pin,INPUT);
  pinMode(volPV_pin,INPUT);
  analogReadResolution(12);
  Serial.begin(9600);
 /* do{//startup
    acquist();
    disp();
    
//  CurOUT=(analogRead(CurOUT_pin)-CurOUT_offset)*CurOUT_gain;
//  CurIN=(analogRead(CurIN_pin)-CurIN_offset)*CurIN_gain;
    if(volPV>startup_vth){
      analogWrite(Duty_pin,Duty);
      Duty=12.0/VDCbus*255.0;
    }
    Serial.println("startup ");
    delay(1000);
  }while(volOUT<9);*/
  Serial.println("startup ok");
  //Duty=12.5/VDCbus;
  acquist();
  Duty=178;//Guess Vmpp=70%*VDCbus
}
void loop() {
  
  if(volPV>startup_vth){
    if(volOUT<Vfchg){
      
       //acquist();
       if(Pin_new<Pin_prv){
       //dir=dir^1;//dir=!dir
       dir=-dir;
       }
       Pin_prv=Pin_new;
       Pout_prv=Pout_new;
       if((Duty<=duty_max-duty_step && dir==1)||(Duty>=duty_min+duty_step && dir==-1)){//saturate Duty_cycle with boundary value
        Duty=Duty+dir*duty_step; 
       }
       /*if(Pout_new-Pout_prv<Pth){
           dir^=1;
       }
       if(!dir){
         Duty-=duty_step;
       }
       else{
         Duty+=duty_step;
       }*/
       analogWrite(Duty_pin,Duty);
       acquist();
       Pin_new=volPV*CurIN;
       Pout_new=volOUT*CurOUT;
    }
    else{
      Duty=(int)(14.6/VDCbus*255);
      analogWrite(Duty_pin,Duty);
    }
  }
  //analogWrite(Duty_pin,Duty);
  disp();
  delay(1000);
}
void acquist(void){
  volPV=0;volOUT=0;VDCbus=0;CurOUT=0;CurIN=0;//VCC=0;
  for(int i=0;i<itr;i++){
  volPV+=analogRead(volPV_pin)*volPV_gain;
  volOUT+=analogRead(volOUT_pin)*volOUT_gain;
  VDCbus+=analogRead(DCbus_pin)*volDCbus_gain;
  CurOUT+=analogRead(CurOUT_pin);
  //CurOUT=analogRead(CurOUT_pin);
  //CurIN=analogRead(CurIN_pin);
  CurIN+=analogRead(CurIN_pin);
  //VCC+=analogRead(VCC_pin);
  }
  volPV=volPV/itr;volOUT=volOUT/itr;VDCbus=VDCbus/itr;//VCC=VCC/itr*VCC_gain;
  CurOUT=float(CurOUT/itr*CTout_Vgain-VCC*0.5-CTout_ofst)*10;
  CurIN=float(CurIN/itr*CTin_Vgain-VCC*0.5-CTin_ofst)*10;
}
void disp(void){
  Serial.println("-----------");
  Serial.print("5V");
  Serial.println(VCC);
  Serial.print("volPV: ");
  Serial.print(volPV);
  Serial.print(" CurIN: ");
  Serial.println(CurIN);
  Serial.print("Pin_new: ");
  Serial.print(Pin_new);
  Serial.print("Pin_prv: ");
  Serial.println(Pin_prv);
  Serial.print("VDCbus: ");
  Serial.print(VDCbus);
  Serial.print(" volOUT: ");
  Serial.print(volOUT);
  Serial.print(" CurOUT: ");
  Serial.println(CurOUT);
  Serial.print("Pout_new: ");
  Serial.print(Pout_new);
  Serial.print(" Pout_prv: ");
  Serial.println(Pout_prv);
  Serial.print("Duty: ");
  Serial.print(Duty);
  Serial.print(" direction: ");
  Serial.println(dir);
}
