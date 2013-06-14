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
#include "httplightclient.h"

HttpLightClient::HttpLightClient()
{
	type = 0;
	pin = 0;
	state = 0;
	retries = 0;
} // HttpLightClient

HttpLightClient::~HttpLightClient()
{

} // ~HttpLightClient

bool HttpLightClient::matches_address( XBeeAddress64 & other_address )
{
	return (address.getMsb() == other_address.getMsb() && address.getLsb() == other_address.getLsb());
} // matches_address

void HttpLightClient::send_server_heartbeat( XBee & xbee )
{
	// send a heartbeat packet

	char packet[32];

	// stream.init( buffer, 32 );
	// stream.write( (void*)SEND_CLIENT_NAME, 1 );
	// stream.write( (void*)client_name, strlen((const char*)client_name) );

	// transmitAndAcknowledge( xbee, lc->address, (uint8_t*)stream.stream_data(), stream.offset_pointer() );	
} // send_server_heartbeat