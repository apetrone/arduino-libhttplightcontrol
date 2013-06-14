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

#include <XBee.h>

const uint16_t kMaxResponseTimeoutMilliseconds = 16;


enum LightClientType
{
	CLIENT_TYPE_UNUSED = 0,		// unused client slot
	CLIENT_TYPE_WIRED_RELAY,		// wired relay (on/off)
	CLIENT_TYPE_WIRELESS,		// simple wireless client (on/off)
	CLIENT_TYPE_WIRELESS_RGB,	// RGB-capable wireless client (rgb * num_pixels)

	CLIENT_TYPE_MAX
}; // LightClientType

enum ClientCommand
{
	ERROR = 0,
	SEND_CLIENT_NAME, // request for client's name,
	RECV_CLIENT_NAME, // command to store the client's name
	CONTROL_LIGHT,    // on/off

	COMMAND_MAX
}; // ClientCommand

struct HttpLightClient;

// must return 0 or 1 for the new light state, if applicable. ie. turning on/off a light
typedef void (*fnCommand)( HttpLightClient * lc, uint8_t * data, uint8_t dataLength );
void setCommand( uint8_t command, fnCommand command_function );

void lightcontrol_set_client_name( HttpLightClient * lc, uint8_t * data, uint8_t data_length );
void lightControl_clientRead( XBee & xbee, HttpLightClient * client );
void lightControl_readXBeePacket( XBee & xbee );
void handleClientCommand( XBee & xbee, HttpLightClient * lc, uint8_t * data, uint8_t dataLength );





bool transmitAndAcknowledge( XBee & xbee, XBeeAddress64 & address, uint8_t * data, uint8_t payload_length );
