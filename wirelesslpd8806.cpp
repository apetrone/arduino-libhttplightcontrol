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
#include "Wirelesslpd8806.h"
#include <LPD8806.h>

//
// WirelessLPD8806
WirelessLPD8806::WirelessLPD8806( LPD8806 * in_strip )
{
	strip = in_strip;
	if ( strip )
	{
		strip->begin();
	}

	current_color = 1;

	color_table[0] = strip->Color( 0, 0, 0 );
	color_table[1] = strip->Color( 127, 0, 0 );
	color_table[2] = strip->Color( 127, 0, 127 );
	color_table[3] = strip->Color( 127, 35, 0 );
	color_table[4] = strip->Color( 0, 127, 0 );
	color_table[5] = strip->Color( 0, 127, 127 );
	color_table[6] = strip->Color( 0, 0, 127 );
	color_table[7] = strip->Color( 127, 127, 127 );

} // WirelessLPD8806

WirelessLPD8806::~WirelessLPD8806()
{

} // ~WirelessLPD8806

void WirelessLPD8806::set_color( uint8_t color_index )
{
	current_color = color_index;

} // set_color

void WirelessLPD8806::show()
{
	if ( strip )
	{
		for( int i = 0; i < strip->numPixels(); ++i )
		{
			strip->setPixelColor( i, color_table[current_color] );
		}

		strip->show();
	}
} // show


