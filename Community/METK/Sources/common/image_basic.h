// Header file for image_basic class
// image_basic is a base class for image handling

#pragma warning(disable:4251)
#ifdef WIN32
#ifndef common_EXPORTS
#define __COMMON_IMPORTEXPORT __declspec(dllimport)
#else
#define __COMMON_IMPORTEXPORT __declspec(dllexport) 
#endif
#else
#define MLIMAGEUTILITIES_EXPORT
#endif

#define NOMINMAX
#undef min
#undef max

#include <string>
#include <vector>
#include <valarray>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

#define GRAY_8BPP		1
#define GRAY_16BPP		2
#define COLOR_24BPP		3

class __COMMON_IMPORTEXPORT point_2d{
	public:
		point_2d();
		point_2d(int x, int y);
		void set(int x, int y);
		void get(int &x, int &y) const;
		point_2d& operator +=(const point_2d& p);
		point_2d& operator -=(const point_2d& p);
		point_2d& operator *=(int mag);
		point_2d& operator /=(int div);
		bool operator !=(const point_2d& p);
		bool operator ==(const point_2d& p);
		double distance(const point_2d& p)const;

		int x;
		int y;
};
class __COMMON_IMPORTEXPORT matrix_2d{
	public:
		matrix_2d();
		void forward(point_2d &p) const;
		void reverse(point_2d &p) const;
		void unit(void);
		int linear(const point_2d &source, const point_2d &destination, double x_zoom, double y_zoom);
		void rotation(double degree);
		void exchange(void);
		void compound(const matrix_2d &m);
	protected:
		std::valarray<double> matrix_forward;
		std::valarray<double> matrix_reverse;
};

class __COMMON_IMPORTEXPORT rect_range {
public:
	int x1;
	int x2;
	int y1;
	int y2;
	rect_range();
	rect_range(int ax1, int ax2, int ay1, int ay2);
	rect_range(const point_2d &p1, const point_2d &p2);
	rect_range& operator +=(const point_2d& p);
	rect_range& operator -=(const point_2d& p);
	bool operator ==(const rect_range &rect);
	bool operator !=(const rect_range &rect);
	void set(int ax1, int ax2, int ay1, int ay2);
	void set(const point_2d &p1, const point_2d &p2);

	int area_get(void){return (x2 - x1 + 1) * (y2 - y1 + 1);}
	bool inside(const point_2d &point) const;
	int intersect(rect_range &rect);
};


class __COMMON_IMPORTEXPORT bmp_info_header{
public:
	bmp_info_header(void);
	struct {
		unsigned long	info_size; 
		unsigned long	width; 
		unsigned long	height; 
		short	plane; 
		short	bpp; 
		long	compression; 
		unsigned long	image_size; 
		long	ppm_x; 
		long	ppm_y;
		long	colors; 
		long	important_colors; 
	} header;
}; 

#define BMP_INFO_SIZE	sizeof(bmp_info_header)

class __COMMON_IMPORTEXPORT image_basic{
public:
	unsigned int width;
	unsigned int height;

	image_basic();
	image_basic(unsigned int size_x, unsigned int size_y, int set_color_mode = GRAY_8BPP);
	image_basic(const rect_range &range, int set_color_mode = GRAY_8BPP);
	~image_basic();
	void initialize(unsigned int size_x, unsigned int size_y, int set_color_mode = NULL);
	void initialize(const rect_range &range, int set_color_mode = NULL);
	void erase(void);
	int empty(void);
	int file_read(const std::string &filename);
	int file_write_bitmap(std::string filename);
	int file_write_tiff(std::string filename);
	int convert(int new_color_mode);
	void set_color_mode(int new_color_mode){ color_mode = new_color_mode; }
	int get_color_mode(void){ return color_mode; }
	int fast_get(int x, int y) const { return data[x + width * y]; };
	int fast_get(point_2d &point) const { return data[point.x + width * point.y]; };
	int get(int x, int y) const;
	int get(point_2d &point) const;
	void fast_set(int x, int y, int value){data[x + width * y] = value;}
	void fast_set(point_2d &point, int value){data[point.x + width * point.y] = value;}
	void set(int x, int y, int value);
	void set(point_2d &point, int value);
	int clip_rect(rect_range &range) const;
	double average(const rect_range &range) const;
	void deviation(const rect_range &range, double &deviation, double &average) const;
	double correlation(const image_basic &image_destination, const point_2d &offset) const;
	double correlation(const image_basic &image_destination, rect_range &region_source, rect_range &region_destination) const;
	double correlation_search(const image_basic &image_destination, rect_range &region_source, rect_range &region_destination, int shift_max) const;
	void binarize(const image_basic &image_source, const rect_range &range_source, int threshold, int value_positive, int value_negative);
	void binarize(int threshold, int value_positive, int value_negative);
	void line_draw(const point_2d &p1, const point_2d &p2, int value);
	void line_draw(const std::vector<point_2d> &points, int value);
	void line_draw(const point_2d &p1, const point_2d &p2, const std::valarray<int> &line);
	void line_get(const point_2d &p1, const point_2d &p2, std::valarray<int> &line) const;
	void rect_get(rect_range &range) const;
	void rect_draw(const rect_range &range, int value);
	void cross_draw(const point_2d &p, int size, int value);
	int file_read_tiff(const std::string &filename);
	int file_read_bitmap(const std::string &filename);
	int file_read_pgm(const std::string &filename);
	void copy(const image_basic &source, const rect_range &range_source, const point_2d &point_destination);
	void copy(const image_basic &source, const matrix_2d &m);
	int count_pixel(int value) const;
	int point_center(point_2d &point, int value) const;
	int transpose(int former, int latter);
	int border(const image_basic &source, const rect_range &range_source, int threshold, int value);
	int labeling(const image_basic &image_source, const rect_range &range_source, int threshold);
	void scale(int minimum, int maximum);
	int max_get(int initial = 0) const;
	int min_get(int initial = 0x7fff) const;
	std::valarray<int> data;
protected:
	unsigned short byte_short(unsigned char *buffer, bool endian);
	unsigned long byte_long(unsigned char *buffer, bool endian);
	void ifd_preparation(unsigned char *byte, unsigned short tag, unsigned short type, unsigned long count, unsigned long value);
	int border_trace4(const image_basic &image_source, const rect_range &range_source, const point_2d &point_start, int threshold, int value, int code);
	int border_trace8(const image_basic &image_source, const rect_range &range_source, const point_2d &point_start, int threshold, int value, int code);

	int color_mode;
};
