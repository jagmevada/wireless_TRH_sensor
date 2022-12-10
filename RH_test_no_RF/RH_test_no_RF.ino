#include  "cactus_io_SHT31.h"
#define nop() __asm__("nop\n\t")
#define ADDRESS 25
cactus_io_SHT31 sht31(0x44);     // for the alternative address 

void setup() {
if(ADDRESS>15)
randomSeed(1345+ADDRESS<<1);
else
randomSeed(1<<ADDRESS);

TCB0.CCMP = 16000;  // main clock 16MHz, 2ms timer 
TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm | TCB_RUNSTDBY_bm;
TCB0.INTCTRL = TCB_CAPT_bm;
  
  Serial.begin(9600);
  delay(1000);
  if (!sht31.begin()) {   
    Serial.println("Could not find the sensor. Check wiring and I2C address");
    while (1) delay(1);
  }
  Serial.println("Address\t\tRandom\t\tHumidity\tTemp"); 
}
unsigned char sensor_update=0;
unsigned char transmit_update=1, tx_ready=0, tx_sent=0;
unsigned long int tx_ready_timer=0, tx_old_timer=0;
unsigned char j=0;
unsigned int timercount=0;
float temperature=0, humidity=0;
void loop() {
  if(sensor_update){
   humidity=sht31.getHumidity();
   temperature=sht31.getTemperature_C();
  //Serial.print(sht31.getTemperature_F()); Serial.print(" *F\t"); 
//  Serial.print(sht31.getDewPoint()); Serial.println(" *C"); 
  sensor_update=0;
  }
  if(transmit_update){
    transmit_update=0;
    tx_ready=0;
       tx_ready_timer=random(200,5000);
       tx_old_timer=tx_ready_timer;
    //send data
  }
  if(tx_ready){ 
  tx_ready_timer=5000;// tx may take 10 second;
      //Send data
Serial.print(ADDRESS); Serial.print("\t\t");  
Serial.print(tx_old_timer); Serial.print(" Ms\t\t");      
Serial.print(humidity); Serial.print(" %\t\t"); 
Serial.print(temperature); Serial.println(" *C");
  transmit_update=1;
  }
nop();
// __asm__("nop\n\t");  //1 cycle delay
}

ISR(TCB0_INT_vect) { // on overflow, we will increment TCA0.CMP0, this will happen after every full cycle - a little over 7 minutes.
  if(tx_ready_timer>0){
    tx_ready_timer--;
  }
  else
  tx_ready=1;
  
  switch(timercount)
  {
    case 1:
    sensor_update=1;
         break;
    case 501:  // 500/second
      timercount=0;
      break;
  default:
  break;
  }
  timercount++;
 TCB0.INTFLAGS = TCB_CAPT_bm; /* Clear the interrupt flag */
}
