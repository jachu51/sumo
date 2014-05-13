/*
 * lcd.cpp
 *
 *  Created on: 27-03-2013
 *      Author: jachu
 */

#include "lcd.h"
#include "sys.h"
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <string.h>

/* Function prototypes */

void Delay      ( void );

/* Global variables */

/* Cache buffer in SRAM 128*64 bits or 1024 bytes */
byte  LcdCache [ LCD_CACHE_SIZE ];

/* Cache index */
int   LcdCacheIdx;

/* Lower part of water mark */
int   LoWaterMark;

/* Higher part of water mark */
int   HiWaterMark;

/* Variable to decide whether update Lcd Cache is active/nonactive */
volatile bool  UpdateLcd;

/* Variable that determine actually updated page index */
volatile byte	PageAddress;

/*
 * Name         :  LcdInit
 * Description  :  Performs MCU SPI & LCD controller initialization.
 * Argument(s)  :  None.
 * Return value :  None.
 */

void LcdInit ( void )
{
    /* GPIO configuration */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = LCD_SPI_MOSI | LCD_SPI_SCK;
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_GPIO, &gpioInit);

	gpioInit.GPIO_Pin = LCD_DC | LCD_RES | LCD_SPI_CS;
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_GPIO, &gpioInit);

	/* SPI configuration */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	SPI_InitTypeDef spiInit;
	spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	spiInit.SPI_CPHA = SPI_CPHA_2Edge;
	spiInit.SPI_CPOL = SPI_CPOL_High;
	spiInit.SPI_DataSize = SPI_DataSize_8b;
	spiInit.SPI_Direction = SPI_Direction_1Line_Tx;
	spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
	spiInit.SPI_Mode = SPI_Mode_Master;
	spiInit.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(LCD_SPI, &spiInit);
	SPI_SSOutputCmd(LCD_SPI, ENABLE);

	/* DMA configuration */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_InitTypeDef dmaInit;
	dmaInit.DMA_BufferSize = LCD_X_RES;
	dmaInit.DMA_DIR = DMA_DIR_PeripheralDST;
	dmaInit.DMA_M2M = DMA_M2M_Disable;
	dmaInit.DMA_MemoryBaseAddr = (uint32_t)(LcdCache);
	dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmaInit.DMA_Mode = DMA_Mode_Normal;
	dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
	dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmaInit.DMA_Priority = DMA_Priority_Low;
	DMA_Init(LCD_DMA_CHANNEL, &dmaInit);
	DMA_ITConfig(LCD_DMA_CHANNEL, DMA_IT_TC, ENABLE);

	NVIC_InitTypeDef initNVIC;
	initNVIC.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	initNVIC.NVIC_IRQChannelPreemptionPriority = 0;
	initNVIC.NVIC_IRQChannelSubPriority = 0;
	initNVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&initNVIC);

	SPI_I2S_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(LCD_SPI, ENABLE);

	/* LCD initialization */
	//reset
	GPIO_ResetBits(LCD_GPIO, LCD_SPI_CS);
	GPIO_ResetBits(LCD_GPIO, LCD_RES);
	sysDelayMs(500);
	GPIO_SetBits(LCD_GPIO, LCD_RES);


	//LCD bias - 1/7
	LcdSend(CMD_SET_BIAS_7, LCD_CMD);

	//ADC selection - normal - default
	LcdSend(CMD_SET_ADC_NORMAL, LCD_CMD);

	//Common output mode - normal - default
	LcdSend(CMD_SET_COM_REVERSE, LCD_CMD);

	// Initial display line
	LcdSend(CMD_SET_DISP_START_LINE, LCD_CMD);

	//Booster on, voltage regulator circuit on, voltage follower circuit on
	LcdSend(CMD_SET_POWER_CONTROL | 0x4, LCD_CMD);
	sysDelayMs(50);
	LcdSend(CMD_SET_POWER_CONTROL | 0x6, LCD_CMD);
	sysDelayMs(50);
	LcdSend(CMD_SET_POWER_CONTROL | 0x7, LCD_CMD);
	sysDelayMs(10);

	//Internal V0 regulator resistor ratio
	LcdSend(CMD_SET_RESISTOR_RATIO | 0x6, LCD_CMD);

	LcdSend(CMD_SET_ALLPTS_NORMAL, LCD_CMD);

	//Display on
	LcdSend(CMD_DISPLAY_ON, LCD_CMD);

	//Electronic volume control - 40
	LcdSend(CMD_SET_VOLUME_FIRST, LCD_CMD);
	LcdSend(CMD_SET_VOLUME_SECOND | 0x3f, LCD_CMD);

    /* Reset watermark pointers to empty */
    LoWaterMark = LCD_CACHE_SIZE;
    HiWaterMark = 0;

    /* Clear display on first time use */
    LcdClear();
    UpdateLcd = false;
    LcdCacheIdx = 0;
}

/*
 * Name         :  LcdContrast
 * Description  :  Set display contrast.
 * Argument(s)  :  contrast -> Contrast value from 0x00 to 0x7F.
 * Return value :  None.
 */
void LcdContrast ( byte contrast )
{
    /* LCD Extended Commands. */
    LcdSend( 0x21, LCD_CMD );

    /* Set LCD contrast level. */
    LcdSend( 0x80 | contrast, LCD_CMD );

    /* LCD Standard Commands, horizontal addressing mode. */
    LcdSend( 0x20, LCD_CMD );
}

/*
 * Name         :  LcdClear
 * Description  :  Clears the display. LcdUpdate must be called next.
 * Argument(s)  :  None.
 * Return value :  None.
 * Note         :  Based on Sylvain Bissonette's code
 */
void LcdClear ( void )
{
// Removed in version 0.2.6, March 14 2009
// Optimized by Jakub Lasinski
//    int i;
//
//    /* Set 0x00 to all LcdCache's contents */
//    for ( i = 0; i < LCD_CACHE_SIZE; i++ )
//    {
//        LcdCache[ i ] = 0x00;
//    }
	memset(LcdCache,0x00,LCD_CACHE_SIZE); //Sugestion - its faster and its 10 bytes less in program mem
    /* Reset watermark pointers to full */
    LoWaterMark = 0;
    HiWaterMark = LCD_CACHE_SIZE - 1;

    /* Set update flag to be true */
    //UpdateLcd = TRUE;
}

/*
 * Name         :  LcdGotoXYFont
 * Description  :  Sets cursor location to xy location corresponding to basic
 *                 font size.
 * Argument(s)  :  x, y -> Coordinate for new cursor position. Range: 1,1 .. 14,6
 * Return value :  see return value in lcd.h
 * Note         :  Based on Sylvain Bissonette's code
 */
byte LcdGotoXYFont ( byte x, byte y )
{
    /* Boundary check, slow down the speed but will guarantee this code wont fail */
    /* Version 0.2.5 - Fixed on Dec 25, 2008 (XMAS) */
	//TODO parametrize
    if( x > 14)
        return OUT_OF_BORDER;
    if( y > 6)
        return OUT_OF_BORDER;
    /*  Calculate index. It is defined as address within 1024 bytes memory */

    LcdCacheIdx = ( x - 1 ) * 6 + ( y - 1 ) * LCD_X_RES;
    return OK;
}

/*
 * Name         :  LcdChr
 * Description  :  Displays a character at current cursor location and
 *                 increment cursor location.
 * Argument(s)  :  size -> Font size. See enum in lcd.h.
 *                 ch   -> Character to write.
 * Return value :  see lcd.h about return value
 */
byte LcdChr ( LcdFontSize size, byte ch )
{
    byte i, c;
    byte b1, b2;
    int  tmpIdx;

    if ( LcdCacheIdx < LoWaterMark )
    {
        /* Update low marker. */
        LoWaterMark = LcdCacheIdx;
    }

    if ( (ch < 0x20) || (ch > 0x7b) )
    {
        /* Convert to a printable character. */
        ch = 92;
    }

    if ( size == FONT_1X )
    {
        for ( i = 0; i < 5; i++ )
        {
            /* Copy lookup table from Flash ROM to LcdCache */
            LcdCache[LcdCacheIdx++] = (byte)(FontLookup[ ch - 32 ][ i ] << 1);
        }
    }
    else if ( size == FONT_2X )
    {
        tmpIdx = LcdCacheIdx - 84;

        if ( tmpIdx < LoWaterMark )
        {
            LoWaterMark = tmpIdx;
        }

        if ( tmpIdx < 0 ) return OUT_OF_BORDER;

        for ( i = 0; i < 5; i++ )
        {
            /* Copy lookup table from Flash ROM to temporary c */
            c = FontLookup[ch - 32][i] << 1;
            /* Enlarge image */
            /* First part */
            b1 =  (c & 0x01) * 3;
            b1 |= (c & 0x02) * 6;
            b1 |= (c & 0x04) * 12;
            b1 |= (c & 0x08) * 24;

            c >>= 4;
            /* Second part */
            b2 =  (c & 0x01) * 3;
            b2 |= (c & 0x02) * 6;
            b2 |= (c & 0x04) * 12;
            b2 |= (c & 0x08) * 24;

            /* Copy two parts into LcdCache */
            LcdCache[tmpIdx] = b1;
            LcdCache[tmpIdx + 1] = b1;
            LcdCache[tmpIdx + LCD_X_RES] = b2;
            LcdCache[tmpIdx + LCD_X_RES + 1] = b2;
            tmpIdx += 2;
        }

        /* Update x cursor position. */
        /* Version 0.2.5 - Possible bug fixed on Dec 25,2008 */
        LcdCacheIdx = (LcdCacheIdx + 11) % LCD_CACHE_SIZE;
    }

    if ( LcdCacheIdx > HiWaterMark )
    {
        /* Update high marker. */
        HiWaterMark = LcdCacheIdx;
    }

    /* Horizontal gap between characters. */
    /* Version 0.2.5 - Possible bug fixed on Dec 25,2008 */
    LcdCache[LcdCacheIdx] = 0x00;
    /* At index number LCD_CACHE_SIZE - 1, wrap to 0 */
    if(LcdCacheIdx == (LCD_CACHE_SIZE - 1) )
    {
        LcdCacheIdx = 0;
        return OK_WITH_WRAP;
    }
    /* Otherwise just increment the index */
    LcdCacheIdx++;
    return OK;
}

/*
 * Name         :  LcdStr
 * Description  :  Displays a character at current cursor location and increment
 *                 cursor location according to font size. This function is
 *                 dedicated to print string laid in SRAM
 * Argument(s)  :  size      -> Font size. See enum.
 *                 dataArray -> Array contained string of char to be written
 *                              into cache.
 * Return value :  see return value on lcd.h
 */
byte LcdStr ( LcdFontSize size, const byte dataArray[] )
{
    byte tmpIdx=0;
    byte response;
    while( dataArray[ tmpIdx ] != '\0' )
	{
        /* Send char */
		response = LcdChr( size, dataArray[ tmpIdx ] );
        /* Just in case OUT_OF_BORDER occured */
        /* Dont worry if the signal == OK_WITH_WRAP, the string will
        be wrapped to starting point */
        if( response == OUT_OF_BORDER)
            return OUT_OF_BORDER;
        /* Increase index */
		tmpIdx++;
	}
    return OK;
}

/*
 * Name         :  LcdPixel
 * Description  :  Displays a pixel at given absolute (x, y) location.
 * Argument(s)  :  x, y -> Absolute pixel coordinates
 *                 mode -> Off, On or Xor. See enum in lcd.h.
 * Return value :  see return value on lcd.h
 * Note         :  Based on Sylvain Bissonette's code
 */
byte LcdPixel ( byte x, byte y, LcdPixelMode mode )
{
    word  index;
    byte  offset;
    byte  data;

    /* Prevent from getting out of border */
    if ( x > LCD_X_RES ) return OUT_OF_BORDER;
    if ( y > LCD_Y_RES ) return OUT_OF_BORDER;

    /* Recalculating index and offset */
    index = ( ( y / 8 ) * LCD_X_RES ) + x;
    offset  = y - ( ( y / 8 ) * 8 );

    data = LcdCache[ index ];

    /* Bit processing */

	/* Clear mode */
    if ( mode == PIXEL_OFF )
    {
        data &= ( ~( 0x01 << offset ) );
    }

    /* On mode */
    else if ( mode == PIXEL_ON )
    {
        data |= ( 0x01 << offset );
    }

    /* Xor mode */
    else if ( mode  == PIXEL_XOR )
    {
        data ^= ( 0x01 << offset );
    }

    /* Final result copied to cache */
    LcdCache[ index ] = data;

    if ( index < LoWaterMark )
    {
        /*  Update low marker. */
        LoWaterMark = index;
    }

    if ( index > HiWaterMark )
    {
        /*  Update high marker. */
        HiWaterMark = index;
    }
    return OK;
}

/*
 * Name         :  LcdLine
 * Description  :  Draws a line between two points on the display.
 * Argument(s)  :  x1, y1 -> Absolute pixel coordinates for line origin.
 *                 x2, y2 -> Absolute pixel coordinates for line end.
 *                 mode   -> Off, On or Xor. See enum in lcd.h.
 * Return value :  see return value on lcd.h
 */
byte LcdLine ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode )
{
    int dx, dy, stepx, stepy, fraction;
    byte response;

    /* Calculate differential form */
    /* dy   y2 - y1 */
    /* -- = ------- */
    /* dx   x2 - x1 */

    /* Take differences */
    dy = y2 - y1;
    dx = x2 - x1;

    /* dy is negative */
    if ( dy < 0 )
    {
        dy    = -dy;
        stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    /* dx is negative */
    if ( dx < 0 )
    {
        dx    = -dx;
        stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dx <<= 1;
    dy <<= 1;

    /* Draw initial position */
    response = LcdPixel( x1, y1, mode );
    if(response)
        return response;

    /* Draw next positions until end */
    if ( dx > dy )
    {
        /* Take fraction */
        fraction = dy - ( dx >> 1);
        while ( x1 != x2 )
        {
            if ( fraction >= 0 )
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;

            /* Draw calculated point */
            response = LcdPixel( x1, y1, mode );
            if(response)
                return response;

        }
    }
    else
    {
        /* Take fraction */
        fraction = dx - ( dy >> 1);
        while ( y1 != y2 )
        {
            if ( fraction >= 0 )
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;

            /* Draw calculated point */
            response = LcdPixel( x1, y1, mode );
            if(response)
                return response;
        }
    }

    /* Set update flag to be true */
    //UpdateLcd = TRUE;
    return OK;
}

/*
 * Name         :  LcdSingleBar
 * Description  :  Display single bar.
 * Argument(s)  :  baseX  -> absolute x axis coordinate
 *                 baseY  -> absolute y axis coordinate
 *				   height -> height of bar (in pixel)
 *				   width  -> width of bar (in pixel)
 *				   mode   -> Off, On or Xor. See enum in lcd.h.
 * Return value :  see return value on lcd.h
 */
byte LcdSingleBar ( byte baseX, byte baseY, byte height, byte width, LcdPixelMode mode )
{
	byte tmpIdxX,tmpIdxY,tmp;

    byte response;

    /* Checking border */
	if ( ( baseX > LCD_X_RES ) || ( baseY > LCD_Y_RES ) ) return OUT_OF_BORDER;

	if ( height >= baseY )
		tmp = 0;
	else
		tmp = baseY - height + 1;

    /* Draw lines */
	for ( tmpIdxY = tmp; tmpIdxY <= baseY; tmpIdxY++ )
	{
		for ( tmpIdxX = baseX; tmpIdxX < (baseX + width); tmpIdxX++ )
        {
			response = LcdPixel( tmpIdxX, tmpIdxY, mode );
            if(response)
                return response;

        }
	}

    /* Set update flag to be true */
	//UpdateLcd = TRUE;
    return OK;
}

/*
 * Name         :  LcdBars
 * Description  :  Display multiple bars.
 * Argument(s)  :  data[] -> data which want to be plotted
 *                 numbBars  -> number of bars want to be plotted
 *				   width  -> width of bar (in pixel)
 * Return value :  see return value on lcd.h
 * Note         :  Please check EMPTY_SPACE_BARS, BAR_X, BAR_Y in lcd.h
 */
byte LcdBars ( byte data[], byte numbBars, byte width, byte multiplier )
{
	byte b;
	byte tmpIdx = 0;
    byte response;

	for ( b = 0;  b < numbBars ; b++ )
	{
        /* Preventing from out of border (LCD_X_RES) */
		if ( tmpIdx > LCD_X_RES ) return OUT_OF_BORDER;

		/* Calculate x axis */
		tmpIdx = ((width + EMPTY_SPACE_BARS) * b) + BAR_X;

		/* Draw single bar */
		response = LcdSingleBar( tmpIdx, BAR_Y, data[ b ] * multiplier, width, PIXEL_ON);
        if(response == OUT_OF_BORDER)
            return response;
	}

	/* Set update flag to be true */
	//UpdateLcd = TRUE;
    return OK;

}
/*
 * Name         :  LcdRect
 * Description  :  Display a rectangle.
 * Argument(s)  :  x1   -> absolute first x axis coordinate
 *                 y1   -> absolute first y axis coordinate
 *				   x2   -> absolute second x axis coordinate
 *				   y2   -> absolute second y axis coordinate
 *				   mode -> Off, On or Xor. See enum in lcd.h.
 * Return value :  see return value on lcd.h.
 */
byte LcdRect ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode )
{
	byte tmpIdxX,tmpIdxY;
    byte response;

	/* Checking border */
	if ( ( x1 > LCD_X_RES ) ||  ( x2 > LCD_X_RES ) || ( y1 > LCD_Y_RES ) || ( y2 > LCD_Y_RES ) )
		/* If out of border then return */
		return OUT_OF_BORDER;

	if ( ( x2 > x1 ) && ( y2 > y1 ) )
	{
		for ( tmpIdxY = y1; tmpIdxY < y2; tmpIdxY++ )
		{
			/* Draw line horizontally */
			for ( tmpIdxX = x1; tmpIdxX < x2; tmpIdxX++ )
            {
				/* Draw a pixel */
				response = LcdPixel( tmpIdxX, tmpIdxY, mode );
                if(response)
                    return response;
            }
		}

		/* Set update flag to be true */
		//UpdateLcd = TRUE;
	}
    return OK;
}
/*
 * Name         :  LcdImage
 * Description  :  Image mode display routine.
 * Argument(s)  :  Address of image in hexes
 * Return value :  None.
 * Example      :  LcdImage(&sample_image_declared_as_array);
 */
void LcdImage ( const byte *imageData )
{
	/* Initialize cache index to 0 */
//	LcdCacheIdx = 0;
//	/* While within cache range */
//    for ( LcdCacheIdx = 0; LcdCacheIdx < LCD_CACHE_SIZE; LcdCacheIdx++ )
//    {
//		/* Copy data from pointer to cache buffer */
//        LcdCache[LcdCacheIdx] = pgm_read_byte( imageData++ );
//    }
	/* optimized by Jakub Lasinski, version 0.2.6, March 14, 2009 */
    memcpy(LcdCache,imageData,LCD_CACHE_SIZE);	//Same as aboeve - 6 bytes less and faster instruction
	/* Reset watermark pointers to be full */
    LoWaterMark = 0;
    HiWaterMark = LCD_CACHE_SIZE - 1;

	/* Set update flag to be true */
    //UpdateLcd = TRUE;
}

/*
 * Name         :  LcdUpdate
 * Description  :  Copies the LCD cache into the device RAM.
 * Argument(s)  :  None.
 * Return value :  None.
 */
void LcdUpdate ( void )
{
	if(UpdateLcd == true){
		return;
	}
    UpdateLcd = true;

    PageAddress = 0;
    LcdSetPage(PageAddress);

    LcdSendPageDMA();
}

/*
 * Name         :  LcdSend
 * Description  :  Sends data to display controller.
 * Argument(s)  :  data -> Data to be sent
 *                 cd   -> Command or data (see enum in lcd.h)
 * Return value :  None.
 */
void LcdSend ( byte data, LcdCmdData cd )
{
	GPIO_ResetBits(LCD_GPIO, LCD_SPI_CS);
	if(cd == LCD_CMD){
	    GPIO_ResetBits(LCD_GPIO, LCD_DC);
	}
	SPI_I2S_SendData(LCD_SPI, data);

	while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY));

	if(cd == LCD_CMD){
	    GPIO_SetBits(LCD_GPIO, LCD_DC);
	}
	GPIO_SetBits(LCD_GPIO, LCD_SPI_CS);
}

/*
 * Name         :  LcdSendPageDMA
 * Description  :  Send page to lcd using DMA.
 * Argument(s)  :  None.
 * Return value :  None.
 */
void LcdSendPageDMA(){
    DMA_Cmd(LCD_DMA_CHANNEL, DISABLE);
    LCD_DMA_CHANNEL->CNDTR = LCD_X_RES;
    LCD_DMA_CHANNEL->CMAR = (uint32_t)(LcdCache + PageAddress*LCD_X_RES);
    //RMW
    //LcdSend(CMD_RMW, LCD_CMD);
    //LcdSend(0xff, LCD_DATA);
	GPIO_ResetBits(LCD_GPIO, LCD_SPI_CS);
    DMA_Cmd(LCD_DMA_CHANNEL, ENABLE);
}

/*
 * Name         :  LcdSetPage
 * Description  :  Sets page number.
 * Argument(s)  :  pg -> Number of page to be set
 * Return value :  None.
 */
void LcdSetPage(int pg){

	//Page
	LcdSend(CMD_SET_PAGE | (pg & 0xf), LCD_CMD);

	//Column 0
    LcdSend(CMD_SET_COLUMN_LOWER, LCD_CMD);
    LcdSend(CMD_SET_COLUMN_UPPER, LCD_CMD);

}


/*
 * Name         :  Delay
 * Description  :  Uncalibrated delay for LCD init routine.
 * Argument(s)  :  None.
 * Return value :  None.
 */
void Delay ( void )
{
    int i;

    for ( i = -32000; i < 32000; i++ );
}


/*
 * Name         :  DMA1_Channel3_IRQHandler
 * Description  :  Routine that handles displaying next page.
 * Argument(s)  :  None.
 * Return value :  None.
 */
extern "C" {

void DMA1_Channel3_IRQHandler(void){
	while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(LCD_GPIO, LCD_SPI_CS);
    PageAddress++;
    if(PageAddress >= LCD_PAGES){
    	UpdateLcd = false;
    	DMA_ClearITPendingBit(DMA1_IT_TC3);
    	return;
    }
    LcdSetPage(PageAddress);
    LcdSendPageDMA();
	DMA_ClearITPendingBit(DMA1_IT_TC3);
}

}
