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
#include <Arduino.h>


void null_command( HttpLightClient * lc, uint8_t * data, uint8_t dataLength ) {}

fnCommand commandTable[ COMMAND_MAX ] = { null_command };

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
} // transmitAndAcknowledge


void handleClientCommand( XBee & xbee, HttpLightClient * lc, uint8_t * data, uint8_t data_length )
{
	if ( data_length > 1 )
	{
		uint8_t command = data[0];
		if ( command > 0 && command < COMMAND_MAX )
		{
			commandTable[ command ]( lc, data+1, data_length-1 );
			return;
		}
	}
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

HttpLightClient::HttpLightClient()
{
	type = 0;
	pin = 0;
	state = 0;

	memset( &this->address, 0, sizeof(XBeeAddress64) );
} // HttpLightClient

HttpLightClient::~HttpLightClient()
{

} // ~HttpLightClient

bool HttpLightClient::matches_address( XBeeAddress64 & other_address )
{
	return (address.getMsb() == other_address.getMsb() && address.getLsb() == other_address.getLsb());
} // matches_address

void HttpLightClient::read_packet( XBee & xbee )
{
	if (xbee.readPacket(2000))
	{
		if ( xbee.getResponse().isAvailable() )
		{
			uint8_t api_id = xbee.getResponse().getApiId();

			//debug_flash_led( 13, 2, 500 );		

			if ( api_id == ZB_RX_RESPONSE )
			{
				ZBRxResponse rx = ZBRxResponse();
				xbee.getResponse().getZBRxResponse( rx );
				this->address = rx.getRemoteAddress64();

				Serial.println( "handle client command" );
				
				// received a response from a client, process this as a command
				handleClientCommand( xbee, this, rx.getData(), rx.getDataLength() );
			}
			else if ( api_id == MODEM_STATUS_RESPONSE )
			{
				ModemStatusResponse mr = ModemStatusResponse();
				xbee.getResponse().getModemStatusResponse( mr );

				if ( mr.getStatus() == HARDWARE_RESET )
				{
					// client is restarting
					Serial.println( "hardware reset..." );
				}
				else if ( mr.getStatus() == ASSOCIATED )
				{
					// client is ready to go
					Serial.println( "client associated." );
				}
			}
			else if ( api_id == AT_COMMAND_RESPONSE )
			{
				Serial.println( "AT_COMMAND_RESPONSE" );
			}
			else if ( api_id == ZB_TX_STATUS_RESPONSE )
			{
				ZBTxStatusResponse tx;
				xbee.getResponse().getZBTxStatusResponse( tx );

				if ( !tx.isSuccess() )
				{
					Serial.println( "tx failed" );
				}
			}
			else
			{
				Serial.print( "unhandled api_id: " );
				Serial.println( api_id );
			}
		}
		else
		{
			Serial.println( "No response is available." );
		}
	}
} // read_packet


void HttpLightClient::discover_coordinator( XBee & xbee )
{
	AtCommandRequest req;
	uint8_t command[] = {"ND"};
	req.setCommand( command );
	req.clearCommandValue();

	Serial.println( "discover coordinator..." );

	xbee.send( req );

} // discover_coordinator