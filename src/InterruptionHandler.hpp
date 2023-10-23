#define DEBOUNCE_TIME 100 // in ms

/*
 * Interruptions variables
*/
extern unsigned long greenStart;
extern unsigned long redStart;
extern bool greenFlag;
extern bool redFlag;

void initializeInterruptions();
void greenInterrupt();
void redInterrupt();
float getColor(byte mode);