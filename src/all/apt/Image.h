#pragma once
#ifndef apt_Image_h
#define apt_Image_h

#include <apt/def.h>
#include <apt/types.h>

namespace apt {

class File;

////////////////////////////////////////////////////////////////////////////////
// Image
// \todo Remove alloc() call from Create()? Have these functions init metadata
//   only, then alloc() on the first call to set data/load?
// \todo Clean the *Size functions, add descriptive comments and/or rename them
//   to be clearer (e.g. include "bytes" in the name).
// \todo Read*() functions and setRawData() should correctly release the 
//   existing image first (or only if the load succeeded).
// \todo 1/2/3d sample functions (need sample/wrap enums).
////////////////////////////////////////////////////////////////////////////////
class Image
{
public:
	static const uint kMaxMipmapCount = 32;

	typedef apt::DataType DataType;

	enum Type
	{
		Type_1d,
		Type_2d,
		Type_3d,
		Type_Cubemap,
		Type_1dArray,
		Type_2dArray,
		Type_3dArray,
		Type_CubemapArray,

		Type_Invalid
	};

	enum CompressionType
	{
		Compression_None,
		Compression_BC1,
		Compression_BC2,
		Compression_BC3,
		Compression_BC4,
		Compression_BC5,
		Compression_BC6,
		Compression_BC7,
	
		Compression_Invalid
	};

	enum Layout
	{
		Layout_R,
		Layout_RG,
		Layout_RGB,
		Layout_RGBA,
	
		Layout_Invalid
	};

	enum FileFormat
	{
	 // read + write supported
		FileFormat_Bmp,
		FileFormat_Dds,
		FileFormat_Hdr,
		FileFormat_Png,
		FileFormat_Tga,

	 // read only
		FileFormat_Jpg, 
		FileFormat_Gif,
		FileFormat_Psd,

		FileFormat_Invalid
	};

	// Allocate an image with the specified parameters. Image data can subsequently be filled via calls to setRawImage().
	static Image* Create1d(
		uint            _width,
		Layout          _layout, 
		DataType        _dataType, 
		uint            _mipmapCount = 1,
		CompressionType _compressionType = Compression_None
		);
	static Image* Create2d(
		uint _width, uint _height, 
		Layout          _layout, 
		DataType        _dataType, 
		uint            _mipmapCount = 1,
		CompressionType _compressionType = Compression_None
		);
	static Image* Create3d(
		uint            _width, uint _height, uint _depth,
		Layout          _layout, 
		DataType        _dataType, 
		uint            _mipmapCount = 1,
		CompressionType _compressionType = Compression_None
		);

	// Release memory, _image_ is set to 0.
	static void Destroy(Image*& _image_);

	// Read from a file. If _format is not provided, the format is assumed from 
	// the file's extension.
	// Return false if an error occurred, in which case img_ remains unchanged.
	static bool Read(Image& img_, const File& _file, FileFormat _format = FileFormat_Invalid);
	static bool Read(Image& img_, const char* _path, FileFormat _format = FileFormat_Invalid);

	// Write _img to a file. If _format is not provided, the format is assumed 
	// from the file's extension. 
	// \note The File* variant only writes to memory. A subsequent call to 
	//   File::Write() is required to actually write to disk.
	// Return false if an error occurred.
	static bool Write(const Image& _img, File& file_, FileFormat _format = FileFormat_Invalid);
	static bool Write(const Image& _img, const char* _path, FileFormat _format = FileFormat_Invalid);

	// Return the maximum number of mips for an image.
	static uint GetMaxMipmapSize(uint _width, uint _height, uint _depth = 1u);

	// Default ctor, initializes metadata but doesn't allocate any memory.
	Image(): m_data(0)                           { init(); }
	~Image();

	uint            getWidth() const             { return m_width; }
	uint            getHeight() const            { return m_height; }
	uint            getDepth() const             { return m_depth; }
	uint            getArrayCount() const        { return m_arrayCount; }
	uint            getArrayLayerSize() const    { return m_arrayLayerSize; }
	uint            getMipmapCount() const       { return m_mipmapCount; }
	uint            getTexelSize() const         { return m_texelSize; }
	Type            getType() const              { return m_type; }
	Layout          getLayout() const            { return m_layout; }
	DataType        getImageDataType() const     { return m_dataType; }
	CompressionType getCompressionType() const   { return m_compression; }

	bool            isCubemap() const            { return m_type == Type_Cubemap; }
	bool            isCompressed() const         { return m_compression != Compression_None; }
	bool            is1d() const                 { return m_height == 1; }
	bool            is2d() const                 { return m_depth == 1 && !isCubemap(); }
	bool            is3d() const                 { return m_depth > 1; }
	bool            isArray() const              { return m_arrayCount > 1 && !isCubemap(); }

	// Return a ptr to raw image data. _array < getArrayCount(), _mip < getMipmapCount(). 
	// Get the size of the returned buffer via getRawImageSize().
	char* getRawImage(uint _array = 0, uint _mip = 0) const;

	// Return size (bytes) of the raw image at the specified mipmap level.
	uint getRawImageSize(uint _mip = 0) const;

	// Fill the image internal data buffer for a given _array/_mip, performing 
	// conversion to the image's internal format from the format of _src.
	void setRawImage(uint _array, uint _mip, const void* _src, Layout _layout, DataType _dataType, CompressionType _compressionType);

private:
	
	// Set defaults.
	void init();

	// Allocate m_data, set mip sizes/offsets. Sets m_errorState.
	void alloc();

	uint m_width, m_height, m_depth;     // Image dimensions, min = 1.
	uint m_arrayCount;                   // 1 for non-arrays, 6 for cubemaps.
	uint m_mipmapCount;                  // Number of valid mipmap levels, min = 1.
	uint m_arrayLayerSize;               // Data size (bytes) of a single array layer (including mip chain).
	uint m_texelSize;                    // Bytes in a single texel (uncompressed images only).
	
	Type m_type;                         // 1d, 2d, cubemap, etc.
	Layout m_layout;                     // Component layout.
	DataType m_dataType;                 // Component type.
	CompressionType m_compression;       // Compression type (or kCompression_None if uncompressed).

	char* m_data;                        // Raw image data. Each image is adjacent to its mipmap chain.
	uint m_mipOffsets[kMaxMipmapCount];  // Offset (bytes, from image start) per mipmap level.
	uint m_mipSizes[kMaxMipmapCount];    // Data size (bytes) per mipmap level.

	// Return true if the file format supports the image layout, data type and compression 
	// (non-savable file formats will always return false).
	bool validateFileFormat(FileFormat _format) const;

	// Number of components associated with a layout.
	static uint GetComponentCount(Layout _layout);

	// Layout from a component count.
	static Layout GuessLayout(uint _cmpCount);

	// Fileformat from a path extension.
	static FileFormat GuessFormat(const char* _path);

	// If _type is compatible with bits per channel _bpc.
	static bool IsDataTypeBpc(DataType _type, int _bpc);

	// extern/dds.cpp
	static bool ReadDds(Image& img_, const char* _data, uint _dataSize);
	static bool WriteDds(File& file_, const Image& _img);
	// Image.cpp
	static bool ReadDefault(Image& img_, const char* _data, uint _dataSize);
	static bool ReadPng(Image& img_, const char* _data, uint _dataSize);
	static bool WritePng(File& file_, const Image& _png);
	static bool WriteBmp(File& file_, const Image& _img);
	static bool WriteTga(File& file_, const Image& _img);
	static bool WriteHdr(File& file_, const Image& _img);

}; // class Image

} // namespace apt

#endif // apt_Image_h
