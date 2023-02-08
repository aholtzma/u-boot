
#include <common.h>
#include <command.h>
#include <asm/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> 
//include <math.h>
#define MICRO_TO_NANO 1000000 // used for nanosleep

// Setup
const float CLK_F = 200e3; // 200kHz
const float CLK_T = 1.0/CLK_F;

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
void nanosleep(int nsecs); //prototype
double atof(const char *str); //prototype

//
void output_bit(int bit, struct TLC5973 *gpio){
    float clk_pw = CLK_T/8.0;
    float pw_usecs = clk_pw *1e6;
    char* sdi_levels[] = {"low", "high"};
    //CLK
    set_value(gpio, 1);  
    nanosleep(pw_usecs * MICRO_TO_NANO);              
    set_value(gpio, 0); 
    //DATA
    nanosleep(2.5 * pw_usecs * MICRO_TO_NANO);                    
    set_value(gpio, bit); 
    printf("SDI %s \n", sdi_levels[bit]);
    nanosleep(pw_usecs * MICRO_TO_NANO);
    set_value(gpio, 0);   
    nanosleep(3.5 * pw_usecs * MICRO_TO_NANO);
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

// int* rgb_convert(char* const* vecstr, int size, float out_array){
//     int i = 1; 
//     for (i = 1; i < size; i++){
//         float num = atof(vecstr[i]);
//         int num2 = roundup(num * 4095.0, 0);
//         out_array[(i-1)] = num2;
//     }
//     return out_array;
// }

int rgb_code_convert(float x){
    int x2 = roundup(x * 4095.0, 0);
    return x2;
}

///////
// void main(int argc, char *const argv[]) {
//     printf("hello world!! \n");
// }

static int do_rgbled(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	printf("RGB LED test\n");

    struct TLC5973 gpio23 = {23}; //instance of struct
    //gpio23.pin = 23;
    gpio_connect(&gpio23);

    printf("args: %s %s %s \n", argv[1], argv[2], argv[3]);

    // float args_new[3];
    // args_new = rgb_convert(argv, argc, *args_new);
    // int red4 = args_new[0];
    // int green4 = args_new[1];
    // int blue4 = args_new[2];
    // printf("red4: %d \ngreen4: %d \nblue4: %d\n", red4,green4,blue4);

    double red0 = atof(argv[1]);
    double green0 = atof(argv[2]);
    double blue0 = atof(argv[3]);
    printf("red0: %f \ngreen0: %f \nblue0: %f\n", red0,green0,blue0);

    int red1 = rgb_code_convert(red0);
    int green1 = rgb_code_convert(green0);
    int blue1 = rgb_code_convert(blue0);
    printf("red1 %d \ngreen1: %d \nblue1: %d\n", red1,green1,blue1);

    int red = rgb_code_convert(atof(argv[1]));
    int green = rgb_code_convert(atof(argv[2]));
    int blue = rgb_code_convert(atof(argv[3]));
    printf("red: %d \ngreen: %d \nblue: %d", red, green, blue);

    output_rgb(green, red, blue, &gpio23);

    assert(output_rgb(green, red, blue, &gpio23) == 0);
	return 0;
}

U_BOOT_CMD(
	rgbled,    4,    0,     do_rgbled,
	"set the indication LED color",
	"red green blue - set the color of the LED with each value between 0 and 1"
);