#define DEBOUNCE_TIME 100UL // in ms

/*
 * Interruptions variables (MUST BE VOLATILE)
*/
extern volatile unsigned long greenStart;
extern volatile unsigned long redStart;
extern volatile bool greenFlag;
extern volatile bool redFlag;

void initializeInterruptions();
void greenInterrupt();
void redInterrupt();
float getColor(byte mode);