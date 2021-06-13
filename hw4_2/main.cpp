#include"mbed.h"
#include"bbcar.h"


BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
Ticker servo_ticker;
PwmOut pin5(D11), pin6(D10);

BBCar car(pin5, pin6, servo_ticker);

Thread getdata;

void Getdata();

int state = 0;
int num = 0;
int x_1, x_2;
int y_1, y_2;
int coun = 0;
int data = 0;

int total_speed = 90;

int main(){
   getdata.start(Getdata);
   int predata = 0;
   while(1){
      if(predata != data){
         predata = data;
         //printf("x1: %d, x2: %d, y1: %d, y2: %d\n", x_1, x_2, y_1, y_2);
         double ratio = x_1 / 150.0;
         double left_speed = total_speed * ratio;
         double right_speed = total_speed - left_speed;
         printf("left: %f, right: %f\n", left_speed, right_speed);
         car.go(left_speed, right_speed);
      } else {
         car.stop();
      }
   }
}

void Getdata() {
   uart.set_baud(9600);
   while(1) {
      if(uart.readable()){
         char recv[1];
         char xy[3];
         uart.read(recv, sizeof(recv));
         uart.write(recv, sizeof(recv));
         if (state == 1) {
            if (num == 0) {
               xy[coun++] = recv[0];
               if (coun == 3) {
                  x_1 = atoi(xy);
                  num++;
                  coun = 0;
               }
            } else if (num == 1) {
               xy[coun++] = recv[0];
               if (coun == 3) {
                  x_2 = atoi(xy);
                  num++;
                  coun = 0;
               }
            } else if (num == 2) {
               xy[coun++] = recv[0];
               if (coun == 3) {
                  y_1 = atoi(xy);
                  num++;
                  coun = 0;
               }
            } else if (num == 3) {
               xy[coun++] = recv[0];
               if (coun == 3) {
                  y_2 = atoi(xy);
                  num++;
                  coun = 0;
                  state = 0;
                  data++;
               }
            }
         } else if (state == 0 && recv[0] == 'l') {
            state = 1;
            num = 0;
            coun = 0;
         }
      }
   }
}