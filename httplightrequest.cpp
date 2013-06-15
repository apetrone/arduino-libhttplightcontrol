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
#include "httplightrequest.h"
#include "httplightclient.h"
#include "httplightcommon.h"

HTTPRequest::HTTPRequest()
{
	_next_keypair = 0;

    for( unsigned int i = 0; i < kMaxKeypairs; ++i )
    {
      KeyPair * kp = &keypairs[ i ];
      kp->key = 0;
      kp->value = 0;
    }
}

void HTTPRequest::parseMethodAndRequest()
{
	const char * start = &buffer[0];
	const char * p = start;
	const char * last = start;
	int len = buffer.length();
	int i;
	int state = 0;
	//  memset( request->method, 0, METHOD_MAX );
	//  memset( request->request, 0, REQUEST_MAX );

	for( i = 0; i < len; ++i )
	{
		// method string: GET
		if ( *p == ' ' && state == 0 )
		{
			method_offset = last-start;
			method_length = p-last;
			//      strncpy( request->method, last, p-last );
			state = 1;  
			last = p+1;
			p = p+1;
		}
		// request string: /?id=1&set=0
		else if ( *p == ' ' && state == 1 )
		{
			request_offset = last-start;
			request_length = p-last;
			//      strncpy( request->request, last, p-last );
			state = 2;
			last = p+1;
			break;
		}

		++p;
	}
}

void keypair_add( struct HTTPRequest * ci, const char * key, const char * value )
{
	KeyPair * kp = &ci->keypairs[ ci->_next_keypair ];
	kp->key = key;
	kp->value = value;
	++ci->_next_keypair;
	ci->_next_keypair = ci->_next_keypair % kMaxKeypairs;
}

// modify the request string as we split it up into Key/Value pairs
void keypair_parse( struct HTTPRequest * ci, char * request, unsigned int len )
{
	char * cur = request;
	int unsigned i;
	char * key = 0;
	char * value = 0;
	int state = 0;

	for( i = 0; i < len; ++i )
	{
		if ( *cur == '?' )
		{
			state = i+1;
			break;
		}

		++cur;
	}

	// nothing to parse
	if ( state == 0 )
	{
		printf( "There is nothing to parse..." );
		return;
	}

	// reset; truncate cur to the start of the GET parameter string
	cur = state+request;
	state = 0;
	key = cur;
	state = 1;

	while( *cur != '\0' )
	{
		if ( state == 0 && *cur == '&' )
		{
			*cur = '\0';
			key = cur+1;
			++cur;
			state = 1;
			continue;
		}
		else if ( state == 1 && *cur == '=' )
		{
			*cur = '\0';
			++cur;
			value = cur;
			state = 2;
			continue;
		}
		else if ( state == 2 && *cur == '&' )
		{
			*cur = '\0';
			keypair_add( ci, key, value );
			++cur;
			key = cur;
			state = 1;
			continue;
		}

		++cur;
	}

	// catch un terminated key/value pairs (if they end up being the last pair in the string)
	if ( state > 0 )
	{
		*cur = '\0';
		keypair_add( ci, key, value );
	}
}




const char * keypair_getvalue( struct HTTPRequest * ci, const char * key )
 {
	for( unsigned int i = 0; i < kMaxKeypairs; ++i )
	{
		KeyPair * kp = &ci->keypairs[ i ];
		if ( strcmp( kp->key, key ) == 0 )
		{
			return kp->value;
		}
	}
	return 0;
}









void null_command( HttpLightClient * lc, uint8_t * data, uint8_t dataLength ) {}

fnCommand commandTable[ COMMAND_MAX ] = { null_command };

void lightcontrol_set_client_name( HttpLightClient * lc, uint8_t * data, uint8_t data_length )
{
	if ( lc )
	{
		strncpy( lc->name, (const char*)data, kMaxClientNameCharacters );
	}
} // lightcontrol_set_client_name

void setCommand( uint8_t command, fnCommand command_function )
{
	if ( command >= COMMAND_MAX )
	{
		return;
	}

	commandTable[ command ] = command_function;
}

void lightControl_clientRead( XBee & xbee, HttpLightClient * client )
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
			handleClientCommand( xbee, client, rx.getData(), rx.getDataLength() );
		}
	}	
}

bool transmitAndAcknowledge( XBee & xbee, XBeeAddress64 & address, uint8_t * data, uint8_t payload_length )
{
	ZBTxRequest request = ZBTxRequest( address, data, payload_length );
	xbee.send( request );

	delay( 250 );

	if ( xbee.readPacket( kMaxResponseTimeoutMilliseconds ) )
	{
		if ( xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE )
		{
			ZBTxStatusResponse txStatus;

			xbee.getResponse().getZBTxStatusResponse( txStatus );

			// delivery OK
			if ( txStatus.getDeliveryStatus() == SUCCESS )
			{
				return true;
			}
		}
	}
	return false;
}


void handleClientCommand( XBee & xbee, HttpLightClient * lc, uint8_t * data, uint8_t dataLength )
{
	if ( dataLength > 1 )
	{
		uint8_t command = data[0];
		if ( command > 0 && command < COMMAND_MAX )
		{
			commandTable[ command ]( lc, data+1, dataLength-1 );
		}
	}
}