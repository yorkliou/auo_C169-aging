/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "I2C_relate.h"  // ????????? .h ??
#include "definitions.h"  // ??? Harmony ? SERCOM API ??????
#include <stdint.h>
#include <stdbool.h>
extern volatile bool i2c_transfer_done_flag;

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */
/** Descriptive Data Item Name

  @Summary
    Brief one-line summary of the data item.
    
  @Description
    Full description, explaining the purpose and usage of data item.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
  @Remarks
    Any additional remarks
 */



/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */

/** 
  @Function
    int ExampleLocalFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Description
    Full description, explaining the purpose and usage of the function.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

  @Precondition
    List and describe any required preconditions. If there are no preconditions,
    enter "None."

  @Parameters
    @param param1 Describe the first parameter to the function.
    
    @param param2 Describe the second parameter to the function.

  @Returns
    List (if feasible) and describe the return values of the function.
    <ul>
      <li>1   Indicates an error occurred
      <li>0   Indicates an error did not occur
    </ul>

  @Remarks
    Describe any special behavior not described above.
    <p>
    Any additional remarks.

  @Example
    @code
    if(ExampleFunctionName(1, 2) == 0)
    {
        return 3;
    }
 */


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */
uint8_t sbuffer[6];
uint8_t crcOutput;

uint8_t CAL_CRC8(uint8_t* pu8_data, uint8_t data_length) {
    uint8_t crcOutput = 0xFF;
    uint8_t i;
    uint8_t din[8];
    uint8_t shift[8];
    uint8_t crc[8];

    while (data_length--) {
        for (i = 0; i < 8; i++) {
            din[i] = (*pu8_data >> i) & 0x01;
            crc[i] = (crcOutput >> i) & 0x01;
        }
        pu8_data++;

        shift[0] = din[7] ^ crc[7];
        shift[1] = din[6] ^ crc[6];
        shift[2] = din[5] ^ crc[5];
        shift[3] = din[4] ^ crc[4];
        shift[4] = din[3] ^ crc[3] ^ shift[0];
        shift[5] = din[2] ^ crc[2] ^ shift[0] ^ shift[1];
        shift[6] = din[1] ^ crc[1] ^ shift[0] ^ shift[1] ^ shift[2];
        shift[7] = din[0] ^ crc[0] ^ shift[1] ^ shift[2] ^ shift[3];

        crc[7] = shift[0] ^ shift[2] ^ shift[3] ^ shift[4];
        crc[6] = shift[1] ^ shift[3] ^ shift[4] ^ shift[5];
        crc[5] = shift[2] ^ shift[4] ^ shift[5] ^ shift[6];
        crc[4] = shift[3] ^ shift[5] ^ shift[6] ^ shift[7];
        crc[3] = shift[4] ^ shift[6] ^ shift[7];
        crc[2] = shift[5] ^ shift[7];
        crc[1] = shift[6];
        crc[0] = shift[7];

        crcOutput = 0;
        for (i = 0; i < 8; i++) {
            crcOutput |= (crc[i] << i);
        }
    }
    return crcOutput;
}

void I2c_Gen_write_1A_1P_TCON(unsigned int reg, unsigned char length, unsigned char parameter1) {

    sbuffer[0] = 0xC2; // slave addr (0x61 << 1 | 0)
    sbuffer[1] = (reg >> 8) & 0xFF;
    sbuffer[2] = reg & 0xFF;
    sbuffer[3] = length;
    sbuffer[4] = parameter1;
    crcOutput = CAL_CRC8(sbuffer, 5);
    sbuffer[5] = crcOutput;
    
    
    // ???????
    while (!i2c_transfer_done_flag);
   
    i2c_transfer_done_flag = false;

    while (SERCOM5_I2C_IsBusy());
    
    SERCOM5_I2C_Write(0x61, &sbuffer[1], 5);
    
    
    // ????????
    while (!i2c_transfer_done_flag)
    {
//        SYSTICK_DelayMs(100);
//        LED1_Toggle();
    }

}

void SET_TCON_BIST() {
    I2c_Gen_write_1A_1P_TCON(0x043C, 0, 0x5A);
}

void show_white_TCON() {
    //SET_TCON_BIST();
    I2c_Gen_write_1A_1P_TCON(0x0580, 0, 0x03);
    SET_TCON_BIST();
}
void show_black_TCON()
{
    SET_TCON_BIST();
    I2c_Gen_write_1A_1P_TCON(0x043C,0,0x01); 	
}
void show_gray128_TCON()
{
 SET_TCON_BIST(); 
 I2c_Gen_write_1A_1P_TCON(0x0580,0,0x25);	
}
void show_h_graybar256_TCON()
{
 SET_TCON_BIST();
 I2c_Gen_write_1A_1P_TCON(0x0580,0,0x0D);		
}
void show_colorbar_vgray256_TCON()
{
 I2c_Gen_write_1A_1P_TCON(0x0580,0,0x15);		
 SET_TCON_BIST();
}
void show_red_TCON()
{
    SET_TCON_BIST();
    I2c_Gen_write_1A_1P_TCON(0x0580, 0, 0x05);  
}

void show_green_TCON() {
    SET_TCON_BIST();
    I2c_Gen_write_1A_1P_TCON(0x0580, 0, 0x07);
}

void show_blue_TCON() {
    SET_TCON_BIST();
    I2c_Gen_write_1A_1P_TCON(0x0580, 0, 0x09);
}

void show_colorbar_TCON()
{
 SET_TCON_BIST(); 
 I2c_Gen_write_1A_1P_TCON(0x0580,0,0x13);			
}

void show_v_graybar256_TCON() {
    SET_TCON_BIST();
    I2c_Gen_write_1A_1P_TCON(0x0580, 0, 0x11);
}
void show_h_graybar16_TCON()
{
 SET_TCON_BIST();
 I2c_Gen_write_1A_1P_TCON(0x0580,0,0x0B);	
}

void I2c_Gen_write_1A_4P_DP(uint8_t reg, uint8_t parameter1, uint8_t parameter2, uint8_t parameter3, uint8_t parameter4) {
    uint8_t txBuffer[6];

    // ????????
    txBuffer[0] = reg; // Register ??
    txBuffer[1] = 0x00; // ???
    txBuffer[2] = parameter1; // ??1
    txBuffer[3] = parameter2; // ??2
    txBuffer[4] = parameter3; // ??3
    txBuffer[5] = parameter4; // ??4

    // ?? 6 byte ??? I2C Slave Address 0x60
    while (!SERCOM5_I2C_Write(0x60, txBuffer, 6)) // 0x60 ? 7-bit ??
    {
       // LED3_Toggle();
    }

    // ??????
    while (SERCOM5_I2C_IsBusy()) {
        // ???? I2C bus ??
        //LED1_Toggle();
    }
}

void I2c_Gen_write_1A_1P_DP(uint8_t reg, uint8_t parameter1) {
    uint8_t txBuffer[3];

    txBuffer[0] = reg;
    txBuffer[1] = 0x00;
    txBuffer[2] = parameter1;

    // ???????
    while (!i2c_transfer_done_flag);

    i2c_transfer_done_flag = false;

    // ?? I2C bus ??
    while (SERCOM5_I2C_IsBusy());

    // ????? Slave Address 0x60 (????0xC0)
    SERCOM5_I2C_Write(0x60, txBuffer, 3);

    // ????????
    while (!i2c_transfer_done_flag);
}
void All_Gate_ON(void)
{
    //RESET=GND, Normal Power off (All Gate on)
    I2c_Gen_write_1A_4P_DP(0xC8,0x58,0xA8,0x18,0x23);	
	I2c_Gen_write_1A_1P_DP(0x00,0x1D);//Page29
	I2c_Gen_write_1A_1P_DP(0xFD,0xCC);//Password
	I2c_Gen_write_1A_1P_DP(0x4C,0xE4);
}
/*  A brief description of a section can be given directly below the section
    banner.
 */

// *****************************************************************************

/** 
  @Function
    int ExampleInterfaceFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Remarks
    Refer to the example_file.h interface header for function usage details.
 */



/* *****************************************************************************
 End of File
 */
