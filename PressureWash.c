
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c

#include "title_nam.h"

// CHR data
//#resource "tileset.chr"
//#link "tileset.s"


#define FP_BITS  4

/*{pal:"nes",layout:"nes"}*/
const unsigned char palTitle[16]={ 0x0f,0x0f,0x0f,0x0f,0x0f,0x1c,0x2c,0x3c,0x0f,0x12,0x22,0x32,0x0f,0x14,0x24,0x34 };

// Everything after here put into Zeropage for efficiency
#pragma bss-name(push,"ZEROPAGE")
#pragma data-name(push,"ZEROPAGE")

static unsigned char i;
static unsigned char frame_cnt;
static unsigned char bright;
static unsigned char wait;
static int iy,dy;


/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x0F,			// screen color

  0x11,0x31,0x30,0x00,	// background palette 0
  0x1C,0x20,0x2C,0x00,	// background palette 1
  0x00,0x10,0x20,0x00,	// background palette 2
  0x06,0x16,0x26,0x00,   // background palette 3

  0x16,0x35,0x24,0x00,	// sprite palette 0
  0x00,0x37,0x25,0x00,	// sprite palette 1
  0x0D,0x2D,0x3A,0x00,	// sprite palette 2
  0x0D,0x27,0x2A	// sprite palette 3
};

/*{pal:"nes",layout:"nes"}*/
//const char TITLE[16] = {
//  0xA6, 0xA7, 0x00,
//  0x53,0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//  0x00, 0xA4, 0xA5
//};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}

//smoothly fade current bright to the given value
//in case when to=0 it also stops the music,
//turns the display off, reset vram update and scroll

void pal_fade_to(unsigned to)
{
  //if(!to) music_stop();

  while(bright!=to) {
    delay(4);
    if(bright<to) ++bright; else --bright;
    pal_bright(bright);
  }

  if(!bright) {
    ppu_off();
    set_vram_update(NULL);
    scroll(0,0);
  }
}


void title_screen(void) {
  
  scroll(-8,240);//title is aligned to the color attributes, so shift it a bit to the right

  vram_adr(NAMETABLE_A);
  vram_unrle(title_nam);
  vram_adr(NAMETABLE_C);//clear second nametable, as it is visible in the jumping effect
  vram_fill(0,1024);
  
  pal_bg(palTitle);
  pal_bright(4);
  ppu_on_bg();
  delay(20);//delay just to make it look better
  
  iy=240<<FP_BITS;
  dy=-8<<FP_BITS;
  frame_cnt=0;
  wait=160;
  bright=4;
  
  // Wait for start, and bounce logo
  while(1)
  {
    ppu_wait_frame();

    scroll(-8,iy>>FP_BITS);

    if(pad_trigger(0)&PAD_START) break;

    iy+=dy;

    if(iy<0)
    {
      iy=0;
      dy=-dy>>1;
    }

    if(dy>(-8<<FP_BITS)) dy-=2;

    if(wait)
    {
      --wait;
    }
    else
    {
      pal_col(2,(frame_cnt&32)?0x0f:0x20);//blinking press start text
      ++frame_cnt;
    }
  }

  scroll(-8,0);//if start is pressed, show the title at whole
  //sfx_play(SFX_START,0);

  for(i=0;i<16;++i)//and blink the text faster
  {
    pal_col(2,i&1?0x0f:0x20);
    delay(4);
  }

  pal_fade_to(0);
}

void main(void)
{
  setup_graphics();
  // Title
  //vram_adr(NTADR_A(10,10));
  //vram_write("SPRAY AWAY", 10);
  
  //vram_adr(NTADR_A(10,14));
  //vram_write("Press Start", 11);
  
  // enable rendering
  //ppu_on_all();
  
  // infinite loop
  while(1) {
    title_screen();
  }
}
