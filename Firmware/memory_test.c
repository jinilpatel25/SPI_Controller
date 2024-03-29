#include <stdio.h>
#include <string.h>
#include <ctype.h>


//IMPORTANT
//
// Uncomment one of the two #defines below
// Define StartOfExceptionVectorTable as 08030000 if running programs from sram or
// 0B000000 for running programs from dram
//
// In your labs, you will initially start by designing a system with SRam and later move to
// Dram, so these constants will need to be changed based on the version of the system you have
// building
//
// The working 68k system SOF file posted on canvas that you can use for your pre-lab
// is based around Dram so #define accordingly before building

#define StartOfExceptionVectorTable 0x08030000
// #define StartOfExceptionVectorTable 0x0B000000

/**********************************************************************************************
**	Parallel port addresses
**********************************************************************************************/

#define PortA   *(volatile unsigned char *)(0x00400000)
#define PortB   *(volatile unsigned char *)(0x00400002)
#define PortC   *(volatile unsigned char *)(0x00400004)
#define PortD   *(volatile unsigned char *)(0x00400006)
#define PortE   *(volatile unsigned char *)(0x00400008)

/*********************************************************************************************
**	Hex 7 seg displays port addresses
*********************************************************************************************/

#define HEX_A        *(volatile unsigned char *)(0x00400010)
#define HEX_B        *(volatile unsigned char *)(0x00400012)
#define HEX_C        *(volatile unsigned char *)(0x00400014)    // de2 only
#define HEX_D        *(volatile unsigned char *)(0x00400016)    // de2 only

/**********************************************************************************************
**	LCD display port addresses
**********************************************************************************************/

#define LCDcommand   *(volatile unsigned char *)(0x00400020)
#define LCDdata      *(volatile unsigned char *)(0x00400022)

/********************************************************************************************
**	Timer Port addresses
*********************************************************************************************/

#define Timer1Data      *(volatile unsigned char *)(0x00400030)
#define Timer1Control   *(volatile unsigned char *)(0x00400032)
#define Timer1Status    *(volatile unsigned char *)(0x00400032)

#define Timer2Data      *(volatile unsigned char *)(0x00400034)
#define Timer2Control   *(volatile unsigned char *)(0x00400036)
#define Timer2Status    *(volatile unsigned char *)(0x00400036)

#define Timer3Data      *(volatile unsigned char *)(0x00400038)
#define Timer3Control   *(volatile unsigned char *)(0x0040003A)
#define Timer3Status    *(volatile unsigned char *)(0x0040003A)

#define Timer4Data      *(volatile unsigned char *)(0x0040003C)
#define Timer4Control   *(volatile unsigned char *)(0x0040003E)
#define Timer4Status    *(volatile unsigned char *)(0x0040003E)

/*********************************************************************************************
**	RS232 port addresses
*********************************************************************************************/

#define RS232_Control     *(volatile unsigned char *)(0x00400040)
#define RS232_Status      *(volatile unsigned char *)(0x00400040)
#define RS232_TxData      *(volatile unsigned char *)(0x00400042)
#define RS232_RxData      *(volatile unsigned char *)(0x00400042)
#define RS232_Baud        *(volatile unsigned char *)(0x00400044)

/*********************************************************************************************
**	PIA 1 and 2 port addresses
*********************************************************************************************/

#define PIA1_PortA_Data     *(volatile unsigned char *)(0x00400050)         // combined data and data direction register share same address
#define PIA1_PortA_Control *(volatile unsigned char *)(0x00400052)
#define PIA1_PortB_Data     *(volatile unsigned char *)(0x00400054)         // combined data and data direction register share same address
#define PIA1_PortB_Control *(volatile unsigned char *)(0x00400056)

#define PIA2_PortA_Data     *(volatile unsigned char *)(0x00400060)         // combined data and data direction register share same address
#define PIA2_PortA_Control *(volatile unsigned char *)(0x00400062)
#define PIA2_PortB_data     *(volatile unsigned char *)(0x00400064)         // combined data and data direction register share same address
#define PIA2_PortB_Control *(volatile unsigned char *)(0x00400066)


/*********************************************************************************************************************************
(( DO NOT initialise global variables here, do it main even if you want 0
(( it's a limitation of the compiler
(( YOU HAVE BEEN WARNED
*********************************************************************************************************************************/

unsigned int i, x, y, z, PortA_Count;
unsigned char Timer1Count, Timer2Count, Timer3Count, Timer4Count ;

/*******************************************************************************************
** Function Prototypes
*******************************************************************************************/
void Wait1ms(void);
void Wait3ms(void);
void Init_LCD(void) ;
void LCDOutchar(int c);
void LCDOutMess(char *theMessage);
void LCDClearln(void);
void LCDline1Message(char *theMessage);
void LCDline2Message(char *theMessage);
int sprintf(char *out, const char *format, ...) ;

/*****************************************************************************************
**	Interrupt service routine for Timers
**
**  Timers 1 - 4 share a common IRQ on the CPU  so this function uses polling to figure
**  out which timer is producing the interrupt
**
*****************************************************************************************/

void Timer_ISR()
{
   	if(Timer1Status == 1) {         // Did Timer 1 produce the Interrupt?
   	    Timer1Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
   	    PortA = Timer1Count++ ;     // increment an LED count on PortA with each tick of Timer 1
   	}

  	if(Timer2Status == 1) {         // Did Timer 2 produce the Interrupt?
   	    Timer2Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
   	    PortC = Timer2Count++ ;     // increment an LED count on PortC with each tick of Timer 2
   	}

   	if(Timer3Status == 1) {         // Did Timer 3 produce the Interrupt?
   	    Timer3Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
        HEX_A = Timer3Count++ ;     // increment a HEX count on Port HEX_A with each tick of Timer 3
   	}

   	if(Timer4Status == 1) {         // Did Timer 4 produce the Interrupt?
   	    Timer4Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
        HEX_B = Timer4Count++ ;     // increment a HEX count on HEX_B with each tick of Timer 4
   	}
}

/*****************************************************************************************
**	Interrupt service routine for ACIA. This device has it's own dedicate IRQ level
**  Add your code here to poll Status register and clear interrupt
*****************************************************************************************/

void ACIA_ISR()
{}

/***************************************************************************************
**	Interrupt service routine for PIAs 1 and 2. These devices share an IRQ level
**  Add your code here to poll Status register and clear interrupt
*****************************************************************************************/

void PIA_ISR()
{}

/***********************************************************************************
**	Interrupt service routine for Key 2 on DE1 board. Add your own response here
************************************************************************************/
void Key2PressISR()
{}

/***********************************************************************************
**	Interrupt service routine for Key 1 on DE1 board. Add your own response here
************************************************************************************/
void Key1PressISR()
{}

/************************************************************************************
**   Delay Subroutine to give the 68000 something useless to do to waste 1 mSec
************************************************************************************/
void Wait1ms(void)
{
    int  i ;
    for(i = 0; i < 1000; i ++)
        ;
}

/************************************************************************************
**  Subroutine to give the 68000 something useless to do to waste 3 mSec
**************************************************************************************/
void Wait3ms(void)
{
    int i ;
    for(i = 0; i < 3; i++)
        Wait1ms() ;
}

/*********************************************************************************************
**  Subroutine to initialise the LCD display by writing some commands to the LCD internal registers
**  Sets it for parallel port and 2 line display mode (if I recall correctly)
*********************************************************************************************/
void Init_LCD(void)
{
    LCDcommand = 0x0c ;
    Wait3ms() ;
    LCDcommand = 0x38 ;
    Wait3ms() ;
}

/*********************************************************************************************
**  Subroutine to initialise the RS232 Port by writing some commands to the internal registers
*********************************************************************************************/
void Init_RS232(void)
{
    RS232_Control = 0x15 ; //  %00010101 set up 6850 uses divide by 16 clock, set RTS low, 8 bits no parity, 1 stop bit, transmitter interrupt disabled
    RS232_Baud = 0x1 ;      // program baud rate generator 001 = 115k, 010 = 57.6k, 011 = 38.4k, 100 = 19.2, all others = 9600
}

/*********************************************************************************************************
**  Subroutine to provide a low level output function to 6850 ACIA
**  This routine provides the basic functionality to output a single character to the serial Port
**  to allow the board to communicate with HyperTerminal Program
**
**  NOTE you do not call this function directly, instead you call the normal putchar() function
**  which in turn calls _putch() below). Other functions like puts(), printf() call putchar() so will
**  call _putch() also
*********************************************************************************************************/

int _putch( int c)
{
    while((RS232_Status & (char)(0x02)) != (char)(0x02))    // wait for Tx bit in status register or 6850 serial comms chip to be '1'
        ;

    RS232_TxData = (c & (char)(0x7f));                      // write to the data register to output the character (mask off bit 8 to keep it 7 bit ASCII)
    return c ;                                              // putchar() expects the character to be returned
}

/*********************************************************************************************************
**  Subroutine to provide a low level input function to 6850 ACIA
**  This routine provides the basic functionality to input a single character from the serial Port
**  to allow the board to communicate with HyperTerminal Program Keyboard (your PC)
**
**  NOTE you do not call this function directly, instead you call the normal getchar() function
**  which in turn calls _getch() below). Other functions like gets(), scanf() call getchar() so will
**  call _getch() also
*********************************************************************************************************/
int _getch( void )
{
    char c ;
    while((RS232_Status & (char)(0x01)) != (char)(0x01))    // wait for Rx bit in 6850 serial comms chip status register to be '1'
        ;

    return (RS232_RxData & (char)(0x7f));                   // read received character, mask off top bit and return as 7 bit ASCII character
}

/******************************************************************************
**  Subroutine to output a single character to the 2 row LCD display
**  It is assumed the character is an ASCII code and it will be displayed at the
**  current cursor position
*******************************************************************************/
void LCDOutchar(int c)
{
    LCDdata = (char)(c);
    Wait1ms() ;
}

/**********************************************************************************
*subroutine to output a message at the current cursor position of the LCD display
************************************************************************************/
void LCDOutMessage(char *theMessage)
{
    char c ;
    while((c = *theMessage++) != 0)     // output characters from the string until NULL
        LCDOutchar(c) ;
}

/******************************************************************************
*subroutine to clear the line by issuing 24 space characters
*******************************************************************************/
void LCDClearln(void)
{
    int i ;
    for(i = 0; i < 24; i ++)
        LCDOutchar(' ') ;       // write a space char to the LCD display
}

/******************************************************************************
**  Subroutine to move the LCD cursor to the start of line 1 and clear that line
*******************************************************************************/
void LCDLine1Message(char *theMessage)
{
    LCDcommand = 0x80 ;
    Wait3ms();
    LCDClearln() ;
    LCDcommand = 0x80 ;
    Wait3ms() ;
    LCDOutMessage(theMessage) ;
}

/******************************************************************************
**  Subroutine to move the LCD cursor to the start of line 2 and clear that line
*******************************************************************************/
void LCDLine2Message(char *theMessage)
{
    LCDcommand = 0xC0 ;
    Wait3ms();
    LCDClearln() ;
    LCDcommand = 0xC0 ;
    Wait3ms() ;
    LCDOutMessage(theMessage) ;
}

/*********************************************************************************************************************************
**  IMPORTANT FUNCTION
**  This function install an exception handler so you can capture and deal with any 68000 exception in your program
**  You pass it the name of a function in your code that will get called in response to the exception (as the 1st parameter)
**  and in the 2nd parameter, you pass it the exception number that you want to take over (see 68000 exceptions for details)
**  Calling this function allows you to deal with Interrupts for example
***********************************************************************************************************************************/

void InstallExceptionHandler( void (*function_ptr)(), int level)
{
    volatile long int *RamVectorAddress = (volatile long int *)(StartOfExceptionVectorTable) ;   // pointer to the Ram based interrupt vector table created in Cstart in debug monitor

    RamVectorAddress[level] = (long int *)(function_ptr);                       // install the address of our function into the exception table
}

/******************************************************************************************************************************
* Start of user program
******************************************************************************************************************************/


unsigned char * RamWriter;
unsigned char * start_address;
unsigned char * end_address;
unsigned int test_type;
unsigned int user_data;
unsigned char * current_address;
unsigned char *  intermediate_address;
int address_increment;
int address_length_flag;
unsigned int read_write_test;

void main()
{
    unsigned int row, i=0, count=0, counter1=1;
    int mem_error_flag;
    char c, text[150] ;

	int PassFailFlag = 1 ;

    i = x = y = z = PortA_Count =0;
    Timer1Count = Timer2Count = Timer3Count = Timer4Count = 0;

    InstallExceptionHandler(PIA_ISR, 25) ;          // install interrupt handler for PIAs 1 and 2 on level 1 IRQ
    InstallExceptionHandler(ACIA_ISR, 26) ;		    // install interrupt handler for ACIA on level 2 IRQ
    InstallExceptionHandler(Timer_ISR, 27) ;		// install interrupt handler for Timers 1-4 on level 3 IRQ
    InstallExceptionHandler(Key2PressISR, 28) ;	    // install interrupt handler for Key Press 2 on DE1 board for level 4 IRQ
    InstallExceptionHandler(Key1PressISR, 29) ;	    // install interrupt handler for Key Press 1 on DE1 board for level 5 IRQ

    Timer1Data = 0x10;		// program time delay into timers 1-4
    Timer2Data = 0x20;
    Timer3Data = 0x15;
    Timer4Data = 0x25;

    Timer1Control = 3;		// write 3 to control register to Bit0 = 1 (enable interrupt from timers) 1 - 4 and allow them to count Bit 1 = 1
    Timer2Control = 3;
    Timer3Control = 3;
    Timer4Control = 3;

    Init_LCD();             // initialise the LCD display to use a parallel data interface and 2 lines of display
    Init_RS232() ;          // initialise the RS232 port for use with hyper terminal


    //-----------------------Common Section-----------------------
    printf("\r\nWhich test do you want to perform? Enter '0' for Read or '1' for Write: "); //prompt user for read or write test
    scanflush();
    scanf("%u", &read_write_test);

    while(read_write_test > 1){ //check for valid input
        printf("\r\nInvalid Input!");
        printf("\r\nWhich test do you want to perform? Enter '0' for Read or '1' for Write: ");
        scanflush();
        scanf("%u", &read_write_test);
    }

    printf("\r\nSpecify the memory test type. Input '0' for Bytes, '1' for Word, and '2' for Long Word: "); //prompt user for test type
    scanflush();
    scanf("%u", &test_type);

    while (test_type > 2) // check for valid input
    {
        printf("\r\nInvalid Input!");
        printf("\r\nSpecify the memory test type. Input '0' for Bytes, '1' for Word, and '2' for Long Word: ");
        scanflush();
        scanf("%u", &test_type);
    }

    //***************Get Addresses***************
    scanflush();
    printf("\r\nProvide a start address for your data: ");
    scanf("%x", &start_address);

    if (test_type != 0) // check that we're aligned properly for start address for word and long word
    {
        while ((unsigned int)start_address % 2) // odd number address
        {
            printf("\r\n Odd address is not allowed for word or long word!");
            scanflush();
            printf("\r\nProvide a start address for your data: ");
            scanf("%x", &start_address);
        }
    }

    while (start_address < 0x08020000 || start_address > 0x08030000) //protect data leak into other memories
    {
        scanflush();
        printf("\r\nError: Invalid address! Start address cannot be less than 08020000 or greater than 08030000");
        printf("\r\nProvide a start address for your data: ");
        scanf("%x", &start_address);
    }

    printf("\r\nYou have entered %x for the start address", start_address);

    scanflush();
    printf("\r\nProvide an end address for your data: "); //protect data leak into other memories
    scanf("%x", &end_address); //protect data leak into other memories

    address_length_flag = 1;

    while(address_length_flag == 1){
        if(end_address <= start_address || end_address > 0x08030000)
        {
            scanflush();
            printf("\r\nError: Invalid address! End address should not be less than or equal to start address or greater than 08030000");
            printf("\r\nProvide an end address for your data: "); //protect data leak into other memories
            scanflush();
            scanf("%x", &end_address); //protect data leak into other memories
            continue;
        }

        if(test_type == 0){
            address_length_flag = 0;
            printf("\r\n++++++++++++");
        }

        else{
            if((unsigned int)end_address % 2) // odd number address
            {
                printf("\r\n Odd address is not allowed for word or long word!");
                scanflush();
                printf("\r\nProvide an end address for your data: ");
                scanf("%x", &end_address);
                continue;
            }

            if(!((end_address - start_address) < 2) & test_type == 1){
                address_length_flag = 0;
                printf("\r\n*****************");
            }
            else if(!((end_address - start_address) < 4) & test_type == 2){
                address_length_flag = 0;
                printf("\r\n-----------------------");
            }
            else{
                scanflush();
                printf("\r\nError: Data cannot be fitted in given address range");
                printf("\r\nProvide an end address for your data: "); //protect data leak into other memories
                scanflush();
                scanf("%x", &end_address);
                continue;
            }
        }
    }

    // input long word
    // start_address 0802_0002
    // end_address 0802_000a

    switch (test_type) { // check if byte, word, or long word
        case 0:
            address_increment = 1; // byte
            break;
        case 1:
            address_increment = 2; // word
            user_data = 0x0000 + user_data;
            break;
        case 2:
            address_increment = 4; // long word
            break;
        default:
            address_increment = 1; // byte
    }

    //-----------------------Read Section-----------------------

    if(!read_write_test){
        for(current_address = start_address; current_address < end_address; current_address += address_increment){
            if(test_type == 0){ //read a byte
                printf("\r\nData at location %x: %x", current_address, *current_address);
            }
            else if(test_type == 1){//read a word
                printf("\r\nWord at location %x: %x%x", current_address, *current_address, *(current_address+1));
            }
            else{
                if( (end_address - current_address) < 3){//read a long word
                    break;
                }
                printf("\r\nLong word at location %x: %x%x%x%x", current_address, *current_address, *(current_address+1), *(current_address+2), *(current_address+3));
            }
        }
    }

    //-----------------------Write Section-----------------------
    else{
        // 1 Byte
        if (test_type == 0)
        {
            printf("\r\nEnter the data in format XX: ");
            scanflush();
            scanf("%x", &user_data);

            while (user_data < 0 || user_data > 255) // out of range/bounds
            {
                scanflush();
                printf("\rData larger than byte!\n");
                printf("\rEnter the data in format XX: ");
                scanf("%x", &user_data);
            }
        }

        //*************** Get One Word ***************
        else if (test_type == 1)
        {
            scanflush();
            printf("\r\nEnter the data in format XXXX: ");
            scanf("%x", &user_data);

            while (user_data < 0 || user_data > 65535) // out of range/bounds
            {
                printf("\r\nData larger than a word!");
                printf("\r\nEnter the data in format XXXX: ");
                scanflush();
                scanf("%x", &user_data);
            }
        }

        //*************** Get Long Word ***************
        else if (test_type == 2)
        {
            scanflush();
            printf("\rEnter the data in format XXXXXXXX: ");
            scanf("%x", &user_data);

            while (user_data < 0 || user_data > 4294967295) // out of range/bounds
            {
                scanflush();
                printf("\r\nData larger than a long word!");
                printf("\r\nEnter the data in format XXXXXXXX: ");
                scanf("%x", &user_data);
            }
        }

        // word XX_XX --> upper 8 bits ((0xFFFF & input) >> 8), shift address by 1, then lower 8 bits (0x00FF & input)
        // long word --> upper 8, shift by 1, next 8, shift by 1, next 8, shift by 1, write least significant 8, then done.

        mem_error_flag = 0;
        for(current_address = start_address; current_address < end_address; current_address += address_increment){
            if((current_address - start_address)%10000 == 0){
                printf("\r\nWriting %x at location %x", user_data, current_address); // Reports the progress every 10k (0x2800) locations
            }

            if(test_type == 0){
                *(current_address) = user_data;
                if (*(current_address) != user_data) //read the data and confirm if it is written correctly
                {
                    printf("\r\nError writing %x to address %x", user_data, current_address);
                    mem_error_flag = 1;
                    break;
                }
            }

            else if(test_type == 1){

                *(current_address) = (0xFF00 & user_data) >> 8;

                if (*(current_address) != ((0xFF00 & user_data) >> 8)) //read the data and confirm if it is written correctly
                {
                    printf("\r\nError writing %x to address %x", (0xFF00 & user_data) >> 8, current_address);
                    mem_error_flag = 1;
                    break;
                }

                *(current_address + 1) = (0x00FF & user_data);

                if (*(current_address + 1) != (0x00FF & user_data)) //read the data and confirm if it is written correctly
                {
                    printf("\r\nError writing %x to address %x", (0x00FF & user_data), (current_address+1));
                    mem_error_flag = 1;
                    break;
                }
            }

            else if(test_type == 2)
            {
                if( (end_address - current_address) < 3){ //  to ensure we do not go past end address
                    *(current_address) = (0xFF000000 & user_data) >> 24;
                    *(current_address + 1) = (0x00FF0000 & user_data) >> 16;
                    printf("\r\nError: End address limit reached");
                    printf("\r\nCannot write %x and %x",(0x0000FF00 & user_data) >> 8, (0x000000FF) & user_data);
                    break;
                }

                *(current_address) = (0xFF000000 & user_data) >> 24;

                if (*(current_address) != ((0xFF000000 & user_data) >> 24)) //read the data and confirm if it is written correctly
                {
                    printf("\r\nError writing %x to address %x", (0xFF000000 & user_data) >> 24, current_address);
                    mem_error_flag = 1;
                    break;
                }

                *(current_address + 1) = (0x00FF0000 & user_data) >> 16;

                if (*(current_address + 1) != ((0x00FF0000 & user_data) >> 16)) //read the data and confirm if it is written correctly
                {
                    printf("\r\nError writing %x to address %x", (0x00FF0000 & user_data) >> 16, (current_address+1));
                    mem_error_flag = 1;
                    break;
                }

                *(current_address + 2) = (0x0000FF00 & user_data) >> 8;

                if (*(current_address + 2) != ((0x0000FF00 & user_data) >> 8)) //read the data and confirm if it is written correctly
                {
                    printf("\r\nError writing %x to address %x", (0x0000FF00 & user_data) >> 8, (current_address+2));
                    mem_error_flag = 1;
                    break;
                }

                *(current_address + 3) = (0x000000FF) & user_data;

                if (*(current_address + 3) != ((0x000000FF) & user_data)) //read the data and confirm if it is written correctly
                {
                    printf("\r\nError writing %x to address %x", (0x000000FF) & user_data, (current_address+3));
                    mem_error_flag = 1;
                    break;
                }
            }
        }

        if(mem_error_flag == 0){
            printf("\r\nWriting finished at %08x", end_address);
        }
    }
    // progress function --> counter every 256 so print out every 16




    // 0x08020000 -> 0x08021000


    // Now we need to prompt the user for start and end addresses
    // 0802_0000 start
    // 0802_0020 end
    // type: byte
    // 4 bytes (pattern goes 4 times)

    // Pseudocode:
    // take difference between end and start addresses
    // divide the difference by test type (size)
    // repeat in a for loop

/*************************************************************************************************
**  Test of scanf function
*************************************************************************************************/

    // scanflush() ;                       // flush any text that may have been typed ahead
    // printf("\r\nEnter Integer: ") ;
    // scanf("%d", &i) ;
    // printf("You entered %d", i) ;

    // sprintf(text, "Hello CPEN 412 Student") ;
    // LCDLine1Message(text) ;

    // printf("\r\nHello CPEN 412 Student\r\nYour LEDs should be Flashing") ;
    // printf("\r\nYour LCD should be displaying") ;

    while(1)
        ;

   // programs should NOT exit as there is nothing to Exit TO !!!!!!
   // There is no OS - just press the reset button to end program and call debug
}