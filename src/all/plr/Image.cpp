////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/Image.h>

#include <plr/def.h>
#include <plr/log.h>
#include <plr/File.h>
#include <plr/Time.h>

#include <cmath>
#include <cstring>

using namespace plr;

template <typename tSrc, typename tDst>
static tDst Convert(tSrc _src) { return tDst(_src); }
	// from uint8
	template <> static uint8  Convert(uint8 _src) { return _src; }
	template <> static uint16 Convert(uint8 _src) { return (uint16)_src * (UINT16_MAX / UINT8_MAX); }
	template <> static uint32 Convert(uint8 _src) { return (uint32)_src * (UINT32_MAX / UINT8_MAX); }
	template <> static float  Convert(uint8 _src) { return (float )_src / (float)UINT8_MAX; }
	// from uint16
	template <> static uint8  Convert(uint16 _src) { return (uint8)(_src / UINT8_MAX); }
	template <> static uint16 Convert(uint16 _src) { return _src; }
	template <> static uint32 Convert(uint16 _src) { return (uint32)_src * (UINT32_MAX / UINT16_MAX); }
	template <> static float  Convert(uint16 _src) { return (float ) _src / (float)UINT16_MAX; }
	// from uint32
	template <> static uint8  Convert(uint32 _src) { return (uint8) (_src / UINT8_MAX); }
	template <> static uint16 Convert(uint32 _src) { return (uint16)(_src / UINT16_MAX); }
	template <> static uint32 Convert(uint32 _src) { return _src; }
	template <> static float  Convert(uint32 _src) { return (float )_src / (float)UINT32_MAX; }
	// from float
	template <> static uint8  Convert(float _src)   { return (uint8) (_src * (float)UINT8_MAX); }
	template <> static uint16 Convert(float _src)   { return (uint16)(_src * (float)UINT16_MAX); }
	template <> static uint32 Convert(float _src)   { return (uint32)(_src * (float)UINT32_MAX); }
	template <> static float  Convert(float _src)   { return _src; }

/// Copy at most _srcCount objects from _src to _dst, performing conversion from 
/// tSrc to tDst. If _srcCount < _dstCount, the remaining elements of _dst are
/// initialized as tDst(0).
template <typename tSrc, typename tDst>
static void ConvertCopy(const tSrc* _src, tDst* _dst, uint _srcCount, uint _dstCount)
{
	do {
		*_dst = Convert<tSrc, tDst>(*_src);
		++_src;
		++_dst;
		--_dstCount;
		--_srcCount;
	} while (_dstCount != 0 && _srcCount != 0);
	while (_dstCount != 0) {
		*_dst = (tDst)0;
		++_dst;
		--_dstCount;
	}
}

/// Copy image data (_size texels) from _src to _dst. _srcCount/_dstCount are the
/// number of components per texel in _src/_dst respectively.
template <typename tSrc, typename tDst>
static void ConvertCopyImage(const void* _src, void* _dst, uint _srcCount, uint _dstCount, uint _size)
{
	const tSrc* src = (const tSrc*)_src;
	tDst* dst = (tDst*)_dst;
	for (uint i = 0, n = _size / _srcCount; i < n; ++i) {
		ConvertCopy(src, dst, _srcCount, _dstCount);
		src += _srcCount;
		dst += _dstCount;
	}
}

/*******************************************************************************

                                   Image

*******************************************************************************/

// PUBLIC

Image* Image::Create1d(uint _width, Layout _layout, DataType _dataType, uint _mipmapCount, CompressionType _compressionType)
{
	Image* ret = new Image;
	PLR_ASSERT(ret);
	ret->init();
	ret->m_type        = Type::k1d;
	ret->m_width       = _width;
	ret->m_layout      = _layout;
	ret->m_dataType    = _dataType;
	ret->m_mipmapCount = _mipmapCount;
	ret->m_compression = _compressionType;
	ret->alloc();

	return ret;
}

Image* Image::Create2d(uint _width, uint _height, Layout _layout, DataType _dataType, uint _mipmapCount, CompressionType _compressionType)
{
	Image* ret = new Image;
	PLR_ASSERT(ret);
	ret->init();
	ret->m_type        = Type::k2d;
	ret->m_width       = _width;
	ret->m_height      = _height;
	ret->m_layout      = _layout;
	ret->m_dataType    = _dataType;
	ret->m_mipmapCount = _mipmapCount;
	ret->m_compression = _compressionType;
	ret->alloc();

	return ret;
}


Image* Image::Create3d(uint _width, uint _height, uint _depth, Layout _layout, DataType _dataType, uint _mipmapCount, CompressionType _compressionType)
{
	Image* ret = new Image;
	PLR_ASSERT(ret);
	ret->init();
	ret->m_type        = Type::k3d;
	ret->m_width       = _width;
	ret->m_height      = _height;
	ret->m_depth       = _depth;
	ret->m_layout      = _layout;
	ret->m_dataType    = _dataType;
	ret->m_mipmapCount = _mipmapCount;
	ret->m_compression = _compressionType;
	ret->alloc();

	return ret;
}

void Image::Destroy(Image*& _img_)
{
	PLR_ASSERT(_img_);
	if (_img_) {
		delete _img_;
		_img_ = 0;
	}
}

bool Image::Read(Image& img_, const File& _file, FileFormat _format)
{
	if (_format == kInvalidFileFormat) {
		_format = GuessFormat(_file.getPath());
		if (_format == kInvalidFileFormat) {
			PLR_LOG_ERR("Image: Unsupported file format '%s'", _file.getPath());
			return false;
		}
	}

	img_.init(); // \todo don't affect the image before loading succeeded
	switch (_format) {
		case FileFormat::kDds: return ReadDds(img_, _file.getData(), _file.getDataSize());
		case FileFormat::kPng: return ReadPng(img_, _file.getData(), _file.getDataSize());
		case FileFormat::kBmp:
		case FileFormat::kGif:
		case FileFormat::kJpg:
		case FileFormat::kPsd:
		case FileFormat::kTga: return ReadDefault(img_, _file.getData(), _file.getDataSize());

		default: PLR_ASSERT(false); return false; // should never happen as we check the ext above
	};

	return true;
}

bool Image::Read(Image& img_, const char* _path, FileFormat _format)
{
	File f;
	f.setPath(_path);
	if (!File::Read(f, _path)) {
		return false;
	}
	return Read(img_, f, _format);
}


bool Image::Write(const Image& _img, File& file_, FileFormat _format)
{
	bool ret = false;

	if (_format == kInvalidFileFormat) {
		_format = GuessFormat(file_.getPath());
		if (_format == kInvalidFileFormat) {
			PLR_LOG_ERR("Image: Unsupported file format '%s'", file_.getPath());
			goto Image_Write_end;
		}
	}

	if (!_img.validateFileFormat(_format)) {
		PLR_ASSERT(false);
		goto Image_Write_end;
	}

	switch (_format) {
		case FileFormat::kBmp: ret = WriteBmp(file_, _img); break;
		case FileFormat::kDds: ret = WriteDds(file_, _img); break;
		case FileFormat::kPng: ret = WritePng(file_, _img); break;
		case FileFormat::kTga: ret = WriteTga(file_, _img); break;
		default: PLR_LOG_ERR("Image: File format does not supported writing '%s'", file_.getPath()); goto Image_Write_end;
	};

	ret = true;

Image_Write_end:
	PLR_ASSERT(ret);
	return ret;
}

bool Image::Write(const Image& _img, const char* _path, FileFormat _format)
{
	File f;
	f.setPath(_path);
	if (!Write(_img, f, _format)) {
		return false;
	}
	return File::Write(f, _path);
}

uint Image::GetMaxMipmapSize(uint _width, uint _height, uint _depth)
{
	const double rlog2    = 1.0 / log(2.0);
	const uint log2Width  = (uint)(log((double)_width)  * rlog2);
	const uint log2Height = (uint)(log((double)_height) * rlog2);
	const uint log2Depth  = (uint)(log((double)_depth)  * rlog2);
	uint mipCount = PLR_MAX(log2Width, PLR_MAX(log2Height, log2Depth)) + 1; // +1 for level 0
	mipCount = PLR_MIN(PLR_MIN(mipCount, (uint)1u), kMaxMipmapCount);
	return mipCount;
}

char* Image::getRawImage(uint _array, uint _mip) const
{
	PLR_ASSERT(m_data);
	PLR_ASSERT(_array < m_arrayCount);
	PLR_ASSERT(_mip < m_mipmapCount);
	if (m_data == 0 || _array >= m_arrayCount || _mip >= m_mipmapCount) {
		return 0;
	}

	uint offset = _array * m_arrayLayerSize + m_mipOffsets[_mip];
	return m_data + offset;
}

uint Image::getRawImageSize(uint _mip) const
{
	PLR_ASSERT(m_data);
	PLR_ASSERT(_mip < m_mipmapCount);
	if (m_data == 0 || _mip >= m_mipmapCount) {
		return 0;
	}

	return m_mipSizes[_mip];
}


void Image::setRawImage(uint _array, uint _mip, const void* _src, Layout _layout, DataType _dataType, CompressionType _compressionType)
{
	PLR_ASSERT(_src);

	PLR_ASSERT_MSG(_compressionType == m_compression, "Compression types must match");

 // layout/type match, memcpy data
	if (_layout == m_layout && _dataType == m_dataType && _compressionType == m_compression) {
		memcpy(const_cast<char*>(getRawImage(_array, _mip)), _src, getRawImageSize(_mip));
		return;
	}

 // layout/type don't match, must convert
	uint srcCount = GetComponentCount(_layout);
	uint dstCount = GetComponentCount(m_layout);
	void* dst = const_cast<char*>(getRawImage(_array, _mip));

	#define CONVERT_FROM(type) \
		switch (m_dataType) { \
			case DataType::kUint8N:   ConvertCopyImage<DataType::ToType<DataType:: ## type>::Type, DataType::ToType<DataType::kUint8N >::Type >(_src, dst, srcCount, dstCount, m_arrayLayerSize); break; \
			case DataType::kUint16N:  ConvertCopyImage<DataType::ToType<DataType:: ## type>::Type, DataType::ToType<DataType::kUint16N>::Type >(_src, dst, srcCount, dstCount, m_arrayLayerSize); break; \
			case DataType::kUint32N:  ConvertCopyImage<DataType::ToType<DataType:: ## type>::Type, DataType::ToType<DataType::kUint32N>::Type >(_src, dst, srcCount, dstCount, m_arrayLayerSize); break; \
			case DataType::kFloat32:  ConvertCopyImage<DataType::ToType<DataType:: ## type>::Type, DataType::ToType<DataType::kFloat32>::Type >(_src, dst, srcCount, dstCount, m_arrayLayerSize); break; \
			default: PLR_ASSERT(false); \
		}
	switch (_dataType) {
		case DataType::kUint8N:  CONVERT_FROM(kUint8N); break;
		case DataType::kUint16N: CONVERT_FROM(kUint16N); break;
		case DataType::kUint32N: CONVERT_FROM(kUint32N); break;
		case DataType::kFloat32: CONVERT_FROM(kFloat32); break;
		default: PLR_ASSERT_MSG(false, "Unknown data type");
	};
	#undef CONVERT_FROM
}

// PRIVATE

Image::~Image()
{
	free(m_data);
}

void Image::init()
{
	m_width       = m_height = m_depth = 1;
	m_arrayCount  = 1;
	m_mipmapCount = 1;
	m_type        = kInvalidType;
	m_compression = CompressionType::kNone;
	m_layout      = kInvalidLayout;
	m_dataType    = DataType::kInvalidType;
	
	char* m_data = 0;
	memset(m_mipOffsets, 0, sizeof(uint) * kMaxMipmapCount);
	memset(m_mipSizes,   0, sizeof(uint) * kMaxMipmapCount);
	m_arrayLayerSize = 0;
	m_texelSize = 0;
}

void Image::alloc()
{
	free(m_data);
	
	m_texelSize = DataType::GetSizeBytes(m_dataType) * GetComponentCount(m_layout);
	uint w = m_width, h = m_height, d = m_depth;
	m_arrayLayerSize = 0;
	uint i = 0, lim = PLR_MIN(m_mipmapCount, GetMaxMipmapSize(w, h, d));
	do {
		m_mipOffsets[i] = m_arrayLayerSize;
		m_mipSizes[i] = m_texelSize * w * h * d;
		m_arrayLayerSize += m_mipSizes[i];
		w = PLR_MAX(w / 2, (uint)1);
		h = PLR_MAX(h / 2, (uint)1);
		d = PLR_MAX(d / 2, (uint)1);
		++i;
		PLR_ASSERT(i < kMaxMipmapCount);
	} while (i < lim);

	m_data = (char*)malloc(m_arrayLayerSize * m_arrayCount);
	PLR_ASSERT(m_data);
}

bool Image::validateFileFormat(FileFormat _format) const
{
	switch (_format) {
		case FileFormat::kBmp:
			if (m_compression != CompressionType::kNone) return false;
			if (DataType::IsFloat(m_dataType))  return false;
			if (DataType::IsSigned(m_dataType)) return false;
			if (!DataType::IsNormalized(m_dataType)) return false;
			if (IsDataTypeBpc(m_dataType, 16) || IsDataTypeBpc(m_dataType, 32)) return false;
			return true;
		case FileFormat::kDds:
			if (m_type == Type::k3dArray) return false;
			return true;
		case FileFormat::kPng:
			if (m_compression != CompressionType::kNone) return false;
			if (DataType::IsFloat(m_dataType))  return false;
			if (DataType::IsSigned(m_dataType)) return false;
			if (!DataType::IsNormalized(m_dataType)) return false;
			if (IsDataTypeBpc(m_dataType, 32)) return false;
			return true;
		case FileFormat::kTga:
			if (m_compression != CompressionType::kNone) return false;
			if (DataType::IsFloat(m_dataType))  return false;
			if (DataType::IsSigned(m_dataType)) return false;
			if (!DataType::IsNormalized(m_dataType)) return false;
			if (IsDataTypeBpc(m_dataType, 16) || IsDataTypeBpc(m_dataType, 32)) return false;
			return true;
		default:
			break;
	};

	return false;
}

uint Image::GetComponentCount(Layout _layout)
{
	switch (_layout) {
		case Layout::kR:         return 1;
		case Layout::kRG:        return 2;
		case Layout::kRGB:       return 3;
		case Layout::kRGBA:      return 4;
		case kInvalidLayout:
		default:                 return 0;
	};
}

Image::Layout Image::GuessLayout(uint _cmpCount)
{
	switch (_cmpCount) {
		case 1:  return Layout::kR;
		case 2:  return Layout::kRG;
		case 3:  return Layout::kRGB;
		case 4:  return Layout::kRGBA;
		default: return kInvalidLayout;
	};
}

static char lowercase(char c)
{
	if (c >= 'A' && c <= 'Z') {
		c += 'a' - 'A';
	}
	return c;
}
static int strcmp_ignore_case(const char* str1, const char* str2)
{
	PLR_ASSERT(str1 && str2);
	while (*str1 && *str2) {
		int dif = (int)lowercase(*str1) - (int)lowercase(*str2);
		if (dif != 0) {
			return dif;
		}
		++str1;
		++str2;
	}
	return 0;
}

Image::FileFormat Image::GuessFormat(const char* _path)
{
	const char* ext = strrchr(_path, (int)'.');
	if (ext) {
		if        (strcmp_ignore_case(ext, ".bmp") == 0) {
			return FileFormat::kBmp;
		} else if (strcmp_ignore_case(ext, ".dds") == 0) {
			return FileFormat::kDds;
		} else if (strcmp_ignore_case(ext, ".png") == 0) {
			return FileFormat::kPng;
		} else if (strcmp_ignore_case(ext, ".tga") == 0) {
			return FileFormat::kTga;
		} else if (strcmp_ignore_case(ext, ".jpg") == 0 || strcmp_ignore_case(ext, ".jpeg") == 0) {
			return FileFormat::kJpg;
		} else if (strcmp_ignore_case(ext, ".gif") == 0) {
			return FileFormat::kGif;
		} else if (strcmp_ignore_case(ext, ".psd") == 0) {
			return FileFormat::kPsd;
		}
	}

	return kInvalidFileFormat;
}

bool Image::IsDataTypeBpc(DataType _type, int _bpc)
{
	return _bpc == DataType::GetSizeBytes(_type) * 8;
}

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_BMP
#define STBI_ONLY_GIF
#define STBI_ONLY_JPEG
#define STBI_ONLY_PSD
#define STBI_ONLY_TGA
#define STBI_FAILURE_USERMSG
#define STBI_ASSERT(x) PLR_ASSERT(x)
#include <plr/extern/stb_image.h>

#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#define STBIW_ASSERT(x) PLR_ASSERT(x)
#include <plr/extern/stb_image_write.h>
static void StbiWriteFile(void* file_, void* _data, int _size)
{
 // \todo need a better approach here, as many calls to appendData is slow (it just calls realloc() on
 //  the internal buffer). Maybe use a smart reallocation scheme or allocate an initial scratch buffer
 //  with an estimated file size to reduce the number of reallocations.
	if (_size == 0) {
		return;
	}
	PLR_ASSERT(file_);
	File* f = (File*)file_;
	f->appendData((const char*)_data, _size);
}

#include <plr/extern/lodepng.h>
static void SwapByteOrder(char* _d_, unsigned _dsize)
{
    for (unsigned i = 0; i < _dsize; i += 2) {
        unsigned j = i + 1;
        unsigned char tmp = _d_[j];
        _d_[j] = _d_[i];
        _d_[i] = tmp;
    }
}

bool Image::ReadDefault(Image& img_, const char* _data, uint _dataSize)
{
	int x, y, cmp;
	unsigned char* d = stbi_load_from_memory((unsigned char*)_data, (int)_dataSize, &x, &y, &cmp, 0);
	if (!d) {
		PLR_LOG_ERR("stbi_load_from_memory() failed '%s'", stbi_failure_reason());
		return false;
	}
	img_.m_width       = x;
	img_.m_height      = y;
	img_.m_depth       = img_.m_arrayCount = img_.m_mipmapCount = 1;
	img_.m_type        = Type::k2d;
	img_.m_layout      = GuessLayout((uint)cmp);
	img_.m_dataType    = DataType::kUint8N;
	img_.m_compression = CompressionType::kNone;
	img_.alloc();
	memcpy(img_.m_data, d, x * y * cmp); // \todo, avoid this
	stbi_image_free(d);
	return true;
}
bool Image::ReadPng(Image& img_, const char* _data, uint _dataSize)
{
	LodePNGDecoderSettings settings;
    lodepng_decoder_settings_init(&settings);
	settings.color_convert = 0;
    LodePNGState state;
    lodepng_state_init(&state);
    state.decoder = settings;
	DataType dataType;	
	bool ret = true;
	unsigned x, y, cmp;
	unsigned char* d;
	unsigned err = lodepng_inspect(&x, &y, &state, (unsigned char*)_data, _dataSize);
	if (err) {
		goto Image_ReadPng_end;
	}
	err = lodepng_decode(&d, &x, &y, &state, (unsigned char*)_data, _dataSize);
	if (err) {
		goto Image_ReadPng_end;
	}

	switch (state.info_raw.colortype) {
		case LCT_GREY:          cmp = 1; break;
		case LCT_GREY_ALPHA:    cmp = 2; break;
		case LCT_RGB:           cmp = 3; break;
		case LCT_RGBA:          cmp = 4; break;
		case LCT_PALETTE:
		default:                PLR_ASSERT_MSG(false, "Unsupported PNG format");
		                        ret = false;
	};
	switch (state.info_raw.bitdepth) {
		case 8:                 dataType = DataType::kUint8N;  break;
		case 16:                dataType = DataType::kUint16N;
		                        SwapByteOrder((char*)d, x * y * cmp * 2); break; // \todo swizzle bytes during copy to img_
		default:                PLR_ASSERT_MSG(false, "Unsupported bit depth (%d)", state.info_raw.bitdepth);
		                        ret = false;
	};

	img_.m_width       = x;
	img_.m_height      = y;
	img_.m_depth       = img_.m_arrayCount = img_.m_mipmapCount = 1;
	img_.m_type        = Type::k2d;
	img_.m_layout      = GuessLayout((uint)cmp);
	img_.m_dataType    = dataType;
	img_.m_compression = CompressionType::kNone;
	img_.alloc();
	memcpy(img_.m_data, d, x * y * cmp * DataType::GetSizeBytes(dataType)); // \todo, avoid this?

Image_ReadPng_end:
	free(d);
	lodepng_state_cleanup(&state);
	if (err) {
		PLR_LOG_ERR("lodepng error:\n\t'%s'", lodepng_error_text(err));
		return false;
	}
	return ret;
}
bool Image::WritePng(File& file_, const Image& _img)
{
	
	LodePNGColorType colorType;
	unsigned bitdepth;
	bool ret = true;

 
	char* buf = 0;

	switch (_img.m_layout) {
		case Layout::kR:     colorType = LCT_GREY;       break;
		case Layout::kRG:    colorType = LCT_GREY_ALPHA; break;
		case Layout::kRGB:   colorType = LCT_RGB;        break;
		case Layout::kRGBA:  colorType = LCT_RGBA;       break;
		default:             PLR_ASSERT_MSG(false, "Invalid image layout");
		                     ret = false;
	};

	switch (_img.m_dataType) {
		case DataType::kUint8N:  bitdepth = 8;   break;
		case DataType::kUint16N: bitdepth = 16;
		                         // \hack \todo Can lodepng be modified to swizzle the bytes automatically on x86?
	                             buf = (char*)malloc(_img.getRawImageSize());
	                             memcpy(buf, _img.getRawImage(), _img.getRawImageSize());
		                         SwapByteOrder(buf, (unsigned)_img.getRawImageSize()); break;
		default:                 PLR_ASSERT_MSG(false, "Unsupported data type");
		                         ret = false;
	};

	unsigned char* d;
	uint dsize;
	unsigned err = lodepng_encode_memory(&d, &dsize, (unsigned char*)(buf ? buf : _img.getRawImage()), (unsigned)_img.getWidth(), (unsigned)_img.getHeight(), colorType, bitdepth);
	if (err) {
		goto Image_WritePng_end;
	}
	file_.setData((const char*)d, dsize);

Image_WritePng_end:
	free(buf);
	free(d);
	if (err) {
		PLR_LOG_ERR("lodepng error:\n\t'%s'", lodepng_error_text(err));
		return false;
	}
	return ret;
}
bool Image::WriteBmp(File& file_, const Image& _img)
{
	if (!stbi_write_bmp_to_func(StbiWriteFile, &file_, (int)_img.m_width, (int)_img.m_height, (int)GetComponentCount(_img.m_layout), _img.m_data)) {
		PLR_LOG_ERR("stbi_write_bmp_to_func() failed");
		return false;
	}
	return true;
}
bool Image::WriteTga(File& file_, const Image& _img)
{
	if (!stbi_write_tga_to_func(StbiWriteFile, &file_, (int)_img.m_width, (int)_img.m_height, (int)GetComponentCount(_img.m_layout), _img.m_data)) {
		PLR_LOG_ERR("stbi_write_tga_to_func() failed");
		return false;
	}
	return true;
}
