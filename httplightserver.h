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

#include "httplightcommon.h"
#include "httplightclient.h"
#include "httplightrequest.h"

#include <XBee.h>

// If no response from client, the maximum number of connection retries before that client is dropped
const uint8_t kMaxClientRetries = 2;

struct HttpLightServer
{
	HttpLightClient * clients;
	uint8_t total_clients;

	HttpLightServer( uint8_t max_clients );
	~HttpLightServer();


	HttpLightClient * find_or_create_client( XBeeAddress64 & addr );
	HttpLightClient * get_client_by_index( uint8_t index );
	void read_packet( XBee & xbee );
}; // HttpLightServer

typedef uint8_t (*fnLightOperator)( struct HttpLightClient * lc, HTTPRequest * request );