// -------------------------------------------------------------
// Copyright 2012- (C) Adam Petrone

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -------------------------------------------------------------
#pragma once

#include <Arduino.h> // for uint8_t
#include <XBee.h>

const uint16_t		kMaxResponseTimeoutMilliseconds 		= 3000;
const uint8_t		kMaxSensorSamples						= 5;		// number of samples per transmission
const uint8_t 		kMaxTransmitFailures 					= 3;		// maximum transmission failures allowed before assumed 'disconnected'

const uint8_t CMD_PING_PONG				= 0;
const uint8_t CMD_CONTROL_LIGHT 		= 1;
const uint8_t CMD_SENSOR_SAMPLES_HT 	= 2;
const uint8_t CMD_MAX 					= 3;

// command callback
struct WirelessClient;
typedef void (*fnCommand)( WirelessClient * client, uint8_t * data, uint8_t dataLength );

struct WirelessClient
{
	XBeeAddress64 address;

	uint8_t failures;	// send failure count
	fnCommand command_table[ CMD_MAX ];	// command -> function pointer mapping

	WirelessClient();
	void map_command( uint8_t command, fnCommand command_function );
	void handle_command( XBee & xbee, uint8_t * data, uint8_t dataLength );
	inline bool is_connected() const { return (failures < kMaxTransmitFailures); }
	void read_packet( XBee & xbee );
	void send_packet( XBee & xbee, uint8_t * data, uint8_t payload_length );

}; // WirelessClient


