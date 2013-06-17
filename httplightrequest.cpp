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

	memset( buffer, 0, kMaxBufferSize );

	for( uint8_t i = 0; i < kMaxKeypairs; ++i )
	{
		KeyPair * kp = &keypairs[ i ];
		kp->key = 0;
		kp->value = 0;
	}
}

void HTTPRequest::parse_method_and_request( uint8_t buffer_size )
{
	const char * start = &buffer[0];
	const char * p = start;
	const char * last = start;
	uint8_t i;
	uint8_t state = 0;

	for( i = 0; i < buffer_size; ++i )
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

void keypair_add( struct HTTPRequest * ci, char * key, char * value )
{
	KeyPair * kp = &ci->keypairs[ ci->_next_keypair ];
	kp->key = key;
	kp->value = value;
	++ci->_next_keypair;
	ci->_next_keypair = ci->_next_keypair % kMaxKeypairs;
}

// modify the request string as we split it up into Key/Value pairs
void keypair_parse( struct HTTPRequest * ci, char * request, uint8_t len )
{
	char * cur = request;
	uint8_t i;
	char * key = 0;
	char * value = 0;
	uint8_t state = 0;

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
		return;
	}

	// reset; truncate cur to the start of the GET parameter string
	cur = state+request;
	state = 0;
	key = cur;
	state = 1;

	while( *cur != '\0' )
	{
		if ( state == 0 && (*cur == '&' || *cur == ' ') )
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
		else if ( state == 2 && (*cur == '&' || *cur == ' ') )
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




char * keypair_getvalue( struct HTTPRequest * ci, const char * key )
 {
	for( uint8_t i = 0; i < kMaxKeypairs; ++i )
	{
		KeyPair * kp = &ci->keypairs[ i ];
		if ( kp->key && (strcmp( kp->key, key ) == 0) )
		{
			return kp->value;
		}
	}
	return 0;
} // keypair_getvalue

void keypair_urldecode( char * value )
{
	for( uint8_t i = 0; value[i]; ++i )
	{
		if ( value[i] == '+' )
		{
			value[i] = ' ';
		}
	}
} // keypair_urldecode






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


void handleClientCommand( XBee & xbee, HttpLightClient * lc, uint8_t * data, uint8_t data_length )
{
	if ( data_length > 1 )
	{
		uint8_t command = data[0];
		if ( command > 0 && command < COMMAND_MAX )
		{
			debug_flash_led( 8, command, 300 );
			commandTable[ command ]( lc, data+1, data_length-1 );
			return;
		}
	}

	debug_flash_led( 8, 3, 500 );
}

void debug_flash_led(uint8_t pin, uint8_t times, uint16_t wait)
{
	for ( uint8_t i = 0; i < times; i++ )
	{
		digitalWrite(pin, HIGH);
		delay(wait);
		digitalWrite(pin, LOW);

		if (i + 1 < times)
		{
			delay(wait);
		}
	}
} // debug_flash_led

