
typedef struct {
  char ch;
  int bits;
} morse_t;

const morse_t MorseCodes[] = {
  {'A', B101},
  {'B', B11000},
  {'C', B11010},
  {'D', B1100},
  {'E', B10},
  {'F', B10010},
  {'G', B1110},
  {'H', B10000},
  {'I', B100},
  {'J', B10111},
  {'K', B1101},
  {'L', B10100},
  {'M', B111},
  {'N', B110},
  {'O', B1111},
  {'P', B10110},
  {'Q', B11101},
  {'R', B1010},
  {'S', B1000},
  {'T', B11},
  {'U', B1001},
  {'V', B10001},
  {'W', B1011},
  {'X', B11001},
  {'Y', B11011},
  {'Z', B11100},

  {'1', B101111},
  {'2', B100111},
  {'3', B100011},
  {'4', B100001},
  {'5', B100000},
  {'6', B110000},
  {'7', B111000},
  {'8', B111100},
  {'9', B111110},
  {'0', B111111},

//  {'"', B10101},
//  {'\u00c6', B10101},
  {0xc6, B10101},

  //{'Ä', B10101},
  {'Ø', B11110},
  //{'Ö', B11110},
  {'Å', B101101},
  {'Ü', B10011},

  {'?', B1001100},
  // {'|', B1001100},

  //http://riddlehelper.blogspot.com/2011/04/morse-code.html
  {'.', B1010101},
  {',', B1110011},
  {':', B1111000},
  {'-', B110001},
  {'+', B101010},
  // {'=', B10001} = V
  // {'*', B11001} = X
  {'\'',B1011110},
  {'/', B110010},
  {'(', B110110},
  {')', B1101101},
  {'w', B101000},   //Wait
  {'s', B110101},   //Start of transmision
  {'e', 0x0100},    // =B100000000},//Error
  {0,   0}
};

char decode() {
  int bits = 1;
  char* p = charDecoder;
  while (char ch = *p++) {
    if (ch == '1') bits = (bits << 1);
    if (ch == '2') bits = (bits << 1) + 1;
  }
  for (byte n=0; char ch = MorseCodes[n].ch; n++) {
    if (bits == MorseCodes[n].bits) {
      //Serial.print("\u00"); 
      //Serial.print(ch,HEX);
      //Serial.print(">>> B"); Serial.print(bits,BIN); Serial.print(" -> "); Serial.println(ch);
      //display.print(">>> "); display.print(bits,BIN); display.print(" -> "); display.println(ch);
      return ch;
    }
  }
  Serial.print(">>> ??? "); Serial.println(bits,BIN); 
  return ' ';
}

//Reverse bits LSB <=> MSB
int reverseBits(int bits) {
  int bits2 = 1;
  while(bits>1) { bits2 = bits2*2 + bits % 2; bits/=2; }  
  return bits2;  
}
byte cwBits(char ch) {
  for (byte n=0; int bits = MorseCodes[n].bits; n++) {
    if (toupper(ch) == MorseCodes[n].ch) {
      int bits2 = reverseBits(bits);
      //Serial.print(ch); Serial.print(" -> "); Serial.print(bits,BIN); Serial.print(" -> "); Serial.println(bits2,BIN); 
      return bits2;
    }
  }
  return 0;
}
