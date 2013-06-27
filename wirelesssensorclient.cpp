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
#include <wirelesssensorclient.h>

//
// WirelessSensorClient

WirelessSensorClient::WirelessSensorClient()
{
	memset( packet_data, 0, (sizeof(uint16_t) * kWirelessSampleDataSize) + kWirelessSensorHeaderSize );
}

uint16_t * WirelessSensorClient::get_sample_pointer()
{
	return packet_data + 1;
} // get_sample_pointer

void WirelessSensorClient::send_sensor_samples( XBee & xbee, uint8_t type, void * data, uint8_t data_size )
{
	uint8_t * packet = (uint8_t*)packet_data;
	packet[0] = type;
	packet[1] = kMaxSensorSamples;

	transmitAndAcknowledge( xbee, this->address, (uint8_t*)packet_data,
		(sizeof(uint16_t) * kWirelessSampleDataSize) + kWirelessSensorHeaderSize );
} // send_sensor_samples

