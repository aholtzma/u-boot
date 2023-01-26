
#include <common.h>
#include <command.h>

static int do_rgbled(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	printf("RGB LED test\n");

	return 0;
}

U_BOOT_CMD(
	rgbled,    4,    0,     do_rgbled,
	"set the indication LED color",
	"red green blue - set the color of the LED with each value between 0 and 4095"
);
