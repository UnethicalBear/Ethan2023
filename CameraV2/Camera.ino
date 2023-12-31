#include <stdint.h>
#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <Wire>
#include "config.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif 


struct regval_list{

  uint8_t reg_num;

  uint16_t value;

};


const struct regval_list qvga_ov7670[] PROGMEM = {

  { REG_COM14, 0x19 },

  { 0x72, 0x11 },

  { 0x73, 0xf1 },


  { REG_HSTART, 0x16 },

  { REG_HSTOP, 0x04 },

  { REG_HREF, 0xa4 },

  { REG_VSTART, 0x02 },

  { REG_VSTOP, 0x7a },

  { REG_VREF, 0x0a },


  { 0xff, 0xff }, /* END MARKER */

};


const struct regval_list yuv422_ov7670[] PROGMEM = {

  { REG_COM7, 0x0 },  /* Selects YUV mode */

  { REG_RGB444, 0 },  /* No RGB444 please */

  { REG_COM1, 0 },

  { REG_COM15, COM15_R00FF },

  { REG_COM9, 0x6A }, /* 128x gain ceiling; 0x8 is reserved bit */

  { 0x4f, 0x80 },   /* "matrix coefficient 1" */

  { 0x50, 0x80 },   /* "matrix coefficient 2" */

  { 0x51, 0 },    /* vb */

  { 0x52, 0x22 },   /* "matrix coefficient 4" */

  { 0x53, 0x5e },   /* "matrix coefficient 5" */

  { 0x54, 0x80 },   /* "matrix coefficient 6" */

  { REG_COM13, COM13_UVSAT },

  { 0xff, 0xff },   /* END MARKER */

};


const struct regval_list ov7670_default_regs[] PROGMEM = {//from the linux driver

  { REG_COM7, COM7_RESET },

  { REG_TSLB, 0x04 }, /* OV */

  { REG_COM7, 0 },  /* VGA */

  /*

  * Set the hardware window.  These values from OV don't entirely

  * make sense - hstop is less than hstart.  But they work...

  */

  { REG_HSTART, 0x13 }, { REG_HSTOP, 0x01 },

  { REG_HREF, 0xb6 }, { REG_VSTART, 0x02 },

  { REG_VSTOP, 0x7a }, { REG_VREF, 0x0a },


  { REG_COM3, 0 }, { REG_COM14, 0 },

  /* Mystery scaling numbers */

  { 0x70, 0x3a }, { 0x71, 0x35 },

  { 0x72, 0x11 }, { 0x73, 0xf0 },

  { 0xa2,/* 0x02 changed to 1*/1 }, { REG_COM10, 0x0 },

  /* Gamma curve values */

  { 0x7a, 0x20 }, { 0x7b, 0x10 },

  { 0x7c, 0x1e }, { 0x7d, 0x35 },

  { 0x7e, 0x5a }, { 0x7f, 0x69 },

  { 0x80, 0x76 }, { 0x81, 0x80 },

  { 0x82, 0x88 }, { 0x83, 0x8f },

  { 0x84, 0x96 }, { 0x85, 0xa3 },

  { 0x86, 0xaf }, { 0x87, 0xc4 },

  { 0x88, 0xd7 }, { 0x89, 0xe8 },

  /* AGC and AEC parameters.  Note we start by disabling those features,

  then turn them only after tweaking the values. */

  { REG_COM8, COM8_FASTAEC | COM8_AECSTEP },

  { REG_GAIN, 0 }, { REG_AECH, 0 },

  { REG_COM4, 0x40 }, /* magic reserved bit */

  { REG_COM9, 0x18 }, /* 4x gain + magic rsvd bit */

  { REG_BD50MAX, 0x05 }, { REG_BD60MAX, 0x07 },

  { REG_AEW, 0x95 }, { REG_AEB, 0x33 },

  { REG_VPT, 0xe3 }, { REG_HAECC1, 0x78 },

  { REG_HAECC2, 0x68 }, { 0xa1, 0x03 }, /* magic */

  { REG_HAECC3, 0xd8 }, { REG_HAECC4, 0xd8 },

  { REG_HAECC5, 0xf0 }, { REG_HAECC6, 0x90 },

  { REG_HAECC7, 0x94 },

  { REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_AGC | COM8_AEC },

  { 0x30, 0 }, { 0x31, 0 },//disable some delays

  /* Almost all of these are magic "reserved" values.  */

  { REG_COM5, 0x61 }, { REG_COM6, 0x4b },

  { 0x16, 0x02 }, { REG_MVFP, 0x07 },

  { 0x21, 0x02 }, { 0x22, 0x91 },

  { 0x29, 0x07 }, { 0x33, 0x0b },

  { 0x35, 0x0b }, { 0x37, 0x1d },

  { 0x38, 0x71 }, { 0x39, 0x2a },

  { REG_COM12, 0x78 }, { 0x4d, 0x40 },

  { 0x4e, 0x20 }, { REG_GFIX, 0 },

  /*{0x6b, 0x4a},*/{ 0x74, 0x10 },

  { 0x8d, 0x4f }, { 0x8e, 0 },

  { 0x8f, 0 }, { 0x90, 0 },

  { 0x91, 0 }, { 0x96, 0 },

  { 0x9a, 0 }, { 0xb0, 0x84 },

  { 0xb1, 0x0c }, { 0xb2, 0x0e },

  { 0xb3, 0x82 }, { 0xb8, 0x0a },


  /* More reserved magic, some of which tweaks white balance */

  { 0x43, 0x0a }, { 0x44, 0xf0 },

  { 0x45, 0x34 }, { 0x46, 0x58 },

  { 0x47, 0x28 }, { 0x48, 0x3a },

  { 0x59, 0x88 }, { 0x5a, 0x88 },

  { 0x5b, 0x44 }, { 0x5c, 0x67 },

  { 0x5d, 0x49 }, { 0x5e, 0x0e },

  { 0x6c, 0x0a }, { 0x6d, 0x55 },

  { 0x6e, 0x11 }, { 0x6f, 0x9e }, /* it was 0x9F "9e for advance AWB" */

  { 0x6a, 0x40 }, { REG_BLUE, 0x40 },

  { REG_RED, 0x60 },

  { REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_AGC | COM8_AEC | COM8_AWB },


  /* Matrix coefficients */

  { 0x4f, 0x80 }, { 0x50, 0x80 },

  { 0x51, 0 },    { 0x52, 0x22 },

  { 0x53, 0x5e }, { 0x54, 0x80 },

  { 0x58, 0x9e },


  { REG_COM16, COM16_AWBGAIN }, { REG_EDGE, 0 },

  { 0x75, 0x05 }, { REG_REG76, 0xe1 },

  { 0x4c, 0 },     { 0x77, 0x01 },

  { REG_COM13, /*0xc3*/0x48 }, { 0x4b, 0x09 },

  { 0xc9, 0x60 },   /*{REG_COM16, 0x38},*/

  { 0x56, 0x40 },


  { 0x34, 0x11 }, { REG_COM11, COM11_EXP | COM11_HZAUTO },

  { 0xa4, 0x82/*Was 0x88*/ }, { 0x96, 0 },

  { 0x97, 0x30 }, { 0x98, 0x20 },

  { 0x99, 0x30 }, { 0x9a, 0x84 },

  { 0x9b, 0x29 }, { 0x9c, 0x03 },

  { 0x9d, 0x4c }, { 0x9e, 0x3f },

  { 0x78, 0x04 },


  /* Extra-weird stuff.  Some sort of multiplexor register */

  { 0x79, 0x01 }, { 0xc8, 0xf0 },

  { 0x79, 0x0f }, { 0xc8, 0x00 },

  { 0x79, 0x10 }, { 0xc8, 0x7e },

  { 0x79, 0x0a }, { 0xc8, 0x80 },

  { 0x79, 0x0b }, { 0xc8, 0x01 },

  { 0x79, 0x0c }, { 0xc8, 0x0f },

  { 0x79, 0x0d }, { 0xc8, 0x20 },

  { 0x79, 0x09 }, { 0xc8, 0x80 },

  { 0x79, 0x02 }, { 0xc8, 0xc0 },

  { 0x79, 0x03 }, { 0xc8, 0x40 },

  { 0x79, 0x05 }, { 0xc8, 0x30 },

  { 0x79, 0x26 },

  { 0xff, 0xff }, /* END MARKER */

};



void error_led(void){

  DDRB |= 32;//make sure led is output

  while (1){//wait for reset

    PORTB ^= 32;// toggle led

    _delay_ms(100);

  }

}


void twiStart(void){

  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);//send start

  while (!(TWCR & (1 << TWINT)));//wait for start to be transmitted

  if ((TWSR & 0xF8) != TW_START)

    error_led();

}


void twiWriteByte(uint8_t DATA, uint8_t type){

  TWDR = DATA;

  TWCR = _BV(TWINT) | _BV(TWEN);

  while (!(TWCR & (1 << TWINT))) {}

  if ((TWSR & 0xF8) != type)

    error_led();

}


void twiAddr(uint8_t addr, uint8_t typeTWI){

  TWDR = addr;//send address

  TWCR = _BV(TWINT) | _BV(TWEN);    /* clear interrupt to start transmission */

  while ((TWCR & _BV(TWINT)) == 0); /* wait for transmission */

  if ((TWSR & 0xF8) != typeTWI)

    error_led();

}


voidwriteReg(uint8_t reg, uint8_t dat){

  //send start condition

  twiStart();

  twiAddr(camAddr_WR, TW_MT_SLA_ACK);

  twiWriteByte(reg, TW_MT_DATA_ACK);

  twiWriteByte(dat, TW_MT_DATA_ACK);

  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);//send stop

  _delay_ms(1);

}


static uint8_t twiRd(uint8_t nack){

  if (nack){

    TWCR = _BV(TWINT) | _BV(TWEN);

    while ((TWCR & _BV(TWINT)) == 0); /* wait for transmission */

    if ((TWSR & 0xF8) != TW_MR_DATA_NACK)

      error_led();

    return TWDR;

  }

  else{

    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);

    while ((TWCR & _BV(TWINT)) == 0); /* wait for transmission */

    if ((TWSR & 0xF8) != TW_MR_DATA_ACK)

      error_led();

    return TWDR;

  }

}


uint8_t rdReg(uint8_t reg){

  uint8_t dat;

  twiStart();

  twiAddr(camAddr_WR, TW_MT_SLA_ACK);

  twiWriteByte(reg, TW_MT_DATA_ACK);

  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);//send stop

  _delay_ms(1);

  twiStart();

  twiAddr(camAddr_RD, TW_MR_SLA_ACK);

  dat = twiRd(1);

  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);//send stop

  _delay_ms(1);

  return dat;

}


void wrSensorRegs8_8(const struct regval_list reglist[]){

  uint8_t reg_addr, reg_val;

  const struct regval_list *next = reglist;

  while ((reg_addr != 0xff) | (reg_val != 0xff)){

    reg_addr = pgm_read_byte(&next->reg_num);

    reg_val = pgm_read_byte(&next->value);

   writeReg(reg_addr, reg_val);

    next++;

  }

}


void setColor(void){

  wrSensorRegs8_8(yuv422_ov7670);

 // wrSensorRegs8_8(qvga_ov7670);

}


void setResolution(void){

 writeReg(REG_COM3, 4); // REG_COM3 enable scaling

  wrSensorRegs8_8(qvga_ov7670);

}


void camInit(void){

 writeReg(0x12, 0x80);

  _delay_ms(100);

  wrSensorRegs8_8(ov7670_default_regs);

 writeReg(REG_COM10, 32);//PCLK does not toggle on HBLANK.

}


void arduinoUnoInut(void) {

  cli();//disable interrupts

  

    /* Setup the 8mhz PWM clock

  * This will be on pin 11*/

  DDRB |= (1 << 3);//pin 11

  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  TCCR2A = (1 << COM2A0) | (1 << WGM21) | (1 << WGM20);

  TCCR2B = (1 << WGM22) | (1 << CS20);

  OCR2A = 0;//(F_CPU)/(2*(X+1))

  DDRC &= ~15;//low d0-d3 camera

  DDRD &= ~252;//d7-d4 and interrupt pins

  _delay_ms(3000);

  

    //set up twi for 100khz

  TWSR &= ~3;//disable prescaler for TWI

  TWBR = 72;//set to 100khz

  

    //enable serial

  UBRR0H = 0;

  UBRR0L = 1;//0 = 2M baud rate. 1 = 1M baud. 3 = 0.5M. 7 = 250k 207 is 9600 baud rate.

  UCSR0A |= 2;//double speed aysnc

  UCSR0B = (1 << RXEN0) | (1 << TXEN0);//Enable receiver and transmitter

  UCSR0C = 6;//async 1 stop bit 8bit char no parity bits

}



void StringPgm(const char * str){

  do{

      while (!(UCSR0A & (1 << UDRE0)));//wait for byte to transmit

      UDR0 = pgm_read_byte_near(str);

      while (!(UCSR0A & (1 << UDRE0)));//wait for byte to transmit

  } while (pgm_read_byte_near(++str));

}


static void captureImg(uint16_t wg, uint16_t hg){

  uint16_t y, x;


  StringPgm(PSTR("*RDY*"));


  while (!(PIND & 8));//wait for high

  while ((PIND & 8));//wait for low


    y = hg;

  while (y--){

        x = wg;

      //while (!(PIND & 256));//wait for high

    while (x--){

      while ((PIND & 4));//wait for low

            UDR0 = (PINC & 15) | (PIND & 240);

          while (!(UCSR0A & (1 << UDRE0)));//wait for byte to transmit

      while (!(PIND & 4));//wait for high

      while ((PIND & 4));//wait for low

      while (!(PIND & 4));//wait for high

    }

    //  while ((PIND & 256));//wait for low

  }

    _delay_ms(100);

}


void setup(){

  arduinoUnoInut();

  camInit();

  setResolution();

  setColor();

 writeReg(0x11, 10); //Earlier it had the value:writeReg(0x11, 12); New version works better for me :) !!!!

}



void loop(){

  captureImg(320, 240);

}
