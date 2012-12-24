#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// from notes.c
extern const unsigned char* buffers[4];
extern const unsigned short buffer_sizes[4];

// an interrupt divider - for choosing the current interrupt handler
volatile uint8_t divi;

// playback buffer properties
volatile uint16_t loc;
const unsigned char * buffer;
uint16_t buffer_len;

// button status
uint8_t current_status;
uint8_t current_button;

#define MAX_SEQUENCE_LEN 32
unsigned char g_sequence[MAX_SEQUENCE_LEN];

unsigned int g_random;

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

void setup()
{
  divi = 0;
  current_status = 0; // no buttons are pressed
  current_button = 0xFF; // start with no button
  g_random = 0;
  loc = 0;
  buffer_len = 0;
  buffer = (unsigned char*)0xFFFF;
  
  //Serial.begin(9600); // for debugging
  
  pinMode(11, OUTPUT);
  
  // Set up Timer 2 to do pulse width modulation on the speaker
  // pin.
  
  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));
  
  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);
  
  // Do non-inverting PWM on pin OC2A (p.155)
  // On the Arduino this is pin 11.
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
  TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));
  
  // No prescaler (p.158)
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // Set initial pulse width to the first sample.
  OCR2A = 0x80;
  
  
  // Set up Timer 1 to send a sample every interrupt.
  
  cli();
  
  // Set CTC mode (Clear Timer on Compare Match) (p.133)
  // Have to set OCR1A *after*, otherwise it gets reset to 0!
  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));
  
  // No prescaler (p.134)
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // Set the compare register (OCR1A).
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = 2000;    // 16e6 / 8000
  
  // Enable interrupt when TCNT1 == OCR1A (p.136)
  TIMSK1 |= _BV(OCIE1A);
  sei();
  
  // LED pin
  pinMode(13,OUTPUT);
  
  // input #1
  pinMode(4,INPUT);
  digitalWrite(4,HIGH);
  // input #2
  pinMode(5,INPUT);
  digitalWrite(5,HIGH);
  // input #3
  pinMode(6,INPUT);
  digitalWrite(6,HIGH);
  // input #4
  pinMode(7,INPUT);
  digitalWrite(7,HIGH);
}

//static unsigned char get_random_pad()
//{
//    // add the timer's value to the random seed
//    if (g_random == 0) {
//        g_random = timerValue;
//        randomSeed(g_random);
//    }
//    
//    return random(4);
//}

static uint8_t get_button_status()
{
  return (digitalRead(4)==LOW ? 1 : 0) +
    (digitalRead(5)==LOW ? 2 : 0) +
    (digitalRead(6)==LOW ? 4 : 0) +
    (digitalRead(7)==LOW ? 8 : 0);
}

static void choose_sound()
{
  uint8_t new_status = get_button_status();
  if (current_status == new_status) return;
  
  // something has changed
  current_status = new_status;
  
  // see if the current button is switched off
  if (((1<<current_button) & new_status) == 0) {
    current_button = 0xFF;
  }
  
  // see if a new button is pressed
  if ((current_button == 0xFF) && (new_status != 0)) {
    // a new button is pressed. see which one it is
    current_button = 0;
    while (((1<<current_button) & new_status) == 0) {
      current_button++;
    }
    
    // now set the playback to the matching buffer
    buffer = (unsigned char*)buffers[current_button];
    buffer_len = buffer_sizes[current_button];
    loc = 0;
  }
}

void loop()
{
  //Serial.println(current_status);
  //Serial.println(current_button);
}

// Timer1 Interrupt Service at 16,000Hz.
// Since the audio is changed only every two interrupts, we get a playback at 8000Hz
ISR(TIMER1_COMPA_vect)
{
  divi = ((divi+1) & 0x0F);
  
  // sample the buttons in a frequency of 500Hz, i.e. every 16 interrupts
  if (((divi & 0x01) == 0x01) &&
      (buffer != (unsigned char*)0xFFFF) && (buffer_len != 0)) {
    OCR2A = pgm_read_byte(&buffer[loc]);
    loc++;
    if (loc >= buffer_len) {
      loc = 0;
      buffer_len = 0;
      buffer = (unsigned char*)0xFFFF;
    }
  }
  else if (divi == 0) {
    choose_sound();
  }
}
