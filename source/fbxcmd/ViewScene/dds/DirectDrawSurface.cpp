/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2009, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools projectmay not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

/*
 * This file is based on a similar file from the NVIDIA texture tools
 * (http://nvidia-texture-tools.googlecode.com/)
 *
 * Original license from NVIDIA follows.
 */

// Copyright NVIDIA Corporation 2007 -- Ignacio Castano <icastano@nvidia.com>
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "DirectDrawSurface.h"
#include "BlockDXT.h"
#include "PixelFormat.h"

#include <stdio.h> // printf
#include <math.h>  // sqrt

/*** declarations ***/

#if !defined(MAKEFOURCC)
#	define MAKEFOURCC(ch0, ch1, ch2, ch3) \
		((uint)((unsigned char)(ch0)) | \
		((uint)((unsigned char)(ch1)) << 8) | \
		((uint)((unsigned char)(ch2)) << 16) | \
		((uint)((unsigned char)(ch3)) << 24 ))
#endif

static const uint FOURCC_DDS = MAKEFOURCC('D', 'D', 'S', ' ');
static const uint FOURCC_DXT1 = MAKEFOURCC('D', 'X', 'T', '1');
static const uint FOURCC_DXT2 = MAKEFOURCC('D', 'X', 'T', '2');
static const uint FOURCC_DXT3 = MAKEFOURCC('D', 'X', 'T', '3');
static const uint FOURCC_DXT4 = MAKEFOURCC('D', 'X', 'T', '4');
static const uint FOURCC_DXT5 = MAKEFOURCC('D', 'X', 'T', '5');
static const uint FOURCC_RXGB = MAKEFOURCC('R', 'X', 'G', 'B');
static const uint FOURCC_ATI1 = MAKEFOURCC('A', 'T', 'I', '1');
static const uint FOURCC_ATI2 = MAKEFOURCC('A', 'T', 'I', '2');

// 32 bit RGB formats.
static const uint D3DFMT_R8G8B8 = 20;
static const uint D3DFMT_A8R8G8B8 = 21;
static const uint D3DFMT_X8R8G8B8 = 22;
static const uint D3DFMT_R5G6B5 = 23;
static const uint D3DFMT_X1R5G5B5 = 24;
static const uint D3DFMT_A1R5G5B5 = 25;
static const uint D3DFMT_A4R4G4B4 = 26;
static const uint D3DFMT_R3G3B2 = 27;
static const uint D3DFMT_A8 = 28;
static const uint D3DFMT_A8R3G3B2 = 29;
static const uint D3DFMT_X4R4G4B4 = 30;
static const uint D3DFMT_A2B10G10R10 = 31;
static const uint D3DFMT_A8B8G8R8 = 32;
static const uint D3DFMT_X8B8G8R8 = 33;
static const uint D3DFMT_G16R16 = 34;
static const uint D3DFMT_A2R10G10B10 = 35;

static const uint D3DFMT_A16B16G16R16 = 36;

// Palette formats.
static const uint D3DFMT_A8P8 = 40;
static const uint D3DFMT_P8 = 41;
	
// Luminance formats.
static const uint D3DFMT_L8 = 50;
static const uint D3DFMT_A8L8 = 51;
static const uint D3DFMT_A4L4 = 52;
static const uint D3DFMT_L16 = 81;

// Floating point formats
static const uint D3DFMT_R16F = 111;
static const uint D3DFMT_G16R16F = 112;
static const uint D3DFMT_A16B16G16R16F = 113;
static const uint D3DFMT_R32F = 114;
static const uint D3DFMT_G32R32F = 115;
static const uint D3DFMT_A32B32G32R32F = 116;
	
static const uint DDSD_CAPS = 0x00000001U;
static const uint DDSD_PIXELFORMAT = 0x00001000U;
static const uint DDSD_WIDTH = 0x00000004U;
static const uint DDSD_HEIGHT = 0x00000002U;
static const uint DDSD_PITCH = 0x00000008U;
static const uint DDSD_MIPMAPCOUNT = 0x00020000U;
static const uint DDSD_LINEARSIZE = 0x00080000U;
static const uint DDSD_DEPTH = 0x00800000U;
	
static const uint DDSCAPS_COMPLEX = 0x00000008U;
static const uint DDSCAPS_TEXTURE = 0x00001000U;
static const uint DDSCAPS_MIPMAP = 0x00400000U;
static const uint DDSCAPS2_VOLUME = 0x00200000U;
static const uint DDSCAPS2_CUBEMAP = 0x00000200U;

static const uint DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400U;
static const uint DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800U;
static const uint DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000U;
static const uint DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000U;
static const uint DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000U;
static const uint DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000U;
static const uint DDSCAPS2_CUBEMAP_ALL_FACES = 0x0000FC00U;

static const uint DDPF_ALPHAPIXELS = 0x00000001U;
static const uint DDPF_ALPHA = 0x00000002U;
static const uint DDPF_FOURCC = 0x00000004U;
static const uint DDPF_RGB = 0x00000040U;
static const uint DDPF_PALETTEINDEXED1 = 0x00000800U;
static const uint DDPF_PALETTEINDEXED2 = 0x00001000U;
static const uint DDPF_PALETTEINDEXED4 = 0x00000008U;
static const uint DDPF_PALETTEINDEXED8 = 0x00000020U;
static const uint DDPF_LUMINANCE = 0x00020000U;
static const uint DDPF_ALPHAPREMULT = 0x00008000U;
static const uint DDPF_NORMAL = 0x80000000U;	// @@ Custom nv flag.

	// DX10 formats.
	enum DXGI_FORMAT
	{
		DXGI_FORMAT_UNKNOWN = 0,
		
		DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
		DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
		DXGI_FORMAT_R32G32B32A32_UINT = 3,
		DXGI_FORMAT_R32G32B32A32_SINT = 4,
		
		DXGI_FORMAT_R32G32B32_TYPELESS = 5,
		DXGI_FORMAT_R32G32B32_FLOAT = 6,
		DXGI_FORMAT_R32G32B32_UINT = 7,
		DXGI_FORMAT_R32G32B32_SINT = 8,
		
		DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
		DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
		DXGI_FORMAT_R16G16B16A16_UNORM = 11,
		DXGI_FORMAT_R16G16B16A16_UINT = 12,
		DXGI_FORMAT_R16G16B16A16_SNORM = 13,
		DXGI_FORMAT_R16G16B16A16_SINT = 14,
		
		DXGI_FORMAT_R32G32_TYPELESS = 15,
		DXGI_FORMAT_R32G32_FLOAT = 16,
		DXGI_FORMAT_R32G32_UINT = 17,
		DXGI_FORMAT_R32G32_SINT = 18,
		
		DXGI_FORMAT_R32G8X24_TYPELESS = 19,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
		DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
		DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
		
		DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
		DXGI_FORMAT_R10G10B10A2_UNORM = 24,
		DXGI_FORMAT_R10G10B10A2_UINT = 25,
		
		DXGI_FORMAT_R11G11B10_FLOAT = 26,
		
		DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
		DXGI_FORMAT_R8G8B8A8_UNORM = 28,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
		DXGI_FORMAT_R8G8B8A8_UINT = 30,
		DXGI_FORMAT_R8G8B8A8_SNORM = 31,
		DXGI_FORMAT_R8G8B8A8_SINT = 32,
		
		DXGI_FORMAT_R16G16_TYPELESS = 33,
		DXGI_FORMAT_R16G16_FLOAT = 34,
		DXGI_FORMAT_R16G16_UNORM = 35,
		DXGI_FORMAT_R16G16_UINT = 36,
		DXGI_FORMAT_R16G16_SNORM = 37,
		DXGI_FORMAT_R16G16_SINT = 38,
		
		DXGI_FORMAT_R32_TYPELESS = 39,
		DXGI_FORMAT_D32_FLOAT = 40,
		DXGI_FORMAT_R32_FLOAT = 41,
		DXGI_FORMAT_R32_UINT = 42,
		DXGI_FORMAT_R32_SINT = 43,
		
		DXGI_FORMAT_R24G8_TYPELESS = 44,
		DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
		DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
		
		DXGI_FORMAT_R8G8_TYPELESS = 48,
		DXGI_FORMAT_R8G8_UNORM = 49,
		DXGI_FORMAT_R8G8_UINT = 50,
		DXGI_FORMAT_R8G8_SNORM = 51,
		DXGI_FORMAT_R8G8_SINT = 52,
		
		DXGI_FORMAT_R16_TYPELESS = 53,
		DXGI_FORMAT_R16_FLOAT = 54,
		DXGI_FORMAT_D16_UNORM = 55,
		DXGI_FORMAT_R16_UNORM = 56,
		DXGI_FORMAT_R16_UINT = 57,
		DXGI_FORMAT_R16_SNORM = 58,
		DXGI_FORMAT_R16_SINT = 59,
		
		DXGI_FORMAT_R8_TYPELESS = 60,
		DXGI_FORMAT_R8_UNORM = 61,
		DXGI_FORMAT_R8_UINT = 62,
		DXGI_FORMAT_R8_SNORM = 63,
		DXGI_FORMAT_R8_SINT = 64,
		DXGI_FORMAT_A8_UNORM = 65,
		
		DXGI_FORMAT_R1_UNORM = 66,
		
		DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
		
		DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
		DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
		
		DXGI_FORMAT_BC1_TYPELESS = 70,
		DXGI_FORMAT_BC1_UNORM = 71,
		DXGI_FORMAT_BC1_UNORM_SRGB = 72,
		
		DXGI_FORMAT_BC2_TYPELESS = 73,
		DXGI_FORMAT_BC2_UNORM = 74,
		DXGI_FORMAT_BC2_UNORM_SRGB = 75,
		
		DXGI_FORMAT_BC3_TYPELESS = 76,
		DXGI_FORMAT_BC3_UNORM = 77,
		DXGI_FORMAT_BC3_UNORM_SRGB = 78,
		
		DXGI_FORMAT_BC4_TYPELESS = 79,
		DXGI_FORMAT_BC4_UNORM = 80,
		DXGI_FORMAT_BC4_SNORM = 81,
		
		DXGI_FORMAT_BC5_TYPELESS = 82,
		DXGI_FORMAT_BC5_UNORM = 83,
		DXGI_FORMAT_BC5_SNORM = 84,
		
		DXGI_FORMAT_B5G6R5_UNORM = 85,
		DXGI_FORMAT_B5G5R5A1_UNORM = 86,
		DXGI_FORMAT_B8G8R8A8_UNORM = 87,
		DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	};

	enum D3D10_RESOURCE_DIMENSION
	{
		D3D10_RESOURCE_DIMENSION_UNKNOWN = 0,
		D3D10_RESOURCE_DIMENSION_BUFFER = 1,
		D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
		D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
		D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4,
	};

/*** implementation ***/

void mem_read(Stream & mem, DDSPixelFormat & pf)
{
	mem_read(mem, pf.size);
	mem_read(mem, pf.flags);
	mem_read(mem, pf.fourcc);
	mem_read(mem, pf.bitcount);
	mem_read(mem, pf.rmask);
	mem_read(mem, pf.gmask);
	mem_read(mem, pf.bmask);
	mem_read(mem, pf.amask);
}

void mem_read(Stream & mem, DDSCaps & caps)
{
	mem_read(mem, caps.caps1);
	mem_read(mem, caps.caps2);
	mem_read(mem, caps.caps3);
	mem_read(mem, caps.caps4);
}

void mem_read(Stream & mem, DDSHeader10 & header)
{
	mem_read(mem, header.dxgiFormat);
	mem_read(mem, header.resourceDimension);
	mem_read(mem, header.miscFlag);
	mem_read(mem, header.arraySize);
	mem_read(mem, header.reserved);
}

void mem_read(Stream & mem, DDSHeader & header)
{
	mem_read(mem, header.fourcc);
	mem_read(mem, header.size);
	mem_read(mem, header.flags);
	mem_read(mem, header.height);
	mem_read(mem, header.width);
	mem_read(mem, header.pitch);
	mem_read(mem, header.depth);
	mem_read(mem, header.mipmapcount);
	for (uint i = 0; i < 11; i++) mem_read(mem, header.reserved[i]);
	mem_read(mem, header.pf);
	mem_read(mem, header.caps);
	mem_read(mem, header.notused);

	if (header.hasDX10Header())
	{
		mem_read(mem, header.header10);
	}
}



DDSHeader::DDSHeader()
{
	this->fourcc = FOURCC_DDS;
	this->size = 124;
	this->flags  = (DDSD_CAPS|DDSD_PIXELFORMAT);
	this->height = 0;
	this->width = 0;
	this->pitch = 0;
	this->depth = 0;
	this->mipmapcount = 0;
	for (uint i = 0; i < 11; i++) this->reserved[i] = 0;

	// Store version information on the reserved header attributes.
	this->reserved[9] = MAKEFOURCC('N', 'V', 'T', 'T');
	this->reserved[10] = (0 << 16) | (9 << 8) | (5);	// major.minor.revision

	this->pf.size = 32;
	this->pf.flags = 0;
	this->pf.fourcc = 0;
	this->pf.bitcount = 0;
	this->pf.rmask = 0;
	this->pf.gmask = 0;
	this->pf.bmask = 0;
	this->pf.amask = 0;
	this->caps.caps1 = DDSCAPS_TEXTURE;
	this->caps.caps2 = 0;
	this->caps.caps3 = 0;
	this->caps.caps4 = 0;
	this->notused = 0;
	this->offset = 128;

	this->header10.dxgiFormat = DXGI_FORMAT_UNKNOWN;
	this->header10.resourceDimension = D3D10_RESOURCE_DIMENSION_UNKNOWN;
	this->header10.miscFlag = 0;
	this->header10.arraySize = 0;
	this->header10.reserved = 0;
}

void DDSHeader::setWidth(uint w)
{
	this->flags |= DDSD_WIDTH;
	this->width = w;
}

void DDSHeader::setHeight(uint h)
{
	this->flags |= DDSD_HEIGHT;
	this->height = h;
}

void DDSHeader::setDepth(uint d)
{
	this->flags |= DDSD_DEPTH;
	this->height = d;
}

void DDSHeader::setMipmapCount(uint count)
{
	if (count == 0 || count == 1)
	{
		this->flags &= ~DDSD_MIPMAPCOUNT;
		this->mipmapcount = 0;

		if (this->caps.caps2 == 0) {
			this->caps.caps1 = DDSCAPS_TEXTURE;
		}
		else {
			this->caps.caps1 = DDSCAPS_TEXTURE | DDSCAPS_COMPLEX;
		}
	}
	else
	{
		this->flags |= DDSD_MIPMAPCOUNT;
		this->mipmapcount = count;

		this->caps.caps1 |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
	}
}

void DDSHeader::setTexture2D()
{
	this->header10.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
}

void DDSHeader::setTexture3D()
{
	this->caps.caps2 = DDSCAPS2_VOLUME;

	this->header10.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE3D;
}

void DDSHeader::setTextureCube()
{
	this->caps.caps1 |= DDSCAPS_COMPLEX;
	this->caps.caps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALL_FACES;

	this->header10.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
	this->header10.arraySize = 6;
}

void DDSHeader::setLinearSize(uint size)
{
	this->flags &= ~DDSD_PITCH;
	this->flags |= DDSD_LINEARSIZE;
	this->pitch = size;
}

void DDSHeader::setPitch(uint pitch)
{
	this->flags &= ~DDSD_LINEARSIZE;
	this->flags |= DDSD_PITCH;
	this->pitch = pitch;
}

void DDSHeader::setFourCC(uint8 c0, uint8 c1, uint8 c2, uint8 c3)
{
	// set fourcc pixel format.
	this->pf.flags = DDPF_FOURCC;
	this->pf.fourcc = MAKEFOURCC(c0, c1, c2, c3);
	this->pf.bitcount = 0;
	this->pf.rmask = 0;
	this->pf.gmask = 0;
	this->pf.bmask = 0;
	this->pf.amask = 0;
}

void DDSHeader::setPixelFormat(uint bitcount, uint rmask, uint gmask, uint bmask, uint amask)
{
	// Make sure the masks are correct.
	if ((rmask & gmask) || \
		(rmask & bmask) || \
		(rmask & amask) || \
		(gmask & bmask) || \
		(gmask & amask) || \
		(bmask & amask)) {
		printf("DDS: bad RGBA masks, pixel format not set\n");
		return;
	}

	this->pf.flags = DDPF_RGB;

	if (amask != 0) {
		this->pf.flags |= DDPF_ALPHAPIXELS;
	}

	if (bitcount == 0)
	{
		// Compute bit count from the masks.
		uint total = rmask | gmask | bmask | amask;
		while(total != 0) {
			bitcount++;
			total >>= 1;
		}
	}

	if (!(bitcount > 0 && bitcount <= 32)) {
		printf("DDS: bad bit count, pixel format not set\n");
		return;
	}

	// Align to 8.
	if (bitcount <= 8) bitcount = 8;
	else if (bitcount <= 16) bitcount = 16;
	else if (bitcount <= 24) bitcount = 24;
	else bitcount = 32;

	this->pf.fourcc = 0; //findD3D9Format(bitcount, rmask, gmask, bmask, amask);
	this->pf.bitcount = bitcount;
	this->pf.rmask = rmask;
	this->pf.gmask = gmask;
	this->pf.bmask = bmask;
	this->pf.amask = amask;
}

void DDSHeader::setDX10Format(uint format)
{
	this->pf.flags = 0;
	this->header10.dxgiFormat = format;
}

void DDSHeader::setNormalFlag(bool b)
{
	if (b) this->pf.flags |= DDPF_NORMAL;
	else this->pf.flags &= ~DDPF_NORMAL;
}

void DDSHeader::setOffset(uint size)
{
	this->offset = size;
}

bool DDSHeader::hasDX10Header() const
{
	return this->pf.flags == 0;
}

DirectDrawSurface::DirectDrawSurface(const unsigned char *mem, uint size) : stream(mem, size), header()
{
	mem_read(stream, header);
	header.offset = 128;
}

DirectDrawSurface::DirectDrawSurface(const DDSHeader &ddsheader, const unsigned char *mem, uint size)
	: stream(mem, size), header(ddsheader)
{
}

DirectDrawSurface::~DirectDrawSurface()
{
}

bool DirectDrawSurface::isValid() const
{
	if (header.fourcc != FOURCC_DDS || header.size != 124)
	{
		return false;
	}
	
	const uint required = (DDSD_WIDTH|DDSD_HEIGHT|DDSD_CAPS|DDSD_PIXELFORMAT);
	if( (header.flags & required) != required ) {
		return false;
	}
	
	if (header.pf.size != 32) {
		return false;
	}

	/* in some files DDSCAPS_TEXTURE is missing: silently ignore */
	/*
	if( !(header.caps.caps1 & DDSCAPS_TEXTURE) ) {
		return false;
	}
	*/

	return true;
}

bool DirectDrawSurface::isSupported() const
{
	if (header.pf.flags & DDPF_FOURCC)
	{
		if (header.pf.fourcc != FOURCC_DXT1 &&
		    header.pf.fourcc != FOURCC_DXT2 &&
		    header.pf.fourcc != FOURCC_DXT3 &&
		    header.pf.fourcc != FOURCC_DXT4 &&
		    header.pf.fourcc != FOURCC_DXT5 &&
		    header.pf.fourcc != FOURCC_RXGB &&
		    header.pf.fourcc != FOURCC_ATI1 &&
		    header.pf.fourcc != FOURCC_ATI2)
		{
			// Unknown fourcc code.
			return false;
		}
	}
	else if (header.pf.flags & DDPF_RGB)
	{
		// All RGB formats are supported now.
	}
	else
	{
		return false;
	}
	
	if (isTextureCube() && (header.caps.caps2 & DDSCAPS2_CUBEMAP_ALL_FACES) != DDSCAPS2_CUBEMAP_ALL_FACES)
	{
		// Cubemaps must contain all faces.
		return false;
	}
	
	if (isTexture3D())
	{
		// @@ 3D textures not supported yet.
		return false;
	}
	
	return true;
}


uint DirectDrawSurface::mipmapCount() const
{
	if (header.flags & DDSD_MIPMAPCOUNT) return header.mipmapcount;
	else return 1;
}


uint DirectDrawSurface::width() const
{
	if (header.flags & DDSD_WIDTH) return header.width;
	else return 1;
}

uint DirectDrawSurface::height() const
{
	if (header.flags & DDSD_HEIGHT) return header.height;
	else return 1;
}

uint DirectDrawSurface::depth() const
{
	if (header.flags & DDSD_DEPTH) return header.depth;
	else return 1;
}

bool DirectDrawSurface::hasAlpha() const
{
	if ((header.pf.flags & DDPF_RGB) && (header.pf.amask == 0))
	{
		return false;
	}
	else if (header.pf.fourcc == FOURCC_DXT1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool DirectDrawSurface::isTexture2D() const
{
	return !isTexture3D() && !isTextureCube();
}

bool DirectDrawSurface::isTexture3D() const
{
	return (header.caps.caps2 & DDSCAPS2_VOLUME) != 0;
}

bool DirectDrawSurface::isTextureCube() const
{
	return (header.caps.caps2 & DDSCAPS2_CUBEMAP) != 0;
}

void DirectDrawSurface::mipmap(Image * img, uint face, uint mipmap)
{
	stream.seek(offset(face, mipmap));
	
	uint w = width();
	uint h = height();
	
	// Compute width and height.
	for (uint m = 0; m < mipmap; m++)
	{
		w = max(1U, w / 2);
		h = max(1U, h / 2);
	}
	
	img->allocate(w, h);
	
	if (header.pf.flags & DDPF_RGB) 
	{
		readLinearImage(img);
	}
	else if (header.pf.flags & DDPF_FOURCC)
	{
		readBlockImage(img);
	}
}

void DirectDrawSurface::readLinearImage(Image * img)
{
	
	const uint w = img->width();
	const uint h = img->height();
	
	uint rshift, rsize;
	PixelFormat::maskShiftAndSize(header.pf.rmask, &rshift, &rsize);
	
	uint gshift, gsize;
	PixelFormat::maskShiftAndSize(header.pf.gmask, &gshift, &gsize);
	
	uint bshift, bsize;
	PixelFormat::maskShiftAndSize(header.pf.bmask, &bshift, &bsize);
	
	uint ashift, asize;
	PixelFormat::maskShiftAndSize(header.pf.amask, &ashift, &asize);

	uint byteCount = (header.pf.bitcount + 7) / 8;

	if (byteCount > 4)
	{
		/* just in case... we could have segfaults later on if byteCount > 4 */
		printf("DDS: bitcount too large");
		return;
	}

	// set image format: RGB or ARGB
	// alpha channel exists if and only if the alpha mask is non-zero
	if (header.pf.amask == 0)
	{
		img->setFormat(Image::Format_RGB);
	}
	else
	{
		img->setFormat(Image::Format_ARGB);
	}

	// Read linear RGB images.
	for (uint y = 0; y < h; y++)
	{
		for (uint x = 0; x < w; x++)
		{
			uint c = 0;
			mem_read(stream, (unsigned char *)(&c), byteCount);

			Color32 pixel(0, 0, 0, 0xFF);
			pixel.r = PixelFormat::convert(c >> rshift, rsize, 8);
			pixel.g = PixelFormat::convert(c >> gshift, gsize, 8);
			pixel.b = PixelFormat::convert(c >> bshift, bsize, 8);
			pixel.a = PixelFormat::convert(c >> ashift, asize, 8);

			img->pixel(x, y) = pixel;
		}
	}
}

void DirectDrawSurface::readBlockImage(Image * img)
{
	const uint w = img->width();
	const uint h = img->height();
	
	const uint bw = (w + 3) / 4;
	const uint bh = (h + 3) / 4;

	// set image format: RGB or ARGB
	// all DXT formats have alpha channel, except DXT1
	if (header.pf.fourcc == FOURCC_DXT1)
	{
		img->setFormat(Image::Format_RGB);
	}
	else
	{
		img->setFormat(Image::Format_ARGB);
	}
	
	for (uint by = 0; by < bh; by++)
	{
		for (uint bx = 0; bx < bw; bx++)
		{
			ColorBlock block;
			
			// Read color block.
			readBlock(&block);
			
			// Write color block.
			for (uint y = 0; y < min(4U, h-4*by); y++)
			{
				for (uint x = 0; x < min(4U, w-4*bx); x++)
				{
					img->pixel(4*bx+x, 4*by+y) = block.color(x, y);
				}
			}
		}
	}
}

static Color32 buildNormal(uint8 x, uint8 y)
{
	float nx = 2 * (x / 255.0f) - 1;
	float ny = 2 * (y / 255.0f) - 1;
	float nz = 0.0f;
	if (1 - nx*nx - ny*ny > 0) nz = sqrtf(1 - nx*nx - ny*ny);
	uint8 z = clamp(int(255.0f * (nz + 1) / 2.0f), 0, 255);
	
	return Color32(x, y, z);
}


void DirectDrawSurface::readBlock(ColorBlock * rgba)
{
	if (header.pf.fourcc == FOURCC_DXT1)
	{
		BlockDXT1 block;
		mem_read(stream, block);
		block.decodeBlock(rgba);
	}
	else if (header.pf.fourcc == FOURCC_DXT2 ||
	    header.pf.fourcc == FOURCC_DXT3)
	{
		BlockDXT3 block;
		mem_read(stream, block);
		block.decodeBlock(rgba);
	}
	else if (header.pf.fourcc == FOURCC_DXT4 ||
	    header.pf.fourcc == FOURCC_DXT5 ||
	    header.pf.fourcc == FOURCC_RXGB)
	{
		BlockDXT5 block;
		mem_read(stream, block);
		block.decodeBlock(rgba);
		
		if (header.pf.fourcc == FOURCC_RXGB)
		{
			// Swap R & A.
			for (int i = 0; i < 16; i++)
			{
				Color32 & c = rgba->color(i);
				uint tmp = c.r;
				c.r = c.a;
				c.a = tmp;
			}
		}
	}
	else if (header.pf.fourcc == FOURCC_ATI1)
	{
		BlockATI1 block;
		mem_read(stream, block);
		block.decodeBlock(rgba);
	}
	else if (header.pf.fourcc == FOURCC_ATI2)
	{
		BlockATI2 block;
		mem_read(stream, block);
		block.decodeBlock(rgba);
	}
	
	// If normal flag set, convert to normal.
	if (header.pf.flags & DDPF_NORMAL)
	{
		if (header.pf.fourcc == FOURCC_ATI2)
		{
			for (int i = 0; i < 16; i++)
			{
				Color32 & c = rgba->color(i);
				c = buildNormal(c.r, c.g);
			}
		}
		else if (header.pf.fourcc == FOURCC_DXT5)
		{
			for (int i = 0; i < 16; i++)
			{
				Color32 & c = rgba->color(i);
				c = buildNormal(c.a, c.g);
			}
		}
	}
}


uint DirectDrawSurface::blockSize() const
{
	switch(header.pf.fourcc)
	{
		case FOURCC_DXT1:
		case FOURCC_ATI1:
			return 8;
		case FOURCC_DXT2:
		case FOURCC_DXT3:
		case FOURCC_DXT4:
		case FOURCC_DXT5:
		case FOURCC_RXGB:
		case FOURCC_ATI2:
			return 16;
	};

	// Not a block image.
	return 0;
}

uint DirectDrawSurface::mipmapSize(uint mipmap) const
{
	uint w = width();
	uint h = height();
	uint d = depth();
	
	for (uint m = 0; m < mipmap; m++)
	{
		w = max(1U, w / 2);
		h = max(1U, h / 2);
		d = max(1U, d / 2);
	}

	if (header.pf.flags & DDPF_FOURCC)
	{
		// @@ How are 3D textures aligned?
		w = (w + 3) / 4;
		h = (h + 3) / 4;
		return blockSize() * w * h;
	}
	else if (header.pf.flags & DDPF_RGB)
	{
		// Align pixels to bytes.
		uint byteCount = (header.pf.bitcount + 7) / 8;
		
		// Align pitch to 4 bytes.
		uint pitch = 4 * ((w * byteCount + 3) / 4);
		
		return pitch * h * d;
	}
	else {
		printf("DDS: mipmap format not supported\n");
		return(0);
	};
}

uint DirectDrawSurface::faceSize() const
{
	const uint count = mipmapCount();
	uint size = 0;
	
	for (uint m = 0; m < count; m++)
	{
		size += mipmapSize(m);
	}
	
	return size;
}

uint DirectDrawSurface::offset(const uint face, const uint mipmap)
{
	uint size = header.offset; //sizeof(DDSHeader);

	if (header.hasDX10Header())
	{
		size += 20; // sizeof(DDSHeader10);
	}
	
	if (face != 0)
	{
		size += face * faceSize();
	}
	
	for (uint m = 0; m < mipmap; m++)
	{
		size += mipmapSize(m);
	}
	
	return size;
}


void DirectDrawSurface::printInfo() const
{
	/* printf("FOURCC: %c%c%c%c\n", ((unsigned char *)&header.fourcc)[0], ((unsigned char *)&header.fourcc)[1], ((unsigned char *)&header.fourcc)[2], ((unsigned char *)&header.fourcc)[3]); */
	printf("Flags: 0x%.8X\n", header.flags);
	if (header.flags & DDSD_CAPS) printf("\tDDSD_CAPS\n");
	if (header.flags & DDSD_PIXELFORMAT) printf("\tDDSD_PIXELFORMAT\n");
	if (header.flags & DDSD_WIDTH) printf("\tDDSD_WIDTH\n");
	if (header.flags & DDSD_HEIGHT) printf("\tDDSD_HEIGHT\n");
	if (header.flags & DDSD_DEPTH) printf("\tDDSD_DEPTH\n");
	if (header.flags & DDSD_PITCH) printf("\tDDSD_PITCH\n");
	if (header.flags & DDSD_LINEARSIZE) printf("\tDDSD_LINEARSIZE\n");
	if (header.flags & DDSD_MIPMAPCOUNT) printf("\tDDSD_MIPMAPCOUNT\n");

	printf("Height: %d\n", header.height);
	printf("Width: %d\n", header.width);
	printf("Depth: %d\n", header.depth);
	if (header.flags & DDSD_PITCH) printf("Pitch: %d\n", header.pitch);
	else if (header.flags & DDSD_LINEARSIZE) printf("Linear size: %d\n", header.pitch);
	printf("Mipmap count: %d\n", header.mipmapcount);
	
	printf("Pixel Format:\n");
	/* printf("\tSize: %d\n", header.pf.size); */
	printf("\tFlags: 0x%.8X\n", header.pf.flags);
	if (header.pf.flags & DDPF_RGB) printf("\t\tDDPF_RGB\n");
	if (header.pf.flags & DDPF_FOURCC) printf("\t\tDDPF_FOURCC\n");
	if (header.pf.flags & DDPF_ALPHAPIXELS) printf("\t\tDDPF_ALPHAPIXELS\n");
	if (header.pf.flags & DDPF_ALPHA) printf("\t\tDDPF_ALPHA\n");
	if (header.pf.flags & DDPF_PALETTEINDEXED1) printf("\t\tDDPF_PALETTEINDEXED1\n");
	if (header.pf.flags & DDPF_PALETTEINDEXED2) printf("\t\tDDPF_PALETTEINDEXED2\n");
	if (header.pf.flags & DDPF_PALETTEINDEXED4) printf("\t\tDDPF_PALETTEINDEXED4\n");
	if (header.pf.flags & DDPF_PALETTEINDEXED8) printf("\t\tDDPF_PALETTEINDEXED8\n");
	if (header.pf.flags & DDPF_ALPHAPREMULT) printf("\t\tDDPF_ALPHAPREMULT\n");
	if (header.pf.flags & DDPF_NORMAL) printf("\t\tDDPF_NORMAL\n");
	
	printf("\tFourCC: '%c%c%c%c'\n", ((header.pf.fourcc >> 0) & 0xFF), ((header.pf.fourcc >> 8) & 0xFF), ((header.pf.fourcc >> 16) & 0xFF), ((header.pf.fourcc >> 24) & 0xFF));
	printf("\tBit count: %d\n", header.pf.bitcount);
	printf("\tRed mask: 0x%.8X\n", header.pf.rmask);
	printf("\tGreen mask: 0x%.8X\n", header.pf.gmask);
	printf("\tBlue mask: 0x%.8X\n", header.pf.bmask);
	printf("\tAlpha mask: 0x%.8X\n", header.pf.amask);

	printf("Caps:\n");
	printf("\tCaps 1: 0x%.8X\n", header.caps.caps1);
	if (header.caps.caps1 & DDSCAPS_COMPLEX) printf("\t\tDDSCAPS_COMPLEX\n");
	if (header.caps.caps1 & DDSCAPS_TEXTURE) printf("\t\tDDSCAPS_TEXTURE\n");
	if (header.caps.caps1 & DDSCAPS_MIPMAP) printf("\t\tDDSCAPS_MIPMAP\n");

	printf("\tCaps 2: 0x%.8X\n", header.caps.caps2);
	if (header.caps.caps2 & DDSCAPS2_VOLUME) printf("\t\tDDSCAPS2_VOLUME\n");
	else if (header.caps.caps2 & DDSCAPS2_CUBEMAP)
	{
		printf("\t\tDDSCAPS2_CUBEMAP\n");
		if ((header.caps.caps2 & DDSCAPS2_CUBEMAP_ALL_FACES) == DDSCAPS2_CUBEMAP_ALL_FACES) printf("\t\tDDSCAPS2_CUBEMAP_ALL_FACES\n");
		else {
			if (header.caps.caps2 & DDSCAPS2_CUBEMAP_POSITIVEX) printf("\t\tDDSCAPS2_CUBEMAP_POSITIVEX\n");
			if (header.caps.caps2 & DDSCAPS2_CUBEMAP_NEGATIVEX) printf("\t\tDDSCAPS2_CUBEMAP_NEGATIVEX\n");
			if (header.caps.caps2 & DDSCAPS2_CUBEMAP_POSITIVEY) printf("\t\tDDSCAPS2_CUBEMAP_POSITIVEY\n");
			if (header.caps.caps2 & DDSCAPS2_CUBEMAP_NEGATIVEY) printf("\t\tDDSCAPS2_CUBEMAP_NEGATIVEY\n");
			if (header.caps.caps2 & DDSCAPS2_CUBEMAP_POSITIVEZ) printf("\t\tDDSCAPS2_CUBEMAP_POSITIVEZ\n");
			if (header.caps.caps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ) printf("\t\tDDSCAPS2_CUBEMAP_NEGATIVEZ\n");
		}
	}

	printf("\tCaps 3: 0x%.8X\n", header.caps.caps3);
	printf("\tCaps 4: 0x%.8X\n", header.caps.caps4);

	if (header.pf.flags == 0)
	{
		printf("DX10 Header:\n");
		printf("\tDXGI Format: %u\n", header.header10.dxgiFormat);
		printf("\tResource dimension: %u\n", header.header10.resourceDimension);
		printf("\tMisc flag: %u\n", header.header10.miscFlag);
		printf("\tArray size: %u\n", header.header10.arraySize);
	}
	
	if (header.reserved[9] == MAKEFOURCC('N', 'V', 'T', 'T'))
	{
		int major = (header.reserved[10] >> 16) & 0xFF;
		int minor = (header.reserved[10] >> 8) & 0xFF;
		int revision= header.reserved[10] & 0xFF;
		
		printf("Version:\n");
		printf("\tNVIDIA Texture Tools %d.%d.%d\n", major, minor, revision);
	}
}

