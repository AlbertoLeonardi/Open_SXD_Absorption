// Utility Functions :: Image Filters
//
// ...

#ifndef _INCLUDE_UTILITY_IMAGE_FILTERS
#define _INCLUDE_UTILITY_IMAGE_FILTERS 1

#include <App_Environment.h>

#include <External_bitmap_image.hpp> // this library uses a standard _WARNING_SIGNAL() function

#include <Signal_Warning.h>

#include <Utility_File_IO.h>
#include <Utility_Math.h>

struct rgb_ellipsoid
{
	double Center[3];
	double EigenValue[3];
	double EigenVector[3][3];
};

struct rgb_ColorPalette
{
	long Color_Count;
	double_3 *color;
};

struct PixelFilter
{
	bool Picked;
	double red;
	double green;
	double blue;
};
struct MaskFilter
{
	long width;
	long height;
	long size;
	
	struct PixelFilter** Mask;
	struct PixelFilter** ServiceMask;
	
	struct PixelFilter* MaskPixel;
	struct PixelFilter* ServicePixel;
};

typedef int(*ImageFilters_Tool)(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
struct ImageFilters_Setting
{
	ImageFilters_Tool Tool;
	
	char   Path[_Nch800];
	
	bool   in_side;
	long   Shell_Thicknes;				// or Max_Color_Count
	double Region[2][2];
	
	double rgb[3];
	double rgb_delta[3];
	double rgb_mismatch[3];
	double rgb_distance;				// or Chanel_Count
	
	struct rgb_ellipsoid Ellipse;
	struct rgb_ColorPalette Palette;
	
	double Scale;						// or Radious
	
	double *Kernel;
	long    Kernel_Size;
};

int ImageFilters_ResetFilter(struct MaskFilter* Filter, bitmap_image* image);
int ImageFilters_ResetFilter_Mask(struct MaskFilter* Filter);
int ImageFilters_FreeFilter(struct MaskFilter* Filter);
int ImageFilters_CopyFilter(struct MaskFilter** Filter_dst, struct MaskFilter** Filter_src);

struct MaskFilter* ImageFilters_GetFilter(bitmap_image* image); // returns NULL pointer if not succeed

void ImageFilters_Reset_Settings(struct ImageFilters_Setting* Filter);

int ImageFilters_GetMaskDataSet(struct MaskFilter* Filter, FILE* BIN);

// Get info from Image

int ImageFilters_GetColorSet_Ellipsoid(char Path[], double lw, double hw, double lh, double hh, struct rgb_ellipsoid* Ellipse, rgb_t Mask_color);
int ImageFilters_GetColorSet_Palette(char Path[], double lw, double hw, double lh, double hh, struct rgb_ColorPalette* Palette, long Max_Palette_Color_Count, rgb_t Mask_color);

// Merge Mask to Image

int ImageFilters_MergeMaskFilter(struct MaskFilter* Filter, bitmap_image* image, rgb_t Mask_color, double Transparency);
int ImageFilters_MergeMask(struct MaskFilter* Filter, bitmap_image* image, rgb_t Mask_color, double Transparency);
void ImageFilters_GetHollowMask(struct MaskFilter* Filter);

// **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** Actual Filters

// Mask operation

int ImageFilters_Mask_Swap(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);

int ImageFilters_Mask_RegionExclude_Square(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Mask_RegionInclude_Square(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);

int ImageFilters_Mask_SquareExpand(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Mask_SquareContract(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Mask_SquareAverage(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);

// Mask update

int ImageFilters_MaskUpdate_SelectColor(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter); // the range will be ignored for negative delta / distance
int ImageFilters_MaskUpdate_SelectColorRGBmismatch(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_MaskUpdate_SelectColor_EllipsoidSet(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_MaskUpdate_SelectColor_Palette(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);

int ImageFilters_MaskUpdate_PickRed(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_MaskUpdate_PickGreen(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_MaskUpdate_PickBlue(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_MaskUpdate_PickGray(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_MaskUpdate_PickWhite(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_MaskUpdate_PickBlack(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);

// Image Filter

int ImageFilters_GrayScale(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_ColorGradient_Scale(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_ColorShift_Scale(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_ColorShift(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Posterize(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);

int ImageFilters_BorderEnhance_3(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_BorderEnhance_5(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);

int ImageFilters_ImageProcessing(struct MaskFilter* Filter, double scale, double Kernel[], long Kernel_Size);

int ImageFilters_Identity(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Sharpen(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Blure_Box_3(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Blure_Box_5(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Blure_Gauss_3(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Blure_Gauss_5(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Edge_3low(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Edge_3mid(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);
int ImageFilters_Edge_3hig(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter);

#endif