#include "calculate_fun.h"

#define OVERFLOW 
Time time = {0};



void adc_init(void){
//thermo.addr = ADS1115_ADD2;
//thermo.dr = ADS1115_DR_8SPS;
//thermo.mux = ADS1115_MUX_AIN1_AIN3;
//thermo.pga = ADS1115_PGA_0_256V; 
//thermo.gain =  4.004; 
//thermo.rst_ct = 300;
    
//for(uint8_t i =0;i<2;i++){  
//pressure[i].addr = ADS1115_ADD1;
//pressure[i].dr = ADS1115_DR_64SPS;
//pressure[i].mux = ADS1115_MUX_AIN0_GND;
//pressure[i].pga = ADS1115_PGA_4_096V; 
//pressure[i].gain =  4.004;   
//pressure[i].rst_ct = 300;
//  
//speed[i].addr = ADS1115_ADD1;
//speed[i].dr = ADS1115_DR_64SPS;
//speed[i].mux = ADS1115_MUX_AIN2_GND;
//speed[i].pga = ADS1115_PGA_4_096V; 
//speed[i].gain =  4.004;
//speed[i].rst_ct = 300;
//}

//inject_r.addr = ADS1115_ADD2;
//inject_r.dr = ADS1115_DR_64SPS;
//inject_r.mux = ADS1115_MUX_AIN0_GND;
//inject_r.pga = ADS1115_PGA_4_096V; 
//inject_r.gain =  4.004;
//inject_r.rst_ct = 300;


//mold_r.addr = ADS1115_ADD2;
//mold_r.dr = ADS1115_DR_64SPS;
//mold_r.mux = ADS1115_MUX_AIN2_GND;
//mold_r.pga = ADS1115_PGA_4_096V; 
//mold_r.gain =  4.004;
//mold_r.rst_ct = 300;


//........................................ Test

for(uint8_t i =0;i<2;i++){  
pressure[i].addr = ADS1115_ADD1;
pressure[i].dr = ADS1115_DR_250SPS;
pressure[i].mux = ADS1115_MUX_AIN0_GND;
pressure[i].pga = ADS1115_PGA_4_096V; 
pressure[i].gain =  4.004;   
pressure[i].rst_ct = 300;
  
speed[i].addr = ADS1115_ADD1;
speed[i].dr = ADS1115_DR_250SPS;;
speed[i].mux = ADS1115_MUX_AIN2_GND;
speed[i].pga = ADS1115_PGA_4_096V; 
speed[i].gain =  4.004;
speed[i].rst_ct = 300;
}

inject_r.addr = ADS1115_ADD1;
inject_r.dr = ADS1115_DR_250SPS;
inject_r.mux = ADS1115_MUX_AIN0_GND;
inject_r.pga = ADS1115_PGA_4_096V; 
inject_r.gain =  4.004;
inject_r.rst_ct = 300;


mold_r.addr = ADS1115_ADD1;
mold_r.dr = ADS1115_DR_250SPS;
mold_r.mux = ADS1115_MUX_AIN0_GND;
mold_r.pga = ADS1115_PGA_4_096V; 
mold_r.gain =  4.004;
mold_r.rst_ct = 300;

//.........................................
    
    
}


float cal_speed(Analog *adc,BASE_TIME base){
           ADS1115_ReadADC(adc); 
//                adc->Tick_pre = adc->Tick; 
//                adc->Tick = time.t;   
    int time_interval = sqrt(adc->Tick - adc->Tick_pre);
   // printf("Time_Interval=%d\n",time_interval);
    adc->prespeed = adc->speed;    
    if(time_interval !=0){
    adc->speed = base*((adc->f_value - adc->f_preval)/ time_interval); // volt/s
        //printf("Value =%f\n",(adc->f_value - adc->f_preval));
    }else{
      adc->speed = 0.0;  
    }

if((adc->speed < adc->minspeed) && (adc->speed<offset_1_0)){
   adc->minspeed = adc->speed;
  }     
    //printf("adc->speed=%f,Value=%f,preValue=%f\n",adc->speed,adc->f_value,adc->f_preval);
    //printf("adc->speed=%f\n",adc->speed);  
    //printf("adc->min__speed=%f\n",adc->minspeed);    
  return adc->speed;  
}


void analog_record(Analog *adc,char *header,VAL_opt opt){
      //memset(str_Temp,0,strlen(str_Temp));   
    adc->str[0] = '\0';
    if(opt == MAX_VAL){
        //printf("f_maxval=%f\n",adc->f_maxval);
    adc->i_maxval = round(adc->f_maxval*1000); //mv
    snprintf(adc->str, sizeof(adc->str), "%s%d,",header,adc->i_maxval);  
        printf("AnalogMAX_str-> %s\n",adc->str);        
    }else if(opt == VALUE){
       adc->i_value = round(adc->f_value*1000); //mv 
       snprintf(adc->str, sizeof(adc->str), "%s%d,",header,adc->i_value);  
    }else if(opt == min_SPEED){  
      adc->i_minspeed = round(adc->minspeed)+0;
      snprintf(adc->str, sizeof(adc->str), "%s%d,",header,adc->i_minspeed);
      printf("AnalogMAX_SPEED_str-> %s\n\n",adc->str);        
    }  
    
}

void Temp_read(void){
   /*
      CJT = readCJTemperature(cs);
      printf("CJT1:%.2f\n",CJT);
      i_Temp1 = round(f_Temp1);      
      ADS1115_ReadADC(&thermo); 
      f_Temp1 = thermocouple_read_temperature(1000.0*thermo.f_value,CJT-2.8,K_TYPE);
      i_Temp1 = round(f_Temp1);
      printf("TEMP=%.3f\n",f_Temp1);
      memset(str_Temp,0,strlen(str_Temp));            
      snprintf(str_Temp, sizeof(str_Temp), "T%d,",i_Temp1);
      printf("Thermocouple STRING data= %s\n",str_Temp);
      printf("\n.....................................\n");


*/    
    
    
}

void analog_reset(Analog *adc){
  adc->f_maxval = -0.0; // reset to lowest voltage
  adc->f_preval = 0.0;
  adc->f_value = 0.0;
  adc->Tick = 0.0;
  adc->Tick_pre = 0.0;  
  adc->i_maxval  = 0.0; 
  adc->speed = 0.0;  
  adc->i_value  = 0.0;
  adc->minspeed = 0.0;
  adc->i_minspeed = 0;   
  adc->prespeed = 0.0;
  adc->maxspeed = 0.0;    
    
}
void analog_queue(Queue *Q,char* str){
    
   snprintf(Q->List[Q->Index],sizeof(Q->List[Q->Index]),"%s",str);
    
   if(Q->Index < Q_LEN-1){  // 1
       Q->Index++;
   }else{
       Q->Index = 0;
   }  
    
}

void pvalidation(char * str){
    
    if(strlen(str) == 0){
        
        strcpy(str,"-0");
    }
    
}
