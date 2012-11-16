void setupDMX(unsigned char *DMXframe){
  
  // thanks to Mike Harrison at White Wing Logic
  
  UCSR0A=0;        // normal UART behaviour
  UCSR0B=0x98;     // enable TX and RX, 8-bit bytes
  UCSR0C=0x0e;     // async UART, no parity, ___2 stop bits___
  UBRR0H=0;        // baud rate high register
  UBRR0L=0x03;     // baud rate low register, 4=250K@20M, 3 @16M  
  DDRD |= (1<<1);  // set TX pin as output for when we need to manipulate it manually later...
  
  DMXframe[0] = 0; // not strictly the UART, but this puts a start code at the beginning of the DMX frame
}

void sendDMX(unsigned char *DMXframe, int DMXframeSize)
{
  
  // thanks to Mike Harrison at White Wing Logic
  
  PORTD &= ~(1<<1);       // set TX pin LOW (break)
  UCSR0B=0x90;            // disable UART
  delayMicroseconds(100); // break for 100 us
  UCSR0B=0x98;            // re-enable UART (MAB)
  delayMicroseconds(100); // mark for 100us
      
  Serial.write(DMXframe, DMXframeSize);
}

