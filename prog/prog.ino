/**
 * Based on https://gist.github.com/racerxdl/c9a592808acdd9cd178e6e97c83f8baf
 * which was based on: https://github.com/jaromir-sukuba/efm8prog/
 Use his SW to program EFM8 using arduino.
**/

// GPIO is manipulated through PORT mappings for better speed.
// Flashes EFM8 at about 10kB/s
// Baud rate: 1000000

// Digital pin 2 on Mega
#define C2D_PORT  PORTE
#define C2D_PIN   4

// Digital pin 3 on Mega
#define C2CK_PORT   PORTE
#define C2CK_PIN    5

#define LED LED_BUILTIN 

#define  INBUSY    0x02
#define OUTREADY  0x01

static void c2_send_bits (unsigned char data, unsigned char len);
static unsigned char c2_read_bits (unsigned char len);

void c2_rst (void);
void c2_write_addr (unsigned char addr);
static unsigned char c2_read_addr (void);
static unsigned char c2_read_data (void);
static void c2_write_data (unsigned char addr);

unsigned char c2_init_PI (void);
unsigned char c2_read_flash_block (unsigned int addr, unsigned char * data, unsigned char len);
static unsigned char c2_poll_bit_low (unsigned char mask);
static unsigned char c2_poll_bit_high (unsigned char mask);
unsigned char c2_write_flash_block (unsigned int addr, unsigned char * data, unsigned char len);
unsigned char c2_erase_device (void);


void c2_rst() {
  C2CK_PORT &= ~(1<<C2CK_PIN);
  delayMicroseconds(50);
  C2CK_PORT |= (1<<C2CK_PIN);
  delayMicroseconds(50);
}

#define c2_pulse_clk()\
  C2CK_PORT &= ~(1<<C2CK_PIN); \
  C2CK_PORT |= (1<<C2CK_PIN);


static unsigned char c2_read_bits (unsigned char len) {
  unsigned char i, data, mask;
  mask = 0x01 << (len-1);
  data = 0;
  //pinMode(C2D, INPUT);
  DDRE &= ~(1<<C2D_PIN);
  PINE &= (1<<C2D_PIN);
  for (i=0;i<len;i++) {
    c2_pulse_clk();
    data = data >> 1;
    if (PINE & (1<<C2D_PIN)) {
      data = data | mask;
    }
  }
  DDRE |= (1<<C2D_PIN);
  //pinMode(C2D, OUTPUT);

  return data;
}

static void c2_send_bits (unsigned char data, unsigned char len) {
  unsigned char i;
  //pinMode(C2D, OUTPUT);
  DDRE |= (1<<C2D_PIN);
  for (i=0;i<len;i++) {
    if (data&0x01) {
      C2D_PORT |= (1<<C2D_PIN);
    } else {
      C2D_PORT &= ~(1<<C2D_PIN); 
    }
    c2_pulse_clk();
    data = data >> 1;
  }
}

static void c2_write_data (unsigned char data) {
  unsigned char retval;
  c2_send_bits(0x0, 1);
  c2_send_bits(0x1, 2);
  c2_send_bits(0x0, 2);
  c2_send_bits(data, 8);
  retval = 0;
  while (retval == 0) {
    retval = c2_read_bits(1);
  }
  c2_send_bits(0x0, 1);
}

static unsigned char c2_poll_bit_high (unsigned char mask) {
  unsigned char retval;
  retval = c2_read_addr();
  while ((retval&mask)==0) retval = c2_read_addr();
}

static unsigned char c2_poll_bit_low (unsigned char mask) {
  unsigned char retval;
  retval = c2_read_addr();
  while (retval&mask) retval = c2_read_addr();
}

unsigned char c2_read_flash_block (unsigned int addr, unsigned char * data, unsigned char len) {
  unsigned char retval,i;
  c2_write_addr(0xB4);
  c2_write_data(0x06);
  c2_poll_bit_low(INBUSY);
  c2_poll_bit_high(OUTREADY);
  retval = c2_read_data();
  c2_write_data(addr>>8);
  c2_poll_bit_low(INBUSY);
  c2_write_data(addr&0xFF);
  c2_poll_bit_low(INBUSY);
  c2_write_data(len);
  c2_poll_bit_low(INBUSY);
  c2_poll_bit_high(OUTREADY);
  retval = c2_read_data();
  for (i=0;i<len;i++) {
    c2_poll_bit_high(OUTREADY);
    retval = c2_read_data();
    data[i] = retval;
  }
  return i;
}

unsigned char c2_write_flash_block (unsigned int addr, unsigned char * data, unsigned char len) {
  unsigned char retval,i;
  c2_write_addr(0xB4);
  c2_write_data(0x07);
  c2_poll_bit_low(INBUSY);
  c2_poll_bit_high(OUTREADY);
  retval = c2_read_data();
  c2_write_data(addr>>8);
  c2_poll_bit_low(INBUSY);
  c2_write_data(addr&0xFF);
  c2_poll_bit_low(INBUSY);
  c2_write_data(len);
  c2_poll_bit_low(INBUSY);  
  c2_poll_bit_high(OUTREADY);
  retval = c2_read_data();  
  for (i=0;i<len;i++) {
    c2_write_data(data[i] );
    c2_poll_bit_low(INBUSY);
  } 
  c2_poll_bit_high(OUTREADY);
}

unsigned char c2_erase_device (void) {
  unsigned char retval;
  c2_write_addr(0xB4);
  c2_write_data(0x03);
  c2_poll_bit_low(INBUSY);
  c2_poll_bit_high(OUTREADY);
  retval = c2_read_data();
  c2_write_data(0xDE);
  c2_poll_bit_low(INBUSY);  
  c2_write_data(0xAD);
  c2_poll_bit_low(INBUSY);  
  c2_write_data(0xA5);
  c2_poll_bit_low(INBUSY);  
  c2_poll_bit_high(OUTREADY);
  retval = c2_read_data();
}

unsigned char c2_init_PI (void) {
  c2_rst();
  c2_write_addr(0x02);
  c2_write_data(0x02);
  c2_write_data(0x04);
  c2_write_data(0x01);
  delay(20);
  return 0;
}

static unsigned char c2_read_data() {
  unsigned char retval;
  c2_send_bits(0x0, 1);
  c2_send_bits(0x0, 2);
  c2_send_bits(0x0, 2);
  retval = 0;
  while (retval == 0) {
    retval = c2_read_bits(1);
  }
  retval = c2_read_bits(8);
  c2_send_bits(0x0, 1);
  return retval;
}

static unsigned char c2_read_addr() {
  unsigned char retval;
  c2_send_bits(0x0, 1);
  c2_send_bits(0x2, 2);
  retval = c2_read_bits(8);
  c2_send_bits(0x0, 1);
  return retval;
}

void c2_write_addr(unsigned char addr) {
  c2_send_bits(0x0,1);
  c2_send_bits(0x3,2);
  c2_send_bits(addr,8);
  c2_send_bits(0x0,1);  
}

void setup() {
  Serial.begin(1000000);
  
  DDRE |= (1<<C2D_PIN);
  DDRE |= (1<<C2CK_PIN);
  C2CK_PORT |= (1<<C2CK_PIN);
  
  digitalWrite(LED, LOW);
}

unsigned int i;
unsigned char retval;
unsigned char rx_message[250],rx_message_ptr;
unsigned char rx,main_state,bytes_to_receive,rx_state;
unsigned char flash_buffer[250];
unsigned long addr;

unsigned char rx_state_machine (unsigned char state, unsigned char rx_char) {
  if (state==0) {
      rx_message_ptr = 0;
      rx_message[rx_message_ptr++] = rx_char;
      return 1;
  }
  if (state==1) {
      bytes_to_receive = rx_char;
      rx_message[rx_message_ptr++] = rx_char;
      if (bytes_to_receive==0) return 3;
      return 2;
  }
  if (state==2) {
      rx_message[rx_message_ptr++] = rx_char;
      bytes_to_receive--;
      if (bytes_to_receive==0) return 3;
  }
  return state;  
}

void loop() {

  if (Serial.available()) {
    rx = Serial.read();
    rx_state = rx_state_machine(rx_state, rx);
    if (rx_state == 3) {
      switch (rx_message[0]) {
        case 0x0:
        Serial.write(0x80);
        break;
        case 0x01:
          c2_init_PI();
          Serial.write(0x81);
          digitalWrite(LED, HIGH);
          rx_state = 0;
          break;
        case 0x02:
          c2_rst();
          Serial.write(0x82);
          digitalWrite(LED, LOW);
          rx_state = 0;
          break;
        case 0x03:
          addr = (((unsigned long)(rx_message[3]))<<16) + (((unsigned long)(rx_message[4]))<<8) + (((unsigned long)(rx_message[5]))<<0);
          for (i=0;i<rx_message[2];i++) {
            flash_buffer[i] = rx_message[i+6];
          }
          c2_write_flash_block(addr,flash_buffer,rx_message[2]);
          Serial.write(0x83);
          rx_state = 0;
          break;
        case 0x04:
          c2_erase_device();
          Serial.write(0x84);
          rx_state = 0;
          break;
        case 0x05:
          Serial.write(0x85);
          addr = (((unsigned long)(rx_message[3]))<<16) + (((unsigned long)(rx_message[4]))<<8) + (((unsigned long)(rx_message[5]))<<0);
          c2_read_flash_block(addr,flash_buffer,rx_message[2]);
          for (i=0;i<rx_message[2];i++) {
            Serial.write(flash_buffer[i]);
          }
          rx_state = 0;
          break;
        case 0x06:
          c2_write_addr(rx_message[3]);
          c2_write_data(rx_message[4]);
          Serial.write(0x86);
          rx_state = 0;
          break;
        case 0x07:
          c2_write_addr(rx_message[3]);
          Serial.write(c2_read_data());
          Serial.write(0x87);
          rx_state = 0;
          break;
      }
    }
  }
}
