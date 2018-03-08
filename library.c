#include "ngpc.h"
#include "library.h"


volatile u8 VBCounter;

// SFX related variables and registers
u8 SFXInstalled;
#define SFXPulse       (*(u8 *)0x7010)
#define SFXPlayBuffer  (*(u8 *)0x7011)
#define SFXLoadPreset  (*(volatile u8 *)0x7012)
#define SFXPlayPreset  (*(u8 *)0x7013)
#define SFXStopAll     (*(u8 *)0x7014)
#define SFXBuffer      (u8 *)0x7020


// dummy Interrupt function used for interrupt that are currently
// unused. To add an interrupt simply supply your own routine in the
// InitNGPC routine in the interrupt vector table initialisation
void __interrupt DummyFunction(void) {}


//////////////////////////////////////////////////////////////////////////////
// VBInterrupt
// Handles the default work required by the VBInterrupt (watchdog,
// shutdown and a standard counter)
//////////////////////////////////////////////////////////////////////////////
void SysShutdown(); // forward reference
void __interrupt VBInterrupt(void)
{
   // clear the watchdog
   WATCHDOG = WATCHDOG_CLEAR;

   // check for power down
   if (USR_SHUTDOWN)
   {
	  SysShutdown();
	  while (1);
   }
   
   // increment a counter
   VBCounter++;

   if (SFXInstalled)
	  SFXPulse = 1;
   
   // TODO: add any other VBI code here.
}

//////////////////////////////////////////////////////////////////////////////
// InitNGPC
// This should be the first function called in main() it initialises the
// NGPC hardware (interrupt vector tables etc)
//////////////////////////////////////////////////////////////////////////////
void InitNGPC(void)
{
   // Set NGP or NGPC mode
   if (OS_VERSION == 0)
   {
	  (*(u8*)0x6f83) &= ~8; // res 3
	  (*(u8*)0x6da0) = 0;
   }

   // set user answer
   USR_ANSWER |= 64; // set 6

   // User hasn't called InstallSoundDriver yet
   SFXInstalled = 0;
   
   // install user interrupt vectors
   SWI3_INT = DummyFunction;
   SWI4_INT = DummyFunction;
   SWI5_INT = DummyFunction;
   SWI6_INT = DummyFunction;
   RTCI_INT = DummyFunction;
   VBL_INT = VBInterrupt;
   Z80_INT = DummyFunction;
   HBL_INT = DummyFunction;
   TI0_INT = DummyFunction;
   TI1_INT = DummyFunction;
   TI2_INT = DummyFunction;
   TI3_INT = DummyFunction;
   STX_INT = DummyFunction;
   SRX_INT = DummyFunction;
   DMA0_INT = DummyFunction;
   DMA1_INT = DummyFunction;
   DMA2_INT = DummyFunction;
   DMA3_INT = DummyFunction;

   ENABLE_INTERRUPTS;
   
   // set screen size
   WIN_X = 0;
   WIN_Y = 0;
   WIN_W = 160;
   WIN_H = 152;

}


//////////////////////////////////////////////////////////////////////////////
// SysShutdown                                                              //
//////////////////////////////////////////////////////////////////////////////
void SysShutdown()
{
   __asm(" ld rw3,0"); // 0 = VECT_SHUTDOWN

   // do the system call
   __asm(" ldf 3");
   __asm(" add w,w");
   __asm(" add w,w");
   __asm(" ld xix,0xfffe00");
   __asm(" ld xix,(xix+w)");
   __asm(" call xix");
}


//////////////////////////////////////////////////////////////////////////////
// SysSetSystemFont                                                         //
//////////////////////////////////////////////////////////////////////////////
void SysSetSystemFont()
{
   __asm(" ld ra3,3"); 
   __asm(" ld rw3,5"); // VECT_SYSFONTSET

   // do the system call
   __asm(" ldf 3");
   __asm(" add w,w");
   __asm(" add w,w");
   __asm(" ld xix, 0xfffe00");
   __asm(" ld xix,(xix+w)");
   __asm(" call xix");
}

//////////////////////////////////////////////////////////////////////////////
// ClearScreen
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//////////////////////////////////////////////////////////////////////////////
void ClearScreen(u8 Plane)
{
   int i;
   u16 * Screen;

   switch(Plane)
   {
	  case SCR_1_PLANE:
		 Screen = SCROLL_PLANE_1;
		 break;
	  case SCR_2_PLANE:
		 Screen = SCROLL_PLANE_2;
		 break;
	  default:
		 return;
   }

   for (i = 0; i < 32*32; i ++)
	  Screen[i] = 32;
}

//////////////////////////////////////////////////////////////////////////////
// SetPalette
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//		PalleteNo - 0-15 the palette no to set
//      Col0-Col3 - The RGB values for the 4 colours within the palette
//////////////////////////////////////////////////////////////////////////////
void SetPalette(u8 Plane, u8 PaletteNo, u16 Col0, u16 Col1, u16 Col2, u16 Col3)
{
   u16 * thePalette;
   u8 Offset = PaletteNo * 4;

   switch (Plane)
   {
	  case SCR_1_PLANE:
		 thePalette = SCROLL_1_PALETTE;
		 break;
	  case SCR_2_PLANE:
		 thePalette = SCROLL_2_PALETTE;
		 break;
      case SPRITE_PLANE:
         thePalette = SPRITE_PALETTE;
         break;
	  default:
		 return;
   }
   
   thePalette[Offset] = Col0;
   thePalette[Offset+1] = Col1;
   thePalette[Offset+2] = Col2;
   thePalette[Offset+3] = Col3;
}


//////////////////////////////////////////////////////////////////////////////
// PutTile
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//		PalleteNo - 0-15 the palette number to set
//      XPos - X Position (0 to 19)
//      YPos - Y Position (0 to 18)
//      TileNo - Tile Number (0 to 511)
//////////////////////////////////////////////////////////////////////////////
void PutTile(u8 Plane, u8 Palette, u8 XPos, u8 YPos, u16 TileNo)
{
   u16 * ScreenPlane1 = SCROLL_PLANE_1;
   u16 * ScreenPlane2 = SCROLL_PLANE_2;

   u16 Offset = ((u16)YPos * 32) + XPos;
   u16 Value = TileNo + ((u16)Palette << 9);

   switch(Plane)
   {
	  case SCR_1_PLANE:
		 ScreenPlane1[Offset] = Value;
		 break;
	  case SCR_2_PLANE:
		 ScreenPlane2[Offset] = Value;
		 break;
	  default:
		 break;
   }
}


//////////////////////////////////////////////////////////////////////////////
// GetTile
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//      XPos - X Position (0 to 19)
//      YPos - Y Position (0 to 18)
// Outputs:
//		PalleteNo - 0-15 the palette number to set
//      TileNo - Tile Number (0 to 511)
//////////////////////////////////////////////////////////////////////////////
void GetTile(u8 Plane, u8 *Palette, u8 XPos, u8 YPos, u16 *TileNo)
{
   u16 * ScreenPlane1 = SCROLL_PLANE_1;
   u16 * ScreenPlane2 = SCROLL_PLANE_2;

   u16 Offset = ((u16)YPos * 32) + XPos;
  
   switch(Plane)
   {
      case SCR_1_PLANE:
	 *Palette = (u8)(ScreenPlane1[Offset] >> 9);
	 *TileNo = (ScreenPlane1[Offset] & 511);
	 break;
      case SCR_2_PLANE:
	 *Palette = (u8)(ScreenPlane2[Offset] >> 9);
	 *TileNo = (ScreenPlane2[Offset] & 511);
	 break;
      default:
	 break;
   }
}


//////////////////////////////////////////////////////////////////////////////
// Sleep
// Waits for specified number of VBlanks (60ths of a second)
//////////////////////////////////////////////////////////////////////////////
void Sleep(u8 VBlanks)
{
   VBCounter = 0;
   while (VBCounter < VBlanks)
      ;
}


//////////////////////////////////////////////////////////////////////////////
// PrintDecimal
// Displays a decimal number of the screen padded with leading zeros
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//		PalleteNo - 0-15 the palette number to set
//      x - X Position (0 to 19)
//      y - Y Position (0 to 18)
//      Value - The number to display
//      Len - The number of tiles to fill
//            e.g. Value = 15, Len = 4 will display 0015
//////////////////////////////////////////////////////////////////////////////
void PrintDecimal(u8 Plane, u8 PaletteNo, u8 x, u8 y, u16 Value, u8 Len)
{
   u8 i;

   for (i = Len; i > 0; i--)
   {
	  PutTile(Plane, PaletteNo, x+i-1, y, '0' + (Value % 10));
	  Value /= 10;
   }
}

//////////////////////////////////////////////////////////////////////////////
// InstallTileSet
// Copies your tile data to the NGPC tileram. Takes a 2D array and a
// length. The length field will equal "sizeof(Tiles)/2" which is the
// number of words in the 2D array.
// This style array can be exported directly from Steve Robb's NeoTile
// NGPC Tile Editing tool
//////////////////////////////////////////////////////////////////////////////
void InstallTileSet(const unsigned short Tiles[][8], u16 Len)
{

   //I want to offset the *custom* tile ram to a new point
   //so that all characters start at *128* rather than 0?
   //What do I have to do...
   //A generic loop variable
   u16 i;
   //TileRam is a pointer to the beginning of tile memory
   u16 * TileRam = TILE_RAM;
   //theTiles is a pointer to the beginning of the Tiles array passed in.
   u16 * theTiles = (u16 *)Tiles;

   //Theoretically, if I change this to Len+128
   TileRam += ((CustomTileBase) * 8);

   //and this to i=128;
   for (i = 0; i < Len; i ++)
   {
        //Then this should work.
        //doesn't seem to do the job though...
	  TileRam[i] = *theTiles++;

   }

}

//////////////////////////////////////////////////////////////////////////////
// SetBackgroundColour
// Changes the background colour of NGPC screen.
// The macro RGB() can be used to specify Col.
// e.g SetBackgroundColour(RGB(15,0,0)); will set the background red
//////////////////////////////////////////////////////////////////////////////
void SetBackgroundColour(u16 Col)
{
   BG_PAL = Col;
   BG_COL = 0x80;
}


//////////////////////////////////////////////////////////////////////////////
// PrintString
// Displays a string on the screen at the specified location
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//		PalleteNo - 0-15 the palette number to set
//      XPos - X Position (0 to 19)
//      YPos - Y Position (0 to 18)
//      theString - The string to be displayed
//////////////////////////////////////////////////////////////////////////////
void PrintString(u8 Plane, u8 Palette, u8 XPos, u8 YPos, const char * theString)
{
   u16 * Screen;

   switch (Plane)
   {
	  case SCR_1_PLANE:
		 Screen = SCROLL_PLANE_1;
		 break;

	  case SCR_2_PLANE:
		 Screen = SCROLL_PLANE_2;
		 break;

	  default:
		 return;
   }

   while (*theString)
   {
	  u16 Offset = ((u16)YPos * 32) + XPos;
	  u16 Value = *theString + ((u16)Palette << 9);
	  Screen[Offset] = Value;

	  theString++;
	  XPos++;
   }
}

//////////////////////////////////////////////////////////////////////////////
// SetSprite
// Initialise a sprite by mapping a tile number and sprite palette no
// to a sprite number and allowing chaining to be set up.
// Inputs:
//		SpriteNo - 0-63 the sprite to move
//      XPos - X Position (0 to 255)
//      YPos - Y Position (0 to 255)
//      Priority - Position in the stack (0 = behind both tile planes, 8 = ?, 16 = between tile planes, 24 = in front of everything)
//////////////////////////////////////////////////////////////////////////////
void SetSprite(u8 SpriteNo, u16 TileNo, u8 Chain, u8 XPos, u8 YPos, u8 PaletteNo, u8 Priority)
{
   u16 SprCtrlReg;
   u8 * theSprite = SPRITE_RAM;
   u8 * theSpriteCol = SPRITE_COLOUR;
   
   theSprite += (SpriteNo * 4);
   theSpriteCol += SpriteNo;

   SprCtrlReg = Priority; // topmost priority
   if (Chain)
      SprCtrlReg += 6; // v and h chaining

   *(theSprite)   = TileNo;
   *(theSprite+1) = SprCtrlReg;
   *(theSprite+2) = XPos;
   *(theSprite+3) = YPos;

   *theSpriteCol = PaletteNo;
}


//////////////////////////////////////////////////////////////////////////////
// SetSpritePosition
// Moves a already initialise sprite
// Inputs:
//		SpriteNo - 0-63 the sprite to move
//      XPos - X Position (0 to 255)
//      YPos - Y Position (0 to 255)
//////////////////////////////////////////////////////////////////////////////
void SetSpritePosition(u8 SpriteNo, u8 XPos, u8 YPos)
{
   u8 * theSprite = SPRITE_RAM;
   theSprite += (SpriteNo * 4);

   *(theSprite+2) = XPos;
   *(theSprite+3) = YPos;
}


//////////////////////////////////////////////////////////////////////////////
// SFX Sound Engine integration
//////////////////////////////////////////////////////////////////////////////

// the pre-assembled Z80 sound effects driver
const unsigned char SFXDriver_Z80[] =
{
   0x31,0xf0,0x0f,0xc3,0x33,0x00,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x3e,0x00,0xfd,0x21,0x50,0x05,0xfd,0x77,0x16,
   0xfd,0x21,0x90,0x05,0xfd,0x77,0x16,0xfd,0x21,0xd0,0x05,0xfd,
   0x77,0x16,0xfd,0x21,0x10,0x06,0xfd,0x77,0x16,0x3a,0x10,0x00,
   0xfe,0x00,0x20,0x22,0x3a,0x11,0x00,0xfe,0x00,0xc2,0x96,0x00,
   0x3a,0x12,0x00,0xfe,0x00,0xc2,0xcc,0x00,0x3a,0x13,0x00,0xfe,
   0x00,0xc2,0x06,0x01,0x3a,0x14,0x00,0xfe,0x00,0xc2,0x55,0x01,
   0x18,0xd7,0x3e,0x00,0x32,0x10,0x00,0x3e,0x00,0xcd,0xb6,0x01,
   0x3e,0x01,0xcd,0xb6,0x01,0x3e,0x02,0xcd,0xb6,0x01,0x3e,0x03,
   0xcd,0xb6,0x01,0xc3,0x58,0x00,0x3e,0x00,0x32,0x11,0x00,0x3a,
   0x20,0x00,0xe6,0x03,0xfe,0x00,0x20,0x06,0xdd,0x21,0x50,0x05,
   0x18,0x18,0xfe,0x01,0x20,0x06,0xdd,0x21,0x90,0x05,0x18,0x0e,
   0xfe,0x02,0x20,0x06,0xdd,0x21,0xd0,0x05,0x18,0x04,0xdd,0x21,
   0x10,0x06,0xfd,0x21,0x20,0x00,0xcd,0x6c,0x01,0xc3,0x60,0x00,
   0xe6,0x1f,0x5f,0x3e,0x00,0xcb,0x23,0xcb,0x23,0xcb,0x23,0xcb,
   0x23,0xce,0x00,0xcb,0x27,0xcb,0x23,0xce,0x00,0x57,0x21,0x50,
   0x06,0x19,0xe5,0xdd,0xe1,0xfd,0x21,0x20,0x00,0x3e,0x00,0xfd,
   0x46,0x00,0xdd,0x70,0x00,0xdd,0x23,0xfd,0x23,0x3c,0xfe,0x13,
   0x20,0xf1,0x3e,0x00,0x32,0x12,0x00,0xc3,0x68,0x00,0xe6,0x1f,
   0x5f,0x3e,0x00,0xcb,0x23,0xcb,0x23,0xcb,0x23,0xcb,0x23,0xce,
   0x00,0xcb,0x27,0xcb,0x23,0xce,0x00,0x57,0x21,0x50,0x06,0x19,
   0xe5,0xfd,0xe1,0xfd,0x7e,0x00,0xe6,0x03,0xfe,0x00,0x20,0x06,
   0xdd,0x21,0x50,0x05,0x18,0x18,0xfe,0x01,0x20,0x06,0xdd,0x21,
   0x90,0x05,0x18,0x0e,0xfe,0x02,0x20,0x06,0xdd,0x21,0xd0,0x05,
   0x18,0x04,0xdd,0x21,0x10,0x06,0xcd,0x6c,0x01,0x3e,0x00,0x32,
   0x13,0x00,0xc3,0x70,0x00,0x3e,0x00,0xcd,0xfa,0x04,0x3e,0x01,
   0xcd,0xfa,0x04,0x3e,0x02,0xcd,0xfa,0x04,0x3e,0x03,0xcd,0xfa,
   0x04,0xc3,0x78,0x00,0xdd,0xe5,0x3e,0x00,0xfd,0x46,0x00,0xdd,
   0x70,0x00,0xfd,0x23,0xdd,0x23,0x3c,0xfe,0x13,0x20,0xf1,0xdd,
   0xe1,0x3e,0x00,0xdd,0x77,0x13,0xdd,0x77,0x14,0xdd,0x77,0x15,
   0xdd,0x7e,0x0d,0xdd,0x77,0x19,0xdd,0x7e,0x0e,0xdd,0x77,0x1c,
   0xdd,0x7e,0x03,0xdd,0x77,0x17,0xdd,0x7e,0x04,0xdd,0x77,0x18,
   0xdd,0x7e,0x05,0xdd,0x77,0x1a,0xdd,0x7e,0x06,0xdd,0x77,0x1b,
   0x3e,0x01,0xdd,0x77,0x16,0xc9,0xfe,0x00,0x20,0x0e,0xfd,0x21,
   0x50,0x05,0x16,0x40,0x1e,0x70,0xdd,0x21,0x00,0x40,0x18,0x30,
   0xfe,0x01,0x20,0x0e,0xfd,0x21,0x90,0x05,0x16,0x00,0x1e,0x10,
   0xdd,0x21,0x01,0x40,0x18,0x1e,0xfe,0x02,0x20,0x0e,0xfd,0x21,
   0xd0,0x05,0x16,0x20,0x1e,0x30,0xdd,0x21,0x01,0x40,0x18,0x0c,
   0xfd,0x21,0x10,0x06,0x16,0x40,0x1e,0x50,0xdd,0x21,0x01,0x40,
   0xfd,0x7e,0x16,0xfe,0x00,0xca,0xdc,0x02,0xfd,0x7e,0x00,0xe6,
   0x03,0xfe,0x00,0x20,0x14,0xfd,0x7e,0x00,0xfe,0x00,0x20,0x04,
   0x3e,0x03,0x18,0x02,0x3e,0x07,0xf6,0x60,0xf6,0x80,0xdd,0x77,
   0x00,0xfd,0x7e,0x0d,0x47,0x3e,0x0f,0x90,0xf6,0x80,0xb3,0xdd,
   0x77,0x00,0xfd,0x4e,0x03,0x79,0xe6,0x0f,0xf6,0x80,0xb2,0xdd,
   0x77,0x00,0xfd,0x7e,0x04,0xe6,0x03,0x47,0xfd,0x7e,0x03,0xcb,
   0x3f,0xcb,0x38,0x30,0x02,0xf6,0x80,0xcb,0x3f,0xcb,0x38,0x30,
   0x02,0xf6,0x80,0xcb,0x3f,0xcb,0x3f,0xdd,0x77,0x00,0xfd,0x7e,
   0x07,0xfe,0x00,0x28,0x33,0x47,0xfd,0x7e,0x14,0x3c,0xfd,0x77,
   0x14,0xb8,0x20,0x28,0x3e,0x00,0xfd,0x77,0x14,0xfd,0x7e,0x08,
   0xfe,0x00,0x28,0x05,0xcd,0x24,0x03,0x18,0x17,0xfd,0x4e,0x05,
   0xfd,0x46,0x06,0xfd,0x6e,0x03,0xfd,0x66,0x04,0x09,0x7c,0xe6,
   0x03,0x67,0xfd,0x75,0x03,0xfd,0x74,0x04,0xfd,0x7e,0x0f,0xfe,
   0x00,0x28,0x29,0x47,0xfd,0x7e,0x15,0x3c,0xfd,0x77,0x15,0xb8,
   0x20,0x1e,0x3e,0x00,0xfd,0x77,0x15,0xfd,0x7e,0x10,0xfe,0x00,
   0x28,0x05,0xcd,0x4f,0x04,0x18,0x0d,0xfd,0x7e,0x0d,0x47,0xfd,
   0x7e,0x0e,0x80,0xe6,0x0f,0xfd,0x77,0x0d,0xfd,0x7e,0x13,0x47,
   0xfd,0x7e,0x01,0xb8,0x20,0x0b,0xfd,0x7e,0x00,0xcd,0xfa,0x04,
   0xcd,0xdd,0x02,0x18,0x07,0xfd,0x7e,0x13,0x3c,0xfd,0x77,0x13,
   0xc9,0xfd,0x7e,0x02,0xfe,0x00,0x28,0x3f,0x3e,0x00,0xfd,0x77,
   0x13,0xfd,0x77,0x14,0xfd,0x77,0x15,0x3e,0x01,0xfd,0x77,0x16,
   0xfd,0x7e,0x19,0xfd,0x77,0x0d,0xfd,0x7e,0x1c,0xfd,0x77,0x0e,
   0xfd,0x7e,0x17,0xfd,0x77,0x03,0xfd,0x7e,0x18,0xfd,0x77,0x04,
   0xfd,0x7e,0x1a,0xfd,0x77,0x05,0xfd,0x7e,0x1b,0xfd,0x77,0x06,
   0xfd,0x7e,0x02,0xfe,0xff,0x28,0x04,0x3d,0xfd,0x77,0x02,0xc9,
   0xdd,0xe5,0xe5,0xd5,0xc5,0xfd,0x7e,0x06,0xe6,0x02,0xfe,0x00,
   0xc2,0xb6,0x03,0xfd,0x6e,0x0b,0xfd,0x66,0x0c,0xfd,0x4e,0x03,
   0xfd,0x46,0x04,0x37,0x3f,0xed,0x42,0xe5,0xdd,0xe1,0xfd,0x4e,
   0x05,0xfd,0x46,0x06,0x37,0x3f,0xed,0x42,0x30,0x4e,0xfd,0x6e,
   0x05,0xfd,0x66,0x06,0xdd,0xe5,0xc1,0x37,0x3f,0xed,0x42,0xfd,
   0x7e,0x08,0xfe,0x01,0x20,0x12,0xe5,0xc1,0xfd,0x6e,0x09,0xfd,
   0x66,0x0a,0x09,0xfd,0x75,0x03,0xfd,0x74,0x04,0xc3,0x41,0x04,
   0xe5,0xc1,0xfd,0x6e,0x0b,0xfd,0x66,0x0c,0x37,0x3f,0xed,0x42,
   0xfd,0x75,0x03,0xfd,0x74,0x04,0x21,0x00,0x04,0xfd,0x4e,0x05,
   0xfd,0x46,0x06,0x37,0x3f,0xed,0x42,0xfd,0x75,0x05,0xfd,0x74,
   0x06,0xc3,0x41,0x04,0xfd,0x6e,0x03,0xfd,0x66,0x04,0xfd,0x4e,
   0x05,0xfd,0x46,0x06,0x09,0xfd,0x75,0x03,0xfd,0x74,0x04,0xc3,
   0x41,0x04,0xfd,0x6e,0x03,0xfd,0x66,0x04,0xfd,0x4e,0x09,0xfd,
   0x46,0x0a,0x37,0x3f,0xed,0x42,0xe5,0xdd,0xe1,0x21,0x00,0x04,
   0xfd,0x4e,0x05,0xfd,0x46,0x06,0x37,0x3f,0xed,0x42,0xe5,0xd1,
   0xdd,0xe5,0xe1,0xd5,0xc1,0x37,0x3f,0xed,0x42,0x30,0x48,0xd5,
   0xe1,0xdd,0xe5,0xc1,0x37,0x3f,0xed,0x42,0xfd,0x7e,0x08,0xfe,
   0x01,0x20,0x15,0xe5,0xc1,0xfd,0x6e,0x0b,0xfd,0x66,0x0c,0x37,
   0x3f,0xed,0x42,0xfd,0x75,0x03,0xfd,0x74,0x04,0xc3,0x41,0x04,
   0xfd,0x4e,0x09,0xfd,0x46,0x0a,0x09,0xfd,0x75,0x03,0xfd,0x74,
   0x04,0x21,0x00,0x04,0xfd,0x4e,0x05,0xfd,0x46,0x06,0x37,0x3f,
   0xed,0x42,0xfd,0x75,0x05,0xfd,0x74,0x06,0xc3,0x41,0x04,0xfd,
   0x6e,0x03,0xfd,0x66,0x04,0xfd,0x4e,0x05,0xfd,0x46,0x06,0x09,
   0xfd,0x75,0x03,0xfd,0x74,0x04,0xc3,0x41,0x04,0xfd,0x7e,0x04,
   0xe6,0x03,0xfd,0x77,0x04,0xc1,0xd1,0xe1,0xdd,0xe1,0xc9,0xd5,
   0xc5,0xfd,0x7e,0x0e,0xe6,0x08,0xfe,0x00,0xc2,0xa2,0x04,0xfd,
   0x7e,0x12,0xfd,0x4e,0x0d,0x91,0x47,0xfd,0x4e,0x0e,0x37,0x3f,
   0x99,0x30,0x2a,0xfd,0x7e,0x0e,0x48,0x91,0x57,0xfd,0x7e,0x10,
   0xfe,0x01,0x20,0x0a,0xfd,0x7e,0x11,0x82,0xfd,0x77,0x0d,0xc3,
   0xef,0x04,0xfd,0x7e,0x12,0x92,0xfd,0x77,0x0d,0x3e,0x10,0xfd,
   0x4e,0x0e,0x91,0xfd,0x77,0x0e,0xc3,0xef,0x04,0xfd,0x7e,0x0d,
   0xfd,0x4e,0x0e,0x81,0xfd,0x77,0x0d,0xc3,0xef,0x04,0xfd,0x7e,
   0x0d,0xfd,0x4e,0x11,0x91,0x47,0x3e,0x10,0xfd,0x4e,0x0e,0x91,
   0x57,0x4a,0x78,0x37,0x3f,0x99,0x30,0x2a,0x7a,0x48,0x91,0x5f,
   0xfd,0x7e,0x10,0xfe,0x01,0x20,0x0b,0xfd,0x7e,0x12,0x4b,0x91,
   0xfd,0x77,0x0d,0xc3,0xef,0x04,0xfd,0x7e,0x11,0x4b,0x81,0xfd,
   0x77,0x0d,0x3e,0x10,0xfd,0x4e,0x0e,0x91,0xfd,0x77,0x0e,0xc3,
   0xef,0x04,0xfd,0x7e,0x0d,0xfd,0x4e,0x0e,0x81,0xfd,0x77,0x0d,
   0xc3,0xef,0x04,0xfd,0x7e,0x0d,0xe6,0x0f,0xfd,0x77,0x0d,0xc1,
   0xd1,0xc9,0xdd,0xe5,0xfd,0xe5,0xd5,0xc5,0xe6,0x03,0xfe,0x00,
   0x20,0x0c,0xdd,0x21,0x00,0x40,0x06,0x70,0xfd,0x21,0x50,0x05,
   0x18,0x2a,0xfe,0x01,0x20,0x0c,0xdd,0x21,0x01,0x40,0x06,0x10,
   0xfd,0x21,0x90,0x05,0x18,0x1a,0xfe,0x02,0x20,0x0c,0xdd,0x21,
   0x01,0x40,0x06,0x30,0xfd,0x21,0xd0,0x05,0x18,0x0a,0xdd,0x21,
   0x01,0x40,0x06,0x50,0xfd,0x21,0x10,0x06,0x78,0xf6,0x80,0xf6,
   0x0f,0xdd,0x77,0x00,0x3e,0x00,0xfd,0x77,0x16,0xc1,0xd1,0xfd,
   0xe1,0xdd,0xe1,0xc9
};


void BlockCopy(u8 * Dest, const u8 * Source, u16 n)
{
   u8 *p1, *p2;

   p1 = Dest;
   p2 = (u8 *)Source;
   
   while (n--)
	  *p1++ = *p2++;
}



//////////////////////////////////////////////////////////////////////////////
// InstallSoundDriver
// To be called once at the start of the code. It copies the Z80 pre
// assembled SFX driver into the Z80 memory space.
//////////////////////////////////////////////////////////////////////////////
void InstallSoundDriver(void)
{
   u8 * pZ80RAM = Z80_RAM;
   u8 i;

   // first stop the z80
   SOUNDCPU_CTRL = 0xAAAA;

   // copy driver
   BlockCopy(pZ80RAM, SFXDriver_Z80, sizeof(SFXDriver_Z80));

   // restart z80
   SOUNDCPU_CTRL = 0x5555;

   // allows the VBI interrupt to start pulsing the Z80
   SFXInstalled = 1;
}


//////////////////////////////////////////////////////////////////////////////
// InstallSounds
// Copies the user defined sounds to the Z80 memory
//////////////////////////////////////////////////////////////////////////////
void InstallSounds(const SOUNDEFFECT SoundData[], u8 NumSounds)
{
   u8 Preset;
   u8 i;
   u8 * SFXBuff1 = (u8 *)0x7020;
   u8 * SFXBuff2 = (u8 *)0x7023;
   
   for (Preset = 0; Preset < NumSounds; Preset++)
   {
	   // copy SFX
	  u8 * SData = (u8 *)&SoundData[Preset];

	  // this is yuk but I couldn't find a way to stop the C compiler
	  // doing word alignment on the SoundEffects structure. This
	  // removes the excess padding.
	  BlockCopy(SFXBuff1, SData, 3);
	  SData+=4;
	  BlockCopy(SFXBuff2, SData, sizeof(SOUNDEFFECT)-4);

	  // instruct driver to store it
	  SFXLoadPreset = Preset+1;

	   // wait for completion
	  while(SFXLoadPreset)
		 ;
   }
}

//////////////////////////////////////////////////////////////////////////////
// PlaySound
// Plays one of the user defined sounds
// Inputs:
//          SoundNumber - the sound preset to play (range 1 to 31)
//
//////////////////////////////////////////////////////////////////////////////
void PlaySound(u8 SoundNumber)
{
   SFXPlayPreset = SoundNumber;
}

//////////////////////////////////////////////////////////////////////////////
// StopAllSound
// Call this to stop the sound playing in all NGPC sound channels
//////////////////////////////////////////////////////////////////////////////
void StopAllSound(void)
{
   SFXStopAll = 1;
}



#define RAND_MAX 32767
volatile u32 RAND_RandomData;

//////////////////////////////////////////////////////////////////////////////
// SeedRandom
// Unfortunately we need a pseudo random number in order to seed our
// pseudo random number generator otherwise the numbers would always
// come out the same.
// To solve this I use VBCounter that is incremented every VB
// Interrupt. If this function was called at initialisation the same
// problem would arrise as VBCounter will have always hit the same
// value.
// Call SeedRandom just after the user has pressed fire/start from the
// title screen of your game.
//////////////////////////////////////////////////////////////////////////////
void SeedRandom(void)
{
   RAND_RandomData = VBCounter;
   GetRandom(100);
}



// helper function as there is no 32bit multiply natively supported by
// the compiler :-0
s32 Multiply32bit(s32 Value1, s32 Value2)
{
   __ASM("  LD      XWA, (XSP+4)");
   __ASM("  LD      XBC, (XSP+8)");
   __ASM("  LD		HL,QWA");
   __ASM("  MUL		XHL,BC");
   __ASM("  LD		DE,QBC");
   __ASM("  MUL		XDE,WA");
   __ASM("  ADD		XHL,XDE");
   __ASM("  LD		QHL,HL");
   __ASM("  LD		HL,0");
   __ASM("  MUL		XWA,BC");
   __ASM("  ADD		XHL,XWA");

   return __XHL;
}

//////////////////////////////////////////////////////////////////////////////
// GetRandom
// Returns a pseudo random number in the range of 0 to the specified
// input value.
// Inputs:
//     Value = the upper limit for the random number (must be less than 32767)
//////////////////////////////////////////////////////////////////////////////
u16 GetRandom(u16 Value)
{
   RAND_RandomData = Multiply32bit(RAND_RandomData,20077);
   RAND_RandomData += 12345;
   
   return (Multiply32bit(((RAND_RandomData >> 16) & RAND_MAX), Value) >> 15);
}

