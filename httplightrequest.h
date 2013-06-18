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

#include "Arduino.h"

// lower level parsing
struct KeyPair
{
  char * key;
  char * value;
};

const uint8_t kMaxKeypairs = 7;
const uint8_t kMaxHTTPMethodLength = 5;
const uint8_t kMaxBufferSize = 96;

struct HTTPRequest
{
  char buffer[ kMaxBufferSize ];

  uint8_t _next_keypair;
  KeyPair keypairs[ kMaxKeypairs ];

  uint8_t method_offset;
  uint8_t method_length;

  uint8_t request_offset;
  uint8_t request_length;

  HTTPRequest();

  void parse_method_and_request( uint8_t buffer_size );
};


void keypair_add( struct HTTPRequest * ci, char * key, char * value );
void keypair_parse( struct HTTPRequest * ci, char * request, uint8_t len );
char * keypair_getvalue( struct HTTPRequest * ci, const char * key );

// this doesn't do any real decoding; simply conversions of basic characters
void keypair_urldecode( char * value );