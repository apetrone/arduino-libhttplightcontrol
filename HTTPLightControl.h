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

// lower level parsing
struct KeyPair
{
  const char * key;
  const char * value;
};


const uint8_t kMaxHTTPRequestLength = 32;
const uint8_t kMaxKeypairs = 4;
const uint8_t kMaxHTTPMethodLength = 8;


struct HTTPRequest
{
  
  String buffer;

  uint8_t _next_keypair;
  KeyPair keypairs[ kMaxKeypairs ];
  //char method[ kMaxHTTPMethodLength ];
  //char request[ kMaxHTTPRequestLength ]; 

  uint8_t method_offset;
  uint8_t method_length;

  uint8_t request_offset;
  uint8_t request_length;

  HTTPRequest();

  void parseMethodAndRequest();
};


void keypair_add( struct HTTPRequest * ci, const char * key, const char * value );
void keypair_parse( struct HTTPRequest * ci, char * request, unsigned int len );
const char * keypair_getvalue( struct HTTPRequest * ci, const char * key );





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

// must return 0 or 1 for the new light _state, if applicable. ie. turning on/off a light
typedef uint8_t (*fnLightOperator)( struct LightClient * lc, HTTPRequest * request );
typedef void (*fnCommand)( LightClient * lc, uint8_t * data, uint8_t dataLength );

void setCommand( uint8_t command, fnCommand command_function );

LightClient * findOrCreateLightClient( XBeeAddress64 & address );
LightClient * getClientAtIndex( uint8_t index );

void lightControl_clientRead( XBee & xbee, LightClient * client );
void lightControl_readXBeePacket( XBee & xbee );

bool transmitAndAcknowledge( XBee & xbee, XBeeAddress64 & address, uint8_t * data, uint8_t payload_length );
void handleClientCommand( XBee & xbee, LightClient * lc, uint8_t * data, uint8_t dataLength );