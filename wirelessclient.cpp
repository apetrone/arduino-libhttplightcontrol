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

void ping_pong_command( WirelessClient * lc, uint8_t * data, uint8_t dataLength ) {}

WirelessClient::WirelessClient()
{
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

bool transmitAndAcknowledge( XBee & xbee, XBeeAddress64 & address, uint8_t * data, uint8_t payload_length, bool do_ack )
{
	ZBTxRequest request = ZBTxRequest( address, data, payload_length );
	xbee.send( request );

	if ( do_ack )
	{
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
	}
	return false;
} // transmitAndAcknowledge

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

bool WirelessClient::is_connected() const
{
	return this->connected != 0;
} // is_connected

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
				this->connected = 1;
				this->failures = 0;

				// received a response from a client, process this as a command
				this->handle_command( xbee, rx.getData(), rx.getDataLength() );
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
					this->connected = 1;
					this->failures = 0;
				}
			}
			else if ( api_id == AT_COMMAND_RESPONSE )
			{
				//Serial.println( "AT_COMMAND_RESPONSE" );
			}
			else if ( api_id == ZB_TX_STATUS_RESPONSE )
			{
				ZBTxStatusResponse tx;
				xbee.getResponse().getZBTxStatusResponse( tx );

				if ( !tx.isSuccess() )
				{
					Serial.println( "tx failed" );
					this->failures++;
					if ( this->failures == kMaxTransmitFailures )
					{
						Serial.println( "completely failed out." );
						this->connected = 0;
					}
				}
			}
			else
			{
				//Serial.print( "unhandled api_id: " );
				//Serial.println( api_id );
			}
		}
		else
		{
			//Serial.println( "No response is available." );
		}
	}
} // read_packet


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



