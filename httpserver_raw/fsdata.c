#include "fs.h"
#include "lwip/def.h"
#include "fsdata.h"

static const unsigned char data_404_html[] = {
	/* /404.html */
	0x2f, 0x34, 0x30, 0x34, 0x2e, 0x68, 0x74, 0x6d, 0x6c, 0,
	0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x34, 
	0x30, 0x34, 0x20, 0x46, 0x69, 0x6c, 0x65, 0x20, 0x6e, 0x6f, 
	0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0xd, 0xa, 0x53, 
	0x65, 0x72, 0x76, 0x65, 0x72, 0x3a, 0x20, 0x6c, 0x77, 0x49, 
	0x50, 0x2f, 0x70, 0x72, 0x65, 0x2d, 0x30, 0x2e, 0x36, 0x20, 
	0x28, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 
	0x77, 0x2e, 0x73, 0x69, 0x63, 0x73, 0x2e, 0x73, 0x65, 0x2f, 
	0x7e, 0x61, 0x64, 0x61, 0x6d, 0x2f, 0x6c, 0x77, 0x69, 0x70, 
	0x2f, 0x29, 0xd, 0xa, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 
	0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 
	0x78, 0x74, 0x2f, 0x68, 0x74, 0x6d, 0x6c, 0xd, 0xa, 0xd, 
	0xa, 0x3c, 0x68, 0x74, 0x6d, 0x6c, 0x3e, 0xa, 0x3c, 0x68, 
	0x65, 0x61, 0x64, 0x3e, 0x3c, 0x74, 0x69, 0x74, 0x6c, 0x65, 
	0x3e, 0x6c, 0x77, 0x49, 0x50, 0x20, 0x2d, 0x20, 0x41, 0x20, 
	0x4c, 0x69, 0x67, 0x68, 0x74, 0x77, 0x65, 0x69, 0x67, 0x68, 
	0x74, 0x20, 0x54, 0x43, 0x50, 0x2f, 0x49, 0x50, 0x20, 0x53, 
	0x74, 0x61, 0x63, 0x6b, 0x3c, 0x2f, 0x74, 0x69, 0x74, 0x6c, 
	0x65, 0x3e, 0x3c, 0x2f, 0x68, 0x65, 0x61, 0x64, 0x3e, 0xa, 
	0x3c, 0x62, 0x6f, 0x64, 0x79, 0x20, 0x62, 0x67, 0x63, 0x6f, 
	0x6c, 0x6f, 0x72, 0x3d, 0x22, 0x77, 0x68, 0x69, 0x74, 0x65, 
	0x22, 0x20, 0x74, 0x65, 0x78, 0x74, 0x3d, 0x22, 0x62, 0x6c, 
	0x61, 0x63, 0x6b, 0x22, 0x3e, 0xa, 0x9, 0x4e, 0x6f, 0x74, 
	0x20, 0x46, 0x6f, 0x75, 0x6e, 0x64, 0x21, 0xa, 0x3c, 0x2f, 
	0x62, 0x6f, 0x64, 0x79, 0x3e, 0xa, 0x3c, 0x2f, 0x68, 0x74, 
	0x6d, 0x6c, 0x3e, 0xa, };

static const unsigned char data_fgstatus_shtml[] = {
	/* /fgstatus.shtml */
	0x2f, 0x66, 0x67, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x2e, 0x73, 0x68, 0x74, 0x6d, 0x6c, 0,
	0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x32, 
	0x30, 0x30, 0x20, 0x4f, 0x4b, 0xd, 0xa, 0x53, 0x65, 0x72, 
	0x76, 0x65, 0x72, 0x3a, 0x20, 0x6c, 0x77, 0x49, 0x50, 0x2f, 
	0x70, 0x72, 0x65, 0x2d, 0x30, 0x2e, 0x36, 0x20, 0x28, 0x68, 
	0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 
	0x73, 0x69, 0x63, 0x73, 0x2e, 0x73, 0x65, 0x2f, 0x7e, 0x61, 
	0x64, 0x61, 0x6d, 0x2f, 0x6c, 0x77, 0x69, 0x70, 0x2f, 0x29, 
	0xd, 0xa, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 
	0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 
	0x2f, 0x70, 0x6c, 0x61, 0x69, 0x6e, 0xd, 0xa, 0xd, 0xa, 
	0x3c, 0x21, 0x2d, 0x2d, 0x23, 0x66, 0x2d, 0x2d, 0x3e, 0x20, 
};

static const unsigned char data_index_html[] = {
	/* /index.html */
	0x2f, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x2e, 0x68, 0x74, 0x6d, 0x6c, 0,
	0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x32, 
	0x30, 0x30, 0x20, 0x4f, 0x4b, 0xd, 0xa, 0x53, 0x65, 0x72, 
	0x76, 0x65, 0x72, 0x3a, 0x20, 0x6c, 0x77, 0x49, 0x50, 0x2f, 
	0x70, 0x72, 0x65, 0x2d, 0x30, 0x2e, 0x36, 0x20, 0x28, 0x68, 
	0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 
	0x73, 0x69, 0x63, 0x73, 0x2e, 0x73, 0x65, 0x2f, 0x7e, 0x61, 
	0x64, 0x61, 0x6d, 0x2f, 0x6c, 0x77, 0x69, 0x70, 0x2f, 0x29, 
	0xd, 0xa, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 
	0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 
	0x2f, 0x68, 0x74, 0x6d, 0x6c, 0xd, 0xa, 0xd, 0xa, 0x3c, 
	0x68, 0x74, 0x6d, 0x6c, 0x3e, 0xa, 0x3c, 0x68, 0x65, 0x61, 
	0x64, 0x3e, 0x3c, 0x74, 0x69, 0x74, 0x6c, 0x65, 0x3e, 0x6c, 
	0x77, 0x49, 0x50, 0x20, 0x2d, 0x20, 0x41, 0x20, 0x4c, 0x69, 
	0x67, 0x68, 0x74, 0x77, 0x65, 0x69, 0x67, 0x68, 0x74, 0x20, 
	0x54, 0x43, 0x50, 0x2f, 0x49, 0x50, 0x20, 0x53, 0x74, 0x61, 
	0x63, 0x6b, 0x3c, 0x2f, 0x74, 0x69, 0x74, 0x6c, 0x65, 0x3e, 
	0x3c, 0x2f, 0x68, 0x65, 0x61, 0x64, 0x3e, 0xa, 0x3c, 0x62, 
	0x6f, 0x64, 0x79, 0x20, 0x62, 0x67, 0x63, 0x6f, 0x6c, 0x6f, 
	0x72, 0x3d, 0x22, 0x77, 0x68, 0x69, 0x74, 0x65, 0x22, 0x20, 
	0x74, 0x65, 0x78, 0x74, 0x3d, 0x22, 0x62, 0x6c, 0x61, 0x63, 
	0x6b, 0x22, 0x3e, 0xa, 0x20, 0x20, 0x20, 0x20, 0x54, 0x65, 
	0x73, 0x74, 0xa, 0x3c, 0x2f, 0x62, 0x6f, 0x64, 0x79, 0x3e, 
	0xa, 0x3c, 0x2f, 0x68, 0x74, 0x6d, 0x6c, 0x3e, 0xa, 0xa, 
};

static const unsigned char data_pwstatus_shtml[] = {
	/* /pwstatus.shtml */
	0x2f, 0x70, 0x77, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x2e, 0x73, 0x68, 0x74, 0x6d, 0x6c, 0,
	0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x32, 
	0x30, 0x30, 0x20, 0x4f, 0x4b, 0xd, 0xa, 0x53, 0x65, 0x72, 
	0x76, 0x65, 0x72, 0x3a, 0x20, 0x6c, 0x77, 0x49, 0x50, 0x2f, 
	0x70, 0x72, 0x65, 0x2d, 0x30, 0x2e, 0x36, 0x20, 0x28, 0x68, 
	0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 
	0x73, 0x69, 0x63, 0x73, 0x2e, 0x73, 0x65, 0x2f, 0x7e, 0x61, 
	0x64, 0x61, 0x6d, 0x2f, 0x6c, 0x77, 0x69, 0x70, 0x2f, 0x29, 
	0xd, 0xa, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 
	0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 
	0x2f, 0x70, 0x6c, 0x61, 0x69, 0x6e, 0xd, 0xa, 0xd, 0xa, 
	0x3c, 0x21, 0x2d, 0x2d, 0x23, 0x70, 0x2d, 0x2d, 0x3e, 0x20, 
};

const struct fsdata_file file_404_html[] = {{NULL, data_404_html, data_404_html + 10, sizeof(data_404_html) - 10}};

const struct fsdata_file file_fgstatus_shtml[] = {{file_404_html, data_fgstatus_shtml, data_fgstatus_shtml + 16, sizeof(data_fgstatus_shtml) - 16}};

const struct fsdata_file file_index_html[] = {{file_fgstatus_shtml, data_index_html, data_index_html + 12, sizeof(data_index_html) - 12}};

const struct fsdata_file file_pwstatus_shtml[] = {{file_index_html, data_pwstatus_shtml, data_pwstatus_shtml + 16, sizeof(data_pwstatus_shtml) - 16}};

#define FS_ROOT file_pwstatus_shtml

#define FS_NUMFILES 4
