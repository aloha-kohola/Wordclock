#include "driver/ws2812b.h"
#include "espressif/esp_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "time.h"
#include "wordclock.h"

#define NUM_ROW 10
#define NUM_COL 11

#define NUM_LEDS (NUM_ROW*NUM_COL+4)

//map leds on field of colors
static struct rgb wclock[NUM_LEDS] = {};

//Foreground color
static struct rgb fg = {};

//Background color
static struct rgb bg = {};

static void set_letter_led_to_fg(uint8_t led){
	wclock[led].red = fg.red;
	wclock[led].green = fg.green;
	wclock[led].blue = fg.blue;

	//printf("LED: %d\n", led);
}

static void set_all_letters_to_bg(){
	for(int i = 0; i < NUM_LEDS; i++) {
		wclock[i].red = bg.red;
		wclock[i].green = bg.green;
		wclock[i].blue = bg.blue;
	}
}

static void clear_wordclock(void)
{
	for(int i = 0; i < NUM_LEDS; i++) {
		wclock[i].red = 0;
		wclock[i].green = 0;
		wclock[i].blue = 0;
	}
}

static void es_ist(void)
{
	//ES
	set_letter_led_to_fg(103);
	set_letter_led_to_fg(104);

	//IST
	set_letter_led_to_fg(106);
	set_letter_led_to_fg(107);
	set_letter_led_to_fg(108);
}

static void fuenf(void)
{
	//FÜNF
	set_letter_led_to_fg(110);
	set_letter_led_to_fg(111);
	set_letter_led_to_fg(112);
	set_letter_led_to_fg(113);
}

static void zehn(void)
{
	//ZEHN
	set_letter_led_to_fg(99);
	set_letter_led_to_fg(100);
	set_letter_led_to_fg(101);
	set_letter_led_to_fg(102);
}

static void zwanzig(void)
{
	//ZWANZIG
	set_letter_led_to_fg(92);
	set_letter_led_to_fg(93);
	set_letter_led_to_fg(94);
	set_letter_led_to_fg(95);
	set_letter_led_to_fg(96);
	set_letter_led_to_fg(97);
	set_letter_led_to_fg(98);
}

static void dreiviertel(void)
{
	//DREIVIERTEL
	set_letter_led_to_fg(81);
	set_letter_led_to_fg(82);
	set_letter_led_to_fg(83);
	set_letter_led_to_fg(84);
	set_letter_led_to_fg(85);
	set_letter_led_to_fg(86);
	set_letter_led_to_fg(87);
	set_letter_led_to_fg(88);
	set_letter_led_to_fg(89);
	set_letter_led_to_fg(90);
	set_letter_led_to_fg(91);
}

static void viertel(void)
{
	//VIERTEL
	set_letter_led_to_fg(85);
	set_letter_led_to_fg(86);
	set_letter_led_to_fg(87);
	set_letter_led_to_fg(88);
	set_letter_led_to_fg(89);
	set_letter_led_to_fg(90);
	set_letter_led_to_fg(91);
}

static void nach(void)
{
	//NACH
	set_letter_led_to_fg(75);
	set_letter_led_to_fg(76);
	set_letter_led_to_fg(77);
	set_letter_led_to_fg(78);
}

static void vor(void)
{
	//VOR
	set_letter_led_to_fg(72);
	set_letter_led_to_fg(73);
	set_letter_led_to_fg(74);
}

static void halb(void)
{
	//HALB
	set_letter_led_to_fg(59);
	set_letter_led_to_fg(60);
	set_letter_led_to_fg(61);
	set_letter_led_to_fg(62);
}

static void h_eins(void)
{
	//EINS
	set_letter_led_to_fg(53);
	set_letter_led_to_fg(54);
	set_letter_led_to_fg(55);
	set_letter_led_to_fg(56);
}

static void h_ein(void)
{
	//EIN (Es ist ein Uhr)
	set_letter_led_to_fg(54);
	set_letter_led_to_fg(55);
	set_letter_led_to_fg(56);
}

static void h_zwei(void)
{
	//ZWEI
	set_letter_led_to_fg(55);
	set_letter_led_to_fg(56);
	set_letter_led_to_fg(57);
	set_letter_led_to_fg(58);
}

static void h_drei(void)
{
	//DREI
	set_letter_led_to_fg(38);
	set_letter_led_to_fg(39);
	set_letter_led_to_fg(40);
	set_letter_led_to_fg(41);
}

static void h_vier(void)
{
	//VIER
	set_letter_led_to_fg(26);
	set_letter_led_to_fg(27);
	set_letter_led_to_fg(28);
	set_letter_led_to_fg(29);
}

static void h_fuenf(void)
{
	//FÜNF
	set_letter_led_to_fg(44);
	set_letter_led_to_fg(45);
	set_letter_led_to_fg(46);
	set_letter_led_to_fg(47);
}

static void h_sechs(void)
{
	//SECHS
	set_letter_led_to_fg(9);
	set_letter_led_to_fg(10);
	set_letter_led_to_fg(11);
	set_letter_led_to_fg(12);
	set_letter_led_to_fg(13);
}

static void h_sieben(void)
{
	//SIEBEN
	set_letter_led_to_fg(48);
	set_letter_led_to_fg(49);
	set_letter_led_to_fg(50);
	set_letter_led_to_fg(51);
	set_letter_led_to_fg(52);
	set_letter_led_to_fg(53);
}

static void h_acht(void)
{
	//ACHT
	set_letter_led_to_fg(16);
	set_letter_led_to_fg(17);
	set_letter_led_to_fg(18);
	set_letter_led_to_fg(19);
}

static void h_neun(void)
{
	//NEUN
	set_letter_led_to_fg(30);
	set_letter_led_to_fg(31);
	set_letter_led_to_fg(32);
	set_letter_led_to_fg(33);
}

static void h_zehn(void)
{
	//ZEHN
	set_letter_led_to_fg(20);
	set_letter_led_to_fg(21);
	set_letter_led_to_fg(22);
	set_letter_led_to_fg(23);
}

static void h_elf(void)
{
	//ELF
	set_letter_led_to_fg(34);
	set_letter_led_to_fg(35);
	set_letter_led_to_fg(36);
}

static void h_zwoelf(void)
{
	//ZWÖLF
	set_letter_led_to_fg(64);
	set_letter_led_to_fg(65);
	set_letter_led_to_fg(66);
	set_letter_led_to_fg(67);
	set_letter_led_to_fg(68);
}

static void uhr(void)
{
	//UHR
	set_letter_led_to_fg(4);
	set_letter_led_to_fg(5);
	set_letter_led_to_fg(6);
}

static void minute_1(void)
{
	set_letter_led_to_fg(0);
}

static void minute_2(void)
{
	set_letter_led_to_fg(1);
}

static void minute_3(void)
{
	set_letter_led_to_fg(2);
}

static void minute_4(void)
{
	set_letter_led_to_fg(3);
}

void wordclock_init(void)
{
	//set forground color to white
	fg.red = 30;
	fg.green = 30;
	fg.blue = 30;

	//set background color to white
	bg.red = 0;
	bg.green = 0;
	bg.blue = 0;
}

void wordclock_show(time_t current_time)
{

	struct tm* tm;
	tm = localtime(&current_time);

	clear_wordclock();
	set_all_letters_to_bg();

	es_ist();

	int hour = tm->tm_hour % 12;
	int minute = tm->tm_min % 5;

	if((tm->tm_min >= 0) && (tm->tm_min <= 4)) {
		// keine Minuten zum Anzeigen
		uhr();
	}
	if((tm->tm_min >= 5) && (tm->tm_min <= 9)) {
		fuenf();
		nach();
	}
	if((tm->tm_min >= 10) && (tm->tm_min <= 14)) {
		zehn();
		nach();
	}
	if((tm->tm_min >= 15) && (tm->tm_min <= 19)) {
		viertel();
		hour += 1;
	}
	if((tm->tm_min >= 20) && (tm->tm_min <= 24)) {
		zwanzig();
		nach();
	}
	if((tm->tm_min >= 25) && (tm->tm_min <= 29)) {
		fuenf();
		vor();
		halb();
		hour += 1;
	}
	if((tm->tm_min >= 30) && (tm->tm_min <= 34)) {
		halb();
		hour += 1;
	}
	if((tm->tm_min >= 35) && (tm->tm_min <= 39)) {
		fuenf();
		nach();
		halb();
		hour += 1;
	}
	if((tm->tm_min >= 40) && (tm->tm_min <= 44)) {
		zehn();
		nach();
		halb();
		hour += 1;
	}
	if((tm->tm_min >= 45) && (tm->tm_min <= 49)) {
		dreiviertel();
		hour += 1;
	}
	if((tm->tm_min >= 50) && (tm->tm_min <= 54)) {
		zehn();
		vor();
		hour += 1;
	}
	if((tm->tm_min >= 55) && (tm->tm_min <= 59)) {
		fuenf();
		vor();
		hour += 1;
	}

	switch(minute) {
		case 1:
			minute_1();
			break;
		case 2:
			minute_2();
			break;
		case 3:
			minute_3();
			break;
		case 4:
			minute_4();
			break;
		default:
			break;
	}

	switch(hour) {
		case 0:
			h_zwoelf();
			break;
		case 1:
			if((tm->tm_min >= 0) && (tm->tm_min <= 4)) {
				h_ein();
			}else{
				h_eins();
			}
			break;
		case 2:
			h_zwei();
			break;
		case 3:
			h_drei();
			break;
		case 4:
			h_vier();
			break;
		case 5:
			h_fuenf();
			break;
		case 6:
			h_sechs();
			break;
		case 7:
			h_sieben();
			break;
		case 8:
			h_acht();
			break;
		case 9:
			h_neun();
			break;
		case 10:
			h_zehn();
			break;
		case 11:
			h_elf();
			break;
		case 12:
			h_zwoelf();
			break;
		default:
			break;
	}

	ws2812b_show(wclock, NUM_LEDS);

	// printf("Start wordclock with %d leds.\n", NUM_LEDS);

	// clear_wordclock(); es_ist(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); fuenf(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); zehn(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); zwanzig(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); dreiviertel(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); viertel(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); nach(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); vor(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); halb(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_eins(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_zwei(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_drei(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_vier(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_fuenf(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_sechs(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_sieben(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_acht(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_neun(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_zehn(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_elf(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); h_zwoelf(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
	// clear_wordclock(); uhr(); ws2812b_show(clock, NUM_LEDS); vTaskDelay(2000 / portTICK_RATE_MS);
}

void wordclock_set_fg_color(struct rgb* color)
{
	fg.red = color->red;
	fg.green = color->green;
	fg.blue = color->blue;
}

struct rgb* wordclock_get_fg_color(void)
{
	return &fg;
}

void wordclock_set_bg_color(struct rgb* color){
	bg.red = color->red;
	bg.green = color->green;
	bg.blue = color->blue;
}

struct rgb* wordclock_get_bg_color(void)
{
	return &bg;
}