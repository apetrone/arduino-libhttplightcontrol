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


void ping_pong_command( WirelessClient * lc, uint8_t * data, uint8_t dataLength ) {}

WirelessClient::WirelessClient()
{
	failures = 0;
	memset( this->command_table, (int)ping_pong_command, sizeof(fnCommand) * CMD_MAX );
} // WirelessClient

void WirelessClient::map_command( uint8_t command, fnCommand command_function )
{
	if ( command >= CMD_MAX )
	{
		return;
	}

	this->command_table[ command ] = command_function;
} // map_command



void WirelessClient::handle_command( XBee & xbee, uint8_t * data, uint8_t data_length )
{
	if ( data_length > 1 )
	{
		uint8_t command = data[0];
		if ( command > 0 && command < CMD_MAX )
		{
			command_table[ command ]( this, data+1, data_length-1 );
			return;
		}
	}
} // handle_command

void WirelessClient::read_packet( XBee & xbee )
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
				this->failures = 0;

				// received a response from a client, process this as a command
				this->handle_command( xbee, rx.getData(), rx.getDataLength() );
			}
			else if ( api_id == MODEM_STATUS_RESPONSE )
			{
				ModemStatusResponse mr = ModemStatusResponse();
				xbee.getResponse().getModemStatusResponse( mr );

				if ( mr.getStatus() == ASSOCIATED )
				{
					// client is ready to go
					// Serial.println( "client associated." );
					this->failures = 0;
				}
			}

			else if ( api_id == ZB_TX_STATUS_RESPONSE )
			{
				ZBTxStatusResponse tx;
				xbee.getResponse().getZBTxStatusResponse( tx );

				if ( !tx.isSuccess() )
				{
					// Serial.println( "tx failed" );
					this->failures++;
				}
			}
		}
	}
} // read_packet

void WirelessClient::send_packet( XBee & xbee, uint8_t * data, uint8_t payload_length )
{
	ZBTxRequest request = ZBTxRequest( address, data, payload_length );
	xbee.send( request );
} // send_packet

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



