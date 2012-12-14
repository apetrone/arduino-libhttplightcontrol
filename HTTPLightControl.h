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

const uint8_t kMaxClientNameCharacters = 12;

// TODO: This should be a parameter definable per project
const uint8_t kMaxLightClients = 6;

// If no response from client, the maximum number of connection retries before that client is dropped
const uint8_t kMaxClientRetries = 2;

const uint16_t kMaxResponseTimeoutMilliseconds = 16;

enum LightClientType
{
  eUnused = 0,      // unused client slot
  eWiredRelay,      // wired relay (on/off)
  eXBeeClient,      // simple XBee client (on/off)
  eXBeeRGBClient    // RGB-capable XBee client (rgb * num_pixels)
};

struct LightClient
{
  LightClient();
  bool matchesAddress( XBeeAddress64 & addr );
  uint8_t _type;
  uint8_t _pin;
  uint8_t _state;
  uint8_t _retries;
  char _name[ kMaxClientNameCharacters ];
  XBeeAddress64 _addr;
};

enum ClientCommand
{
  ERROR = 0,
  TOGGLE_LED,
  SEND_CLIENT_NAME, // request for client's name
  CONTROL_LIGHT,    // on/off


  COMMAND_MAX
};

typedef void (*pLightOperator)( struct LightClient * lc, boolean enable );
typedef void (*fnCommand)( LightClient * lc, uint8_t * data, uint8_t dataLength );

void setCommand( uint8_t command, fnCommand command_function );

LightClient * findOrCreateLightClient( XBeeAddress64 & address );
LightClient * getClientAtIndex( uint8_t index );

void lightControl_readXBeePacket( XBee & xbee );

bool transmitAndAcknowledge( XBee & xbee, XBeeAddress64 & address, uint8_t * data, uint8_t payload_length );
void handleClientCommand( XBee & xbee, LightClient * lc, uint8_t * data, uint8_t dataLength );