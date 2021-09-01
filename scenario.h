#pragma once

static const uint8_t X_SIZE = 8;
static const uint8_t Y_SIZE = 8;

static const uint16_t N_PKT = 29;
static const uint32_t PACKETS[N_PKT][5] = {
	{   4,  4, 0, 0x01, SVC_ALL},
	{  80,  0, 0, 0x02, SVC_ALL},
	{  80,  3, 0, 0x03, SVC_ALL},
	{  80,  5, 0, 0x04, SVC_ALL},

	{ 150,  8, 6, 0xBA, SVC_TGT},

	{ 250,  8, 5, 0xA8, SVC_TGT},
	{ 300,  0, 5, 0xA0, SVC_TGT},
	{ 350,  6, 5, 0xA6, SVC_TGT},
	{ 400,  1, 5, 0xA1, SVC_TGT},
	{ 410,  3, 5, 0xA3, SVC_TGT},
	{ 410,  7, 5, 0xA7, SVC_TGT},
	{ 420,  4, 5, 0xA4, SVC_TGT},

	{ 500,  2, 0, 0xEE, SVC_TGT},
	{ 550,  1, 0, 0xFF, SVC_TGT},
	{ 650,  5, 0, 0x88, SVC_TGT},

	{ 650,  6, 4, 0x66, SVC_TGT},
	{ 680, 30, 0, 0x9F, SVC_ALL},
	{ 750,  3, 4, 0x77, SVC_ALL},
	{ 770,  0, 4, 0xCA, SVC_TGT},
	{ 790,  2, 4, 0xBE, SVC_TGT},

	{ 850,  6, 0, 0x11, SVC_TGT},
	{ 900,  1, 7, 0x22, SVC_ALL},
	{ 950,  8, 7, 0x33, SVC_TGT},
	{ 950,  2, 7, 0x44, SVC_TGT},

	{1100,  7, 0, 0xAF, SVC_TGT},
	{1150,  4, 0, 0xDE, SVC_TGT},
	{1200,  8, 0, 0xBC, SVC_ALL},
	{1200,  5, 0, 0xF1, SVC_ALL},
	{1300,  2, 7, 0x33, SVC_TGT},
};
