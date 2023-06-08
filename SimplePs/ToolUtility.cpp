#include "pch.h"
#include "ToolUtility.h"
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/hal/intrin.hpp>
#include <opencv2/imgproc/types_c.h>
using cv::GaussianBlur;

void ToolUtility::Mat2CImage(const Mat& mat, CImage& cImage)
{
	//create new CImage  
	int width = mat.cols;
	int height = mat.rows;
	int channels = mat.channels();

	cImage.Destroy(); //clear  
	cImage.Create(width, height, 8 * channels); //默认图像像素单通道占用1个字节  

	//copy values  
	const uchar* ps;
	uchar* pimg = (uchar*)cImage.GetBits(); //A pointer to the bitmap buffer  
	int step = cImage.GetPitch();

	for (int i = 0; i < height; ++i)
	{
		ps = (mat.ptr<uchar>(i));
		for (int j = 0; j < width; ++j)
		{
			if (channels == 1) //gray  
			{
				*(pimg + i * step + j) = ps[j];
			}
			else if (channels == 3) //color  
			{
				for (int k = 0; k < 3; ++k)
				{
					*(pimg + i * step + j * 3 + k) = ps[j * 3 + k];
				}
			}
		}
	}
	/*
	if (0 == mat.total())
	{
		return;
	}

	int nChannels = mat.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = mat.cols;
	int nHeight = mat.rows;


	//重建cimage
	if(!cimage.IsNull()) cimage.Destroy();
	cimage.Create(nWidth, nHeight, 8 * nChannels);


	//拷贝数据


	const uchar* pucRow;						    //指向数据区的行指针
	uchar* pucImage = (uchar*)cimage.GetBits();		//指向数据区的指针
	int nStep = cimage.GetPitch();					//每行的字节数,注意这个返回值有正有负


	if (1 == nChannels)								//对于单通道的图像需要初始化调色板
	{
		int nMaxColors = 256;
		RGBQUAD* rgbquadColorTable = new RGBQUAD[nMaxColors];
		cimage.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		cimage.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}

	omp_set_num_threads(8);
#pragma omp parallel for
	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
				}
			}
		}
	}
	*/
}

Mat ToolUtility::CImage2Mat(CImage cimg)
{
	BITMAP bmp;
	::GetObject(cimg.Detach(), sizeof(BITMAP), &bmp);

	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

	IplImage* img = cvCreateImageHeader(cvSize(bmp.bmWidth, bmp.bmHeight), depth, nChannels);

	img->imageData = (char*)malloc(bmp.bmHeight * bmp.bmWidth * nChannels * sizeof(char));
	memcpy(img->imageData, (char*)(bmp.bmBits), static_cast<size_t>(bmp.bmHeight) * bmp.bmWidth * nChannels);
	cvFlip(img, img, 0);
	return cv::cvarrToMat(static_cast<IplImage*>(img));
}