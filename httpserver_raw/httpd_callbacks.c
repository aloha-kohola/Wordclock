#include "driver/ws2812b.h"
#include "wordclock.h"
#include "httpserver_raw/httpd.h"
#include "httpserver_raw/httpd_callbacks.h"

#include <string.h>

//CGI Handler
static const char * HTTP_Set_FG_Color_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const char * HTTP_Set_BG_Color_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const char * HTTP_Set_Power_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const tCGI CGI_Handler_FG = {"/FG.cgi", HTTP_Set_FG_Color_Handler};
static const tCGI CGI_Handler_BG = {"/BG.cgi", HTTP_Set_BG_Color_Handler};
static const tCGI CGI_Handler_PW = {"/PW.cgi", HTTP_Set_Power_Handler};
static tCGI CGI_Handler[3];

//SSI Handler
u16_t HTTP_Status_Handler(int iIndex, char *pcInsert, int iInsertLen);
// char const* TAGCHAR1 = "p";
// char const* TAGCHAR2 = "f";
char const* TAGCHAR[2] = {"p", "f"};
char const** TAGS = TAGCHAR;

//CGI Set Foreground Color Handler of HTTP-Server
static const char * HTTP_Set_FG_Color_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    //printf("%s\n", "CGI Handler");
    if (iIndex == 0) {
        //printf("%s\n", "CGI Handler");
        if (strcmp(pcParam[0], "fg") == 0){
            //printf("%s\n", "fg");
            char* color_c = pcValue[0];
            char* ptr;
            uint32_t color = strtol(color_c, &ptr, 16);
            //printf("%x\n", color);
            struct rgb fg;
            fg.red = ((color & 0x00FF0000) >> 16);
            fg.green = ((color & 0x0000FF00) >> 8);
            fg.blue = (color & 0x000000FF);

            wordclock_set_fg_color(&fg);
        }
    }

    return "/index.html";
}

static const char * HTTP_Set_BG_Color_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    if (iIndex == 1) {
        //printf("%s\n", "CGI Handler");
        if (strcmp(pcParam[0], "bg") == 0){
            //printf("%s\n", "fg");
            char* color_c = pcValue[0];
            char* ptr;
            uint32_t color = strtol(color_c, &ptr, 16);
            //printf("%x\n", color);
            struct rgb bg;
            bg.red = ((color & 0x00FF0000) >> 16);
            bg.green = ((color & 0x0000FF00) >> 8);
            bg.blue = (color & 0x000000FF);

            wordclock_set_bg_color(&bg);
        }
    }

    return "/index.html";
}

static const char* HTTP_Set_Power_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
	if (iIndex == 2) {
        if(strcmp(pcParam[0], "power") == 0){

            static struct rgb fg_save;
            static struct rgb bg_save;

            static uint8_t power_status_flag = 1;

            if((strcmp(pcValue[0], "ON") == 0) && (power_status_flag == 0)){
            	// fg.red = 0x10;
            	// fg.green = 0x10;
            	// fg.blue = 0x10;
            	wordclock_set_fg_color(&fg_save);
                // bg.red = 0x00;
                // bg.green = 0x00;
                // bg.blue = 0x00;
                wordclock_set_bg_color(&bg_save);
                power_status_flag = 1;
            }
            if((strcmp(pcValue[0], "OFF") == 0) && (power_status_flag == 1)){

                struct rgb fg;
                struct rgb bg;

                fg_save.red = wordclock_get_fg_color()->red;
                fg_save.green = wordclock_get_fg_color()->green;
                fg_save.blue = wordclock_get_fg_color()->blue;
                bg_save.red = wordclock_get_bg_color()->red;
                bg_save.green = wordclock_get_bg_color()->green;
                bg_save.blue = wordclock_get_bg_color()->blue;

            	fg.red = 0x00;
            	fg.green = 0x00;
            	fg.blue = 0x00;
            	wordclock_set_fg_color(&fg);
                bg.red = 0x00;
                bg.green = 0x00;
                bg.blue = 0x00;
                wordclock_set_bg_color(&bg);
                power_status_flag = 0;
            }
        }
    }

    return "/index.html";
}

//SSI Handler Implementation
u16_t HTTP_Status_Handler(int iIndex, char *pcInsert, int iInsertLen){
	//printf("SSI_Handler\n");
	if(iIndex == 0){
		struct rgb* fg = wordclock_get_fg_color();
		if(fg->red == 0 && fg->green == 0 && fg->blue == 0){
			*pcInsert = '0';
		}else{
			*pcInsert = '1';
		}
		return 1;
	}
    if(iIndex == 1){
        struct rgb* fg = wordclock_get_fg_color();
        uint32_t c_temp = 0;
        c_temp = (((uint32_t)fg->red << 16) & 0x00FF0000) | (((uint32_t)fg->green << 8) & 0x0000FF00) | ((((uint32_t)fg->blue) & 0x000000FF));

        char c_buffer[20] = {};

        //itoa(c_temp, c_buffer, 16);
        sprintf(c_buffer, "%06x", c_temp);
        //printf("%s\n", c_buffer);

        *pcInsert = c_buffer[0];
        *(pcInsert + 1) = c_buffer[1];
        *(pcInsert + 2) = c_buffer[2];
        *(pcInsert + 3) = c_buffer[3];
        *(pcInsert + 4) = c_buffer[4];
        *(pcInsert + 5) = c_buffer[5];

        return 6;
    }
	// *pcInsert = '1';
 //    *(pcInsert+1) = '1';
	return 0;
}

void httpd_init_cgi_handler(void){
	CGI_Handler[0] = CGI_Handler_FG;
	CGI_Handler[1] = CGI_Handler_BG;
    CGI_Handler[2] = CGI_Handler_PW;
    http_set_cgi_handlers(CGI_Handler, 3);
}

void httpd_init_ssi_handler(void){
	// TAGS[0] = TAGCHAR1;
	// TAGS[1] = TAGCHAR2;
	http_set_ssi_handler(HTTP_Status_Handler, (char const **)TAGS, 2);
}