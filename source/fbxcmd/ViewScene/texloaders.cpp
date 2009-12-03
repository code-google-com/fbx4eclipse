#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <math.h>
#include <sys/stat.h>
#include <fbxsdk.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "GL/glew.h"
//#define GLUT_DISABLE_ATEXIT_HACK
#if defined(__MACH__)
#include <GLUT/glut.h>
#else
#include "GL/glut.h"
#endif

#include <fbxfilesdk/fbxfilesdk_nsuse.h>

#include "Texture.h"
#include "Common.h"
#include "texloaders.h"
#include "targa.h"
#include "dds/Image.h"
#include "dds/dds_api.h"
#include "dds/DirectDrawSurface.h"
#include "daostream.h"
#include "ResourceManager.h"

typedef char int8;
typedef short int16;
typedef int int32;
typedef unsigned char byte;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


// sprintf for TSTR without having to worry about buffer size.
KString FormatString(const char* format,...)
{
	char buffer[512];
	va_list args;
	va_start(args, format);
	int nChars = _vsnprintf(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		va_end(args);
		return KString((char*)buffer);
	} else {
		size_t Size = _vscprintf(format, args);
		KString text('\0', Size + 1);
		nChars = _vsnprintf(text.Buffer(), Size, format, args);
		va_end(args);
		return text;
	}
}

class IOStream
{
public:
	IOStream() : fh(NULL){}

	~IOStream() { 
		Close(); 
	}

	void *&pdata() { return data; }
	void* data;
	FILE* fh;
	
	bool Open(const char *file, bool readonly, KError & lError)
	{
		if (fh) Close();

		if (file == NULL && readonly)
		{
			fh = stdin;
		}
		else if (file == NULL && !readonly)
		{
			fh = stdout;
		}
		else
		{
			fh = fopen(file, readonly ? "rbS" : "wbS");
			if (!fh) {
				//lError.SetLastError( 1, FormatString("Unable to open file: %s", file) );
				return false;
			}
			setvbuf(fh, NULL, _IOFBF, 0x8000);
		}
		return true;
	}

	bool Open(const wchar_t *file, bool readonly, KError & lError)
	{
		if (fh) Close();

		if (file == NULL && readonly)
		{
			fh = stdin;
		}
		else if (file == NULL && !readonly)
		{
			fh = stdout;
		}
		else
		{
			fh = _wfopen(file, readonly ? L"rbS" : L"wbS");
			if (!fh) {
				lError.SetLastError( 1, FormatString("Unable to open file: %s", file) );
				return false;
			}
			setvbuf(fh, NULL, _IOFBF, 0x8000);
		}
		return true;
	}

	void Close()
	{
		if (fh) {
			fclose(fh);
			fh = NULL;
		}
	}

	size_t Read(void *buf, size_t size, size_t count)
	{
		size_t n = (size_t)fread(buf, size, count, fh);
		int err = errno;
		if (!(n == count && err == 0))
		{
			int t = Tell();
			//ASSERT(n == count && err == 0);
		}
		return n;
	}

	size_t Read(void *buf, size_t count)
	{
		return Read(buf, 1, count);
	}

	size_t Write(const void *buf, size_t size, size_t count)
	{
		return (size_t)fwrite(buf, size, count, fh);
	}

	size_t Write(const void *buf, size_t count)
	{
		return Write(buf, 1, count);
	}


	int Seek(int whence, long offset)
	{
		return fseek(fh, offset, whence);
	}

	int Seek(long offset)
	{
		return fseek(fh, offset, SEEK_SET);
	}

	int Tell()
	{
		return ftell(fh);
	}

	int TellEnd()
	{
		struct _stat64 data;
		memset(&data, 0, sizeof(data));
		_fstat64(_fileno(fh), &data);
		return int(data.st_size);
	}
	
	int TellRemain()
	{
		return (TellEnd() - Tell());
	}


	bool Eof() const
	{
		return (feof(fh) != 0);
	}

};



//************************************
// Method:    loadTargaFile
// FullName:  loadTargaFile
// Access:    public static 
// Returns:   bool
// Qualifier:
// Parameter: const KString & lFileName
// Parameter: VSTexture * lTexture
// Parameter: KError & lError
//************************************
static bool loadTargaFile( const KString & lFileName, VSTexture * lTexture, KError & lError )
{
	tga_image lTGAImage;

	tga_result errcode = tga_read(&lTGAImage, lFileName.Buffer());
	if (TGA_NOERR == errcode)
	{
		// Make sure the image is left to right
		if (tga_is_right_to_left(&lTGAImage)) tga_flip_horiz(&lTGAImage);

		// Make sure the image is bottom to top
		if (tga_is_top_to_bottom(&lTGAImage)) tga_flip_vert(&lTGAImage);

		// Make the image BGR 24
		tga_convert_depth(&lTGAImage, 24);

		lTexture->mW = lTGAImage.width;
		lTexture->mH = lTGAImage.height;
		lTexture->internalformat = 3;
		lTexture->format = GL_BGR_EXT;
		lTexture->type = GL_UNSIGNED_BYTE;

		lTexture->mImageData  = new unsigned char[lTGAImage.width*lTGAImage.height*lTGAImage.pixel_depth/8];
		memcpy(lTexture->mImageData, lTGAImage.image_data, lTGAImage.width*lTGAImage.height*lTGAImage.pixel_depth/8);

		tga_free_buffers(&lTGAImage);

		return true;
	}
	else
	{
		lError.SetLastError(errcode, tga_error(errcode) );

		return false;
	}
}

//! Check whether a number is a power of two.
bool isPowerOfTwo( unsigned int x )
{
	while ( ! ( x == 0 || x & 1 ) )
		x = x >> 1;
	return ( x == 1 );
}


//! Converts RLE-encoded data into pixel data.
/*!
* TGA in particular uses the PackBits format described at
* http://en.wikipedia.org/wiki/PackBits and in the TGA spec.
*/
bool uncompressRLE( IDAOStream & f, int w, int h, int bytespp, uint8 * pixel )
{
    int size = (f.TellEnd() - f.Tell());
	byte *data = new byte[ size ];
	f.Read(data, sizeof(byte), size); 
	
	int c = 0; // total pixel count
	int o = 0; // data offset

	bool ok = true;
	uint8 rl; // runlength - 1
	while ( c < w * h && ok )
	{
		rl = data[o++];
		if ( rl & 0x80 ) // if RLE packet
		{
			uint8 px[8]; // pixel data in this packet (assume bytespp < 8)
			for ( int b = 0; b < bytespp; b++ )
				px[b] = data[o++];
			rl &= 0x7f; // strip RLE bit
			do
			{
				for ( int b = 0; b < bytespp; b++ )
					*pixel++ = px[b]; // expand pixel data (rl+1) times
			}
			while ( ++c < w*h && rl-- > 0  );
		}
		else
		{
			do
			{
				for ( int b = 0; b < bytespp; b++ )
					*pixel++ = data[o++]; // write (rl+1) raw pixels
			}
			while (  ++c < w*h && rl-- > 0 );
		}
		if ( o >= size ) 
			ok = false;
	}
	delete [] data;
	return ok;
}

//! Convert pixels to RGBA
/*!
* \param data Pixels to convert
* \param w Width of the image
* \param h Height of the image
* \param bytespp Number of bytes per pixel
* \param mask Bitmask for pixel data
* \param flipV Whether to flip the data vertically
* \param flipH Whether to flip the data horizontally
* \param pixl Pixels to output
*/
void convertToRGBA( const uint8 * data, int w, int h, int bytespp, const uint32 mask[], bool flipV, bool flipH, uint8 * pixl )
{
	memset( pixl, 0, w * h * 4 );

	static const int rgbashift[4] = { 0, 8, 16, 24 };

	for ( int a = 0; a < 4; a++ )
	{
		if ( mask[a] )
		{
			uint32 msk = mask[ a ];
			int rshift = 0;
			while ( msk != 0 && ( msk & 0xffffff00 ) ) { msk = msk >> 1; rshift++; }
			int lshift = rgbashift[ a ];
			while ( msk != 0 && ( ( msk & 0x80 ) == 0 ) )	{ msk = msk << 1; lshift++; }
			msk = mask[ a ];

			const uint8 * src = data;
			const uint32 inc = ( flipH ? -1 : 1 );
			for ( int y = 0; y < h; y++ )
			{
				uint32 * dst = (uint32 *) ( pixl + 4 * ( w * ( flipV ? h - y - 1 : y ) + ( flipH ? w - 1 : 0 ) ) );
				if ( rshift == lshift )
				{
					for ( int x = 0; x < w; x++ )
					{
						*dst |= *( (const uint32 *) src ) & msk;
						dst += inc;
						src += bytespp;
					}
				}
				else
				{
					for ( int x = 0; x < w; x++ )
					{
						*dst |= ( *( (const uint32 *) src ) & msk ) >> rshift << lshift;
						dst += inc;
						src += bytespp;
					}
				}
			}
		}
		else if ( a == 3 )
		{
			uint32 * dst = (uint32 *) pixl;
			uint32 x = 0xff << rgbashift[ a ];
			for ( int c = w * h; c > 0; c-- )
				*dst++ |= x;
		}
	}
}

int texLoadRaw( IDAOStream & f, VSTexture * lTexture
              , int width, int height, int num_mipmaps
			  , int bpp, int bytespp, const uint32 mask[]
			  , bool flipV /*= false*/, bool flipH /*= false*/, bool rle /*= false*/
			  , KError & lError
			  )
{
	lError.ClearLastError();

	if ( bytespp * 8 != bpp || bpp > 32 || bpp < 8 ) {
		lError.SetLastError( 1, FormatString( "unsupported image depth %d / %d", bpp, bytespp ) );
		return 0;
	}

	uint8 * data1 = new uint8[ width * height * 4 ];
	uint8 * data2 = new uint8[ width * height * 4 ];

	int w = width;
	int h = height;
	int m = 0;

	if ( m < num_mipmaps )
	{
		w = width >> m;
		h = height >> m;

		if ( w == 0 ) w = 1;
		if ( h == 0 ) h = 1;

		if ( rle )
		{
			if ( ! uncompressRLE( f, w, h, bytespp, data1 ) )
			{
				lError.SetLastError(1,"unexpected EOF" );
			}
		}
		else if ( f.Read( (char *) data1, w * h * bytespp ) != w * h * bytespp )
		{
			lError.SetLastError(1,"unexpected EOF" );
		}

		if ( -1 == lError.GetLastErrorID())
		{
			convertToRGBA( data1, w, h, bytespp, mask, flipV, flipH, data2 );
			lTexture->mH = h;
			lTexture->mW = w;
			lTexture->mImageData = data2;
			lTexture->internalformat = 4;
			lTexture->format = GL_RGBA;
			lTexture->type = GL_UNSIGNED_BYTE;

			data2 = NULL;
			++m;
		}
	}

	delete [] data2 ;
	delete [] data1 ;

	return m;
}

int texLoadPal( IDAOStream & f, VSTexture * lTexture
			   , int width, int height, int num_mipmaps, int bpp, int bytespp
			   , const uint32 colormap[], bool flipV, bool flipH, bool rle
			   , KError & lError
			   )
{
	lError.ClearLastError();
	if ( bpp != 8 || bytespp != 1 )
	{
		lError.SetLastError( 1, FormatString( "unsupported image depth %d / %d", bpp, bytespp ) );
		return 0;
	}

	uint8 * data = new uint8[ width * height * 1 ];
	uint8 * pixl = new uint8[ width * height * 4 ];


	int w = width;
	int h = height;
	int m = 0;

	while ( m < num_mipmaps )
	{
		w = width >> m;
		h = height >> m;

		if ( w == 0 ) w = 1;
		if ( h == 0 ) h = 1;

		if ( rle )
		{
			if ( ! uncompressRLE( f, w, h, bytespp, data ) )
			{
				lError.SetLastError( 1, FormatString( "unsupported image depth %d / %d", bpp, bytespp ) );
			}
		}
		else if ( f.Read( (char *) data, w * h * bytespp ) != w * h * bytespp )
		{
			lError.SetLastError( 1, FormatString( "unsupported image depth %d / %d", bpp, bytespp ) );
		}

		if ( -1 == lError.GetLastErrorID())
		{
			uint8 * src = data;
			for ( int y = 0; y < h; y++ )
			{
				uint32 * dst = (uint32 *) ( pixl + 4 * ( w * ( flipV ? h - y - 1 : y ) + ( flipH ? w - 1 : 0 ) ) );
				for ( int x = 0; x < w; x++ )
				{
					*dst++ = colormap[*src++];
				}
			}

			lTexture->mH = h;
			lTexture->mW = w;
			lTexture->mImageData = pixl;
			lTexture->internalformat = 4;
			lTexture->format = GL_RGBA;
			lTexture->type = GL_UNSIGNED_BYTE;
			pixl = NULL;
			++m;
		}
	}

	delete [] pixl ;
	delete [] data ;

	return m;
}

// thanks nvidia for providing the source code to flip dxt images

typedef struct
{
	unsigned short col0, col1;
	unsigned char row[4];
} DXTColorBlock_t;

typedef struct
{
	unsigned short row[4];
} DXT3AlphaBlock_t;

typedef struct
{
	unsigned char alpha0, alpha1;
	unsigned char row[6];
} DXT5AlphaBlock_t;

void SwapMem(void *byte1, void *byte2, int size)
{
	unsigned char *tmp=(unsigned char *)malloc(sizeof(unsigned char)*size);
	memcpy(tmp, byte1, size);
	memcpy(byte1, byte2, size);
	memcpy(byte2, tmp, size);
	free(tmp);
}

inline void SwapChar( unsigned char * x, unsigned char * y )
{
	unsigned char z = *x;
	*x = *y;
	*y = z;
}

inline void SwapShort( unsigned short * x, unsigned short * y )
{
	unsigned short z = *x;
	*x = *y;
	*y = z;
}

void flipDXT1Blocks(DXTColorBlock_t *Block, int NumBlocks)
{
	int i;
	DXTColorBlock_t *ColorBlock=Block;
	for(i=0;i<NumBlocks;i++)
	{
		SwapChar( &ColorBlock->row[0], &ColorBlock->row[3] );
		SwapChar( &ColorBlock->row[1], &ColorBlock->row[2] );
		ColorBlock++;
	}
}

void flipDXT3Blocks(DXTColorBlock_t *Block, int NumBlocks)
{
	int i;
	DXTColorBlock_t *ColorBlock=Block;
	DXT3AlphaBlock_t *AlphaBlock;
	for(i=0;i<NumBlocks;i++)
	{
		AlphaBlock=(DXT3AlphaBlock_t *)ColorBlock;
		SwapShort( &AlphaBlock->row[0], &AlphaBlock->row[3] );
		SwapShort( &AlphaBlock->row[1], &AlphaBlock->row[2] );
		ColorBlock++;
		SwapChar( &ColorBlock->row[0], &ColorBlock->row[3] );
		SwapChar( &ColorBlock->row[1], &ColorBlock->row[2] );
		ColorBlock++;
	}
}

void flipDXT5Alpha(DXT5AlphaBlock_t *Block)
{
	unsigned long *Bits, Bits0=0, Bits1=0;

	memcpy(&Bits0, &Block->row[0], sizeof(unsigned char)*3);
	memcpy(&Bits1, &Block->row[3], sizeof(unsigned char)*3);

	Bits=((unsigned long *)&(Block->row[0]));
	*Bits&=0xff000000;
	*Bits|=(unsigned char)(Bits1>>12)&0x00000007;
	*Bits|=(unsigned char)((Bits1>>15)&0x00000007)<<3;
	*Bits|=(unsigned char)((Bits1>>18)&0x00000007)<<6;
	*Bits|=(unsigned char)((Bits1>>21)&0x00000007)<<9;
	*Bits|=(unsigned char)(Bits1&0x00000007)<<12;
	*Bits|=(unsigned char)((Bits1>>3)&0x00000007)<<15;
	*Bits|=(unsigned char)((Bits1>>6)&0x00000007)<<18;
	*Bits|=(unsigned char)((Bits1>>9)&0x00000007)<<21;

	Bits=((unsigned long *)&(Block->row[3]));
	*Bits&=0xff000000;
	*Bits|=(unsigned char)(Bits0>>12)&0x00000007;
	*Bits|=(unsigned char)((Bits0>>15)&0x00000007)<<3;
	*Bits|=(unsigned char)((Bits0>>18)&0x00000007)<<6;
	*Bits|=(unsigned char)((Bits0>>21)&0x00000007)<<9;
	*Bits|=(unsigned char)(Bits0&0x00000007)<<12;
	*Bits|=(unsigned char)((Bits0>>3)&0x00000007)<<15;
	*Bits|=(unsigned char)((Bits0>>6)&0x00000007)<<18;
	*Bits|=(unsigned char)((Bits0>>9)&0x00000007)<<21;
}

void flipDXT5Blocks(DXTColorBlock_t *Block, int NumBlocks)
{
	DXTColorBlock_t *ColorBlock=Block;
	DXT5AlphaBlock_t *AlphaBlock;
	int i;

	for(i=0;i<NumBlocks;i++)
	{
		AlphaBlock=(DXT5AlphaBlock_t *)ColorBlock;

		flipDXT5Alpha(AlphaBlock);
		ColorBlock++;

		SwapChar( &ColorBlock->row[0], &ColorBlock->row[3] );
		SwapChar( &ColorBlock->row[1], &ColorBlock->row[2] );
		ColorBlock++;
	}
}

void flipDXT( GLenum glFormat, int width, int height, unsigned char * image )
{
	int linesize, j;

	DXTColorBlock_t *top;
	DXTColorBlock_t *bottom;
	int xblocks=width/4;
	int yblocks=height/4;

	switch ( glFormat)
	{
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: 
		linesize=xblocks*8;
		for(j=0;j<(yblocks>>1);j++)
		{
			top=(DXTColorBlock_t *)(image+j*linesize);
			bottom=(DXTColorBlock_t *)(image+(((yblocks-j)-1)*linesize));
			flipDXT1Blocks(top, xblocks);
			flipDXT1Blocks(bottom, xblocks);
			SwapMem(bottom, top, linesize);
		}
		break;

	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		linesize=xblocks*16;
		for(j=0;j<(yblocks>>1);j++)
		{
			top=(DXTColorBlock_t *)(image+j*linesize);
			bottom=(DXTColorBlock_t *)(image+(((yblocks-j)-1)*linesize));
			flipDXT3Blocks(top, xblocks);
			flipDXT3Blocks(bottom, xblocks);
			SwapMem(bottom, top, linesize);
		}
		break;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		linesize=xblocks*16;
		for(j=0;j<(yblocks>>1);j++)
		{
			top=(DXTColorBlock_t *)(image+j*linesize);
			bottom=(DXTColorBlock_t *)(image+(((yblocks-j)-1)*linesize));
			flipDXT5Blocks(top, xblocks);
			flipDXT5Blocks(bottom, xblocks);
			SwapMem(bottom, top, linesize);
		}
		break;
	default:
		return;
	}
}


GLuint texLoadDXT( IDAOStream & f, VSTexture * lTexture
				  , GLenum /*glFormat*/, int /*blockSize*/, uint32 /*width*/, uint32 /*height*/
				  , uint32 mipmaps, bool flipV 
				  , KError & lError
				  )
{
	// load the pixels
	f.Seek(SEEK_SET, 0);

	int size = f.TellEnd() - f.Tell();
	byte *bytes = new byte[ size ];
	f.Read(bytes, sizeof(byte), size); 

	GLuint m = 0;
	if ( m < mipmaps )
	{
		// load face 0, mipmap m
		if ( Image * img = load_dds(bytes, size, 0, m) )
		{
			// convert texture to OpenGL RGBA format
			unsigned int w = img->width();
			unsigned int h = img->height();
			GLubyte * pixels = new GLubyte[w * h * 4];
			Color32 * src = img->pixels();
			GLubyte * dst = pixels;
			//qWarning() << "flipV = " << flipV;
			for ( uint32 y = 0; y < h; y++ )
			{
				for ( uint32 x = 0; x < w; x++ )
				{
					*dst++ = src->r;
					*dst++ = src->g;
					*dst++ = src->b;
					*dst++ = src->a;
					src++;
				}
			}
			delete img;
			
			lTexture->mH = h;
			lTexture->mW = w;
			lTexture->mImageData = pixels;
			lTexture->internalformat = 4;
			lTexture->format = GL_RGBA;
			lTexture->type = GL_UNSIGNED_BYTE;
			pixels = NULL;
			++m;
		}
		else
		{
			m = 0;
		}
	}
	delete [] bytes;
	return m;
}

//! Load a (possibly compressed) dds texture.
GLuint texLoadDDS( IDAOStream & f, KString & texformat, VSTexture * lTexture, KError & lError )
{
	char tag[4];
	f.Read(&tag[0], 4);
	DDSFormat ddsHeader;

	if ( strncmp( tag,"DDS ", 4 ) != 0 || f.Read((char *) &ddsHeader, sizeof(DDSFormat), 1) != 1 )
	{
		lError.SetLastError( 1, "not a DDS file" );
		return 0;
	}

	texformat = "DDS";

	if ( !( ddsHeader.dwFlags & DDSD_MIPMAPCOUNT ) )
		ddsHeader.dwMipMapCount = 1;

	if ( ! ( isPowerOfTwo( ddsHeader.dwWidth ) && isPowerOfTwo( ddsHeader.dwHeight ) ) )
	{
		lError.SetLastError( 1, "image dimensions must be power of two" );
		return 0;
	}

	f.Seek(SEEK_SET, ddsHeader.dwSize + 4);

	if ( ddsHeader.ddsPixelFormat.dwFlags & DDPF_FOURCC )
	{
		int blockSize = 8;
		GLenum glFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

		switch( ddsHeader.ddsPixelFormat.dwFourCC )
		{
		case FOURCC_DXT1:
			glFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockSize = 8;
			texformat += " (DXT1)";
			break;
		case FOURCC_DXT3:
			glFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			blockSize = 16;
			texformat += " (DXT3)";
			break;
		case FOURCC_DXT5:
			glFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			blockSize = 16;
			texformat += " (DXT5)";
			break;
		default:
			lError.SetLastError( 1, "unknown texture compression" );
			return 0;
		}

		return texLoadDXT( f, lTexture
			, glFormat, blockSize, ddsHeader.dwWidth, ddsHeader.dwHeight, ddsHeader.dwMipMapCount, false
			, lError );
	}
	else
	{
		texformat += " (RAW)";

		if ( ddsHeader.ddsPixelFormat.dwRMask != 0 && ddsHeader.ddsPixelFormat.dwGMask == 0 && ddsHeader.ddsPixelFormat.dwBMask == 0 )
		{	// fixup greyscale
			ddsHeader.ddsPixelFormat.dwGMask = ddsHeader.ddsPixelFormat.dwRMask;
			ddsHeader.ddsPixelFormat.dwBMask = ddsHeader.ddsPixelFormat.dwRMask;
		}

		return texLoadRaw( f, lTexture
			, ddsHeader.dwWidth, ddsHeader.dwHeight
			, ddsHeader.dwMipMapCount, ddsHeader.ddsPixelFormat.dwBPP, ddsHeader.ddsPixelFormat.dwBPP / 8
			, &ddsHeader.ddsPixelFormat.dwRMask
			, false, false, false
			, lError );
	}
}


//************************************
// Method:    LoadTexture
// FullName:  LoadTexture
// Access:    public 
// Returns:   extern bool
// Qualifier:
// Parameter: const KString & lFileName
// Parameter: VSTexture * lTexture
// Parameter: KError & lError
//************************************
extern bool LoadTexture( const KString & lFileName, VSTexture * lTexture, KError & lError )
{
	_tstring fname = ResourceManager::FindFile( _tstring(lFileName) );


	KString lExt = lFileName.Right(4).Lower();
	if (lExt == ".tga")
	{
		return loadTargaFile(lFileName, lTexture, lError);
	}
	else if (lExt == ".dds")
	{
		DAOStreamPtr stream = ResourceManager::OpenStream( lFileName );
		if (!stream.isNull() )
		{
			KString format;
			return texLoadDDS( *stream, format, lTexture, lError ) > 0;
		}
		return false;
	}
	lError.SetLastError(1, "Unknown file extension" );
	return false;
}


