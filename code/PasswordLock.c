#include <lpc17xx.h> // includes the header file for the LPC1768 microcontroller

unsigned char rej[15] = {"Wrong passcode"};                                         // a string storing the message for access denied
unsigned char acc[17] = {"Access Approved"};                                        // a string storing the message for access granted
int passcode[4] = {7, 1, 8, 5};                                                     // an integer array storing the correct passcode
unsigned char scancode[9] = {0x11, 0x21, 0x41, 0x12, 0x22, 0x42, 0x14, 0x24, 0x44}; // an array storing the scancodes for each key
unsigned int input[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};                                // an array storing the input values for each key

int a, b, c, d;      // integer variables to store the digits of the passcode
int flag;            // a flag variable to indicate if a key has been pressed
int rowval;          // integer variable to store the current row value
int key;             // integer variable to store the scancode of the pressed key
int count = 0;       // integer variable to keep count of the number of digits entered
int row;             // integer variable to store the current row being scanned
int asterisk = 0x2A; // integer variable storing the ascii value for the '*' character

// function prototypes
void init(void);
void delay(int val);
void send(int val, int type);
void clear_ports(void);
void write(int val, int type);
void send_char(unsigned char *arr);
void scan(void);
void clockwise(void);
void anticlockwise(void);

int main(void)
{
    int j, rowvar;
    LPC_GPIO0->FIODIR = 0X1F8000F0; // sets the direction of pins 4-8 and 23-28 of port 0 as output
    LPC_GPIO2->FIODIR = 0X00001C00; // sets the direction of pins 10-12 of port 2 as output
    LPC_GPIO1->FIODIR = 0XFC7FFFFF; // sets the direction of pins 2-8 and 12-31 of port 1 as input
    init();                         // initializes the LCD display
    while (1)
    {                  // infinite loop to continuously check for user input
        send(0x80, 0); // sets the cursor to the beginning of the first line of the display
        delay(800);    // delay
        count=0;
        while (count < 4)
        { // loop to wait for all 4 digits of the passcode to be entered
            while (1)
            { // loop to scan for a key press
                for (row = 1; row < 4; row++)
                { // loop through each row
                    if (row == 1)
                    {
                        rowvar = 0x00000400; // sets the row variable to the value for row 1
                    }
                    if (row == 2)
                    {
                        rowvar = 0x00000800; // sets the row variable to the value for row 2
                    }
                    if (row == 3)
                    {
                        rowvar = 0x00001000; // sets the row variable to the value for row 3
                    }
                    LPC_GPIO2->FIOCLR = 0X00001C00; // clears the output pins for the rows
                    LPC_GPIO2->FIOSET = rowvar;     // sets the output pin for the current row
                    flag = 0;                       // resets the flag variable
                    scan();                         // scans for a key press
                    if (flag == 1)
                    {
                        break; // Break the loop if a key press is detected
                    }
                }
                if (flag == 1)
                {
                    break; // Break the outer loop if a key press is detected
                }
            }
            send(asterisk, 1); // Send asterisk character to the LCD to be displayed in the lcd
            if (count == 0)
            {
                a = key; // Assign the key value to variable a if count is 0
            }
            else if (count == 1)
            {
                b = key; // Assign the key value to variable b if count is 1
            }
            else if (count == 2)
            {
                c = key; // Assign the key value to variable c if count is 2
            }
            else if (count == 3)
            {
                d = key; // Assign the key value to variable d if count is 3
            }
            count++; // Increment the count variable
        }
        if (a == passcode[0] & b == passcode[1] & c == passcode[2] & d == passcode[3])
        {
            send_char(&acc[0]); // Send "Access Granted" string to the LCD
            delay(1000);        // Delay for 1000 cycles
            for (j = 0; j < 12; j++)
            {
                clockwise(); // Rotate the motor clockwise to open the door
            }
            delay(10000); // Delay for 10000 cycles before rotating the motor back
            for (j = 0; j < 12; j++)
            {
                anticlockwise(); // Rotate the motor anticlockwise
            }
        }
        else
        {
            send_char(&rej[0]); // Send "Access Denied" string to the LCD
        }
    }
}

void init(void)
{
    // Set GPIO0 pins 23-26, 27, and 28 as outputs
    LPC_GPIO0->FIODIR |= 0Xf << 23 | 1 << 27 | 1 << 28;
    clear_ports(); // Clear GPIO0 pins 23, 27, and 28
    delay(32000);  // Delay for 32000 cycles
    send(0x33, 0);
    delay(30000);  // Delay for 30000 cycles
    send(0x32, 0); // configuring to 4 bit mode
    delay(300000); // Delay for 300000 cycles
    send(0x28, 0); // function set
    delay(30000);  // Delay for 30000 cycles
    send(0x0c, 0); // Display mode
    delay(800);    // Delay for 800 cycles
    send(0x06, 0); // Entry mode
    delay(800);    // Delay for 800 cycles
    send(0x01, 0); // Display clear
    delay(10000);  // Delay for 10000 cycles
    return;
}

void delay(int val)
{
    int x;
    for (x = 0; x < val; x++) ; // Delay for val cycles
    return;
}

void send(int val, int type)
{
    int snd;
    snd = (val & 0xf0) << 19; // Prepare the high nibble of val for sending
    write(snd, type);         // Write the high nibble to LCD
    delay(1000);              // Delay for 1000 cycles
    snd = (val & 0x0f) << 23; // Prepare the low nibble of val for sending
    write(snd, type);         // Write the low nibble to LCD
    delay(1000);              // Delay for 1000 cycles
    return;
}

void clear_ports(void)
{
    LPC_GPIO0->FIOCLR = 0X0F << 23; // Clear GPIO0 pins 23-26
    LPC_GPIO0->FIOCLR = 1 << 27;    // Clear GPIO0 pin 27
    LPC_GPIO0->FIOCLR = 1 << 28;    // Clear GPIO0 pin 28
    return;
}

void write(int val, int type)
{
    clear_ports();           // Clear GPIO0 pins 23-26, 27, and 28
    LPC_GPIO0->FIOPIN = val; // Write val to GPIO0 pins
    if (type == 0)
    {
        LPC_GPIO0->FIOCLR = 1 << 27; // Clear GPIO0 pin 27 (RS pin)
    }
    else
    {
        LPC_GPIO0->FIOSET = 1 << 27; // Set GPIO0 pin 27 (RS pin)
    }
    LPC_GPIO0->FIOSET = 1 << 28; // Set GPIO0 pin 28 (Enable pin)
    delay(100);                  // Delay for 100 cycles
    LPC_GPIO0->FIOCLR = 1 << 28; // Clear GPIO0 pin 28 (Enable pin)
    return;
}

void send_char(unsigned char *arr)
{
    int i = 0;
    unsigned int temp;
    while (arr[i] != '\0')
    {
        temp = arr[i]; // Assign the current character in the array to temp
        send(temp, 1); // Call the send() function with temp as the argument to send the character to LCD
        i++;           // Move to the next character in the array
    }
}
void scan(void)
{
    unsigned long int colval;
    int temp,k;
    colval = LPC_GPIO1->FIOPIN &= 0x03800000; // Check if any key has been pressed
    if (colval != 0x00000000)
    {                        // If any of the bits in colval is not zero (i.e. a key press is detected)
        flag = 1;            // Set the flag variable to 1 to indicate a key press
        key = rowval >> 10;  // Shift the value of rowval 10 bits to the right and assign it to the key variable
        temp = colval >> 19; // Shift the value of colval 19 bits to the right and assign it to the temp variable
        key = key | temp;    // Perform a bitwise OR operation between key and temp to combine their values to get scancode;
        for(k=0;k<9;k++){
            if(scancode[k]==key){
                key=input[k]; //get corresponding input for key pressed
            }
        }

    }
    return;
}
void clockwise(void)
{
    int var1 = 0x00000008; // Initialize var1 to 0x00000008
    int i;
    for (i = 0; i < 3; i++)
    {
        var1 = var1 << 1;         // Shift the value of var1 1 bit to the left
        LPC_GPIO0->FIOPIN = var1; // Set the GPIO0 pins to the value of var1
        delay(500);               // Delay for 500 cycles
    }
}
void anticlockwise(void)
{
    int var1 = 0x00000100; // Initialize var1 to 0x00000100
    int i;
    for (i = 0; i < 3; i++)
    {
        var1 = var1 >> 1;         // Shift the value of var1 1 bit to the right
        LPC_GPIO0->FIOPIN = var1; // Set the GPIO0 pins to the value of var1
        delay(500);               // Delay for 500 cycles
    }
}
