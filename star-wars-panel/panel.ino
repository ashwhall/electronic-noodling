#define BTN_YELLOW 5
#define BTN_BLUE 4
#define BTN_RED 3
#define BTN_WHITE 2

#define LED_RED1 7
#define LED_RED2 6
#define LED_WHITE1 8
#define LED_WHITE2 9

#define SEV_SEG_PIN1 11
#define SEV_SEG_PIN2 10
#define SEV_SEG_PIN4 A0
#define SEV_SEG_PIN5 A1
#define SEV_SEG_PIN6 A2
#define SEV_SEG_PIN7 A3
#define SEV_SEG_PIN9 A4
#define SEV_SEG_PIN10 A5

class SegmentDisplay
{
public:
  int pins[8];
  SegmentDisplay(int pin1, int pin2, int pin4, int pin5, int pin6, int pin7, int pin9, int pin10);
  void displayHex(int number, boolean decimalPointFlag);
  void displayRawSegments(byte pattern);
  void displayDecimalPoint();
  void testDisplay();
};

SegmentDisplay segDisplay(SEV_SEG_PIN1, SEV_SEG_PIN2, SEV_SEG_PIN4, SEV_SEG_PIN5, SEV_SEG_PIN6, SEV_SEG_PIN7, SEV_SEG_PIN9, SEV_SEG_PIN10);

#define NUM_PAIRS 4
#define DEBOUNCE_MS 50
#define LED_ON_MS 1000

const int btnPins[NUM_PAIRS] = {BTN_YELLOW, BTN_BLUE, BTN_RED, BTN_WHITE};
const int ledPins[NUM_PAIRS] = {LED_RED1, LED_RED2, LED_WHITE1, LED_WHITE2};

bool lastBtnState[NUM_PAIRS];
bool stableBtnState[NUM_PAIRS];
unsigned long lastDebounceTime[NUM_PAIRS];
unsigned long ledOffTime[NUM_PAIRS];

// Software PWM for LED brightness
#define PWM_PERIOD_US 1000
#define PWM_DUTY_RED 10  // percentage (0-100) — for red LEDs (indices 0,1)
#define PWM_DUTY_WHITE 1 // percentage (0-100) — for white LEDs (indices 2,3)
bool ledDesiredState[NUM_PAIRS];

// Sequence input system
#define MAX_SEQUENCE_LEN 3
#define SEQUENCE_TIMEOUT_MS 1000
#define IDLE_TIMEOUT_MS 5000
#define IDLE_ANIM_MIN_MS 1000
#define IDLE_ANIM_MAX_MS 3000

int sequenceBuffer[MAX_SEQUENCE_LEN];
int sequenceLen = 0;
unsigned long lastSequencePress = 0;
unsigned long lastActivityTime = 0;
unsigned long lastIdleAnimTime = 0;
unsigned long nextIdleAnimDelay = 0;
bool inIdleMode = false;

// Alien character patterns for idle animation
const byte alienChars[] = {
    B00111010, // Partial segments
    B10100011, // Scattered pattern
    B01010101, // Alternating
    B00110110, // Angular
    B11000101, // Asymmetric
    B01110010, // Cross-like
    B10010110, // L-shaped variant
    B00101110, // Inverted T
    B11010001, // Corner emphasis
    B01101010  // Diagonal-ish
};

// Special character markers for text display
#define CHAR_DOT 0xFE
#define CHAR_UNDERSCORE B11111101
#define LETTER_DELAY_MS 500
#define BETWEEN_DELAY_MS 100

bool readDebouncedPress(int index)
{
  bool reading = digitalRead(btnPins[index]) == LOW;
  if (reading != lastBtnState[index])
  {
    lastDebounceTime[index] = millis();
  }
  lastBtnState[index] = reading;

  if (millis() - lastDebounceTime[index] >= DEBOUNCE_MS)
  {
    if (reading != stableBtnState[index])
    {
      stableBtnState[index] = reading;
      if (reading)
      {
        return true;
      }
    }
  }
  return false;
}

void setLed(int index, bool on)
{
  ledDesiredState[index] = on;
  if (!on)
  {
    digitalWrite(ledPins[index], LOW);
  }
}

void pwmCycle()
{
  unsigned long onTimeRed = PWM_PERIOD_US * PWM_DUTY_RED / 100;
  unsigned long onTimeWhite = PWM_PERIOD_US * PWM_DUTY_WHITE / 100;
  unsigned long minOn = min(onTimeRed, onTimeWhite);
  unsigned long maxOn = max(onTimeRed, onTimeWhite);
  unsigned long offTime = PWM_PERIOD_US - maxOn;

  // ON phase - all LEDs on
  for (int i = 0; i < NUM_PAIRS; i++)
  {
    if (ledDesiredState[i])
      digitalWrite(ledPins[i], HIGH);
  }
  delayMicroseconds(minOn);

  // Turn off shorter-duty LEDs (white)
  for (int i = 2; i < NUM_PAIRS; i++)
  {
    if (ledDesiredState[i])
      digitalWrite(ledPins[i], LOW);
  }
  delayMicroseconds(maxOn - minOn);

  // Turn off longer-duty LEDs (red)
  for (int i = 0; i < 2; i++)
  {
    if (ledDesiredState[i])
      digitalWrite(ledPins[i], LOW);
  }
  delayMicroseconds(offTime);
}

void pwmDelay(unsigned long ms)
{
  unsigned long start = millis();
  while (millis() - start < ms)
  {
    pwmCycle();
  }
}

void turnOnLed(int index)
{
  setLed(index, true);
  ledOffTime[index] = millis() + LED_ON_MS;
}

void updateLedTimeout(int index)
{
  if (ledOffTime[index] != 0 && millis() >= ledOffTime[index])
  {
    setLed(index, false);
    ledOffTime[index] = 0;
  }
}

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(A6)); // Seed random for idle animation

  for (int i = 0; i < NUM_PAIRS; i++)
  {
    pinMode(btnPins[i], INPUT_PULLUP);

    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
    lastBtnState[i] = false;
    stableBtnState[i] = false;
    lastDebounceTime[i] = 0;
    ledOffTime[i] = 0;
    ledDesiredState[i] = false;
  }

  segDisplay.testDisplay();
  for (int i = 0; i < NUM_PAIRS; i++)
  {
    setLed(i, true);
    pwmDelay(100);
    setLed(i, false);
  }
  clearSegDisplay();
}

// Mode implementations
void modeChase()
{
  // Single LED chases R1→R2→W2→W1 loop for 3s
  // Single segment chases around outer perimeter (E→D→C→B→A→F)
  const byte segmentRing[] = {
      B11111110, // E (bottom left, bit 0)
      B11111101, // D (bottom, bit 1)
      B11111011, // C (bottom right, bit 2)
      B11101111, // B (top right, bit 4)
      B11011111, // A (top, bit 5)
      B10111111  // F (top left, bit 6)
  };

  unsigned long startTime = millis();
  int step = 0;

  while (millis() - startTime < 3000)
  {
    int ledIndex = step % NUM_PAIRS;
    int segIndex = step % 6;

    allLedsOff();
    setLed(ledIndex, true);
    segDisplay.displayRawSegments(segmentRing[segIndex]);

    pwmDelay(150);
    step++;
  }

  restoreNormalState();
}

void modeCrescendo()
{
  // LEDs flash with decreasing intervals (crescendo)
  // 7-seg flashes all segments along with the LEDs
  const byte fullPerimeter = B00000000; // All segments on

  int interval = 500;
  int minInterval = 1;
  int decrement = 49;

  while (interval >= minInterval)
  {
    allLedsOn();
    segDisplay.displayRawSegments(fullPerimeter);
    pwmDelay(interval / 2);
    allLedsOff();
    clearSegDisplay();
    pwmDelay(interval / 2);

    interval -= decrement;
  }

  restoreNormalState();
}

void modeBinaryCount()
{
  // 4 LEDs show binary 0000→1111
  // 7-seg shows hex 0→F
  for (int i = 0; i < 16; i++)
  {
    for (int led = 0; led < NUM_PAIRS; led++)
    {
      setLed(led, (i & (1 << led)) != 0);
    }
    segDisplay.displayHex(i, false);
    pwmDelay(250);
  }

  restoreNormalState();
}

void modeBoobs()
{
  // Shows "I_LOvE_BOOBS"
  const byte letters[] = {
      B11101011,       // I (same as 1)
      CHAR_UNDERSCORE, // _
      B10111100,       // L
      B10001000,       // O
      B11111000,       // v (bottom left + bottom + bottom right)
      B00011100,       // E
      CHAR_UNDERSCORE, // _
      B00111000,       // B
      B10001000,       // O
      B10001000,       // O
      B00111000,       // B
      B00011001        // S
  };
  const int numLetters = 12;

  allLedsOff();
  for (int i = 0; i < numLetters; i++)
  {
    if (letters[i] == CHAR_DOT)
    {
      segDisplay.displayDecimalPoint();
    }
    else
    {
      segDisplay.displayRawSegments(letters[i]);
    }
    pwmDelay(LETTER_DELAY_MS);
    clearSegDisplay();
    pwmDelay(BETWEEN_DELAY_MS);
  }

  restoreNormalState();
}

void modeCascade()
{
  // LEDs turn on one-by-one, hold, then off one-by-one
  // Segments fill one by one similarly
  const byte segmentCascade[] = {
      B11111110, // A
      B11111100, // A+B
      B11111000, // A+B+C
      B11110000, // A+B+C+D
      B11100000, // A+B+C+D+E
      B11000000, // A+B+C+D+E+F
      B10000000, // A+B+C+D+E+F+G
      B00000000  // All segments
  };

  // Turn on LEDs one by one
  for (int i = 0; i < NUM_PAIRS; i++)
  {
    setLed(i, true);
    segDisplay.displayRawSegments(segmentCascade[i * 2]);
    pwmDelay(300);
  }

  // Hold
  segDisplay.displayRawSegments(segmentCascade[7]);
  pwmDelay(500);

  // Turn off LEDs one by one
  for (int i = NUM_PAIRS - 1; i >= 0; i--)
  {
    setLed(i, false);
    segDisplay.displayRawSegments(segmentCascade[i * 2]);
    pwmDelay(300);
  }

  restoreNormalState();
}

void modeOpen()
{
  // Spin border segments, light white LEDs, display "AIrLOCk_OPEn", cascade off
  const byte letters[] = {
      B00001010,       // A
      B11101011,       // I
      B01111110,       // r
      B10111100,       // L
      B10001000,       // O
      B10011100,       // C
      B00111010,       // k
      CHAR_UNDERSCORE, // _
      B10001000,       // O
      B00001110,       // P
      B00011100,       // E
      B01111010        // n
  };
  const int numLetters = 12;

  // Spin border segments rapidly
  const byte segmentRing[] = {
      B11011111, // A (top)
      B11101111, // B (top right)
      B11111011, // C (bottom right)
      B11111101, // D (bottom)
      B11111110, // E (bottom left)
      B10111111  // F (top left)
  };
  for (int spin = 0; spin < 24; spin++)
  {
    segDisplay.displayRawSegments(segmentRing[spin % 6]);
    pwmDelay(50);
  }
  clearSegDisplay();

  // Light up white LEDs only
  setLed(2, true);
  setLed(3, true);

  // Display "AIrLOCk_OPEn"
  for (int i = 0; i < numLetters; i++)
  {
    if (letters[i] == CHAR_DOT)
    {
      segDisplay.displayDecimalPoint();
    }
    else
    {
      segDisplay.displayRawSegments(letters[i]);
    }
    pwmDelay(LETTER_DELAY_MS);
    clearSegDisplay();
    pwmDelay(BETWEEN_DELAY_MS);
  }
  clearSegDisplay();

  // Turn off LEDs one by one
  for (int i = NUM_PAIRS - 1; i >= 0; i--)
  {
    setLed(i, false);
    pwmDelay(100);
  }

  restoreNormalState();
}

// Mode dispatch system
typedef void (*ModeFunction)();

struct SequenceMode
{
  int sequence[MAX_SEQUENCE_LEN];
  int length;
  ModeFunction mode;
};

const SequenceMode modes[] = {
    {{0}, 1, modeChase},       // Y+W
    {{1}, 1, modeCrescendo},   // B+W
    {{2}, 1, modeBinaryCount}, // R+W
    {{0, 2}, 2, modeBoobs},    // Y,R+W
    {{0, 1}, 2, modeCascade},  // Y,B+W
    {{0, 1, 2}, 3, modeOpen}   // Y,B,R+W
};

const int numModes = sizeof(modes) / sizeof(SequenceMode);

bool matchSequence(const int *seq1, const int *seq2, int len)
{
  for (int i = 0; i < len; i++)
  {
    if (seq1[i] != seq2[i])
    {
      return false;
    }
  }
  return true;
}

void dispatchMode()
{
  for (int i = 0; i < numModes; i++)
  {
    if (modes[i].length == sequenceLen && matchSequence(sequenceBuffer, modes[i].sequence, sequenceLen))
    {
      modes[i].mode();
      return;
    }
  }

  // No match found - flash invalid
  flashAllLeds(3, 100, 100);
  allLedsOff();
}

void clearSequence()
{
  sequenceLen = 0;
  lastSequencePress = 0;
}

void onYellowPressed()
{
  turnOnLed(0);
  lastActivityTime = millis();
  inIdleMode = false;

  // Add to sequence buffer
  if (sequenceLen < MAX_SEQUENCE_LEN)
  {
    sequenceBuffer[sequenceLen++] = 0;
    lastSequencePress = millis();
  }
}

void onBluePressed()
{
  turnOnLed(1);
  lastActivityTime = millis();
  inIdleMode = false;

  // Add to sequence buffer
  if (sequenceLen < MAX_SEQUENCE_LEN)
  {
    sequenceBuffer[sequenceLen++] = 1;
    lastSequencePress = millis();
  }
}

void onRedPressed()
{
  turnOnLed(2);
  lastActivityTime = millis();
  inIdleMode = false;

  // Add to sequence buffer
  if (sequenceLen < MAX_SEQUENCE_LEN)
  {
    sequenceBuffer[sequenceLen++] = 2;
    lastSequencePress = millis();
  }
}

void onWhitePressed()
{
  turnOnLed(3);
  lastActivityTime = millis();
  inIdleMode = false;

  // Finalize sequence and dispatch mode
  if (sequenceLen > 0)
  {
    dispatchMode();
    clearSequence();
  }
  else
  {
    // White pressed without sequence - invalid
    flashAllLeds(3, 100, 100);
    allLedsOff();
  }
}

typedef void (*ButtonHandler)();
const ButtonHandler onPressed[NUM_PAIRS] = {onYellowPressed, onBluePressed, onRedPressed, onWhitePressed};

// Helper functions
void allLedsOn()
{
  for (int i = 0; i < NUM_PAIRS; i++)
  {
    setLed(i, true);
  }
}

void allLedsOff()
{
  for (int i = 0; i < NUM_PAIRS; i++)
  {
    setLed(i, false);
    ledOffTime[i] = 0;
  }
}

void flashAllLeds(int times, int onMs, int offMs)
{
  for (int i = 0; i < times; i++)
  {
    allLedsOn();
    pwmDelay(onMs);
    allLedsOff();
    if (i < times - 1)
    {
      pwmDelay(offMs);
    }
  }
}

void clearSegDisplay()
{
  segDisplay.displayRawSegments(B11111111);
}

void restoreNormalState()
{
  allLedsOff();
  clearSegDisplay();
  inIdleMode = false;
}

void loop()
{
  unsigned long now = millis();

  // Check for sequence timeout
  if (sequenceLen > 0 && lastSequencePress > 0 && (now - lastSequencePress) > SEQUENCE_TIMEOUT_MS)
  {
    flashAllLeds(3, 100, 100);
    clearSequence();
    allLedsOff();
  }

  // Handle idle animation
  if (!inIdleMode && (lastActivityTime == 0 || (now - lastActivityTime) > IDLE_TIMEOUT_MS))
  {
    inIdleMode = true;
    lastIdleAnimTime = now;
    nextIdleAnimDelay = random(IDLE_ANIM_MIN_MS, IDLE_ANIM_MAX_MS + 1);
  }

  if (inIdleMode && (now - lastIdleAnimTime) > nextIdleAnimDelay)
  {
    int randomIndex = random(0, sizeof(alienChars) / sizeof(alienChars[0]));
    segDisplay.displayRawSegments(alienChars[randomIndex]);
    lastIdleAnimTime = now;
    nextIdleAnimDelay = random(IDLE_ANIM_MIN_MS, IDLE_ANIM_MAX_MS + 1);
  }

  // Button handling
  for (int i = 0; i < NUM_PAIRS; i++)
  {
    if (readDebouncedPress(i))
    {
      onPressed[i]();
    }
    updateLedTimeout(i);
  }

  // Software PWM for LED brightness
  pwmCycle();
}

SegmentDisplay::SegmentDisplay(int pin1, int pin2, int pin4, int pin5, int pin6, int pin7, int pin9, int pin10)
{

  pins[0] = pin1;
  pins[1] = pin2;
  pins[2] = pin4;
  pins[3] = pin5;
  pins[4] = pin6;
  pins[5] = pin7;
  pins[6] = pin9;
  pins[7] = pin10;

  for (int i = 0; i < 8; i++)
  {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], HIGH);
  }
}

void SegmentDisplay::displayHex(int number, boolean decimalPointFlag)
{

  byte numbersToDisplay[] = {
      B10001000, //  0
      B11101011, //  1
      B01001100, //  2
      B01001001, //  3
      B00101011, //  4
      B00011001, //  5
      B00011000, //  6
      B11001011, //  7
      B00001000, //  8
      B00001011, //  9
      B00001010, //  A
      B00111000, //  B
      B10011100, //  C
      B01101000, //  D
      B00011100, //  E
      B00011110, //  F
      B01011101  //  Error

  };

  boolean bitToWrite;

  for (int segment = 0; segment < 8; segment++)
  {
    if (number < 0 || number > 15)
    {
      bitToWrite = bitRead(numbersToDisplay[16], segment);
    }
    else
    {
      bitToWrite = bitRead(numbersToDisplay[number], segment);
    }

    // Always keep decimal point (segment 3) off
    if (segment == 3)
    {
      bitToWrite = 1;
    }

    digitalWrite(pins[segment], bitToWrite);
  }
}

void SegmentDisplay::displayRawSegments(byte pattern)
{
  for (int segment = 0; segment < 8; segment++)
  {
    boolean bitToWrite = bitRead(pattern, segment);
    // Always keep decimal point (segment 3) off
    if (segment == 3)
    {
      bitToWrite = 1;
    }
    digitalWrite(pins[segment], bitToWrite);
  }
}

void SegmentDisplay::displayDecimalPoint()
{
  for (int i = 0; i < 8; i++)
  {
    if (i == 3)
    {
      digitalWrite(pins[i], 0);
    }
    else
    {
      digitalWrite(pins[i], 1);
    }
  }
}

void SegmentDisplay::testDisplay()
{
  for (int i = 0; i <= 15; i++)
  {
    displayHex(i, false);
    pwmDelay(100);
  }
}
