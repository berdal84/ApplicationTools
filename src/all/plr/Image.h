////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_Image_h
#define plr_Image_h

#include <plr/def.h>
#include <plr/types.h>

namespace plr {

class File;

////////////////////////////////////////////////////////////////////////////////
/// \class Image
/// \todo Remove alloc() call from Create()? Have these functions init metadata
///   only, then alloc() on the first call to set data/load?
/// \todo Clean the *Size functions, add descriptive comments and/or rename them
///   to be clearer (e.g. include "bytes" in the name).
/// \todo Read*() functions and setRawData() should correctly release the 
///   existing image first (or only if the load succeeded).
/// \todo Conversion behaviour between data types is not well define. Need to
///   consider how to manage unnormalized floating point data.
////////////////////////////////////////////////////////////////////////////////
class Image
{
public:
	static const uint kMaxMipmapCount = 32;

	typedef plr::DataType DataType;

	enum Type
	{
		k1d,
		k2d,
		k3d,
		kCubemap,
		k1dArray,
		k2dArray,
		k3dArray,
		kCubemapArray,

		kInvalidType
	};

	enum CompressionType
	{
		kNone,
		kBC1,
		kBC2,
		kBC3,
		kBC4,
		kBC5,
		kBC6,
		kBC7,
	
		kInvalidCompressionType
	};

	enum Layout
	{
		kR,
		kRG,
		kRGB,
		kRGBA,
	
		kInvalidLayout
	};

	enum FileFormat
	{
	 // load + save supported
		kBmp,
		kDds,
		kPng,
		kTga,

	 // load only
		kJpg, 
		kGif,
		kPsd,

		kInvalidFileFormat
	};

	/// Allocate an image with the specified parameters. Image data can subsequently
	/// be filled via calls to setRawImage().
	static Image* Create1d(
		uint            _width,
		Layout          _layout, 
		DataType        _dataType, 
		uint            _mipmapCount = 1,
		CompressionType _compressionType = CompressionType::kNone
		);
	static Image* Create2d(
		uint _width, uint _height, 
		Layout          _layout, 
		DataType        _dataType, 
		uint            _mipmapCount = 1,
		CompressionType _compressionType = CompressionType::kNone
		);
	static Image* Create3d(
		uint            _width, uint _height, uint _depth,
		Layout          _layout, 
		DataType        _dataType, 
		uint            _mipmapCount = 1,
		CompressionType _compressionType = CompressionType::kNone
		);

	/// Release memory, _image_ is set to 0.
	static void Destroy(Image*& _image_);

	/// Read from a file. If _format is not provided, the format is assumed from 
	/// the file's extension.
	/// \return false if an error occurred, in which case img_ remains unchanged.
	static bool Read(Image& img_, const File& _file, FileFormat _format = kInvalidFileFormat);
	static bool Read(Image& img_, const char* _path, FileFormat _format = kInvalidFileFormat);

	/// Write _img to a file. If _format is not provided, the format is assumed 
	/// from the file's extension. 
	/// \note The File* variant only writes to memory. A subsequent call to 
	///   File::Write() is required to actually write to disk.
	/// \return false if an error occurred.
	static bool Write(const Image& _img, File& file_, FileFormat _format = kInvalidFileFormat);
	static bool Write(const Image& _img, const char* _path, FileFormat _format = kInvalidFileFormat);

	/// \return The maximum number of mips for an image.
	static uint GetMaxMipmapSize(uint _width, uint _height, uint _depth = 1u);

	/// Default ctor, initializes metadata but doesn't allocate any memory.
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

	bool            isCubemap() const            { return m_type == Type::kCubemap; }
	bool            isCompressed() const         { return m_compression != CompressionType::kNone; }
	bool            is1d() const                 { return m_height == 1; }
	bool            is2d() const                 { return m_depth == 1 && !isCubemap(); }
	bool            is3d() const                 { return m_depth > 1; }
	bool            isArray() const              { return m_arrayCount > 1 && !isCubemap(); }

	/// \return Ptr to raw image data. _array < getArrayCount(), _mip < getMipmapCount(). 
	/// The size of the returned buffer can be found via getRawImageSize().
	char* getRawImage(uint _array = 0, uint _mip = 0) const;

	/// \return Size (bytes) of the raw image at the specified mipmap level.
	uint getRawImageSize(uint _mip = 0) const;

	/// Fill the image internal data buffer for a given _array/_mip, performing 
	/// conversion to the image's internal format from the format of _src.
	void setRawImage(uint _array, uint _mip, const void* _src, Layout _layout, DataType _dataType, CompressionType _compressionType);

private:
	
	/// Set defaults.
	void init();

	/// Allocate m_data, set mip sizes/offsets. Sets m_errorState.
	void alloc();

	uint m_width, m_height, m_depth;     //< Image dimensions, min = 1.
	uint m_arrayCount;                   //< 1 for non-arrays, 6 for cubemaps.
	uint m_mipmapCount;                  //< Number of valid mipmap levels, min = 1.
	uint m_arrayLayerSize;               //< Data size (bytes) of a single array layer (including mip chain).
	uint m_texelSize;                    //< Bytes in a single texel (uncompressed images only).
	
	Type m_type;                         //< 1d, 2d, cubemap, etc.
	Layout m_layout;                     //< Component layout.
	DataType m_dataType;                 //< Component type.
	CompressionType m_compression;       //< Compression type (or kCompression_None if uncompressed).

	char* m_data;                        //< Raw image data. Each image is adjacent to its mipmap chain.
	uint m_mipOffsets[kMaxMipmapCount];  //< Offset (bytes, from image start) per mipmap level.
	uint m_mipSizes[kMaxMipmapCount];    //< Data size (bytes) per mipmap level.

	/// \return true if the file format supports the image layout, data type 
	///   and compression (non-savable file formats will always return false).
	bool validateFileFormat(FileFormat _format) const;

	/// \return Size (bytes) of the data type.
	static uint GetDataTypeSize(DataType _type);

	/// \return Number of components associated with a layout.
	static uint GetComponentCount(Layout _layout);

	/// \return Layout from a component count.
	static Layout GuessLayout(uint _cmpCount);

	/// \return Fileformat from a path extension.
	static FileFormat GuessFormat(const char* _path);

	/// Additional information about data types.
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

}; // class Image

} // namespace plr

#endif// plr_Image_h
