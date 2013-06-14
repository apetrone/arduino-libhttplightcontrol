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
#include "httplightserver.h"

HttpLightServer::HttpLightServer( uint8_t max_clients )
{
	clients = new HttpLightClient[ max_clients ];
	total_clients = max_clients;
} // HttpLightServer

HttpLightServer::~HttpLightServer()
{
	delete [] clients;
} // ~HttpLightServer

HttpLightClient * HttpLightServer::find_or_create_client( XBeeAddress64 & addr )
{
	HttpLightClient * c = 0;

	// iterate over clients; first-pass to see if there's an active
	// client with the address passed in
	for( uint8_t i = 0; i < total_clients; ++i )
	{
		c = &clients[ i ];
		if ( c->type != CLIENT_TYPE_UNUSED && c->matches_address( addr ) )
		{
			return c;
		}
		else
		{
			c = 0;
		}
	}

	// no client matched the input address
	// see if we can find an unused slot
	if ( !c )
	{
		for( uint8_t i = 0; i < total_clients; ++i )
		{
			c = &clients[ i ];
			if ( c->type == CLIENT_TYPE_UNUSED )
			{
				break;
			}
			else
			{
				c = 0;
			}
		}
	}

	return c;
} // find_or_create_client

HttpLightClient * HttpLightServer::get_client_by_index( uint8_t index )
{
	if ( index >= total_clients )
	{
		return 0;
	}

	return &clients[ index ];
} // get_client_by_index

void HttpLightServer::read_packet( XBee & xbee )
{
	ZBRxResponse rx = ZBRxResponse();
	xbee.readPacket();

	if ( xbee.getResponse().isAvailable() )
	{
		uint8_t api_id = xbee.getResponse().getApiId();
		xbee.getResponse().getZBRxResponse( rx );
		XBeeAddress64 & addr = rx.getRemoteAddress64();

		if ( api_id == ZB_RX_RESPONSE )
		{
			// received a response from a client, process this as a command
			HttpLightClient * lc = find_or_create_client( addr );
			if ( lc )
			{
				handleClientCommand( xbee, lc, rx.getData(), rx.getDataLength() );
			}
		}
		else if ( api_id == ZB_IO_NODE_IDENTIFIER_RESPONSE )
		{
			HttpLightClient * lc = find_or_create_client( addr );
			if ( lc )
			{
				// at this moment, all I know is that it's an XBee client.
				// Will query for more information...

				lc->address = addr;
				lc->retries = kMaxClientRetries;

				if (lc->type == CLIENT_TYPE_UNUSED)
				{
					lc->type = CLIENT_TYPE_WIRELESS;
					lc->state = 0;

					// request client name; minimum two bytes for a request
					uint8_t name_request[] = { SEND_CLIENT_NAME, 0 };
					transmitAndAcknowledge( xbee, lc->address, name_request, 2 );					
				}
			}
		}
	}
} // read_packet