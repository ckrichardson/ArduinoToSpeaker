// Clifford Richardson, Kripash Shrestha Version 1.2
// CPE 301 - Lab 7
// 10/30/17
void MyDelay(unsigned long len);
#define RDA 0x80
#define TBE 0x20
//global variables intializing for the ports
volatile unsigned char* myUCSR0A = (unsigned char*) 0xC0;
volatile unsigned char* myUCSR0B = (unsigned char*) 0xC1;
volatile unsigned char* myUCSR0C = (unsigned char*) 0xC2;
volatile unsigned int* myUBRR0 = (unsigned int*) 0xC4;
//replaced with one line since the baud register low and high bytes can be done at once
//volatile unsigned int* myUBRR0H = (unsigned int*) 0xC5;
//volatile unsigned int* myUBRR0L = (unsigned int*) 0xC4;
volatile unsigned char* myUDR0 = (unsigned char*) 0xC6;

// TIMER variables
volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned int *myTCINT1 = (unsigned int *) 0x84;
volatile unsigned char *myTIFR1 = (unsigned char *) 0x36;
// GPIO PORTS
volatile unsigned char *portDDRB = (unsigned char *) 0x24;
volatile unsigned char *portB = (unsigned char *) 0x25;
 //A B middleC D E F G A# n/a C# D# n/a F# G# Uppercase letters represent
//normals and lowercase represent sharp in the code
unsigned long ticks[] = {18182,16194, 15296, 13629, 12140, 11461, 10204, 17167, 0 , 14440, 12821, 0 ,
10811,9627};
//The array index 0-6 represent the normal notes of the Piano and the index 7 - 13 represent the sharp of the
//notes.
//Since there is no B# or E# , i represented them with the value of 0 in the program so that they are just ignored to
//not mess with the algorithm.
byte inputRead;
int index = 0;
int hold = 0;

void setup()
{
   //Enable serial
 
   U0init(9600);
  *portDDRB |= 0x40;  //Setting the sixth bit to write
  
   //Initialize the timer
   *myTCCR1A = 0x00;
   *myTCCR1B = 0x00;
   *myTCCR1C = 0x00;
   *myTIMSK1 |= 0x01;
   *myTIFR1 |= 0x01;
    *myTCCR1B |= 0x01;
   
}
void loop()
{
 if(U0kbhit())
 {
    inputRead = U0getchar();
 }

 U0putchar(inputRead);
 if(inputRead >= 'A' && inputRead <= 'G') //If the input is from A to G for the normal notes
 {
 *portDDRB = 0x40; //set sixth bit to write and do index = the inputRead character - 65
 index = inputRead - 65; //this is set to that A is set to index 0 of the array and B is index 1 and so on....
 }
 if(inputRead >= 'a' && inputRead <= 'g') //If the input is from a to g for the sharp notes
 {
 *portDDRB = 0x40; //set sixth bit to write and check if the character is b or e
 hold = index; //use int hold to hold index for the inputs b and e

 index = inputRead - 97 + 7; //otherwise do index = inputREad - 97 + 7, -97 will subtract from the ASCII
//table and set index to 0 but since the sharps start at index 7, we add 7 so a will be 7 and so on...
 if(inputRead == 'b' || inputRead == 'e'){
 index = hold; //since there is no B# or E# for our table, we just breaks from the function and the
//index is not modified and keeps the previous note
 }
 }
 if(inputRead == 'q') //if input is q
 {
 *portDDRB = 0x00; //turn off the note if the input is q
 }
 //myDelay(ticks[index]); //call my delay function
}

void U0init(int U0baud)
{
// Students are responsible for understanding
// this initialization code for the ATmega2560 USART0
// and will be expected to be able to intialize
// the USART in differrent modes.
//
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0 = tbaud;
 //replaced with one line (above) since the baud register low and high bytes can be done at once
 //*myUBRR0H = (tbaud>>8);
 //*myUBRR0L = (tbaud & 0xFF);
}



// Read USART0 RDA status bit and return 0 if RDA is false, other wise return 1 if RDA is true
unsigned char U0kbhit()
{
 if(*myUCSR0A & RDA){ //Mask *myUCSR0A with the RDA define mask to check if it is true
 return 1; //if it is true, return 1
 }
 else{
 return 0; //otherwise return false
 }
}


// Read one character from the serial port and return it
unsigned char U0getchar()
{
 return *myUDR0; //return *myUDR0 which will return the character
}


// Wait until the serial port TBE status bit is high and then take the
//character U0pdata and send the character out to the serial port
void U0putchar(unsigned char U0pdata)
{
 while(!(*myUCSR0A & TBE)){ //Check if the Serial port TBE status bit is high with the TBE define mask
 }
 *myUDR0 = U0pdata; //send the character out to the serial port if it is high
}


//did the load timer twice in this function instead of calling the function twice.
//example: instead of doing
// myDelay(ticks{index];
// *portB |= 0x40;
// myDelay(ticks{index}; I would just be calling mydelay once like i did in the loop.
/*void myDelay(unsigned long len)
{
 *portB |= 0x40;
 *myTCCR1B &= 0xF8; //turn off timer
 *myTCINT1 = (unsigned int) (65536 - (long)len); //load
 *myTCCR1B |= 0x01; //prescalar of 1
 while((*myTIFR1 & 0x01)==0); //wait for TIFR overflow flag bit to be set
 *myTCCR1B &= 0xFB; //turn off timer after getting delay
 *myTIFR1 |= 0x01; //clear the flag bit for next use
 *portB &= 0x00;
 *myTCCR1B &= 0xF8; //turn off timer
 *myTCINT1 = (unsigned int) (65536 - (long)len); //load
 *myTCCR1B |= 0x01; //prescalar of 1
 while((*myTIFR1 & 0x01)==0); //wait for TIFR overflow flag bit to be set
 *myTCCR1B &= 0xFB; //turn off timer after getting delay
 *myTIFR1 |= 0x01; //clear the flag bit for next use
}*/
ISR(TIMER1_OVF_vect)
{
  *myTCCR1B &= 0xF8; 
  *myTCINT1 = (unsigned int) (65536 - ticks[index]); 
  *portB ^= 0x40;
  *myTCCR1B |= 0x01; 

 /*portB &= 0x00;
  *myTCCR1B &= 0xF8; 
  *myTCINT1 = (unsigned int) (65536 - (long)len); 
  *myTCCR1B |= 0x01;*/

}