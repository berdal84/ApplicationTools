////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_Image_h
#define plr_Image_h

#include <plr/def.h>

namespace plr {

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
/// \todo Use File API for stbi_write
////////////////////////////////////////////////////////////////////////////////
class Image
{
public:
	static const uint kMaxMipmapCount = 32;

	enum class Type
	{
		k1d,
		k2d,
		k3d,
		kCubemap,
		k1dArray,
		k2dArray,
		k3dArray,
		kCubemapArray,

		kInvalid
	};

	enum class CompressionType
	{
		kNone,
		kBC1,
		kBC2,
		kBC3,
		kBC4,
		kBC5,
		kBC6,
		kBC7,
	
		kInvalid
	};
	
	enum class DataType
	{
		kUint8,
		kUint16,
		kUint32,
		kSint8,
		kSint16,
		kSint32,
		kFloat32,
	
		kInvalid // set for compressed formats
	};

	enum class Layout
	{
		kR,
		kRG,
		kRGB,
		kRGBA,
	
		kInvalid
	};

	enum class FileFormat
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

		kInvalid
	};

	/// Allocate an image with the specified parameters. Image data can subsequently
	/// be filled via calls to setRawImage().
	static Image* Create1d(
		uint            _width,
		Layout          _layout, 
		DataType        _dataType, 
		uint            _mipmapCount = 1u,
		CompressionType _compressionType = CompressionType::kNone
		);
	static Image* Create2d(
		uint _width, uint _height, 
		Layout          _layout, 
		DataType        _dataType, 
		uint            _mipmapCount = 1u,
		CompressionType _compressionType = CompressionType::kNone
		);
	static Image* Create3d(
		uint            _width, uint _height, uint _depth,
		Layout          _layout, 
		DataType        _dataType, 
		uint            _mipmapCount = 1u,
		CompressionType _compressionType = CompressionType::kNone
		);

	/// Release memory, _image_ is set to 0.
	static void Destroy(Image*& _image_);

	/// Read from a file specified by _path. If _format is not provided, the format
	/// is assumed from the extension in _path.
	/// \return false if an error occurred, in which case img_ remains unchanged.
	static bool Read(Image* img_, const char* _path, FileFormat _format = FileFormat::kInvalid);

	/// Write _img to the file specified by _path. The file format is specified by
	/// _format. Mipmaps, array layers, cubemap faces and 3d slices will be saved as 
	/// separate files if the format doesn't support them. In this case the filename
	/// will be appended with "layer_mip", e.g. "img001_03". If _format is not 
	/// provided, the format is assumed from the extension in _path.
	/// \return false if an error occurred.
	static bool Write(Image* _img, const char* _path, FileFormat _format = FileFormat::kInvalid);

	/// \return The maximum number of mips for an image.
	static uint GetMaxMipmapSize(uint _width, uint _height, uint _depth = 1u);

	/// Default ctor, initializes metadata but doesn't allocate any memory.
	Image(): m_data(0)                           { init(); }
	~Image();

	uint getWidth() const                        { return m_width; }
	uint getHeight() const                       { return m_height; }
	uint getDepth() const                        { return m_depth; }
	uint getArrayCount() const                   { return m_arrayCount; }
	uint getArrayLayerSize() const               { return m_arrayLayerSize; }
	uint getMipmapCount() const                  { return m_mipmapCount; }
	uint getTexelSize() const                    { return m_texelSize; }
	Type getType() const                         { return m_type; }
	DataType getImageDataType() const            { return m_dataType; }
	CompressionType getCompressionType() const   { return m_compression; }

	bool isCubemap() const                       { return m_type == Type::kCubemap; }
	bool isCompressed() const                    { return m_compression != CompressionType::kNone; }
	bool is1d() const                            { return m_height == 1; }
	bool is2d() const                            { return m_depth == 1 && !isCubemap(); }
	bool is3d() const                            { return m_depth > 1; }
	bool isArray() const                         { return m_arrayCount > 1 && !isCubemap(); }

	/// \return Ptr to raw image data. _array < getArrayCount(), _mip < getMipmapCount(). 
	/// The size of the returned buffer can be found via getRawImageSize().
	const char* getRawImage(uint _array = 0u, uint _mip = 0u) const;

	/// \return Size (bytes) of the raw image at the specified mipmap level.
	uint getRawImageSize(uint _mip = 0u) const;

	///
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
	static bool IsDataTypeFloat(DataType _type);
	static bool IsDataTypeSigned(DataType _type);
	static bool IsDataTypeBpc(DataType _type, int _bpc);


	// extern/dds.cpp
	static bool ReadDds(Image* img_, const char* _data, uint _dataSize);
	static bool WriteDds(const char* _path, const Image* _img);
	// Image.cpp
	static bool ReadDefault(Image* img_, const char* _data, uint _dataSize);
	static bool WriteBmp(const char* _path, const Image* _img);
	static bool WritePng(const char* _path, const Image* _img);
	static bool WriteTga(const char* _path, const Image* _img);

}; // class Image

} // namespace plr

#endif// plr_Image_h
