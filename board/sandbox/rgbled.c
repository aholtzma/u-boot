#include <common.h>
#include <command.h>
#include <asm/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> 

// Setup
const float CLK_F = 200e3; // 200kHz

// define struct
struct TLC5973 {
    int pin;
};

// methods
void gpio_connect(struct TLC5973 *gpio) {
    gpio_request(gpio->pin, "LED serial");
}
    
void set_value(struct TLC5973 *gpio, int val){    
    gpio_direction_output(gpio->pin, val);
}    

//
void output_bit(int bit, struct TLC5973 *gpio){
    float CLK_T = 1.0/CLK_F;
    float clk_pw = CLK_T/8.0;
    float pw_usecs = clk_pw *1e6;
    //char* sdi_levels[] = {"low", "high"};
    //CLK
    set_value(gpio, 1);  
    udelay(pw_usecs);              
    set_value(gpio, 0); 
    //DATA
    udelay(2.5 * pw_usecs);                    
    set_value(gpio, bit); 
    //printf("SDI %s \n", sdi_levels[bit]);
    udelay(pw_usecs);
    set_value(gpio, 0);   
    udelay(3.5 * pw_usecs);
}

//
void write_bits(int x, int n_bits, struct TLC5973 *chan) {
    int i = 0;
    for (i = 0; i < n_bits; i++){
        int out_bit = (x >> (n_bits - i - 1)) & 1;
        output_bit(out_bit, chan);  
    }
}

//
int output_rgb(int g, int r, int b, struct TLC5973 *chan) {
    int outs[3] = {g,r,b};
    int i = 0;
    for (i=0; i< 3; i++){
        if (outs[i] < 0 || outs[i] > 4095){
          return -1;
        }
    }
    write_bits(0x3aa, 12, chan); // data write command
    write_bits(g, 12, chan);
    write_bits(r, 12, chan);
    write_bits(b, 12, chan);
    printf("Done \n");
    return 0;
}

static int do_rgbled(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    struct TLC5973 gpio23 = {23}; //instance of struct
    gpio_connect(&gpio23);

    //printf("RGB LED test! \nargs: %s %s %s \n", argv[1], argv[2], argv[3]);
    int red = (4095 * (int)simple_strtol(argv[1], NULL, 10) / 1000);
    int green = (4095 * (int)simple_strtol(argv[2], NULL, 10) / 1000);
    int blue = (4095 * (int)simple_strtol(argv[3], NULL, 10) / 1000);
    //printf("red %d \ngreen: %d \nblue: %d\n", red, green, blue);

    output_rgb(green, red, blue, &gpio23);
    return 0;
}

U_BOOT_CMD(
	rgbled,    4,    0,     do_rgbled,
	"set the indication LED color",
	"red green blue - set the color of the LED with each value between 0 and 1"
);
