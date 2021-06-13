#include "mbed.h"
#include"bbcar.h"


BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
Ticker servo_ticker;
PwmOut pin5(D11), pin6(D10);

DigitalInOut ping(D6);

BBCar car(pin5, pin6, servo_ticker);

Thread dis;
Thread getdata;

Timer t;

void Distance();
void Getdata();

float val = 0;

int state = 0;
int num = 0;
int TX, TY, TZ, RX, RY, RZ;
int coun = 0;
int data = 0;

int main() {
   dis.start(Distance);
   getdata.start(Getdata);
   int predata = 0;
   while(1) {
      if (predata != data) {
            //printf("TX: %d, TY: %d, TZ: %d, RX: %d, RY: %d, RZ: %d", TX, TY, TZ, RX, RY, RZ);
            double angle = atan(-TX *1.0 / TZ) * 54.7;
            if ((angle > 15) || (angle < -15)) {
                  car.turnangle(angle);
            } 
            printf("Ping = %lf\r\n", val*17700.4f);
            predata = data;
      } else {
            car.stop();
      }
   }
}

void Distance() {
   pc.set_baud(9600);
   while(1) {
      ping.output();
      ping = 0; wait_us(200);
      ping = 1; wait_us(5);
      ping = 0; wait_us(5);
      ping.input();
      while(ping.read() == 0);
      t.start();
      while(ping.read() == 1);
      val = t.read();
      t.stop();
      t.reset();
      ThisThread::sleep_for(1s);
   }
}

void Getdata() {
   uart.set_baud(9600);
   while(1) {
      if(uart.readable()){
         char recv[1];
         char T[4], R[3];
         uart.read(recv, sizeof(recv));
         if (state == 1) {
            if (num == 0) {
               T[coun++] = recv[0];
               if (coun == 4) {
                  TX = atoi(T);
                  num++;
                  coun = 0;
               }
            } else if (num == 1) {
               T[coun++] = recv[0];
               if (coun == 4) {
                  TY = atoi(T);
                  num++;
                  coun = 0;
               }
            } else if (num == 2) {
               T[coun++] = recv[0];
               if (coun == 4) {
                  TZ = atoi(T);
                  num++;
                  coun = 0;
               }
            } else if (num == 3) {
               R[coun++] = recv[0];
               if (coun == 3) {
                  RX = atoi(R);
                  num++;
                  coun = 0;
               }
            } else if (num == 4) {
               R[coun++] = recv[0];
               if (coun == 3) {
                  RY = atoi(R);
                  num++;
                  coun = 0;
               }
            } else if (num == 5) {
               R[coun++] = recv[0];
               if (coun == 3) {
                  RZ = atoi(R);
                  num++;
                  coun = 0;
                  state = 0;
                  data++;
               }
            }
         } else if (state == 0 && recv[0] == 's') {
            state = 1;
            num = 0;
            coun = 0;
         }
      }
   }
}