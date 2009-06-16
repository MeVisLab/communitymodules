
#include <cmath>
#include "image_basic.h"

#define TIFF_LITTLE_ENDIAN	1
#define TIFF_BIG_ENDIAN	0

#define TIFF_SHORT	3
#define TIFF_LONG	4
#define TIFF_RATIONAL 5
using namespace std;

image_basic::image_basic()
{
	color_mode = GRAY_8BPP;
	width = 0;
	height = 0;
	//	data.free();
}
image_basic::image_basic(unsigned int size_x, unsigned int size_y, int set_color_mode)
{
	width = size_x;
	height = size_y;
	data.resize(width * height);
	color_mode = set_color_mode;
}
image_basic::image_basic(const rect_range &range, int set_color_mode)
{
	width = abs((int)range.x2 - (int)range.x1 + 1);
	height = abs((int)range.y2 - (int)range.y1 + 1);
	data.resize(width * height);
	color_mode = set_color_mode;
}
void image_basic::initialize(unsigned int size_x, unsigned int size_y, int set_color_mode)
{
	width = size_x;
	height = size_y;
	data.resize(width * height);
	erase();
	if(set_color_mode != NULL)
		color_mode = set_color_mode;
}
void image_basic::initialize(const rect_range &range, int set_color_mode)
{
	width = abs((int)range.x2 - (int)range.x1 + 1);
	height = abs((int)range.y2 - (int)range.y1 + 1);
	data.resize(width * height);
	erase();
	if(set_color_mode != NULL)
		color_mode = set_color_mode;
}
void image_basic::erase(void)
{
	data = 0;
}
int image_basic::empty(void)
{
	if(width == 0 || height == 0)
		return 1;
	else
		return 0;
}
image_basic::~image_basic()
{
  //data.free();
}
int image_basic::file_read(const std::string &filename)
{
	ifstream file;
	unsigned char buffer[4];

	file.open(filename.c_str() , ios_base::in | ios_base::binary);
	if(file.fail()){
		cerr << "[image_basic::file_read] Error opening " << filename << endl;
		return -1;
	}
	file.read((char*)buffer, 4);
	if(file.fail()){
		cerr << "[image_basic::file_read] Error reading " << filename << endl;
		return -1;
	}
	file.close();

	if((buffer[0] == 0x49 && buffer[1] == 0x49) || (buffer[0] == 0x4d && buffer[1] == 0x4d)){
		if(file_read_tiff(filename))
			return -1;
	} else if(buffer[0] == 'B' && buffer[1] == 'M'){
		if(file_read_bitmap(filename))
			return -1;
	} else if(buffer[0] == 'P' && buffer[1] == '5'){
		if(file_read_pgm(filename))
			return -1;
	} else {
		cerr << "[image_basic::file_read] " << filename << " format is not supported" << endl;
		return -1;
	}

	return 0;
}
int image_basic::file_read_pgm(const std::string &filename)
{
	ifstream file;
	unsigned char buffer[256];
	int flag, position_begin, position_end, value[3], y, offset_row;
   unsigned x;
	std::string line;

	file.open(filename.c_str() , ios_base::in | ios_base::binary);
	if(file.fail()){
		cerr << "[image_basic::file_read_pgm] Error opening " << filename << endl;
		return -1;
	}
	file.getline((char *)buffer, 256);
	if(file.fail()){
		cerr << "[image_basic::file_read_pgm] Error reading " << filename << endl;
		return -1;
	}
	if(buffer[0] != 'P' || buffer[1] != '5'){
		cerr << "[image_basic::file_read_pgm] "<< filename << "is not a raw pgm file"<< endl;
		return -1;
	}
	cout << "[image_basic::file_read_pgm] " << filename << "... ";
	flag = 0;
	position_begin = 2;
	while(flag != 3){
		if(buffer[position_begin] == '\0' || buffer[0] == '#'){
			file.getline((char*)buffer, 256);
			if(file.fail()){
				cerr << "[image_basic::file_read_pgm] Error reading " << filename << endl;
				return -1;
			}
			position_begin = 0;
			continue;
		}
		for( ; (isdigit(buffer[position_begin]) == 0) && position_begin < 256 ; position_begin++)
			if(buffer[position_begin] == '\0')
				continue;
		for(position_end = position_begin; isdigit(buffer[position_end]) && position_end < 256; position_end++)
			;
		value[flag] = atoi((const char*)&buffer[position_begin]);
		flag++;
		position_begin = position_end;
	}
	
	width = value[0];
	height = value[1];
	if(value[2] < 0 || value[2] > 255){
		cerr << "[image_basic::file_read_pgm] Maximum value of " << filename << "should be 0-255" << endl;
		return -1;
	}
	data.resize(width * height);
	color_mode = GRAY_8BPP;
	for(y = height - 1; y >= 0; y--){
		offset_row = y * width;
		for(x = 0; x < width; x++){
			file.read((char *)buffer, 1);
			data[offset_row + x] = (int)buffer[0];
		}
	}
	if(file.fail()){
		cerr << "[image_basic::file_read_pgm] Error reading " << filename << endl;
		return -1;
	}
	cout << width << "*" << height <<  " 8bpp" << endl;

	return 0;
}
inline unsigned short image_basic::byte_short(unsigned char *buffer, bool endian)
{
	if(endian) // == TIFF_LITTLE_ENDIAN
		return ((short)buffer[1] << 8) | (short)buffer[0];
	else
		return ((short)buffer[0] << 8) | (short)buffer[1];
}
inline unsigned long image_basic::byte_long(unsigned char *buffer, bool endian)
{
	if(endian) // == TIFF_LITTLE_ENDIAN
		return ((long)buffer[3] << 24) | ((long)buffer[2] << 16) | ((long)buffer[1] << 8) | (long)buffer[0];
	else
		return ((long)buffer[0] << 24) | ((long)buffer[1] << 16) | ((long)buffer[2] << 8) | (long)buffer[3];
}
int image_basic::file_read_tiff(const std::string &filename)
{
	ifstream file;
	unsigned long ifd_offset, parameter_offset, height_strip;
	unsigned char buffer[12];
	unsigned short ifd_tag, ifd_count, strip_count;
	valarray<unsigned long> strip_offset;
	int i, j, y, offset_row;
   unsigned int y_strip, x;
	bool endian;

	file.open(filename.c_str() , ios_base::in | ios_base::binary);
	if(file.fail()){
		cerr << "[image_basic::file_read_tiff] Error opening " << filename << endl;
		return -1;
	}

	file.seekg(0, ios_base::beg);
	file.read((char*)buffer, 4);
	if(file.fail()){
		cerr << "[image_basic::file_read_tiff] Error reading " << filename << endl;
		return -1;
	}

	if(buffer[0] == 0x49 && buffer[1] == 0x49){
		endian = TIFF_LITTLE_ENDIAN;
		cout << "[image_basic::file_read_tiff] " << filename << "(little endian)..." << endl;
	} else if(buffer[0] == 0x4d && buffer[1] == 0x4d){
		endian = TIFF_BIG_ENDIAN;
		cout << "[image_basic::file_read_tiff] " << filename << "(big endian)..." << endl;
	} else {
		cerr << "[image_basic::file_read_tiff] " << filename << " is not a TIFF file" << endl;
		return -1;
	}
	if(byte_short(&buffer[2], endian) != 0x002a){
		cerr << "[image_basic::file_read_tiff] " << filename << " is not a TIFF file" << endl;
		return -1;
	}
	file.read((char*)buffer, 4);
	ifd_offset = byte_long(buffer, endian);
	file.seekg(ifd_offset, ios_base::beg);
	file.read((char*)buffer, 2);
	ifd_count = byte_short(buffer, endian);
	for(j = 0; j < ifd_count; j++){
		file.seekg(ifd_offset + 2 + j * 12, ios_base::beg);
		file.read((char*)buffer, 12);
		ifd_tag = byte_short(buffer, endian);
		switch(ifd_tag){
			case 0x0100: // TIFF image width
				if(byte_long(&buffer[4], endian) != 1){
					cerr << "[image_basic::file_read_tiff] Number of width paramerter should be 1" << endl;
					return -1;
				}
				switch(byte_short(&buffer[2], endian)){
					case TIFF_SHORT:
						width = byte_short(&buffer[8], endian);
						break;
					case TIFF_LONG:
						width = byte_long(&buffer[8], endian);
						break;
					default:
						cerr << "[image_basic::file_read_tiff] Width should be either long or short integer" << endl;
						return -1;
				}
				// cout << "width " << width << endl;
				break;

			case 0x0101: // TIFF image height
				if(byte_long(&buffer[4], endian) != 1){
					cerr << "[image_basic::file_read_tiff] Number of height paramerter should be 1" << endl;
					return -1;
				}
				switch(byte_short(&buffer[2], endian)){
					case TIFF_SHORT:
						height = byte_short(&buffer[8], endian);
						break;
					case TIFF_LONG:
						height = byte_long(&buffer[8], endian);
						break;
					default:
						cerr << "[image_basic::file_read_tiff] Height should be either long or short integer" << endl;
						return -1;
				}
				// cout << "height " << height << endl;
				break;
			case 0x0102: // TIFF image BPP
				if(byte_short(&buffer[2], endian) != TIFF_SHORT){
					cerr << "[image_basic::file_read_tiff] BPP should be short integer" << endl;
					return -1;
				}
				switch(byte_long(&buffer[4], endian)){
					case 1:
						color_mode = byte_short(&buffer[8], endian);
						if(color_mode == 8){
							color_mode = GRAY_8BPP;
							// cout << "Grayscale 8bpp" << endl;
						} else if(color_mode == 16){
							color_mode = GRAY_16BPP;
							// cout << "Grayscale 16bpp" << endl;
						} else {
							cerr << "[image_basic::file_read_tiff] Color mode not supported" << endl;
							return -1;
						}
						break;
					case 3:
						parameter_offset = byte_long(&buffer[8], endian);
						file.seekg(parameter_offset, ios_base::beg);
						file.read((char*)buffer, 6);
						if((byte_short(buffer, endian) == 8) && (byte_short(&buffer[2], endian) == 8)
							&& (byte_short(&buffer[4], endian) == 8)){
							color_mode = COLOR_24BPP;
							// cout << "True color 24bpp" << endl;
						} else {
							cerr << "[image_basic::file_read_tiff] Color mode not supported" << endl;
							return -1;
						}
						break;
					default:
						cerr << "[image_basic::file_read_tiff] Color mode not supported" << endl;
						return -1;
				}
				break;

			case 0x0103: // TIFF compression
				if(byte_short(&buffer[2], endian) != TIFF_SHORT){
					cerr << "[image_basic::file_read_tiff] Compression flag should be short integer" << endl;
					return -1;
				}
				if(byte_short(&buffer[8], endian) != 1){
					cerr << "[image_basic::file_read_tiff] Compressed image is not supported" << endl;
					return -1;
				}
				break;

			case 0x0106: // TIFF photometric interpretation
				if(byte_short(&buffer[2], endian) != TIFF_SHORT){
					cerr << "[image_basic::file_read_tiff] Photometric interpretation should be short integer" << endl;
					return -1;
				}
				switch(byte_short(&buffer[8], endian)){
					case 0:
						// cout << "Grayscale negative" << endl;
						break;
					case 1:
						// cout << "Grayscale positive" << endl;
						break;
					case 2:
						// cout << "True color 24bpp" << endl;
						color_mode = COLOR_24BPP;
						break;
					case 3:
						cerr << "[image_basic::file_read_tiff] Pallet color not supported" << endl; 
						return -1;
					default:
						cerr << "[image_basic::file_read_tiff] Unknown photometory" << endl; 
						return -1;
				}
				break;

			case 0x0111: // TIFF strip offset(s)
				if(byte_short(&buffer[2], endian) != TIFF_LONG){
					cerr << "[image_basic::file_read_tiff] Strip offset should be long integer" << endl;
					return -1;
				}
				strip_count = byte_short(&buffer[4], endian);
				if(strip_count == 1){
					strip_offset.resize(1);
					strip_offset[0] = byte_long(&buffer[8], endian);
					// cout << "Single strip" << endl;
				} else {
					strip_offset.resize(strip_count);
					parameter_offset = byte_long(&buffer[8], endian);
					file.seekg(parameter_offset, ios_base::beg);
					for(i = 0; i < strip_count; i++){
						file.read((char*)buffer, 4);
						strip_offset[i] = byte_long(buffer, endian);
					}
					// cout << strip_count << " strips" << endl;
				}
				break;

			case 0x0115: // TIFF Sample per pixel
				if(byte_short(&buffer[2], endian) != TIFF_SHORT){
					cerr << "[image_basic::file_read_tiff] Sample per pixel should be short integer" << endl;
					return -1;
				}
				switch(byte_short(&buffer[8], endian)){
					case 1:
						// cout << "Grayscale" << endl;
						break;
					case 3:
						// cout << "True color 24bpp" << endl;
						color_mode = COLOR_24BPP;
						break;
					default:
						cerr << "[image_basic::file_read_tiff] Sample per pixel should be either 1 or 3" << endl; 
						return -1;
				}
				break;

			case 0x0116: // TIFF strip height
				if(byte_long(&buffer[4], endian) != 1){
					cerr << "[image_basic::file_read_tiff] Number of strip height paramerter should be 1" << endl;
					return -1;
				}
				switch(byte_short(&buffer[2], endian)){
					case TIFF_SHORT:
						height_strip = byte_short(&buffer[8], endian);
						break;
					case TIFF_LONG:
						height_strip = byte_long(&buffer[8], endian);
						break;
					default:
						cerr << "[image_basic::file_read_tiff] Strip height should be either short or long integer" << endl; 
						return -1;
				}
				// cout << "strip height " << height_strip << endl;
				break;
			case 0x011c: // TIFF Planar Configulation
				if(byte_short(&buffer[2], endian) != TIFF_SHORT){
					cerr << "[image_basic::file_read_tiff] Planar configuration should be short integer" << endl;
					return -1;
				}
				if(byte_short(&buffer[8], endian) != 1){
					cerr << "[image_basic::file_read_tiff] Planer format is not supported" << endl;
					return -1;
				}
				// cout << "RGB chunky format" << endl;
				break;
			case 0x0140: // TIFF Pallet data
				if(byte_short(&buffer[2], endian) != TIFF_SHORT){
					cerr << "[image_basic::file_read_tiff] Pallet data should be short integer" << endl;
					return -1;
				}
				cout << "[image_basic::file_read_tiff] Pallet data is ignored" << endl;
				break;
//			default:
//				cout << "Tag " << ifd_tag << " is ignored" << endl;
		}
	}			
	file.read((char*)buffer, 4);
	if(byte_long(buffer, endian) != 0)
		cout << "[image_basic::file_read_tiff] Next IFD is ignored" << endl;
	
	if(width == 0 || height ==0){
		cerr << "[image_basic::file_read_tiff] Image geometry should be specified" << endl;
		return -1;
	}
	if(strip_offset.size() == 0){
		cerr << "[image_basic::file_read_tiff] Strip offset(s) should be specified" << endl;
		return -1;
	}

	data.resize(width * height);
	for(i = 0; i < strip_count; i++){
		file.seekg(strip_offset[i], ios_base::beg);
		for(y_strip = 0; y_strip < height_strip; y_strip++){
			y =  height - i * height_strip - y_strip - 1;
			if(y < 0)
				break;
			offset_row = y * width;
			for(x = 0; x < width; x++){
				if(color_mode == GRAY_8BPP){
					file.read((char*)buffer, 1);
					data[offset_row + x] = (int)buffer[0];
				} else if(color_mode == GRAY_16BPP){
					file.read((char*)buffer, 2);
					data[offset_row + x] = (int)byte_short(buffer, endian);
				} else if (color_mode == COLOR_24BPP){
					file.read((char*)buffer, 3);
					data[offset_row + x] = (int)buffer[0] | (((int)buffer[1]) << 8) | (((int)buffer[2]) << 16);
				}
			}
		}
	}
	if(file.fail()){
		cerr << "[image_basic::file_read_tiff] Error reading file." << endl;
		return -1;
	}
	cout << width << "*" << height;
	if(color_mode == GRAY_8BPP)
		cout << " grayscale 8bpp" << endl;
	else if(color_mode == GRAY_16BPP)
		cout << " grayscale 16bpp" << endl;
	else if (color_mode == COLOR_24BPP)
		cout << " true color 24bpp" << endl;

	return 0;

}
int image_basic::file_read_bitmap(const std::string &filename)
{
	ifstream file;
	unsigned long filesize, data_offset, i, x, y, offset_row;
	unsigned long fill_null, count_row;
	unsigned char buffer[4];

	file.open(filename.c_str() , ios_base::in | ios_base::binary);
	if(file.fail()){
		cerr << "[image_basic::file_read_bitmap] Error opening " << filename << endl;
		return -1;
	}
	file.seekg(0, ios_base::beg);
	file.read((char*)buffer, 2);
	if(file.fail()){
		cerr << "[image_basic::file_read_bitmap] Error reading " << filename << endl;
		return -1;
	}

	if(buffer[0] != 'B' || buffer[1] != 'M'){
		cerr << "[image_basic::file_read_bitmap] File is not a bitmap file" << endl;
		return -1;
	} 
	cout << "[image_basic::file_read_bitmap] " << filename << "...";
	// file length : offset 2
	file.read((char *)&filesize, 4);
	// reserved: offser 6
	file.read((char *)&i, 4);
	// data offset: offset 10
	file.read((char *)&data_offset, 4);
	
	// Bitmap info header
	bmp_info_header info;
	file.read((char *)&info.header, BMP_INFO_SIZE);
/*
	cout << "Info size: " << info.header.info_size << " (should be 40)\n";
	cout << "Image geometry: " << info.header.width << '*' << info.header.height << endl;
	cout << "Plane: " << info.header.plane << " (should be 1)\n";
	cout << "Bit per pixel: " << info.header.bpp << endl;
	cout << "Compression: " << info.header.compression << endl;
	cout << "Image size: " << info.header.image_size << endl;
	cout << "Pixel per meter: " << info.header.ppm_x << ',' << info.header.ppm_y << endl;
	cout << "Colors: " << info.header.colors << endl;
	cout << "Important:" << info.header.important_colors << endl;
*/
	if(info.header.compression != 0){
		cerr << "[image_basic::file_read_bitmap] Compressed bitmap is not supported" << endl;
		return -1;
	}
	width = info.header.width;
	height = info.header.height;
	data.resize(width * height);

	if(info.header.bpp == 24){
		fill_null = 3 - (width * 3 - 1) % 4;
		count_row = width * 3 + fill_null;
		for(y = 0; y < height; y ++){
			file.seekg(data_offset, ios_base::beg);
			offset_row = width * y;
			for(x = 0; x < width; x++){
				file.read((char*)buffer, 3);
				data[offset_row + x] = (((int)buffer[0]) << 16) | (((int)buffer[1]) << 8) | (int)buffer[2];
			}
			data_offset += count_row;
		}
		color_mode = COLOR_24BPP;
		cout << width << "*" << height << " 24bpp true color" << endl;
	} else if(info.header.bpp == 8){
		fill_null = 3 - (width - 1) % 4;
		count_row = width + fill_null;
		for(y = 0; y < height; y ++){
			file.seekg(data_offset, ios_base::beg);
			offset_row = width * y;
			for(x = 0; x < width; x++){
				file.read((char *)buffer, 1);
				data[offset_row + x] = buffer[0];
			}
			data_offset += count_row;
		}
		color_mode = GRAY_8BPP;
		cout << width << "*" << height << " 8bpp gray scale" << endl;
	}
	if(file.fail()){
		cerr << "[image_basic::file_read_bmp] Error reading file." << endl;
		return -1;
	}
	return 0;
}
void image_basic::ifd_preparation(unsigned char *byte, unsigned short tag, unsigned short type, unsigned long count, unsigned long value)
{
	byte[0] = (unsigned char)(tag & 0xff);
	byte[1] = (unsigned char)((tag >> 8) & 0xff);

	byte[2] = (unsigned char)(type & 0xff);
	byte[3] = (unsigned char)((type >> 8) & 0xff);

	byte[4] = (unsigned char)(count & 0xff);
	byte[5] = (unsigned char)((count >> 8) & 0xff);
	byte[6] = (unsigned char)((count >> 16) & 0xff);
	byte[7] = (unsigned char)((count >> 24) & 0xff);

	byte[8] = (unsigned char)(value & 0xff);
	byte[9] = (unsigned char)((value >> 8) & 0xff);
	byte[10] = (unsigned char)((value >> 16) & 0xff);
	byte[11] = (unsigned char)((value >> 24) & 0xff);
}

int image_basic::file_write_tiff(std::string &filename)
{
	ofstream file;
	unsigned char buffer[12];
	unsigned long offset_data, offset_row;
	int y;
   unsigned x;

	if(color_mode == GRAY_8BPP || color_mode == GRAY_16BPP)
		// header and IFD of 12 tags
		offset_data = 8 + 2 + 12 * 12 + 4 + 8 * 2;
	else if(color_mode == COLOR_24BPP)
		// header, IFD of 12 tags, and RGB depth data
		offset_data = 8 + 2 + 12 * 12 + 4 + 8 * 2 + 8;
	else {
		cerr << "[image_basic::file_write_tiff] Color mode " << color_mode << " is not supported" << endl;
		return -1;
	}

	file.open(filename.c_str() , ios::out | ios::binary | ios::trunc);
	if(file.fail()){
		cerr << "[image_basic::file_write_tiff] Error opening " << filename << endl;
		return -1;
	}
	// Little endian TIFF file header
	buffer[0] = 0x49;
	buffer[1] = 0x49;
	buffer[2] = 0x2a;
	buffer[3] = 0x00;
	// IFD starts at offset 8
	buffer[4] = 8;
	buffer[5] = 0;
	buffer[6] = 0;
	buffer[7] = 0;
	// Number of IFD is 12
	buffer[8] = 12;
	buffer[9] = 0;
	file.write((char*)buffer, 10);

	// IFD Width
	ifd_preparation(buffer, 0x0100, TIFF_LONG, 1, width);
	file.write((char*)buffer, 12);
	// IFD Length
	ifd_preparation(buffer, 0x0101, TIFF_LONG, 1, height);
	file.write((char*)buffer, 12);
	// IFD BPP
	if(color_mode == GRAY_8BPP){
		ifd_preparation(buffer, 0x0102, TIFF_SHORT, 1, 8);
	} else if (color_mode == GRAY_16BPP){
		ifd_preparation(buffer, 0x0102, TIFF_SHORT, 1, 16);
	} else if (color_mode == COLOR_24BPP){
		ifd_preparation(buffer, 0x0102, TIFF_SHORT, 3, 8 + 2 + 12 * 12 + 4 + 8 * 2);
	}
	file.write((char*)buffer, 12);
	// IFD Compression
	ifd_preparation(buffer, 0x0103, TIFF_SHORT, 1, 1);
	file.write((char*)buffer, 12);
	// IFD Photometric Interpretation
	if(color_mode == GRAY_8BPP || color_mode == GRAY_16BPP)
		ifd_preparation(buffer, 0x0106, TIFF_SHORT, 1, 1);
	else if (color_mode == COLOR_24BPP)
		ifd_preparation(buffer, 0x0106, TIFF_SHORT, 1, 2);
	file.write((char*)buffer, 12);
	// IFD Strip Offsets
	ifd_preparation(buffer, 0x0111, TIFF_LONG, 1, offset_data);
	file.write((char*)buffer, 12);
	// IFD Samples per pixel
	if(color_mode == GRAY_8BPP || color_mode == GRAY_16BPP)
		ifd_preparation(buffer, 0x0115, TIFF_SHORT, 1, 1);
	else if (color_mode == COLOR_24BPP)
		ifd_preparation(buffer, 0x0115, TIFF_SHORT, 1, 3);
	file.write((char*)buffer, 12);
	// IFD Rows per strip
	ifd_preparation(buffer, 0x0116, TIFF_LONG, 1, height);
	file.write((char*)buffer, 12);
	// IFD Strip byte counts
	if(color_mode == GRAY_8BPP){
		ifd_preparation(buffer, 0x0117, TIFF_LONG, 1, width * height);
	} else if (color_mode == GRAY_16BPP){
		ifd_preparation(buffer, 0x0117, TIFF_LONG, 1, width * height * 2);
	} else if (color_mode == COLOR_24BPP){
		ifd_preparation(buffer, 0x0117, TIFF_LONG, 1, width * height * 3);
	}
	file.write((char*)buffer, 12);
	// IFD X Resolution
	ifd_preparation(buffer, 0x11a, TIFF_RATIONAL, 1, 8 + 2 + 12 * 12 + 4);
	file.write((char*)buffer, 12);
	// IFD Y Resolution
	ifd_preparation(buffer, 0x11b, TIFF_RATIONAL, 1, 8 + 2 + 12 * 12 + 4 + 8);
	file.write((char*)buffer, 12);
	// Resolution Unit
	ifd_preparation(buffer, 0x128, TIFF_SHORT, 1, 2);
	file.write((char*)buffer, 12);
	// End of IFD section
	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	file.write((char*)buffer, 4);

	// X and Y Resolution value 96dpi
	buffer[0] = 96;
 	buffer[4] = 1;
	buffer[5] = 0;
	buffer[6] = 0;
	buffer[7] = 0;
	file.write((char*)buffer, 8);
	file.write((char*)buffer, 8);

	if(color_mode == COLOR_24BPP){
		buffer[0] = 8;
		buffer[2] = 8;
		buffer[4] = 8;
		file.write((char *)buffer, 8);
	}
	for(y = height - 1; y >= 0; y--){
		offset_row = width * y;
		for(x = 0; x < width; x++){
			if(color_mode == GRAY_8BPP){
				buffer[0]= (unsigned char)data[offset_row + x];
				file.write((char *)buffer, 1);
			} else if(color_mode == GRAY_16BPP){
				buffer[0]= (unsigned char)(data[offset_row + x] & 0xff);
				buffer[1]= (unsigned char)((data[offset_row + x] & 0xff00) >> 8);
				file.write((char *)buffer, 2);
			} else if(color_mode == COLOR_24BPP){
				buffer[0] = (data[offset_row + x] & 0xff); // Red
				buffer[1] = (data[offset_row + x] & 0xff00) >> 8; // Green
				buffer[2] = (data[offset_row + x] & 0xff0000) >> 16; // Brue
				file.write((char *)buffer, 3);
			}
		}
	}
	if(file.fail()){
		cerr << "[image_basic::file_write_tiff] Error writing " << filename << endl;
		return -1;
	}
	cout << "[image_basic::file_write_tiff] " << filename << endl;
	return 0;

}
int image_basic::file_write_bitmap(std::string &filename)
{
	ofstream file;

	unsigned char header[] = "BM", buffer[4];
	unsigned long filesize, data_offset;
	unsigned int i, x, y, offset, offset_row, fill_dword, count_row;

	bmp_info_header info;
	info.header.width = width;
	info.header.height = height;

	if(color_mode == GRAY_8BPP){
		info.header.bpp = 8;
		data_offset = 14 + BMP_INFO_SIZE + 256*4;
		fill_dword = 3 - (width - 1) % 4;
		count_row = width + fill_dword;
	} else if(color_mode == COLOR_24BPP){
		info.header.bpp = 24;
		data_offset = 14 + BMP_INFO_SIZE;
		fill_dword = 3 - (width * 3 - 1) % 4;
		count_row = width * 3 + fill_dword;
	} else {
		cerr << "[image_basic::file_write_bitmap] Color mode " << color_mode << " is not supported" << endl;
		return -1;
	}
	info.header.image_size = count_row * height;
	filesize = data_offset + info.header.image_size; 

	file.open(filename.c_str() , ios::out | ios::binary | ios::trunc);
	if(file.fail()){
		cerr << "[image_basic::file_write_bitmap] Error opening " << filename << endl;
		return -1;
	}
	file.write((char*)header, 2);
	file.write((char *)&filesize, 4);
	i = 0;
	file.write((char *)&i, 4);
	file.write((char *)&data_offset, 4);
	file.write((char *)&info.header, BMP_INFO_SIZE);
	if(color_mode == GRAY_8BPP){
		unsigned char pallet[256 * 4];
		for(i = 0;i < 256;i++){
			offset = i << 2;
			pallet[offset] = (unsigned char)i; // Blue
			pallet[offset + 1] = (unsigned char)i; // Green
			pallet[offset + 2] = (unsigned char)i; // Red
			pallet[offset + 3] = 0;
		}
		file.write((char *)pallet, 256 * 4);
	}
	i = 0;
	for(y = 0; y < height; y++){
		offset_row = width * y;
		for(x = 0; x < width; x++){
			if(color_mode == GRAY_8BPP){
				buffer[0]= (unsigned char)data[offset_row + x];
				file.write((char *)buffer, 1);
			} else if(color_mode == COLOR_24BPP){
				buffer[0] = (data[offset_row + x] & 0xff0000) >> 16; // Blue
				buffer[1] = (data[offset_row + x] & 0xff00) >> 8; // Green
				buffer[2] = (data[offset_row + x] & 0xff); // Red
				file.write((char *)buffer, 3);
			}
		}
		if(fill_dword)
			file.write((char *)&i, fill_dword);
	}
	if(file.fail()){
		cerr << "[image_basic::file_write_bitmap] Error writing " << filename << endl;
		return -1;
	}
	cout << "[image_basic::file_write_bitmap] " << filename << endl;
	return 0;
}
int image_basic::convert(int new_color_mode)
{
	int count, i;

	if(color_mode == COLOR_24BPP && new_color_mode == GRAY_8BPP){
		count = width * height;
		for(i = 0; i < count; i++){
			data[i] = ((data[i] & 0xff) * 299 + ((data[i] & 0xff00) >> 8) * 587 + ((data[i] & 0xff0000) >> 16) * 114) / 1000;
		}
		color_mode = GRAY_8BPP;
	}
	return 0;
}
int image_basic::get(unsigned int x, unsigned int y) const
{
	if(x >= 0 && x < width && y >= 0 && y < height)
		return fast_get(x, y);
	else
		return 0;
}
int image_basic::get(point_2d &point) const
{
	if(point.x >= 0 && point.x < width && point.y >= 0 && point.y < height)
		return fast_get(point);
	else
		return 0;
}

void image_basic::set(unsigned int x, unsigned int y, int value)
{
	if(x >= 0 && x < width && y >= 0 && y < height)
		fast_set(x, y, value);
}
void image_basic::set(point_2d &point, int value)
{
	if(point.x >= 0 && point.x < width && point.y >= 0 && point.y < height)
		fast_set(point, value);
}

int image_basic::clip_rect(rect_range &range) const
{
	int i;
	if(range.x1 < 0) range.x1 = 0;
	if(range.x1 >= width) range.x1 = width - 1;
	if(range.x2 < 0) range.x2 = 0;
	if(range.x2 >= width) range.x2 = width - 1;
	if(range.x1 > range.x2){
		i = range.x1;
		range.x1 = range.x2;
		range.x2 = i;
	}
	if(range.y1 < 0) range.y1 = 0;
	if(range.y1 >= height) range.y1 = height - 1;
	if(range.y2 < 0) range.y2 = 0;
	if(range.y2 >= height) range.y2 = height - 1;
	if(range.y1 > range.y2){
		i = range.y1;
		range.y1 = range.y2;
		range.y2 = i;
	}
	if(range.x1 == range.x2 && range.y1 == range.y2)
		return -1;
	return 0;
}
int image_basic::max_get(int initial) const
{
	int v;
	v = initial;
	for(int i = data.size() - 1; i >= 0; i--){
		if(data[i] > v)
			v = data[i];
	}
	return v;
}
int image_basic::min_get(int initial) const
{
	int v;
	v = initial;
	for(int i = data.size() - 1; i >= 0; i--){
		if(data[i] < v)
			v = data[i];
	}
	return v;
}
double image_basic::average(const rect_range &range) const
{
	unsigned int x, y, offset_row;
	double average = 0.0;
	rect_range calc = range;

	if(clip_rect(calc) == -1)
		return 0.0;

	if(color_mode == GRAY_8BPP || color_mode == GRAY_16BPP){
		double total = 0.0;
		for(y = calc.y1; y <= calc.y2 ;y++){
			offset_row = y * width;
			for(x = calc.x1; x <= calc.x2; x++){
				total += (double)data[x + offset_row];
			}

		}
		average = total / (double)calc.area_get() ;
	}
	return average;
}
void image_basic::binarize(const image_basic &image_source, const rect_range &range_source, int threshold, int value_positive, int value_negative)
{
	unsigned int x, y, x_range, y_range;

	x_range = range_source.x2 - range_source.x1;
	if(x_range >= width)
		x_range = width - 1;
	y_range = range_source.y2 - range_source.y1;
	if(y_range >= height)
		y_range = height - 1;
	for(y = 0; y <= y_range ;y++){
		for(x = 0; x <= x_range; x++){
			if(image_source.get(x + range_source.x1, y + range_source.y1) >= threshold)
				fast_set(x, y, value_positive);
			else
				fast_set(x, y, value_negative);
		}
	}

}
void image_basic::binarize(int threshold, int value_positive, int value_negative)
{
	rect_range range;
	image_basic::rect_get(range);
	image_basic::binarize(*this, range, threshold, value_positive, value_negative);
}
int image_basic::count_pixel(int value) const
{
	int count = 0;
	for(int i = data.size() - 1;i >= 0; i--){
		if(data[i] == value)
			count++;
	}
	return count;
}
int image_basic::point_center(point_2d &point, int value) const
{
	unsigned int count = 0, x, y, offset_row;
	double x_center = 0.0, y_center = 0.0;

	for(y = 0; y < height; y++){
		offset_row = y * width;
		for(x = 0; x < width; x++){
			if(data[offset_row + x] == value){
				x_center += (double)x;
				y_center += (double)y;
				count++;
			}
		}
	}
	if(count != 0){
		point.x = (int)(x_center / (double)count);
		point.y = (int)(y_center / (double)count);
	}
	return count;
}

void image_basic::line_draw(const point_2d &p1, const point_2d &p2, int value)
{
	unsigned int x, y;
	double a, c;

	if(p1.x != p2.x){
		a = ((double)(p2.y - p1.y)) / ((double)(p2.x - p1.x));
		c = (double)p1.y - a * (double)p1.x;
		for(x = p1.x; x <= p2.x; x++){
			y = (int)(a * (double)x + c);
			set(x, y, value);
		}
		for(x = p1.x; x >= p2.x; x--){
			y = (int)(a * (double)x + c);
			set(x, y, value);
		}

	}
	if(p1.y != p2.y){
		a = ((double)(p2.x - p1.x)) / ((double)(p2.y - p1.y));
		c = (double)p1.x - a * (double)p1.y;
		for(y = p1.y; y <= p2.y; y++){
			x = (int)(a * (double)y + c);
			set(x, y, value);
		}
		for(y = p1.y; y >= p2.y; y--){
			x = (int)(a * (double)y + c);
			set(x, y, value);
		}

	}
}
void image_basic::line_draw(const std::vector<point_2d> &points, int value)
{
	for(unsigned int i = 1; i < points.size(); i++){
		image_basic::line_draw(points[i -1], points[i], value);
	}
}

void image_basic::rect_draw(const rect_range &range, int value)
{
	point_2d p1, p2, p3, p4;

	p1.set(range.x1, range.y1);
	p2.set(range.x2, range.y1);
	p3.set(range.x2, range.y2);
	p4.set(range.x1, range.y2);

	image_basic::line_draw(p1, p2, value);
	image_basic::line_draw(p2, p3, value);
	image_basic::line_draw(p3, p4, value);
	image_basic::line_draw(p4, p1, value);
}
void image_basic::cross_draw(const point_2d &p, int size, int value)
{
	point_2d p1, p2;

	p1.set(p.x - size, p.y);
	p2.set(p.x + size, p.y);
	image_basic::line_draw(p1, p2, value);
	p1.set(p.x, p.y - size);
	p2.set(p.x, p.y + size);
	image_basic::line_draw(p1, p2, value);
}
int image_basic::transpose(int former, int latter)
{
	int count = 0;
	for(int i = data.size() - 1;i >= 0; i--){
		if(data[i] == former){
			data[i] = latter;
			count++;
		}
	}
	return count;

}

int image_basic::labeling(const image_basic &image_source, const rect_range &range_source, int threshold)
{
	unsigned int count = 0, x_range, y_range, i, count_outer;
	point_2d point, offset;
	rect_range range = range_source;

	x_range = range_source.x2 - range_source.x1;
	if(x_range >= width)
		x_range = width - 1;
	y_range = range_source.y2 - range_source.y1;
	if(y_range >= height)
		y_range = height - 1;

	for(point.y = range.y1; point.y <= range.y2; point.y++){
		for(point.x = range.x1; point.x <= range.x2; point.x++){
			if(image_source.get(point) >= threshold){
				if(get(point.x - offset.x, point.y - offset.y) == 0){
					if(image_source.get(point.x - 1, point.y) < threshold){
						count++;
						// cout << "[image_basic::labeling] outer bondary " << count;
						// cout << " [" << (point.x - offset.x) << ", " << (point.y - offset.y) << "]";
						border_trace8(image_source, range_source, point, threshold, count, 3);

					} else if(image_source.get(point.x + 1, point.y) < threshold) {
						count_outer = 0;
						for(i = point.x; i >= range_source.x1; i--){
							if(image_source.get(i, point.y) < threshold)
								break;
							if(get(i - offset.x, point.y - offset.y) != 0){
								count_outer = get(i - offset.x, point.y - offset.y);
								break;
							}
						}
						if(count_outer == 0){
							for(i = point.x; i <= range_source.x2; i++){
								if(image_source.get(i, point.y) < threshold)
									break;
								if(get(i - offset.x, point.y - offset.y) != 0){
									count_outer = get(i - offset.x, point.y - offset.y);
									break;
								}
							}
						}
						if(count_outer == 0){
							cout << "[image_basic::labeling] no outer boundary" << endl;
						} else {
							//cout << "[image_basic::labeling] inner border [" << (point.x - offset.x) << ", " << (point.y - offset.y) << "] " << count_outer;
							border_trace8(image_source, range_source, point, threshold, count_outer, 7);
						}
					}
				}
			}
		}
	}

	for(point.y = range.y1 + 1; point.y < range.y2; point.y++){
		for(point.x = range.x1 + 1; point.x < range.x2; point.x++){
			if(image_source.get(point) >= threshold && get(point.x - 1, point.y) != 0){
				set(point, get(point.x - 1, point.y));
			}
		}
	}
	return count;
}
int image_basic::border(const image_basic &image_source, const rect_range &range_source, int threshold, int value)
{
	int count = 0;
	point_2d point, offset;
	rect_range range = range_source;
	
	offset.x = range_source.x1;
	offset.y = range_source.y1;
	range -= offset;

	for(point.y = range.y1; point.y <= range.y2; point.y++){
		for(point.x = range.x1; point.x <= range.x2; point.x++){
			if(image_source.get(point) >= threshold){
				if(get(point.x - offset.x, point.y - offset.y) == 0){
					if(image_source.get(point.x - 1, point.y) < threshold){
						border_trace8(image_source, range_source, point, threshold, 255, 3);
						count++;
					} else if(image_source.get(point.x + 1, point.y) < threshold) {
						border_trace8(image_source, range_source, point, threshold, 128, 7);
						count++;
					}
				}
			}
		}
	}
	return count;
}
int image_basic::border_trace4(const image_basic &image_source, const rect_range &range_source, const point_2d &point_start, int threshold, int value, int code)
{
	int count = 0, code_start = code;	
	point_2d point, point_current = point_start;
  
	do{
		point = point_current;
		switch(code){
		case 0:
			point.y--;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 6;
			} else {
				code = 2;
			}
			break;
		case 2:
			point.x++;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 0;
			} else {
				code = 4;
			}
			break;
		case 4:
			point.y++;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 2;
			} else {
				code = 6;
			}
			break;
		case 6:
			point.x--;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 4;
			} else {
				code = 0;
			}
			break;
		}
		if(point_current == point_start && code == code_start){
			set(point.x - range_source.x1, point.y - range_source.y1, value);
			count++;
			break;
		}
		if(image_source.get(point) >= threshold && range_source.inside(point)){
			point_current = point;
			if(get(point.x - range_source.x1, point.y - range_source.y1) == 0){
				count++;
				set(point.x - range_source.x1, point.y - range_source.y1, value);
			}
			code_start = code;
		}
	} while(point != point_start);
	return count;
}
int image_basic::border_trace8(const image_basic &image_source, const rect_range &range_source, const point_2d &point_start, int threshold, int value, int code)
{
	int count = 0, code_start = code;	
	point_2d point, point_current = point_start;

	do{
		point = point_current;
		switch(code){
		case 0:
			point.y++;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 6;
			} else {
				code = 1;
			}
			break;
		case 1:
			point.x--;
			point.y++;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 7;
			} else {
				code = 2;
			}
			break;

		case 2:
			point.x--;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 0;
			} else {
				code = 3;
			}
			break;
		case 3:
			point.x--;
			point.y--;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 1;
			} else {
				code = 4;
			}
			break;
		case 4:
			point.y--;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 2;
			} else {
				code = 5;
			}
			break;
		case 5:
			point.x++;
			point.y--;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 3;
			} else {
				code = 6;
			}
			break;
		case 6:
			point.x++;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 4;
			} else {
				code = 7;
			}
			break;
		case 7:
			point.x++;
			point.y++;
			if(image_source.get(point) >= threshold && range_source.inside(point)){
				code = 5;
			} else {
				code = 0;
			}
			break;
		}
		if(point_current == point_start && code == code_start){
			set(point_start.x - range_source.x1, point_start.y - range_source.y1, value);
			count++;
			break;
		}
		if(image_source.get(point) >= threshold && range_source.inside(point)){
			point_current = point;
			if(get(point.x - range_source.x1, point.y - range_source.y1) == 0){
				count++;
				set(point.x - range_source.x1, point.y - range_source.y1, value);
			}
			code_start = code;
		}
	} while(point != point_start);
	return count ;
}
void image_basic::deviation(const rect_range &range, double &deviation, double &average) const
{
	unsigned int x, y, rect_area, offset_row;
	rect_range calc = range;

	if(clip_rect(calc) == -1)
		return;
	average = image_basic::average(calc);

	if(color_mode == GRAY_8BPP || color_mode == GRAY_16BPP){
		double accum = 0.0, difference;
		for(y = calc.y1; y <= calc.y2 ;y++){
			offset_row = y * width;
			for(x = calc.x1; x <= calc.x2; x++){
				difference = (double)data[x + offset_row] - average;
				accum += difference * difference;
			}
		}
		rect_area = (calc.x2 - calc.x1 + 1) * (calc.y2 - calc.y1 + 1);
		deviation = sqrt(accum / (double)rect_area);
	}
}
void image_basic::rect_get(rect_range &range) const
{
	range.x1 = 0;
	range.x2 = width - 1;
	range.y1 = 0;
	range.y2 = height -1;
}
double image_basic::correlation(const image_basic &image_destination, const point_2d &shift_destination) const
{
	rect_range region_source, region_destination;

	rect_get(region_source);
	image_destination.rect_get(region_destination);
	region_destination += shift_destination;
	return correlation(image_destination, region_source, region_destination);
}
double image_basic::correlation(const image_basic &image_destination, rect_range &region_source, rect_range &region_destination) const
{
	int x, y, offset, offset_destination, length_x, length_y;
	double cross, ave, ave_destination, dev, dev_destination, value, value_destination;
	point_2d shift_destination;

	shift_destination.x = region_destination.x1 - region_source.x1;
	shift_destination.y = region_destination.y1 - region_source.y1;

	clip_rect(region_source);
	image_destination.clip_rect(region_destination);

	region_destination -= shift_destination;
	if(region_source.intersect(region_destination) == -1)
		return 0.0;
	region_destination += shift_destination;

	ave = average(region_source);
	ave_destination = image_destination.average(region_destination);
	length_x = region_source.x2 - region_source.x1 + 1;
	length_y = region_source.y2 - region_source.y1 + 1;
	if(length_x * length_y == 1)
		return 0.0;
	cross = 0.0;
	dev = 0.0;
	dev_destination = 0.0;
	for(y = 0; y < length_y ; y++){
		offset = (y + region_source.y1) * width;
		offset_destination = (y + region_destination.y1) * image_destination.width;
		for(x = 0; x < length_x; x++){
			value = ((double)data[x + region_source.x1 + offset] - ave);
			value_destination = ((double)image_destination.data[x + region_destination.x1 + offset_destination] - ave_destination);
			cross += value * value_destination;
			dev += value * value;
			dev_destination += value_destination * value_destination;
		}
	}
	return cross / sqrt(dev * dev_destination);
}
double image_basic::correlation_search(const image_basic &image_destination, rect_range &region_source_initial, rect_range &region_destination_initial, int shift_max) const
{
	rect_range region_source = region_source_initial;
	rect_range region_destination = region_destination_initial;
	double correlation_max = 0.0, correlation_value;
	int x, y;
	point_2d shift, position_best;

	for(y = 0 - shift_max; y <= shift_max; y++){
		for(x = 0 - shift_max; x <= shift_max; x++){
			region_destination = region_destination_initial;
			region_source = region_source_initial;
			shift.set(x, y);
			region_destination += shift;
			correlation_value = correlation(image_destination, region_source, region_destination);
			if(correlation_value > correlation_max){
				correlation_max = correlation_value;
				position_best.set(x, y);
			}
		}
	}
	if(correlation_max == 0.0)
		return 0.0;
	cout << "[image_basic::correlation_serach] " << position_best.x << "," << position_best.y << endl;
	region_destination_initial += position_best;
	return correlation_max;
	//return correlation(image_destination, region_source_initial, region_destination_initial);
}
void image_basic::copy(const image_basic &source, const rect_range &range_source, const point_2d &point_destination)
{

	int x, y, span_x, span_y;
	
	span_x = range_source.x2 - range_source.x1 + 1;
	span_y = range_source.y2 - range_source.y1 + 1;

	for(y = 0; y < span_y; y++){
		for(x = 0; x < span_x; x++){
			set(x + point_destination.x, y + point_destination.y, source.get(x + range_source.x1, y + range_source.y1));
		}
	}
}
void image_basic::copy(const image_basic &source, const matrix_2d &m)
{
	unsigned int x, y;
	point_2d p;
	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			p.x = x;
			p.y = y;
			m.reverse(p);
			fast_set(x, y, source.get(p.x, p.y));
		}
	}
}

void image_basic::scale(unsigned int minimum, unsigned int maximum)
{
	unsigned int x, y, row_offset, value, top_value, range;

	if(color_mode == GRAY_8BPP)
		top_value = 0xff;
	if(color_mode == GRAY_16BPP)
		top_value = 0xffff;
	if(maximum > minimum){
		range = maximum - minimum;
	} else {
		range = 1;
		top_value = 0;
	}
	for(y = 0; y < height; y++){
		row_offset = y * width;
		for(x = 0; x < width; x++){
			value = data[row_offset + x];
			if(value < minimum)
				value = 0;
			else if(value > maximum)
				value = top_value;
			else
				value = (value - minimum) * top_value / range;
			data[row_offset + x] = value;
		}
	}
	
}
void image_basic::line_get(const point_2d &p1, const point_2d &p2, std::valarray<int> &line) const
{
	unsigned int i;

	if(p1.x < p2.x && p1.y == p2.y){
		line.resize(p2.x - p1.x + 1);
		for(i = p1.x; i <= p2.x; i++)
			line[i - p1.x] = get(i, p1.y);
	} else if(p1.x == p2.x && p1.y < p2.y){
		line.resize(p2.y - p1.y + 1);
		for(i = p1.y; i <= p2.y; i++)
			line[i - p1.y] = get(p1.x, i);
	}
}
void image_basic::line_draw(const point_2d &p1, const point_2d &p2, const std::valarray<int> &line)
{
	unsigned int i;

	if(p1.x < p2.x && p1.y == p2.y){
		for(i = 0; i < line.size(); i++)
			set(p1.x + i, p1.y, line[i]);
	} else if(p1.x == p2.x && p1.y < p2.y){
		for(i = 0; i < line.size(); i++)
			set(p1.x, p1.y + i, line[i]);
	}
}

bmp_info_header::bmp_info_header(void)
{
	header.info_size = 40;
	header.width = 0;
	header.height = 0;
	header.plane = 1;
	header.bpp = 8;
	header.compression = 0;
	header.image_size = 0;
	header.ppm_x = 0;
	header.ppm_y = 0;
	header.colors = 0;
	header.important_colors = 0;
}

rect_range::rect_range()
{
	x1 = 0;
	x2 = 0;
	y1 = 0;
	y2 = 0;
}
rect_range::rect_range(unsigned int ax1, unsigned int ax2, unsigned int ay1, unsigned int ay2)
{
	x1 = ax1;
	x2 = ax2;
	y1 = ay1;
	y2 = ay2;
}
rect_range::rect_range(const point_2d &p1, const point_2d &p2)
{
	x1 = p1.x;
	x2 = p2.x;
	y1 = p1.y;
	y2 = p2.y;
}
void rect_range::set(int ax1, int ax2, int ay1, int ay2)
{
	x1 = ax1;
	x2 = ax2;
	y1 = ay1;
	y2 = ay2;
}

void rect_range::set(const point_2d &p1, const point_2d &p2)
{
	x1 = p1.x;
	x2 = p2.x;
	y1 = p1.y;
	y2 = p2.y;
}
rect_range& rect_range::operator +=(const point_2d& p)
{
	x1 += p.x;
	x2 += p.x;
	y1 += p.y;
	y2 += p.y;
	return *this;
}
rect_range& rect_range::operator -=(const point_2d& p)
{
	x1 -= p.x;
	x2 -= p.x;
	y1 -= p.y;
	y2 -= p.y;
	return *this;
}
bool rect_range::operator ==(const rect_range &range)
{
	return (x1 == range.x1 && x2 == range.x2 && y1 == range.y1 && y2 == range.y2);
}
bool rect_range::operator !=(const rect_range &range)
{
	return !(x1 == range.x1 && x2 == range.x2 && y1 == range.y1 && y2 == range.y2);
}

int rect_range::intersect(rect_range &rect)
{
	if(x1 < rect.x1)
		x1 = rect.x1;
	else
		rect.x1 = x1;
	if(x2 > rect.x2)
		x2 = rect.x2;
	else
		rect.x2 = x2;
	if(y1 < rect.y1)
		y1 = rect.y1;
	else
		rect.y1 = y1;
	if(y2 > rect.y2)
		y2 = rect.y2;
	else
		rect.y2 = y2;
	if(x1 == x2 && y1 == y2)
		return -1;
	return 0;
}
bool rect_range::inside(const point_2d &point)const
{
	return point.x >= x1 && point.x <= x2 && point.y >= y1 && point.y <= y2;
}
point_2d::point_2d()
{
	x = y = 0;
}
point_2d::point_2d(unsigned int ax, unsigned int ay)
{
	x = ax;
	y = ay;
}
point_2d& point_2d::operator +=(const point_2d &p)
{
	x += p.x;
	y += p.y;
	return *this;
}
point_2d& point_2d::operator -=(const point_2d &p)
{
	x -= p.x;
	y -= p.y;
	return *this;
}
point_2d& point_2d::operator *=(int mag)
{
	x *= mag;
	y *= mag;
	return *this;
}
point_2d& point_2d::operator /=(int div)
{
	if(div != 0){
		x /= div;
		y /= div;
	}
	return *this;
}
bool point_2d::operator !=(const point_2d& p)
{
	return (x != p.x) || (y != p.y);
}
bool point_2d::operator ==(const point_2d& p)
{
	return (x == p.x) && (y == p.y);
}


void point_2d::set(unsigned int xa, unsigned int ya)
{
	x = xa;
	y = ya;
}
void point_2d::get(unsigned int &xa, unsigned int &ya) const
{
	xa = x;
	ya = y;
}
double point_2d::distance(const point_2d &p) const
{
	return sqrt((double)((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y)));
}

matrix_2d::matrix_2d()
{
	matrix_forward.resize(9);
	matrix_reverse.resize(9);
}
void matrix_2d::unit(void)
{
	matrix_forward[0] = 1.0;
	matrix_forward[1] = 0.0;
	matrix_forward[2] = 0.0;
	matrix_forward[3] = 0.0;
	matrix_forward[4] = 1.0;
	matrix_forward[5] = 0.0;
	matrix_forward[6] = 0.0;
	matrix_forward[7] = 0.0;
	matrix_forward[8] = 1.0;

	matrix_reverse = matrix_forward;
}

int matrix_2d::linear(const point_2d &source, const point_2d &destination, double x_zoom, double y_zoom)
{
	if(x_zoom == 0.0 || y_zoom == 0.0)
		return 0;
	matrix_forward[0] = x_zoom;
	matrix_forward[1] = 0.0;
	matrix_forward[2] = destination.x - source.x * x_zoom;

	matrix_forward[3] = 0.0;
	matrix_forward[4] = y_zoom;
	matrix_forward[5] = destination.y - source.y * y_zoom;

	matrix_forward[6] = 0.0;
	matrix_forward[7] = 0.0;
	matrix_forward[8] = 1.0;

	matrix_reverse[0] = 1.0 / x_zoom;
	matrix_reverse[1] = 0.0;
	matrix_reverse[2] = source.x - destination.x / x_zoom;

	matrix_reverse[3] = 0.0;
	matrix_reverse[4] = 1.0 / y_zoom;
	matrix_reverse[5] = source.y - destination.y / y_zoom;

	matrix_reverse[6] = 0.0;
	matrix_reverse[7] = 0.0;
	matrix_reverse[8] = 1.0;

	return 0;
}
void matrix_2d::forward(point_2d &p) const 
{
	point_2d temp(p);

	p.x = (unsigned int)matrix_forward[0] * temp.x + (unsigned int)matrix_forward[1] * temp.y + (unsigned int)matrix_forward[2];
	p.y = (unsigned int)matrix_forward[3] * temp.x + (unsigned int)matrix_forward[4] * temp.y + (unsigned int)matrix_forward[5];
}
void matrix_2d::reverse(point_2d &p) const
{
	point_2d temp(p);

	p.x = (unsigned int)matrix_reverse[0] * temp.x + (unsigned int)matrix_reverse[1] * temp.y + (unsigned int)matrix_reverse[2];
	p.y = (unsigned int)matrix_reverse[3] * temp.x + (unsigned int)matrix_reverse[4] * temp.y + (unsigned int)matrix_reverse[5];
}
void matrix_2d::compound(const matrix_2d &m)
{
	std::valarray<double> temp_f = matrix_forward;
	std::valarray<double> temp_r = matrix_reverse;

	matrix_forward[0] = temp_f[0] * m.matrix_forward[0] + temp_f[1] * m.matrix_forward[3] + temp_f[2] * m.matrix_forward[6];
	matrix_forward[1] = temp_f[0] * m.matrix_forward[1] + temp_f[1] * m.matrix_forward[4] + temp_f[2] * m.matrix_forward[7];
	matrix_forward[2] = temp_f[0] * m.matrix_forward[2] + temp_f[1] * m.matrix_forward[5] + temp_f[2] * m.matrix_forward[8];
	matrix_forward[3] = temp_f[3] * m.matrix_forward[0] + temp_f[4] * m.matrix_forward[3] + temp_f[5] * m.matrix_forward[6];
	matrix_forward[4] = temp_f[3] * m.matrix_forward[1] + temp_f[4] * m.matrix_forward[4] + temp_f[5] * m.matrix_forward[7];
	matrix_forward[5] = temp_f[3] * m.matrix_forward[2] + temp_f[4] * m.matrix_forward[5] + temp_f[5] * m.matrix_forward[8];
	matrix_forward[6] = temp_f[6] * m.matrix_forward[0] + temp_f[7] * m.matrix_forward[3] + temp_f[7] * m.matrix_forward[6];
	matrix_forward[7] = temp_f[6] * m.matrix_forward[1] + temp_f[7] * m.matrix_forward[4] + temp_f[7] * m.matrix_forward[7];
	matrix_forward[8] = temp_f[6] * m.matrix_forward[2] + temp_f[7] * m.matrix_forward[5] + temp_f[7] * m.matrix_forward[8];

	matrix_reverse[0] = temp_r[0] * m.matrix_reverse[0] + temp_r[3] * m.matrix_reverse[1] + temp_f[6] * m.matrix_reverse[2];
	matrix_reverse[1] = temp_r[1] * m.matrix_reverse[0] + temp_r[4] * m.matrix_reverse[1] + temp_f[7] * m.matrix_reverse[2];
	matrix_reverse[2] = temp_r[2] * m.matrix_reverse[0] + temp_r[5] * m.matrix_reverse[1] + temp_f[8] * m.matrix_reverse[2];
	matrix_reverse[3] = temp_r[0] * m.matrix_reverse[3] + temp_r[3] * m.matrix_reverse[4] + temp_f[6] * m.matrix_reverse[5];
	matrix_reverse[4] = temp_r[1] * m.matrix_reverse[3] + temp_r[4] * m.matrix_reverse[4] + temp_f[7] * m.matrix_reverse[5];
	matrix_reverse[5] = temp_r[2] * m.matrix_reverse[3] + temp_r[5] * m.matrix_reverse[4] + temp_f[8] * m.matrix_reverse[5];
	matrix_reverse[6] = temp_r[0] * m.matrix_reverse[6] + temp_r[3] * m.matrix_reverse[7] + temp_f[6] * m.matrix_reverse[8];
	matrix_reverse[7] = temp_r[1] * m.matrix_reverse[6] + temp_r[4] * m.matrix_reverse[7] + temp_f[7] * m.matrix_reverse[8];
	matrix_reverse[8] = temp_r[2] * m.matrix_reverse[6] + temp_r[5] * m.matrix_reverse[7] + temp_f[8] * m.matrix_reverse[8];

}
void matrix_2d::rotation(double degree)
{
	double value_cos, value_sin;
	const double pi = 3.14159265;

	if(degree == 0.0 || degree == 360.0){
		value_cos = 1.0;
		value_sin = 0.0;
	} else if(degree == 90.0){
		value_cos = 0.0;
		value_sin = 1.0;
	} else if(degree == 180.0){
		value_cos = -1.0;
		value_sin = 0.0;
	} else if(degree == 270.0){
		value_cos = 0.0;
		value_sin = -1.0;
	} else {
		value_cos = cos(degree / 180.0 * pi);
		value_sin = sin(degree / 180.0 * pi);
	}

	matrix_forward[0] = value_cos;
	matrix_forward[1] = value_sin;
	matrix_forward[2] = 0.0;
	matrix_forward[3] = -value_sin;
	matrix_forward[4] = value_cos;
	matrix_forward[5] = 0.0;
	matrix_forward[6] = 0.0;
	matrix_forward[7] = 0.0;
	matrix_forward[8] = 1.0;

	matrix_reverse[0] = value_cos;
	matrix_reverse[1] = -value_sin;
	matrix_reverse[2] = 0.0;
	matrix_reverse[3] = value_sin;
	matrix_reverse[4] = value_cos;
	matrix_reverse[5] = 0.0;
	matrix_reverse[6] = 0.0;
	matrix_reverse[7] = 0.0;
	matrix_reverse[8] = 1.0;

}
void matrix_2d::exchange(void)
{
	double temp[3];
	temp[0] = matrix_forward[0];
	temp[1] = matrix_forward[1];
	temp[2] = matrix_forward[2];
	matrix_forward[0] = matrix_forward[3];
	matrix_forward[1] = matrix_forward[4];
	matrix_forward[2] = matrix_forward[5];
	matrix_forward[3] = temp[0];
	matrix_forward[4] = temp[1];
	matrix_forward[5] = temp[2];

	temp[0] = matrix_reverse[0];
	temp[1] = matrix_reverse[1];
	temp[2] = matrix_reverse[2];
	matrix_reverse[0] = matrix_reverse[3];
	matrix_reverse[1] = matrix_reverse[4];
	matrix_reverse[2] = matrix_reverse[5];
	matrix_reverse[3] = temp[0];
	matrix_reverse[4] = temp[1];
	matrix_reverse[5] = temp[2];
}


