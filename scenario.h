#pragma once

#include "BrLiteRouter.hpp"

static const uint8_t X_SIZE = 8;
static const uint8_t Y_SIZE = 8;

static const uint16_t N_PKT = 29;
static const uint32_t PACKETS[N_PKT][5] = {
	{   4,  4, 0, 0x01, static_cast<uint32_t>(BrLiteRouter::Service::ALL)},
	{  80,  0, 0, 0x02, static_cast<uint32_t>(BrLiteRouter::Service::ALL)},
	{  80,  3, 0, 0x03, static_cast<uint32_t>(BrLiteRouter::Service::ALL)},
	{  80,  5, 0, 0x04, static_cast<uint32_t>(BrLiteRouter::Service::ALL)},

	{ 150,  8, 6, 0xBA, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},

	{ 250,  8, 5, 0xA8, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 300,  0, 5, 0xA0, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 350,  6, 5, 0xA6, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 400,  1, 5, 0xA1, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 410,  3, 5, 0xA3, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 410,  7, 5, 0xA7, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 420,  4, 5, 0xA4, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},

	{ 500,  2, 0, 0xEE, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 550,  1, 0, 0xFF, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 650,  5, 0, 0x88, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},

	{ 650,  6, 4, 0x66, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 680, 30, 0, 0x9F, static_cast<uint32_t>(BrLiteRouter::Service::ALL)},
	{ 750,  3, 4, 0x77, static_cast<uint32_t>(BrLiteRouter::Service::ALL)},
	{ 770,  0, 4, 0xCA, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 790,  2, 4, 0xBE, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},

	{ 850,  6, 0, 0x11, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 900,  1, 7, 0x22, static_cast<uint32_t>(BrLiteRouter::Service::ALL)},
	{ 950,  8, 7, 0x33, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{ 950,  2, 7, 0x44, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},

	{1100,  7, 0, 0xAF, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{1150,  4, 0, 0xDE, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
	{1200,  8, 0, 0xBC, static_cast<uint32_t>(BrLiteRouter::Service::ALL)},
	{1200,  5, 0, 0xF1, static_cast<uint32_t>(BrLiteRouter::Service::ALL)},
	{1300,  2, 7, 0x33, static_cast<uint32_t>(BrLiteRouter::Service::TARGET)},
};
