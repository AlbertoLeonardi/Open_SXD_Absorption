// Utility Functions :: Image Filters
//

#include "Utility_ImageFilters.h"

int ImageFilters_ResetFilter(struct MaskFilter* Filter, bitmap_image* image)
{
	long i, j;
	rgb_t colour;
	
	if ((*Filter).width  != (long)(*image).width())  return 1;
	if ((*Filter).height != (long)(*image).height()) return 1;
	
	memset((*Filter).MaskPixel, 0, (*Filter).size * sizeof(struct PixelFilter));
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		(*image).get_pixel(i, j, colour);
		(*Filter).Mask[i][j].red   = (double)colour.red;
		(*Filter).Mask[i][j].green = (double)colour.green;
		(*Filter).Mask[i][j].blue  = (double)colour.blue;
	}}
	
	memcpy((*Filter).ServicePixel, (*Filter).MaskPixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_ResetFilter_Mask(struct MaskFilter* Filter)
{
	long i, j;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		(*Filter).Mask[i][j].Picked = false;
	}}
	
	memcpy((*Filter).ServicePixel, (*Filter).MaskPixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_FreeFilter(struct MaskFilter* Filter)
{
	if (Filter != NULL)
	{
		if ((*Filter).Mask != NULL) free((*Filter).Mask);
		if ((*Filter).ServiceMask != NULL) free((*Filter).ServiceMask);
		
		if ((*Filter).MaskPixel != NULL) free((*Filter).MaskPixel);
		if ((*Filter).ServicePixel != NULL) free((*Filter).ServicePixel);
		
		free(Filter);
	}
	
	return 0;
}
int ImageFilters_CopyFilter(struct MaskFilter** Filter_dst, struct MaskFilter** Filter_src)
{
	long i, column_id;
	
	(*Filter_dst) = NULL;
	(*Filter_dst) = (struct MaskFilter*)calloc(1, sizeof(struct MaskFilter));
	if (_APP_WARNING_MESSAGE((*Filter_dst), "Utility_ImageFilters.cpp", "ImageFilters_CopyFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"(*Filter_dst)\" to", 1, 0)) { _USER_IO_WAIT(); return NULL; }
	
	(*(*Filter_dst)).width  = (*(*Filter_src)).width;
	(*(*Filter_dst)).height = (*(*Filter_src)).height;
	(*(*Filter_dst)).size   = (*(*Filter_src)).size;
	
	(*(*Filter_dst)).MaskPixel = (struct PixelFilter*)calloc((*(*Filter_dst)).size, sizeof(struct PixelFilter));
	if (_APP_WARNING_MESSAGE((*(*Filter_dst)).MaskPixel, "Utility_ImageFilters.cpp", "ImageFilters_CopyFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"PixelMask\" to", (*(*Filter_dst)).size, 0)) { _USER_IO_WAIT(); ImageFilters_FreeFilter((*Filter_dst)); return 1; }
	
	(*(*Filter_dst)).ServicePixel = (struct PixelFilter*)calloc((*(*Filter_dst)).size, sizeof(struct PixelFilter));
	if (_APP_WARNING_MESSAGE((*(*Filter_dst)).ServicePixel, "Utility_ImageFilters.cpp", "ImageFilters_CopyFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"PixelMask\" to", (*(*Filter_dst)).size, 0)) { _USER_IO_WAIT(); ImageFilters_FreeFilter((*Filter_dst)); return 1; }
	
	(*(*Filter_dst)).Mask = (struct PixelFilter**)calloc((*(*Filter_dst)).width, sizeof(struct PixelFilter*));
	if (_APP_WARNING_MESSAGE((*(*Filter_dst)).Mask, "Utility_ImageFilters.cpp", "ImageFilters_CopyFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"Mask\" to", (*(*Filter_dst)).width, 0)) { _USER_IO_WAIT(); ImageFilters_FreeFilter((*Filter_dst)); return 1; }
	
	(*(*Filter_dst)).ServiceMask = (struct PixelFilter**)calloc((*(*Filter_dst)).width, sizeof(struct PixelFilter*));
	if (_APP_WARNING_MESSAGE((*(*Filter_dst)).ServiceMask, "Utility_ImageFilters.cpp", "ImageFilters_CopyFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"Mask\" to", (*(*Filter_dst)).width, 0)) { _USER_IO_WAIT(); ImageFilters_FreeFilter((*Filter_dst)); return 1; }
	
	for (i = 0; i < (*(*Filter_dst)).width; i++)
	{
		column_id = i * (*(*Filter_dst)).height;
		(*(*Filter_dst)).Mask[i] = &((*(*Filter_dst)).MaskPixel[column_id]);
		(*(*Filter_dst)).ServiceMask[i] = &((*(*Filter_dst)).ServicePixel[column_id]);
	}
	
	memcpy((*(*Filter_dst)).MaskPixel,    (*(*Filter_src)).MaskPixel,    (*(*Filter_dst)).size * sizeof(struct PixelFilter));
	memcpy((*(*Filter_dst)).ServicePixel, (*(*Filter_src)).ServicePixel, (*(*Filter_dst)).size * sizeof(struct PixelFilter));
	
	return 0;
}

struct MaskFilter* ImageFilters_GetFilter(bitmap_image* image)
{
	long i, j, column_id;
	
	rgb_t colour;
	
	struct MaskFilter* Filter;
	
	Filter = NULL;
	Filter = (struct MaskFilter*)calloc(1, sizeof(struct MaskFilter));
	if (_APP_WARNING_MESSAGE(Filter, "Utility_ImageFilters.cpp", "ImageFilters_GetFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"Filter\" to", 1, 0)) { _USER_IO_WAIT(); return NULL; }
	
	(*Filter).width  = (*image).width();
	(*Filter).height = (*image).height();
	(*Filter).size   = (*Filter).width * (*Filter).height;
	
	(*Filter).MaskPixel = (struct PixelFilter*)calloc((*Filter).size, sizeof(struct PixelFilter));
	if (_APP_WARNING_MESSAGE((*Filter).MaskPixel, "Utility_ImageFilters.cpp", "ImageFilters_GetFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"PixelMask\" to", (*Filter).size, 0)) { _USER_IO_WAIT(); ImageFilters_FreeFilter(Filter); return Filter; }
	
	(*Filter).ServicePixel = (struct PixelFilter*)calloc((*Filter).size, sizeof(struct PixelFilter));
	if (_APP_WARNING_MESSAGE((*Filter).ServicePixel, "Utility_ImageFilters.cpp", "ImageFilters_GetFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"PixelMask\" to", (*Filter).size, 0)) { _USER_IO_WAIT(); ImageFilters_FreeFilter(Filter); return Filter; }
	
	(*Filter).Mask = (struct PixelFilter**)calloc((*Filter).width, sizeof(struct PixelFilter*));
	if (_APP_WARNING_MESSAGE((*Filter).Mask, "Utility_ImageFilters.cpp", "ImageFilters_GetFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"Mask\" to", (*Filter).width, 0)) { _USER_IO_WAIT(); ImageFilters_FreeFilter(Filter); return Filter; }
	
	(*Filter).ServiceMask = (struct PixelFilter**)calloc((*Filter).width, sizeof(struct PixelFilter*));
	if (_APP_WARNING_MESSAGE((*Filter).ServiceMask, "Utility_ImageFilters.cpp", "ImageFilters_GetFilter", _WARNING_MESSAGE_KEY_CALLOC, "\"Mask\" to", (*Filter).width, 0)) { _USER_IO_WAIT(); ImageFilters_FreeFilter(Filter); return Filter; }
	
	for (i = 0; i < (*Filter).width; i++)
	{
		column_id = i * (*Filter).height;
		(*Filter).Mask[i] = &((*Filter).MaskPixel[column_id]);
		(*Filter).ServiceMask[i] = &((*Filter).ServicePixel[column_id]);
	}
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		(*image).get_pixel(i, j, colour);
		(*Filter).Mask[i][j].red   = (double)colour.red;
		(*Filter).Mask[i][j].green = (double)colour.green;
		(*Filter).Mask[i][j].blue  = (double)colour.blue;
	}}
	
	memcpy((*Filter).ServicePixel, (*Filter).MaskPixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return Filter;
}

void ImageFilters_Reset_Settings(struct ImageFilters_Setting* Filter)
{
	(*Filter).Tool = NULL;
	
	memset(&(*Filter).Path[0], 0, _Nch800 * sizeof(char));
	
	(*Filter).in_side = true;
	
	(*Filter).Shell_Thicknes = 1;
	
	(*Filter).Region[0][0] = 0.0;
	(*Filter).Region[0][1] = 1.0;
	(*Filter).Region[1][0] = 0.0;
	(*Filter).Region[1][1] = 1.0;
	
	(*Filter).rgb[0] = 0.0;
	(*Filter).rgb[1] = 0.0;
	(*Filter).rgb[2] = 0.0;
	
	(*Filter).rgb_delta[0] = 0.0;
	(*Filter).rgb_delta[1] = 0.0;
	(*Filter).rgb_delta[2] = 0.0;
	
	(*Filter).rgb_mismatch[0] = 0.0;
	(*Filter).rgb_mismatch[1] = 0.0;
	(*Filter).rgb_mismatch[2] = 0.0;
	
	(*Filter).rgb_distance = 0.0;
	
	(*Filter).Scale = 1.0;
	
	(*Filter).Kernel = NULL;
	(*Filter).Kernel_Size = 0;
	
	memset(&(*Filter).Ellipse, 0, sizeof(struct rgb_ellipsoid));
	memset(&(*Filter).Palette, 0, sizeof(struct rgb_ColorPalette));
	
	return;
}

// Get info from Image

int ImageFilters_GetColorSet_Ellipsoid(char Path[], double lw, double hw, double lh, double hh, struct rgb_ellipsoid* Ellipse, rgb_t Mask_color)
{
	long i, j, k, l;
	
	long Count;
	long Threhold[2][2];
	
	double Norm;
	double c[3];
	double II[3][3];
	
	rgb_t colour;
	
	bitmap_image image(Path);
	if (!image) return 1;
	
	Threhold[0][0] = (long)round(lw * (double)image.width());
	Threhold[0][1] = (long)round(hw * (double)image.width());
	Threhold[1][0] = (long)round(lh * (double)image.height());
	Threhold[1][1] = (long)round(hh * (double)image.height());
	
	Threhold[0][1]++;
	Threhold[1][1]++;
	
	if (Threhold[0][0] < 0)						Threhold[0][0] = 0;
	if (Threhold[1][0] < 0)						Threhold[1][0] = 0;
	if (Threhold[0][1] > (long)image.width())	Threhold[0][1] = (long)image.width();
	if (Threhold[1][1] > (long)image.height())	Threhold[1][1] = (long)image.height();
	
	for (i = 0; i < 3; i++)
	{
		(*Ellipse).Center[i] = 0.0;
		for (j = i; j < 3; j++) II[i][j] = 0.0;
	}
	
	Count = 0;
	for (i = Threhold[0][0]; i < Threhold[0][1]; i++) {
	for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
		Count++;
		
		image.get_pixel(i, j, colour);
		c[0] = (double)colour.red;
		c[1] = (double)colour.green;
		c[2] = (double)colour.blue;
		
		for (k = 0; k < 3; k++)
		{
			(*Ellipse).Center[k] += c[k];
			for (l = k; l < 3; l++) II[k][l] += c[k] * c[l];
		}
	}}
	
	Norm = 1.0 / (double)Count;
	for (i = 0; i < 3; i++) (*Ellipse).Center[i] *= Norm;
	for (i = 0; i < 3; i++) { for (j = i; j < 3; j++) { II[i][j] *= Norm; II[i][j] -= (*Ellipse).Center[i] * (*Ellipse).Center[j]; } }
	for (i = 0; i < 3; i++) { for (j = 0; j < i; j++) { II[i][j] = II[j][i]; } }
	
	EigenValueVectors_Jacobi_Cyclic_Method(&(*Ellipse).EigenValue[0], &(*Ellipse).EigenVector[0][0], &II[0][0], 3);
	
	/*/
	for (i = Threhold[0][0]; i < Threhold[0][1]; i++) {
	for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
		image.get_pixel(i, j, colour);
		c[0] = (double)colour.red;
		c[1] = (double)colour.green;
		c[2] = (double)colour.blue;
			
		Norm = 0.0;
		for (k = 0; k < 3; k++) c[k] -= (*Ellipse).Center[k];
		for (k = 0; k < 3; k++) { ax = (c[0] * (*Ellipse).EigenVector[k][0] + c[1] * (*Ellipse).EigenVector[k][1] + c[2] * (*Ellipse).EigenVector[k][2]) / (*Ellipse).EigenValue[k]; Norm += ax * ax; }
			
		if (Norm - 1.0 < FLT_EPSILON) image.set_pixel(i, j, Mask_color);
	}}
	
	image.save_image(Ou_Path);
	/**/
	
	return 0;
}

int ImageFilters_GetColorSet_Palette(char Path[], double lw, double hw, double lh, double hh, struct rgb_ColorPalette* Palette, long Max_Palette_Color_Count, rgb_t Mask_color)
{
	long i, j, k;
	
	long Threhold[2][2];

	bool Include;
	
	rgb_t colour;
	double_3 cc, dc;
	
	bitmap_image image(Path);
	if (!image) return 1;
	
	(*Palette).color = (double_3*)calloc(Max_Palette_Color_Count, sizeof(double_3));
	if (_APP_WARNING_MESSAGE((*Palette).color, "Utility_ImageFilters.cpp", "ImageFilters_GetColorSet_Palette", _WARNING_MESSAGE_KEY_CALLOC, "\"(*Palette).color\" to", Max_Palette_Color_Count, 0)) { _USER_IO_WAIT(); return 1; }

	Threhold[0][0] = (long)round(lw * (double)image.width());
	Threhold[0][1] = (long)round(hw * (double)image.width());
	Threhold[1][0] = (long)round(lh * (double)image.height());
	Threhold[1][1] = (long)round(hh * (double)image.height());
	
	Threhold[0][1]++;
	Threhold[1][1]++;

	if (Threhold[0][0] < 0)						Threhold[0][0] = 0;
	if (Threhold[1][0] < 0)						Threhold[1][0] = 0;
	if (Threhold[0][1] > (long)image.width())	Threhold[0][1] = (long)image.width();
	if (Threhold[1][1] > (long)image.height())	Threhold[1][1] = (long)image.height();
	
	(*Palette).Color_Count = 0;
	for (i = Threhold[0][0]; i < Threhold[0][1]; i++) {
	for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
		if ((*Palette).Color_Count < Max_Palette_Color_Count)
		{
			image.get_pixel(i, j, colour);
			cc[0] = (double)colour.red;
			cc[1] = (double)colour.green;
			cc[2] = (double)colour.blue;
			
			Include = true;
			for (k = 0; k < (*Palette).Color_Count; k++)
			{
				dc[0] = cc[0] - (*Palette).color[k][0];
				dc[1] = cc[1] - (*Palette).color[k][1];
				dc[2] = cc[2] - (*Palette).color[k][2];
				if (dc[0] * dc[0] + dc[1] * dc[1] + dc[2] * dc[2] < FLT_EPSILON) { Include = false; k = (*Palette).Color_Count; }
			}
			
			if (Include)
			{
				(*Palette).color[(*Palette).Color_Count][0] = cc[0];
				(*Palette).color[(*Palette).Color_Count][1] = cc[1];
				(*Palette).color[(*Palette).Color_Count][2] = cc[2];
				(*Palette).Color_Count++;
			}
		}
		else return 1;
	}}
	
	(*Palette).color = (double_3*)realloc((*Palette).color, (*Palette).Color_Count * sizeof(double_3));
	if (_APP_WARNING_MESSAGE((*Palette).color, "Utility_ImageFilters.cpp", "ImageFilters_GetColorSet_Palette", _WARNING_MESSAGE_KEY_REALLOC, "\"(*Palette).color\" to", (*Palette).Color_Count, 0)) { _USER_IO_WAIT(); return 1; }

	/**/
	char Path_n[_Nch800];
	
	rgb_t Mask;
	Mask.red = 255;
	Mask.green = 0;
	Mask.blue = 0;
	
	for (i = Threhold[0][0]; i < Threhold[0][1]; i++) {
	for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
		image.set_pixel(i, j, Mask);
	}}
	
	sprintf_s(&Path_n[0], _Nch800, "%s_palette.bmp", Path);
	image.save_image(Path_n);
	/**/

	return 0;
}

int ImageFilters_GetMaskDataSet(struct MaskFilter* Filter, FILE* BIN)
{
	long i, j;
	
	long Count, id;
	double_2 Node[2000];
	double_2 Center;
	
	Count = 0; id = 0;
	
	Center[0] = (0.50 * (double)(*Filter).width) - 0.50;
	Center[1] = (0.50 * (double)(*Filter).height) + 0.50;
	
	fwrite(&Center, sizeof(double_2), 1, BIN);
	fwrite(&Count, sizeof(long), 1, BIN);
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if ((*Filter).Mask[i][j].Picked)
		{
			Node[id][0] = (double)i;
			Node[id][1] = (double)((*Filter).height - j);
			Count++; id++;
			if (id == 2000)
			{
				fwrite(&Node[0], sizeof(double_2), id, BIN);
				fflush(BIN);
				id = 0;
			}
		}
	}}
	
	fwrite(&Node[0], sizeof(double_2), id, BIN);
	fflush(BIN);
	
	fseek(BIN, sizeof(double_2), SEEK_SET);
	fwrite(&Count, sizeof(long), 1, BIN);
	fflush(BIN);
	
	return 0;
}

// Merge Mask to Image

int ImageFilters_MergeMaskFilter(struct MaskFilter* Filter, bitmap_image* image, rgb_t Mask_color, double Transparency)
{
	long i, j;
	
	rgb_t color;
	double_3 Shadow, cc;

	if (((long)(*image).width() != (*Filter).width) || ((long)(*image).height() != (*Filter).height)) { _APP_WARNING_MESSAGE(Filter, "Utility_ImageFilters.cpp", "ImageFilters_MaskUpdate_Color", _WARNING_MESSAGE_KEY_VALUE, "Image and Mask sizes do NOT match", 0, 1); _USER_IO_WAIT(); return 1; }
	
	memcpy((*Filter).ServicePixel, (*Filter).MaskPixel, (*Filter).size * sizeof(struct PixelFilter));
	
	Shadow[0] = (1.0 - Transparency) * (double)Mask_color.red;
	Shadow[1] = (1.0 - Transparency) * (double)Mask_color.green;
	Shadow[2] = (1.0 - Transparency) * (double)Mask_color.blue;

	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if ((*Filter).Mask[i][j].Picked)
		{
			if ((*Filter).Mask[i][j].red   < 0.0) cc[0] = 0.0; else if ((*Filter).Mask[i][j].red   > 255.0) cc[0] = 255.0; else cc[0] = (*Filter).Mask[i][j].red;
			if ((*Filter).Mask[i][j].green < 0.0) cc[1] = 0.0; else if ((*Filter).Mask[i][j].green > 255.0) cc[1] = 255.0; else cc[1] = (*Filter).Mask[i][j].green;
			if ((*Filter).Mask[i][j].blue  < 0.0) cc[2] = 0.0; else if ((*Filter).Mask[i][j].blue  > 255.0) cc[2] = 255.0; else cc[2] = (*Filter).Mask[i][j].blue;

			(*Filter).ServiceMask[i][j].red   = Shadow[0] + Transparency * (cc[0] - Shadow[0]);
			(*Filter).ServiceMask[i][j].green = Shadow[1] + Transparency * (cc[1] - Shadow[1]);
			(*Filter).ServiceMask[i][j].blue  = Shadow[2] + Transparency * (cc[2] - Shadow[2]);

			color.red   = (unsigned char)round((*Filter).ServiceMask[i][j].red);
			color.green = (unsigned char)round((*Filter).ServiceMask[i][j].green);
			color.blue  = (unsigned char)round((*Filter).ServiceMask[i][j].blue);
			(*image).set_pixel(i, j, color);

		}
		/**/ // if included this section will update also the image for the image-filters applied but it will slow down the processin of the images !!!!
		else
		{
			if ((*Filter).Mask[i][j].red   < 0.0) (*Filter).ServiceMask[i][j].red   = 0.0; else if ((*Filter).Mask[i][j].red   > 255.0) (*Filter).ServiceMask[i][j].red   = 255.0;
			if ((*Filter).Mask[i][j].green < 0.0) (*Filter).ServiceMask[i][j].green = 0.0; else if ((*Filter).Mask[i][j].green > 255.0) (*Filter).ServiceMask[i][j].green = 255.0;
			if ((*Filter).Mask[i][j].blue  < 0.0) (*Filter).ServiceMask[i][j].blue  = 0.0; else if ((*Filter).Mask[i][j].blue  > 255.0) (*Filter).ServiceMask[i][j].blue  = 255.0;

			color.red   = (unsigned char)round((*Filter).ServiceMask[i][j].red);
			color.green = (unsigned char)round((*Filter).ServiceMask[i][j].green);
			color.blue  = (unsigned char)round((*Filter).ServiceMask[i][j].blue);
			(*image).set_pixel(i, j, color);
		}
		/**/
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_MergeMask(struct MaskFilter* Filter, bitmap_image* image, rgb_t Mask_color, double Transparency)
{
	long i, j;
	
	rgb_t color;
	double_3 Shadow, cc;

	if (((long)(*image).width() != (*Filter).width) || ((long)(*image).height() != (*Filter).height)) { _APP_WARNING_MESSAGE(Filter, "Utility_ImageFilters.cpp", "ImageFilters_MaskUpdate_Color", _WARNING_MESSAGE_KEY_VALUE, "Image and Mask sizes do NOT match", 0, 1); _USER_IO_WAIT(); return 1; }
	
	memcpy((*Filter).ServicePixel, (*Filter).MaskPixel, (*Filter).size * sizeof(struct PixelFilter));
	
	Shadow[0] = (1.0 - Transparency) * (double)Mask_color.red;
	Shadow[1] = (1.0 - Transparency) * (double)Mask_color.green;
	Shadow[2] = (1.0 - Transparency) * (double)Mask_color.blue;

	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if ((*Filter).Mask[i][j].Picked)
		{
			(*image).get_pixel(i, j, color);
			cc[0] = (double)color.red;
			cc[1] = (double)color.green;
			cc[2] = (double)color.blue;
			
			//if ((*Filter).Mask[i][j].red   < 0.0) cc[0] = 0.0; else if ((*Filter).Mask[i][j].red   > 255.0) cc[0] = 255.0; else cc[0] = (*Filter).Mask[i][j].red;
			//if ((*Filter).Mask[i][j].green < 0.0) cc[1] = 0.0; else if ((*Filter).Mask[i][j].green > 255.0) cc[1] = 255.0; else cc[1] = (*Filter).Mask[i][j].green;
			//if ((*Filter).Mask[i][j].blue  < 0.0) cc[2] = 0.0; else if ((*Filter).Mask[i][j].blue  > 255.0) cc[2] = 255.0; else cc[2] = (*Filter).Mask[i][j].blue;

			(*Filter).ServiceMask[i][j].red   = Shadow[0] + Transparency * (cc[0] - Shadow[0]);
			(*Filter).ServiceMask[i][j].green = Shadow[1] + Transparency * (cc[1] - Shadow[1]);
			(*Filter).ServiceMask[i][j].blue  = Shadow[2] + Transparency * (cc[2] - Shadow[2]);

			color.red   = (unsigned char)round((*Filter).ServiceMask[i][j].red);
			color.green = (unsigned char)round((*Filter).ServiceMask[i][j].green);
			color.blue  = (unsigned char)round((*Filter).ServiceMask[i][j].blue);
			(*image).set_pixel(i, j, color);
			//(*image).set_pixel(i, j, Mask_color);

		}
		/*/ // if included this section will update also the image for the image-filters applied but it will slow down the processin of the images !!!!
		else
		{
			if ((*Filter).Mask[i][j].red   < 0.0) (*Filter).ServiceMask[i][j].red   = 0.0; else if ((*Filter).Mask[i][j].red   > 255.0) (*Filter).ServiceMask[i][j].red   = 255.0;
			if ((*Filter).Mask[i][j].green < 0.0) (*Filter).ServiceMask[i][j].green = 0.0; else if ((*Filter).Mask[i][j].green > 255.0) (*Filter).ServiceMask[i][j].green = 255.0;
			if ((*Filter).Mask[i][j].blue  < 0.0) (*Filter).ServiceMask[i][j].blue  = 0.0; else if ((*Filter).Mask[i][j].blue  > 255.0) (*Filter).ServiceMask[i][j].blue  = 255.0;

			color.red   = (unsigned char)round((*Filter).ServiceMask[i][j].red);
			color.green = (unsigned char)round((*Filter).ServiceMask[i][j].green);
			color.blue  = (unsigned char)round((*Filter).ServiceMask[i][j].blue);
			(*image).set_pixel(i, j, color);
		}
		/**/
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
void ImageFilters_GetHollowMask(struct MaskFilter* Filter)
{
	long i, j, k, l;
	
	double Bvoid;
	
	memcpy((*Filter).ServicePixel, (*Filter).MaskPixel, (*Filter).size * sizeof(struct PixelFilter));
	
	for (i = 1; i < (*Filter).width - 1;  i++) {
	for (j = 1; j < (*Filter).height - 1; j++) {
		if ((*Filter).Mask[i][j].Picked)
		{
			Bvoid = true;
			for (k = i - 1; k < i + 2; k++) {
			for (l = j - 1; l < j + 2; l++) {
				if (!(*Filter).Mask[k][l].Picked) { Bvoid = false; k = i + 2; l = j + 2; }
			}}
			if (Bvoid)(*Filter).ServiceMask[i][j].Picked = false;
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return;
}

// **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** Actual Filters

// Mask Operation

int ImageFilters_Mask_Swap(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if ((*Filter).Mask[i][j].Picked) (*Filter).Mask[i][j].Picked = false;
		else                             (*Filter).Mask[i][j].Picked = true;
	}}
	
	memcpy((*Filter).ServicePixel, (*Filter).MaskPixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}

int ImageFilters_Mask_RegionExclude_Square(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	long Threhold[2][2];
	
	Threhold[0][0] = (long)round((*Tool).Region[0][0] * (double)(*Filter).width);	if (Threhold[0][0] < 0)					Threhold[0][0] = 0;
	Threhold[0][1] = (long)round((*Tool).Region[0][1] * (double)(*Filter).width);	if (Threhold[0][1] >= (*Filter).width)	Threhold[0][1] = (*Filter).width - 1;
	Threhold[1][0] = (long)round((*Tool).Region[1][0] * (double)(*Filter).height);	if (Threhold[1][0] < 0)					Threhold[1][0] = 0;
	Threhold[1][1] = (long)round((*Tool).Region[1][1] * (double)(*Filter).height);	if (Threhold[1][1] >= (*Filter).height)	Threhold[1][1] = (*Filter).height - 1;
	
	if ((*Tool).in_side)
	{
		Threhold[0][1]++;
		Threhold[1][1]++;
		
		for (i = Threhold[0][0]; i < Threhold[0][1]; i++) {
		for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
			(*Filter).Mask[i][j].Picked = false;
		}}
	}
	else
	{
		Threhold[1][1]++;
		
		for (i = 0; i < (*Filter).width; i++) {
			for (j = 0; j < Threhold[1][0]; j++) {
				(*Filter).Mask[i][j].Picked = false;
			}
			for (j = Threhold[1][1]; j < (*Filter).height; j++) {
				(*Filter).Mask[i][j].Picked = false;
			}
		}
		for (i = 0; i < Threhold[0][0]; i++) {
			for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
				(*Filter).Mask[i][j].Picked = false;
			}
		}
		for (i = Threhold[0][1] + 1; i < (*Filter).width; i++) {
			for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
				(*Filter).Mask[i][j].Picked = false;
			}
		}
	}
	
	memcpy((*Filter).ServicePixel, (*Filter).MaskPixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_Mask_RegionInclude_Square(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	long Threhold[2][2];
	
	Threhold[0][0] = (long)round((*Tool).Region[0][0] * (double)(*Filter).width);	if (Threhold[0][0] < 0)					Threhold[0][0] = 0;
	Threhold[0][1] = (long)round((*Tool).Region[0][1] * (double)(*Filter).width);	if (Threhold[0][1] >= (*Filter).width)	Threhold[0][1] = (*Filter).width - 1;
	Threhold[1][0] = (long)round((*Tool).Region[1][0] * (double)(*Filter).height);	if (Threhold[1][0] < 0)					Threhold[1][0] = 0;
	Threhold[1][1] = (long)round((*Tool).Region[1][1] * (double)(*Filter).height);	if (Threhold[1][1] >= (*Filter).height)	Threhold[1][1] = (*Filter).height - 1;
	
	if ((*Tool).in_side)
	{
		Threhold[0][1]++;
		Threhold[1][1]++;
		
		for (i = Threhold[0][0]; i < Threhold[0][1]; i++) {
		for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
			(*Filter).Mask[i][j].Picked = true;
		}}
	}
	else
	{
		Threhold[1][1]++;
		
		for (i = 0; i < (*Filter).width; i++) {
			for (j = 0; j < Threhold[1][0]; j++) {
				(*Filter).Mask[i][j].Picked = true;
			}
			for (j = Threhold[1][1]; j < (*Filter).height; j++) {
				(*Filter).Mask[i][j].Picked = true;
			}
		}
		for (i = 0; i < Threhold[0][0]; i++) {
			for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
				(*Filter).Mask[i][j].Picked = true;
			}
		}
		for (i = Threhold[0][1] + 1; i < (*Filter).width; i++) {
			for (j = Threhold[1][0]; j < Threhold[1][1]; j++) {
				(*Filter).Mask[i][j].Picked = true;
			}
		}
	}
	
	memcpy((*Filter).ServicePixel, (*Filter).MaskPixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}

int ImageFilters_Mask_SquareExpand(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j, k, l;
	
	long Threhold[2][2];
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if ((*Filter).Mask[i][j].Picked)
		{
			Threhold[0][0] = i - (*Tool).Shell_Thicknes;		if (Threhold[0][0] < 0)					Threhold[0][0] = 0;
			Threhold[0][1] = i + (*Tool).Shell_Thicknes + 1;	if (Threhold[0][1] > (*Filter).width)	Threhold[0][1] = (*Filter).width;
			Threhold[1][0] = j - (*Tool).Shell_Thicknes;		if (Threhold[1][0] < 0)					Threhold[1][0] = 0;
			Threhold[1][1] = j + (*Tool).Shell_Thicknes + 1;	if (Threhold[1][1] > (*Filter).height)	Threhold[1][1] = (*Filter).height;
			
			for (k = Threhold[0][0]; k < Threhold[0][1]; k++) {
			for (l = Threhold[1][0]; l < Threhold[1][1]; l++) {
				(*Filter).ServiceMask[k][l].Picked = true;
			}}
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_Mask_SquareContract(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j, k, l;
	
	long Threhold[2][2];
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			Threhold[0][0] = i - (*Tool).Shell_Thicknes;		if (Threhold[0][0] < 0)					Threhold[0][0] = 0;
			Threhold[0][1] = i + (*Tool).Shell_Thicknes + 1;	if (Threhold[0][1] > (*Filter).width)	Threhold[0][1] = (*Filter).width;
			Threhold[1][0] = j - (*Tool).Shell_Thicknes;		if (Threhold[1][0] < 0)					Threhold[1][0] = 0;
			Threhold[1][1] = j + (*Tool).Shell_Thicknes + 1;	if (Threhold[1][1] > (*Filter).height)	Threhold[1][1] = (*Filter).height;
			
			for (k = Threhold[0][0]; k < Threhold[0][1]; k++) {
			for (l = Threhold[1][0]; l < Threhold[1][1]; l++) {
				(*Filter).ServiceMask[k][l].Picked = false;
			}}
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_Mask_SquareAverage(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j, k, l;
	
	long Threhold[2][2];
	long Count, Sum;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		Threhold[0][0] = i - (*Tool).Shell_Thicknes;		if (Threhold[0][0] < 0)					Threhold[0][0] = 0;
		Threhold[0][1] = i + (*Tool).Shell_Thicknes + 1;	if (Threhold[0][1] > (*Filter).width)	Threhold[0][1] = (*Filter).width;
		Threhold[1][0] = j - (*Tool).Shell_Thicknes;		if (Threhold[1][0] < 0)					Threhold[1][0] = 0;
		Threhold[1][1] = j + (*Tool).Shell_Thicknes + 1;	if (Threhold[1][1] > (*Filter).height)	Threhold[1][1] = (*Filter).height;
		
		Sum = 0;
		Count = 0;
		
		for (k = Threhold[0][0]; k < Threhold[0][1]; k++) {
		for (l = Threhold[1][0]; l < Threhold[1][1]; l++) {
			Count++;
			if ((*Filter).Mask[k][l].Picked) Sum++;
		}}
		
		if ((*Filter).ServiceMask[i][j].Picked) { if (Sum * 2 < Count) (*Filter).ServiceMask[i][j].Picked = false; }
		else                                    { if (Sum * 2 > Count) (*Filter).ServiceMask[i][j].Picked = true; }
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}

// Mask Update

int ImageFilters_MaskUpdate_SelectColor(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter) // the range will be ignored for negative delta / distance
{
	long i, j;
	
	double delta, rgb_distance_2;
	double_3 drgb;
	
	bool Test, Distance;
	
	rgb_distance_2 = (*Tool).rgb_distance * (*Tool).rgb_distance;
	if ((*Tool).rgb_distance > -FLT_EPSILON) Distance = true; else Distance = false;

	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			Test = true;
			
			if (Test) { delta = abs((*Filter).Mask[i][j].red   - (*Tool).rgb[0]); if ((*Tool).rgb_delta[0] >= 0) { if (delta > (*Tool).rgb_delta[0]) Test = false; else drgb[0] = delta; } else drgb[0] = delta; }
			if (Test) { delta = abs((*Filter).Mask[i][j].green - (*Tool).rgb[1]); if ((*Tool).rgb_delta[1] >= 0) { if (delta > (*Tool).rgb_delta[1]) Test = false; else drgb[1] = delta; } else drgb[1] = delta; }
			if (Test) { delta = abs((*Filter).Mask[i][j].blue  - (*Tool).rgb[2]); if ((*Tool).rgb_delta[2] >= 0) { if (delta > (*Tool).rgb_delta[2]) Test = false; else drgb[2] = delta; } else drgb[2] = delta; }
			if (Test) { if (Distance) { if (rgb_distance_2 - (drgb[0] * drgb[0] + drgb[1] * drgb[1] + drgb[2] * drgb[2]) < -FLT_EPSILON) Test = false; } }
			
			if (Test) (*Filter).ServiceMask[i][j].Picked = true;
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_MaskUpdate_SelectColorRGBmismatch(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	double delta;
	double drgb[3];
	
	bool Test;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			Test = true;
			
			if (Test) { delta = abs((*Filter).Mask[i][j].green - (*Filter).Mask[i][j].red   - (*Tool).rgb_mismatch[0]); if ((*Tool).rgb_delta[0] >= FLT_EPSILON) { if (delta > (*Tool).rgb_delta[0]) Test = false; else drgb[0] = delta; } else drgb[0] = delta; }
			if (Test) { delta = abs((*Filter).Mask[i][j].blue  - (*Filter).Mask[i][j].red   - (*Tool).rgb_mismatch[1]); if ((*Tool).rgb_delta[1] >= FLT_EPSILON) { if (delta > (*Tool).rgb_delta[1]) Test = false; else drgb[1] = delta; } else drgb[1] = delta; }
			if (Test) { delta = abs((*Filter).Mask[i][j].blue  - (*Filter).Mask[i][j].green - (*Tool).rgb_mismatch[2]); if ((*Tool).rgb_delta[2] >= FLT_EPSILON) { if (delta > (*Tool).rgb_delta[2]) Test = false; else drgb[2] = delta; } else drgb[2] = delta; }
			if (Test) { if ((*Tool).rgb_distance > -FLT_EPSILON) { if ((*Tool).rgb_distance - (drgb[0] * drgb[0] + drgb[1] * drgb[1] + drgb[2] * drgb[2]) < -FLT_EPSILON) Test = false; } }
			
			if (Test) (*Filter).ServiceMask[i][j].Picked = true;
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_MaskUpdate_SelectColor_EllipsoidSet(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j, k;
	
	double ax, distance;
	double c[3];
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			c[0] = (*Filter).Mask[i][j].red   - (*Tool).Ellipse.Center[0];
			c[1] = (*Filter).Mask[i][j].green - (*Tool).Ellipse.Center[1];
			c[2] = (*Filter).Mask[i][j].blue  - (*Tool).Ellipse.Center[2];
			
			distance = 0.0;
			for (k = 0; k < 3; k++) { ax = c[0] * (*Tool).Ellipse.EigenVector[k][0] + c[1] * (*Tool).Ellipse.EigenVector[k][1] + c[2] * (*Tool).Ellipse.EigenVector[k][2]; distance += (ax * ax) / (*Tool).Ellipse.EigenValue[k]; }
			
			if (distance - (*Tool).Scale < FLT_EPSILON) (*Filter).ServiceMask[i][j].Picked = true;
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_MaskUpdate_SelectColor_Palette(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j, k;
	
	double c[3];
	double r2 = (*Tool).rgb_distance * (*Tool).rgb_distance;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			for (k = 0; k < (*Tool).Palette.Color_Count; k++)
			{
				c[0] = (*Filter).Mask[i][j].red   - (*Tool).Palette.color[k][0];
				c[1] = (*Filter).Mask[i][j].green - (*Tool).Palette.color[k][1];
				c[2] = (*Filter).Mask[i][j].blue  - (*Tool).Palette.color[k][2];

				if (c[0] * c[0] + c[1] * c[1] + c[2] * c[2] - r2 < FLT_EPSILON) { (*Filter).ServiceMask[i][j].Picked = true; k = (*Tool).Palette.Color_Count; }
			}
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}

int ImageFilters_MaskUpdate_PickRed(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			if ((*Filter).Mask[i][j].red > (*Tool).Scale) {
			if ((*Filter).Mask[i][j].red - (*Filter).Mask[i][j].blue  > (*Tool).rgb_distance) {
			if ((*Filter).Mask[i][j].red - (*Filter).Mask[i][j].green > (*Tool).rgb_distance) {
				(*Filter).ServiceMask[i][j].Picked = true;
			}}}
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_MaskUpdate_PickGreen(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			if ((*Filter).Mask[i][j].green > (*Tool).Scale) {
			if ((*Filter).Mask[i][j].green - (*Filter).Mask[i][j].red  > (*Tool).rgb_distance) {
			if ((*Filter).Mask[i][j].green - (*Filter).Mask[i][j].blue > (*Tool).rgb_distance) {
				(*Filter).ServiceMask[i][j].Picked = true;
			}}}
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_MaskUpdate_PickBlue(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			if ((*Filter).Mask[i][j].blue > (*Tool).Scale) {
			if ((*Filter).Mask[i][j].blue - (*Filter).Mask[i][j].red   > (*Tool).rgb_distance) {
			if ((*Filter).Mask[i][j].blue - (*Filter).Mask[i][j].green > (*Tool).rgb_distance) {
				(*Filter).ServiceMask[i][j].Picked = true;
			}}}
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_MaskUpdate_PickGray(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			if (abs((*Filter).Mask[i][j].blue - (*Filter).Mask[i][j].red)   < (*Tool).rgb_distance) {
			if (abs((*Filter).Mask[i][j].blue - (*Filter).Mask[i][j].green) < (*Tool).rgb_distance) {
			if (abs((*Filter).Mask[i][j].red  - (*Filter).Mask[i][j].green) < (*Tool).rgb_distance) {
				(*Filter).ServiceMask[i][j].Picked = true;
			}}}
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_MaskUpdate_PickWhite(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			if ((*Filter).Mask[i][j].red   + (*Tool).rgb_distance > 255.0) {
			if ((*Filter).Mask[i][j].green + (*Tool).rgb_distance > 255.0) {
			if ((*Filter).Mask[i][j].blue  + (*Tool).rgb_distance > 255.0) {
				(*Filter).ServiceMask[i][j].Picked = true;
			}}}
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_MaskUpdate_PickBlack(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	for (i = 0; i < (*Filter).width;  i++) {
	for (j = 0; j < (*Filter).height; j++) {
		if (!(*Filter).Mask[i][j].Picked)
		{
			if ((*Filter).Mask[i][j].red   < (*Tool).rgb_distance) {
			if ((*Filter).Mask[i][j].green < (*Tool).rgb_distance) {
			if ((*Filter).Mask[i][j].blue  < (*Tool).rgb_distance) {
				(*Filter).ServiceMask[i][j].Picked = true;
			}}}
		}
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}

// Image Filter

int ImageFilters_GrayScale(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	double Gray;
	
	for (i = 0; i < (*Filter).width; i++)  {
	for (j = 0; j < (*Filter).height; j++) {
		Gray = ((*Filter).Mask[i][j].red + (*Filter).Mask[i][j].green + (*Filter).Mask[i][j].blue) * _CONST_1_3;
		(*Filter).ServiceMask[i][j].red   = Gray;
		(*Filter).ServiceMask[i][j].green = Gray;
		(*Filter).ServiceMask[i][j].blue  = Gray;
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_ColorGradient_Scale(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	double dr, dg, db;
	
	for (i = 0; i < (*Filter).width; i++)  {
	for (j = 0; j < (*Filter).height; j++) {
		dr = (*Filter).Mask[i][j].red   - 0.50 * ((*Filter).Mask[i][j].green + (*Filter).Mask[i][j].blue);
		dg = (*Filter).Mask[i][j].green - 0.50 * ((*Filter).Mask[i][j].red   + (*Filter).Mask[i][j].blue);
		db = (*Filter).Mask[i][j].blue  - 0.50 * ((*Filter).Mask[i][j].red   + (*Filter).Mask[i][j].green);

		//dd = (abs(dr) + abs(dg) + abs(db)) * _CONST_1_3;
		if      ((dr >= dg) && (dr >= db)) { dg = -dr; db = -dr; }
		else if ((dg >= dr) && (dg >= db)) { dr = -dg; db = -dg; }
		else if ((db >= dr) && (db >= dg)) { dr = -db; dg = -db; }

		(*Filter).ServiceMask[i][j].red   = (*Filter).Mask[i][j].red   + (*Tool).Scale * dr;// (dr - dd);
		(*Filter).ServiceMask[i][j].green = (*Filter).Mask[i][j].green + (*Tool).Scale * dg;// (dg - dd);
		(*Filter).ServiceMask[i][j].blue  = (*Filter).Mask[i][j].blue  + (*Tool).Scale * db;// (db - dd);
		
		if ((*Filter).ServiceMask[i][j].red   < 0.0) (*Filter).ServiceMask[i][j].red   = 0.0; if ((*Filter).ServiceMask[i][j].red   > 255.0) (*Filter).ServiceMask[i][j].red   = 255.0;
		if ((*Filter).ServiceMask[i][j].green < 0.0) (*Filter).ServiceMask[i][j].green = 0.0; if ((*Filter).ServiceMask[i][j].green > 255.0) (*Filter).ServiceMask[i][j].green = 255.0;
		if ((*Filter).ServiceMask[i][j].blue  < 0.0) (*Filter).ServiceMask[i][j].blue  = 0.0; if ((*Filter).ServiceMask[i][j].blue  > 255.0) (*Filter).ServiceMask[i][j].blue  = 255.0;
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_ColorShift_Scale(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	double dr, dg, db;
	
	for (i = 0; i < (*Filter).width; i++)  {
	for (j = 0; j < (*Filter).height; j++) {
		dr = (*Filter).Mask[i][j].red   - ((*Tool).rgb_delta[1] * (*Filter).Mask[i][j].green + (*Tool).rgb_delta[2] * (*Filter).Mask[i][j].blue)  / ((*Tool).rgb_delta[1] + (*Tool).rgb_delta[2]);
		dg = (*Filter).Mask[i][j].green - ((*Tool).rgb_delta[0] * (*Filter).Mask[i][j].red   + (*Tool).rgb_delta[2] * (*Filter).Mask[i][j].blue)  / ((*Tool).rgb_delta[0] + (*Tool).rgb_delta[2]);
		db = (*Filter).Mask[i][j].blue  - ((*Tool).rgb_delta[0] * (*Filter).Mask[i][j].red   + (*Tool).rgb_delta[1] * (*Filter).Mask[i][j].green) / ((*Tool).rgb_delta[0] + (*Tool).rgb_delta[1]);

		//dd = (abs(dr) + abs(dg) + abs(db)) * _CONST_1_3;
		//if      ((dr >= dg) && (dr >= db)) { dg = -dr; db = -dr; }
		//else if ((dg >= dr) && (dg >= db)) { dr = -dg; db = -dg; }
		//else if ((db >= dr) && (db >= dg)) { dr = -db; dg = -db; }

		(*Filter).ServiceMask[i][j].red   = (*Filter).Mask[i][j].red   + (*Tool).Scale * dr;// (dr - dd);
		(*Filter).ServiceMask[i][j].green = (*Filter).Mask[i][j].green + (*Tool).Scale * dg;// (dg - dd);
		(*Filter).ServiceMask[i][j].blue  = (*Filter).Mask[i][j].blue  + (*Tool).Scale * db;// (db - dd);
		
		if ((*Filter).ServiceMask[i][j].red   < 0.0) (*Filter).ServiceMask[i][j].red   = 0.0; if ((*Filter).ServiceMask[i][j].red   > 255.0) (*Filter).ServiceMask[i][j].red   = 255.0;
		if ((*Filter).ServiceMask[i][j].green < 0.0) (*Filter).ServiceMask[i][j].green = 0.0; if ((*Filter).ServiceMask[i][j].green > 255.0) (*Filter).ServiceMask[i][j].green = 255.0;
		if ((*Filter).ServiceMask[i][j].blue  < 0.0) (*Filter).ServiceMask[i][j].blue  = 0.0; if ((*Filter).ServiceMask[i][j].blue  > 255.0) (*Filter).ServiceMask[i][j].blue  = 255.0;
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_ColorShift(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	for (i = 0; i < (*Filter).width; i++)  {
	for (j = 0; j < (*Filter).height; j++) {
		(*Filter).ServiceMask[i][j].red   = (*Filter).Mask[i][j].red   + (*Tool).rgb[0];
		(*Filter).ServiceMask[i][j].green = (*Filter).Mask[i][j].green + (*Tool).rgb[1];
		(*Filter).ServiceMask[i][j].blue  = (*Filter).Mask[i][j].blue  + (*Tool).rgb[2];

		if ((*Filter).ServiceMask[i][j].red   < 0.0) (*Filter).ServiceMask[i][j].red   = 0.0; if ((*Filter).ServiceMask[i][j].red   > 255.0) (*Filter).ServiceMask[i][j].red   = 255.0;
		if ((*Filter).ServiceMask[i][j].green < 0.0) (*Filter).ServiceMask[i][j].green = 0.0; if ((*Filter).ServiceMask[i][j].green > 255.0) (*Filter).ServiceMask[i][j].green = 255.0;
		if ((*Filter).ServiceMask[i][j].blue  < 0.0) (*Filter).ServiceMask[i][j].blue  = 0.0; if ((*Filter).ServiceMask[i][j].blue  > 255.0) (*Filter).ServiceMask[i][j].blue  = 255.0;
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_Posterize(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j;
	
	double Chanel_Width = 255.0 / (*Tool).rgb_distance;
	double Chanel_Width_I = (*Tool).rgb_distance / 255.0;
	
	for (i = 0; i < (*Filter).width; i++)  {
	for (j = 0; j < (*Filter).height; j++) {
		(*Filter).ServiceMask[i][j].red   = Chanel_Width * round(Chanel_Width_I * (*Filter).Mask[i][j].red);
		(*Filter).ServiceMask[i][j].green = Chanel_Width * round(Chanel_Width_I * (*Filter).Mask[i][j].green);
		(*Filter).ServiceMask[i][j].blue  = Chanel_Width * round(Chanel_Width_I * (*Filter).Mask[i][j].blue);
	}}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}

int ImageFilters_BorderEnhance_3(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j, k, l, m, id;
	
	long Extra[2], Rest_Extra;
	long Margin[2][2];
	
	double Kernel[4][9] = { { +1.0, +1.0, +1.0,
							  -2.0, -2.0, -2.0,
							  +1.0, +1.0, +1.0 },
							{ +1.0, -2.0, +1.0,
							  +1.0, -2.0, +1.0,
							  +1.0, -2.0, +1.0 },
							{ +2.0, +1.0, -2.0,
							  +1.0, -2.0, +1.0,
							  -2.0, +1.0, +2.0 },
							{ -2.0, +1.0, +2.0,
							  +1.0, -2.0, +1.0,
							  +2.0, +1.0, -2.0 } };
	
	for (j = 0; j < 9; j++) Kernel[0][j] *= 0.50;
	for (j = 0; j < 9; j++) Kernel[1][j] *= 0.50;
	for (j = 0; j < 9; j++) Kernel[2][j] *= 0.50;
	for (j = 0; j < 9; j++) Kernel[3][j] *= 0.50;
	
	for (i = 0; i < 4; i++) for (j = 0; j < 9; j++) Kernel[i][j] *= (*Tool).Scale * 0.250;
	
	memset((*Filter).ServicePixel, 0, (*Filter).size * sizeof(struct PixelFilter));
	for (i = 0; i < (*Filter).width; i++) { for (j = 0; j < (*Filter).height; j++) { (*Filter).ServiceMask[i][j].Picked = (*Filter).Mask[i][j].Picked; } }
	
	Extra[0]   = (*Filter).width  - 1; if (Extra[0]   < 1) { if (Extra[0]   > 0) Margin[0][1] = Extra[0];   else Margin[0][1] = 0; }
	Rest_Extra = (*Filter).height - 1; if (Rest_Extra < 1) { if (Rest_Extra > 0) Margin[1][1] = Rest_Extra; else Margin[1][1] = 0; }
	
	Margin[0][0] = 0;
	Margin[0][1] = 1;
	for (i = 0; i < (*Filter).width; i++) {
		Extra[1] = Rest_Extra;
		
		Margin[1][0] = 0;
		Margin[1][1] = 1;
		for (j = 0; j < (*Filter).height; j++) {
			//
			(*Filter).ServiceMask[i][j].red   = 0.0;
			(*Filter).ServiceMask[i][j].green = 0.0;
			(*Filter).ServiceMask[i][j].blue  = 0.0;
			//
			for (l = j - Margin[1][0]; l <= j + Margin[1][1]; l++) {
			for (k = i - Margin[0][0]; k <= i + Margin[0][1]; k++) {
				id = (k - i + 1)* 3 + (l - j + 1);
				for (m = 0; m < 4; m++)
				{
					(*Filter).ServiceMask[i][j].red   += Kernel[m][id] * (*Filter).Mask[k][l].red;
					(*Filter).ServiceMask[i][j].green += Kernel[m][id] * (*Filter).Mask[k][l].green;
					(*Filter).ServiceMask[i][j].blue  += Kernel[m][id] * (*Filter).Mask[k][l].blue;
				}
			}}
			//
			(*Filter).ServiceMask[i][j].red   *= (*Filter).Mask[i][j].red;
			(*Filter).ServiceMask[i][j].green *= (*Filter).Mask[i][j].green;
			(*Filter).ServiceMask[i][j].blue  *= (*Filter).Mask[i][j].blue;
			
			(*Filter).ServiceMask[i][j].red   += (*Filter).Mask[i][j].red;
			(*Filter).ServiceMask[i][j].green += (*Filter).Mask[i][j].green;
			(*Filter).ServiceMask[i][j].blue  += (*Filter).Mask[i][j].blue;
			
			if ((*Filter).ServiceMask[i][j].red   < 0.0) (*Filter).ServiceMask[i][j].red   = 0.0; if ((*Filter).ServiceMask[i][j].red   > 255.0) (*Filter).ServiceMask[i][j].red   = 255.0;
			if ((*Filter).ServiceMask[i][j].green < 0.0) (*Filter).ServiceMask[i][j].green = 0.0; if ((*Filter).ServiceMask[i][j].green > 255.0) (*Filter).ServiceMask[i][j].green = 255.0;
			if ((*Filter).ServiceMask[i][j].blue  < 0.0) (*Filter).ServiceMask[i][j].blue  = 0.0; if ((*Filter).ServiceMask[i][j].blue  > 255.0) (*Filter).ServiceMask[i][j].blue  = 255.0;
			
			Extra[1]--;
			if (Margin[1][0] < 1)        Margin[1][0]++;
			if (Margin[1][1] > Extra[1]) Margin[1][1]--;
		}
		Extra[0]--;
		if (Margin[0][0] < 1)        Margin[0][0]++;
		if (Margin[0][1] > Extra[0]) Margin[0][1]--;
	}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}
int ImageFilters_BorderEnhance_5(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)
{
	long i, j, k, l, m, id;
	
	long Extra[2], Rest_Extra;
	long Margin[2][2];
	
	long Kernel_Size = 5;
	long Kernel_Size_2 = (Kernel_Size - 1) / 2;
	
	double Kernel[4][25] = { { +2.0, +2.0, +2.0, +2.0, +2.0,
							   +1.0, +1.0, +1.0, +1.0, +1.0,
							   -6.0, -6.0, -6.0, -6.0, -6.0,
							   +1.0, +1.0, +1.0, +1.0, +1.0,
							   +2.0, +2.0, +2.0, +2.0, +2.0 },
							 { +2.0, +1.0, -6.0, +1.0, +2.0,
							   +2.0, +1.0, -6.0, +1.0, +2.0,
							   +2.0, +1.0, -6.0, +1.0, +2.0,
							   +2.0, +1.0, -6.0, +1.0, +2.0,
							   +2.0, +1.0, -6.0, +1.0, +2.0 },
							 { +4.0, +3.0, +2.0, +1.0, -8.0,
							   +3.0, +2.0, +1.0, -8.0, +1.0,
							   +2.0, +1.0, -8.0, +1.0, +2.0,
							   +1.0, -8.0, +1.0, +2.0, +3.0,
							   -8.0, +1.0, +2.0, +3.0, +4.0 },
							 { -8.0, +1.0, +2.0, +3.0, +4.0,
							   +1.0, -8.0, +1.0, +2.0, +3.0,
							   +2.0, +1.0, -8.0, +1.0, +2.0,
							   +3.0, +2.0, +1.0, -8.0, +1.0,
							   +4.0, +3.0, +2.0, +1.0, -8.0 } };
	
	for (j = 0; j < 25; j++) Kernel[0][j] *= 0.50 * _CONST_1_3;
	for (j = 0; j < 25; j++) Kernel[1][j] *= 0.50 * _CONST_1_3;
	for (j = 0; j < 25; j++) Kernel[2][j] *= 0.1250;
	for (j = 0; j < 25; j++) Kernel[3][j] *= 0.1250;
	
	for (i = 0; i < 4; i++) for (j = 0; j < 25; j++) Kernel[i][j] *= (*Tool).Scale * 0.250;
	
	memset((*Filter).ServicePixel, 0, (*Filter).size * sizeof(struct PixelFilter));
	for (i = 0; i < (*Filter).width; i++) { for (j = 0; j < (*Filter).height; j++) { (*Filter).ServiceMask[i][j].Picked = (*Filter).Mask[i][j].Picked; } }
	
	Extra[0]   = (*Filter).width  - Kernel_Size_2; if (Extra[0]   < Kernel_Size_2) { if (Extra[0]   > 0) Margin[0][1] = Extra[0];   else Margin[0][1] = 0; }
	Rest_Extra = (*Filter).height - Kernel_Size_2; if (Rest_Extra < Kernel_Size_2) { if (Rest_Extra > 0) Margin[1][1] = Rest_Extra; else Margin[1][1] = 0; }
	
	Margin[0][0] = 0;
	Margin[0][1] = Kernel_Size_2;
	for (i = 0; i < (*Filter).width; i++) {
		Extra[1] = Rest_Extra;
		
		Margin[1][0] = 0;
		Margin[1][1] = Kernel_Size_2;
		for (j = 0; j < (*Filter).height; j++) {
			//
			for (l = j - Margin[1][0]; l <= j + Margin[1][1]; l++) {
			for (k = i - Margin[0][0]; k <= i + Margin[0][1]; k++) {
				id = (k - i + Kernel_Size_2)* Kernel_Size + (l - j + Kernel_Size_2);
				for (m = 0; m < 4; m++)
				{
					(*Filter).ServiceMask[i][j].red   += Kernel[m][id] * (*Filter).Mask[k][l].red;
					(*Filter).ServiceMask[i][j].green += Kernel[m][id] * (*Filter).Mask[k][l].green;
					(*Filter).ServiceMask[i][j].blue  += Kernel[m][id] * (*Filter).Mask[k][l].blue;
				}
			}}
			//
			(*Filter).ServiceMask[i][j].red   *= (*Filter).Mask[i][j].red;
			(*Filter).ServiceMask[i][j].green *= (*Filter).Mask[i][j].green;
			(*Filter).ServiceMask[i][j].blue  *= (*Filter).Mask[i][j].blue;
			
			(*Filter).ServiceMask[i][j].red   += (*Filter).Mask[i][j].red;
			(*Filter).ServiceMask[i][j].green += (*Filter).Mask[i][j].green;
			(*Filter).ServiceMask[i][j].blue  += (*Filter).Mask[i][j].blue;
			
			if ((*Filter).ServiceMask[i][j].red   < 0.0) (*Filter).ServiceMask[i][j].red   = 0.0; if ((*Filter).ServiceMask[i][j].red   > 255.0) (*Filter).ServiceMask[i][j].red   = 255.0;
			if ((*Filter).ServiceMask[i][j].green < 0.0) (*Filter).ServiceMask[i][j].green = 0.0; if ((*Filter).ServiceMask[i][j].green > 255.0) (*Filter).ServiceMask[i][j].green = 255.0;
			if ((*Filter).ServiceMask[i][j].blue  < 0.0) (*Filter).ServiceMask[i][j].blue  = 0.0; if ((*Filter).ServiceMask[i][j].blue  > 255.0) (*Filter).ServiceMask[i][j].blue  = 255.0;
			
			Extra[1]--;
			if (Margin[1][0] < Kernel_Size_2) Margin[1][0]++;
			if (Margin[1][1] > Extra[1])      Margin[1][1]--;
		}
		Extra[0]--;
		if (Margin[0][0] < Kernel_Size_2) Margin[0][0]++;
		if (Margin[0][1] > Extra[0])      Margin[0][1]--;
	}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	return 0;
}

int ImageFilters_ImageProcessing(struct MaskFilter* Filter, double scale, double Kernel_Source[], long Kernel_Size)
{
	long i, j, k, l, id;
	
	long Extra[2], Rest_Extra;
	long Margin[2][2];
	
	long Kernel_Size_2 = (Kernel_Size - 1) / 2;
	
	double* Kernel;
	
	Kernel = (double*)calloc(Kernel_Size * Kernel_Size, sizeof(double));
	if (_APP_WARNING_MESSAGE(Kernel, "Utility_ImageFilters.cpp", "ImageFilters_ImageProcessing", _WARNING_MESSAGE_KEY_CALLOC, "\"Kernel\" to", Kernel_Size * Kernel_Size, 0)) { _USER_IO_WAIT(); return 1; }
	
	if (abs(scale) < FLT_EPSILON) return 0;
	
	id = Kernel_Size * Kernel_Size_2 + Kernel_Size_2;
		
	for (i = 0; i < Kernel_Size * Kernel_Size; i++) Kernel[i] = Kernel_Source[i] * scale;
	Kernel[id] = 1.0 + (Kernel_Source[id] - 1.0) * scale;
	
	memset((*Filter).ServicePixel, 0, (*Filter).size * sizeof(struct PixelFilter));
	for (i = 0; i < (*Filter).width; i++) { for (j = 0; j < (*Filter).height; j++) { (*Filter).ServiceMask[i][j].Picked = (*Filter).Mask[i][j].Picked; } }
	
	Extra[0]   = (*Filter).width  - Kernel_Size_2; if (Extra[0]   < Kernel_Size_2) { if (Extra[0]   > 0) Margin[0][1] = Extra[0];   else Margin[0][1] = 0; }
	Rest_Extra = (*Filter).height - Kernel_Size_2; if (Rest_Extra < Kernel_Size_2) { if (Rest_Extra > 0) Margin[1][1] = Rest_Extra; else Margin[1][1] = 0; }
	
	Margin[0][0] = 0;
	Margin[0][1] = Kernel_Size_2;
	for (i = 0; i < (*Filter).width; i++) {
		Extra[1] = Rest_Extra;
		
		Margin[1][0] = 0;
		Margin[1][1] = Kernel_Size_2;
		for (j = 0; j < (*Filter).height; j++) {
			//
			for (l = j - Margin[1][0]; l <= j + Margin[1][1]; l++) {
			for (k = i - Margin[0][0]; k <= i + Margin[0][1]; k++) {
				id = (k - i + Kernel_Size_2)* Kernel_Size + (l - j + Kernel_Size_2);
				(*Filter).ServiceMask[i][j].red   += Kernel[id] * (*Filter).Mask[k][l].red;
				(*Filter).ServiceMask[i][j].green += Kernel[id] * (*Filter).Mask[k][l].green;
				(*Filter).ServiceMask[i][j].blue  += Kernel[id] * (*Filter).Mask[k][l].blue;
			}}
			//
			
			if ((*Filter).ServiceMask[i][j].red   < 0.0) (*Filter).ServiceMask[i][j].red   = 0.0; if ((*Filter).ServiceMask[i][j].red   > 255.0) (*Filter).ServiceMask[i][j].red   = 255.0;
			if ((*Filter).ServiceMask[i][j].green < 0.0) (*Filter).ServiceMask[i][j].green = 0.0; if ((*Filter).ServiceMask[i][j].green > 255.0) (*Filter).ServiceMask[i][j].green = 255.0;
			if ((*Filter).ServiceMask[i][j].blue  < 0.0) (*Filter).ServiceMask[i][j].blue  = 0.0; if ((*Filter).ServiceMask[i][j].blue  > 255.0) (*Filter).ServiceMask[i][j].blue  = 255.0;
			
			Extra[1]--;
			if (Margin[1][0] < Kernel_Size_2) Margin[1][0]++;
			if (Margin[1][1] > Extra[1])      Margin[1][1]--;
		}
		Extra[0]--;
		if (Margin[0][0] < Kernel_Size_2) Margin[0][0]++;
		if (Margin[0][1] > Extra[0])      Margin[0][1]--;
	}
	
	memcpy((*Filter).MaskPixel, (*Filter).ServicePixel, (*Filter).size * sizeof(struct PixelFilter));
	
	free(Kernel);
	
	return 0;
}

double _FILTER_KERNEL_IDENTITY[9]		{ +0.0, +0.0, +0.0,
										  +0.0, +1.0, +0.0,
										  +0.0, +0.0, +0.0 };
double _FILTER_KERNEL_SHARPEN[9]		{ +0.0, -1.0, +0.0,
										  -1.0, +5.0, -1.0,
										  +0.0, -1.0, +0.0 };
double _FILTER_KERNEL_BLUR_BOX_3[9]		{ 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
										  1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
										  1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0};
double _FILTER_KERNEL_BLUR_BOX_5[25]	{ 0.040, 0.040, 0.040, 0.040, 0.040,
										  0.040, 0.040, 0.040, 0.040, 0.040,
										  0.040, 0.040, 0.040, 0.040, 0.040,
										  0.040, 0.040, 0.040, 0.040, 0.040,
										  0.040, 0.040, 0.040, 0.040, 0.040 };
double _FILTER_KERNEL_BLUR_GAUSS_3[9]	{ 0.062500, 0.1250000, 0.062500,
										  0.125000, 0.2500000, 0.125000,
										  0.062500, 0.1250000, 0.062500 };
double _FILTER_KERNEL_BLUR_GAUSS_5[25]	{ -0.003906250, -0.015625000, -0.023437500, -0.015625000, -0.003906250,
										  -0.015625000, -0.062500000, -0.093750000, -0.062500000, -0.015625000,
										  -0.023437500, -0.093750000, +1.859375000, -0.093750000, -0.023437500,
										  -0.015625000, -0.062500000, -0.093750000, -0.062500000, -0.015625000,
										  -0.003906250, -0.015625000, -0.023437500, -0.015625000, -0.003906250 };
double _FILTER_KERNEL_EDGE_3_low[9]		{ +1.0, +0.0, -1.0,
										  +0.0, +0.0, +0.0,
										  -1.0, +0.0, +1.0 };
double _FILTER_KERNEL_EDGE_3_mid[9]		{ +0.0, -1.0, +0.0,
										  -1.0, +4.0, -1.0,
										  +0.0, -1.0, +0.0 };
double _FILTER_KERNEL_EDGE_3_hig[9]		{ -1.0, -1.0, -1.0,
										  -1.0, -8.0, -1.0,
										  -1.0, -1.0, -1.0 };

int ImageFilters_Identity(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)			{ return ImageFilters_ImageProcessing(Filter, (*Tool).Scale, _FILTER_KERNEL_IDENTITY,     3); }
int ImageFilters_Sharpen(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)			{ return ImageFilters_ImageProcessing(Filter, (*Tool).Scale, _FILTER_KERNEL_SHARPEN,      3); }
int ImageFilters_Blure_Box_3(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)		{ return ImageFilters_ImageProcessing(Filter, (*Tool).Scale, _FILTER_KERNEL_BLUR_BOX_3,   3); }
int ImageFilters_Blure_Box_5(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)		{ return ImageFilters_ImageProcessing(Filter, (*Tool).Scale, _FILTER_KERNEL_BLUR_BOX_5,   5); }
int ImageFilters_Blure_Gauss_3(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)	{ return ImageFilters_ImageProcessing(Filter, (*Tool).Scale, _FILTER_KERNEL_BLUR_GAUSS_3, 3); }
int ImageFilters_Blure_Gauss_5(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)	{ return ImageFilters_ImageProcessing(Filter, (*Tool).Scale, _FILTER_KERNEL_BLUR_GAUSS_5, 5); }
int ImageFilters_Edge_3low(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)		{ return ImageFilters_ImageProcessing(Filter, (*Tool).Scale, _FILTER_KERNEL_EDGE_3_low,   3); }
int ImageFilters_Edge_3mid(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)		{ return ImageFilters_ImageProcessing(Filter, (*Tool).Scale, _FILTER_KERNEL_EDGE_3_mid,   3); }
int ImageFilters_Edge_3hig(struct ImageFilters_Setting* Tool, struct MaskFilter* Filter)		{ return ImageFilters_ImageProcessing(Filter, (*Tool).Scale, _FILTER_KERNEL_EDGE_3_hig,   3); }