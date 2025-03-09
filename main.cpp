// header files 
#include "LCD_DISCO_F429ZI.h"
#include "fonts.h"
#include "mbed.h"
#include <time.h>
#include <cstdio>
#include <cstring>
#include "DebouncedInterrupt.h"


// Global constants 
#define SDA_PIN PC_9
#define SCL_PIN PA_8
#define EEPROM_ADDR 0xA0

// LCD and i2c pin that communicates with the EEPROM chip
// communication protocol used to connect the chip and the controller using two wires
// the SDA (serial data line) - transfers data 
// the SCL (serial clock line) - synchronizes the data transfer
LCD_DISCO_F429ZI LCD;
I2C i2c(SDA_PIN, SCL_PIN); 

// Buttons
// User Button will log the time 
// Set button cycles through the time-set mode
// Exit button will toggle the view of the log
// Increment button will incremeent the desired field
InterruptIn user_button(BUTTON1);  
DebouncedInterrupt external_button(PA_5);   
DebouncedInterrupt set_button(PA_6);   
DebouncedInterrupt increment_button(PA_7);

// states of the finate state machine 
enum State { IDLE_MODE_STATE, VIEW_LOGS_STATE, SET_TIME_STATE };
State system_state = IDLE_MODE_STATE;

// Global variables used by the set-time state
// the selected field is as follows (0-5) = (hour, minute, second, day, month, year)
tm new_time;
int current_field = 0;  
bool in_set_time_state = false;

// Flags set by ISRs (Intrrupt service routine) - initialized to false 
// volatile ensures the main loop sees the latest value of the variable when changed within an ISR
volatile bool log_time_requested = false;
volatile bool view_logs_requested = false;
volatile bool set_time_mode_requested = false;
volatile bool increment_time_requested = false;

// Protototypes of all functions used 
void WriteEEPROM(int address, unsigned int eeaddress, char *data, int size);
void ReadEEPROM(int address, unsigned int eeaddress, char *data, int size);
void logTimeToEEPROM();
void viewLogs();
void setTimeMode();
void incrementTime();
void displayCurrentTime();

// Interrupt service routines 

// When user button is pressed, log time requested will be true
void logTimeISR() {
    log_time_requested = true;
}

// When external button is pressed, log view is requested
void viewLogsISR() {
    view_logs_requested = true;
}

// When set button is presseed, cycles through the time setting-mode 
void setTimeModeISR() {
    set_time_mode_requested = true;
}

// When increment button is pressed, Incremeents the desired time field 
void incTimeISR() {
    increment_time_requested = true;
}


int main() {
    // LCD initialization of font and colour 
    LCD.SetFont(&Font12);
    LCD.SetTextColor(LCD_COLOR_BLACK);


    // Initialize the real time clock (Jan 1st 2025 00:00:00)
    tm t;
    t.tm_year = 125;  // 1900 (C standard)
    t.tm_mon  = 0;
    t.tm_mday = 1;
    t.tm_hour = 0;
    t.tm_min  = 0;
    t.tm_sec  = 0;

    // Set the RTC to the time above 
    set_time(mktime(&t));


    user_button.fall(&logTimeISR); // User button calls log time ISR when pressed
    external_button.attach(&viewLogsISR, IRQ_FALL, 10, true); // External button calls View logs when pressed
    set_button.attach(&setTimeModeISR, IRQ_FALL, 10, true); // Set button calls sete time mode when pressed
    increment_button.attach(&incTimeISR, IRQ_FALL, 10, true); // Increment button calls inrement time when pressed

    while (1) {
        
        // When User button is pressed this if statement will run
        if (log_time_requested) {
            logTimeToEEPROM(); // writes current time to chip
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.DisplayStringAt(0, 0, (uint8_t *)"Time Logged", CENTER_MODE);
            thread_sleep_for(500); // TODO: for debugging, can be removed after
            log_time_requested = false;
        }

        // When External Button is pressed this if statement will run 
        if (view_logs_requested) {
            // Check if we are already viewing logs if so -> IDLEn otherwise view the logs
            if (system_state == VIEW_LOGS_STATE) {
                system_state = IDLE_MODE_STATE;
                LCD.Clear(LCD_COLOR_WHITE);
            } else {
                viewLogs();
            }
            view_logs_requested = false;
        }

        // When set button is pressed this if statement will run (calls set time function)
        if (set_time_mode_requested) {
            setTimeMode();
            set_time_mode_requested = false;
        }

        // When increment button is pressed this if statement will run (calls increment function)
        if (increment_time_requested) {
            incrementTime();
            increment_time_requested = false;
        }

        // If system is in IDLE state display the current time on LCD
        if (system_state == IDLE_MODE_STATE) {
            displayCurrentTime();
        }

       thread_sleep_for(1000); 
    }
}

// Displays the current time on LCD in IDLE state 
void displayCurrentTime() {
    time_t rawtime;
    time(&rawtime);
    char timebuf[30];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", localtime(&rawtime));
    LCD.Clear(LCD_COLOR_WHITE);
    
    LCD.DisplayStringAt(0, 0, (uint8_t *)"Time:", CENTER_MODE);
    LCD.DisplayStringAt(0, 12, (uint8_t *)timebuf, CENTER_MODE);
}

// Writes data to chip 
void WriteEEPROM(int address, unsigned int eeaddress, char *data, int size) {
    char i2cBuffer[size + 2]; // 2 extra bytes needed for chip address
    i2cBuffer[0] = (unsigned char)(eeaddress >> 8);  // Most significant byte
    i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF);  // Least significant byte (mask upper 8 bits)
  
    // iterate through buffer copying data array values to the buffer
    for (int i = 0; i < size; i++) {
        i2cBuffer[i + 2] = data[i]; // start after address bytes 
    }

    // write to chip
    int result = i2c.write(address, i2cBuffer, size + 2, false);
    thread_sleep_for(10); //  ensure chip write completes.
}

// Read data from chip memory 
void ReadEEPROM(int address, unsigned int eeaddress, char *data, int size) {
    char i2cBuffer[2]; // address
    i2cBuffer[0] = (unsigned char)(eeaddress >> 8);  
    i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF);  

    // write the address to the chip telling it where to begin reading from 
    int result = i2c.write(address, i2cBuffer, 2, false);

    // Ensure suffidient time for the controller to read from the chip
    thread_sleep_for(10);
    i2c.read(address, data, size);
    thread_sleep_for(10);
}

// Function logs the current time to the chip
void logTimeToEEPROM() {
    // current time 
    time_t rawtime;
    time(&rawtime);
    char timebuf[10];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", localtime(&rawtime));

    // Initialize an "empty" array of size 10
    char oldTime[10] = {0};

    // read the first 10 bytes from the chip starting at the 10th byte and store it in oldTime
    ReadEEPROM(EEPROM_ADDR, 10, oldTime, 10);
    // write the old time to the first 10 bytes of the chip 
    WriteEEPROM(EEPROM_ADDR, 0, oldTime, 10);

    // write the new time (current time) to the chip at the 10th byte
    WriteEEPROM(EEPROM_ADDR, 10, timebuf, 10);
}

// Prints the data stored in the chip to the LCD
void viewLogs() {
    //last time 1 will store old time, last time 2 will store the current time
    char lastTime1[10] = {0}, lastTime2[10] = {0};

    // read those two times stored in the chip (last time 1 - byte 0-10, last time 2 = byte 10-20)
    ReadEEPROM(EEPROM_ADDR, 0, lastTime1, 10);
    ReadEEPROM(EEPROM_ADDR, 10, lastTime2, 10);

    // format the lCD
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.SetFont(&Font12); 
    LCD.DisplayStringAt(0, 0, (uint8_t *)"Log Times:", CENTER_MODE);
    
    // format of "old" time
    LCD.DisplayStringAt(0, 12, (uint8_t *)"Old:", LEFT_MODE);
    LCD.DisplayStringAt(0, 12, (uint8_t *)lastTime1, CENTER_MODE);

    // Format of "new" time
    LCD.DisplayStringAt(0, 24, (uint8_t *)"New:", LEFT_MODE);
    LCD.DisplayStringAt(0, 24, (uint8_t *)lastTime2, CENTER_MODE);
    
    // switch state to log view 
    system_state = VIEW_LOGS_STATE;
}


// Function will run when the Set button is pressed
void setTimeMode() {
    // If we are not in the set time state, then enter the state
    if (!in_set_time_state) {
         in_set_time_state = true;
         system_state = SET_TIME_STATE;
         current_field = 0; // hour field 
         time_t rawtime;
         time(&rawtime);
         new_time = *localtime(&rawtime); // fetched the current time 
    } else {
        // if we were already in the set time state do the following
         if (current_field == 5) { // if we are in the year field
            // exit the state, return to IDLE
            in_set_time_state = false;
            system_state = IDLE_MODE_STATE;

            // display the time, and set the current time to the new time
            set_time(mktime(&new_time));
            displayCurrentTime();
            return;
         } else { // otherwise, iterate through all the fields 
            current_field++;
         }
    }
    
    // timebuf array will store the current time in yyyy-mm-dd  and hh-mm0ss format
    char timebuf[40];
    sprintf(timebuf, "%04d-%02d-%02d %02d:%02d:%02d", 
            new_time.tm_year + 1900, new_time.tm_mon + 1, new_time.tm_mday,
            new_time.tm_hour, new_time.tm_min, new_time.tm_sec);
    
    // clear the LCD and the state is displayed at the top while the current time is placed in the center
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.DisplayStringAt(0, 0, (uint8_t *)"Set Time Mode", CENTER_MODE);
    LCD.DisplayStringAt(0, 10, (uint8_t *)timebuf, CENTER_MODE);

    // field label will hold the current selected time field, and is displayed on the LCD
    char fieldLabel[30];
    switch(current_field) {
        case 0: strcpy(fieldLabel, "Hour"); break;
        case 1: strcpy(fieldLabel, "Minute"); break;
        case 2: strcpy(fieldLabel, "Second"); break;
        case 3: strcpy(fieldLabel, "Day"); break;
        case 4: strcpy(fieldLabel, "Month"); break;
        case 5: strcpy(fieldLabel, "Year (SET exit)"); break;
        default: strcpy(fieldLabel, "Unknown"); break;
    }
    // the field label is then displayed (whichever is being edited)
    LCD.DisplayStringAt(0, 22, (uint8_t *)fieldLabel, CENTER_MODE);
}

// runs when the increment button is pressed, in the set time state
void incrementTime() {
    if (!in_set_time_state) return; // if we are not in set time state exit the function
    
    // increment the selected field 
    switch(current_field) {
         case 0: new_time.tm_hour = (new_time.tm_hour + 1) % 24; break;
         case 1: new_time.tm_min = (new_time.tm_min + 1) % 60; break;
         case 2: new_time.tm_sec = (new_time.tm_sec + 1) % 60; break;
         case 3: new_time.tm_mday = (new_time.tm_mday % 31) + 1; break;
         case 4: new_time.tm_mon = (new_time.tm_mon + 1) % 12; break;
         case 5: new_time.tm_year += 1; break;
    }
    
    set_time(mktime(&new_time)); // set the system time to new time
    
    // display on the LCD (time)
    char timebuf[40];
    sprintf(timebuf, "%04d-%02d-%02d %02d:%02d:%02d", 
            new_time.tm_year + 1900, new_time.tm_mon + 1, new_time.tm_mday,
            new_time.tm_hour, new_time.tm_min, new_time.tm_sec);
    LCD.Clear(LCD_COLOR_WHITE);
    LCD.DisplayStringAt(0, 0, (uint8_t *)"Set Time Mode", CENTER_MODE);
    LCD.DisplayStringAt(0, 10, (uint8_t *)timebuf, CENTER_MODE);

    // display field on the LCD
    char fieldLabel[30];
    switch(current_field) {
        case 0: strcpy(fieldLabel, "Hour"); break;
        case 1: strcpy(fieldLabel, "Minute"); break;
        case 2: strcpy(fieldLabel, "Second"); break;
        case 3: strcpy(fieldLabel, "Day"); break;
        case 4: strcpy(fieldLabel, "Month"); break;
        case 5: strcpy(fieldLabel, "Year (SET exit)"); break;
        default: strcpy(fieldLabel, "Unknown"); break;
    }
    LCD.DisplayStringAt(0, 22, (uint8_t *)fieldLabel, CENTER_MODE);
} 
