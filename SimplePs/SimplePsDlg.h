
// SimplePsDlg.h: 头文件
//

#pragma once

#include <opencv2/opencv.hpp>
#include <stack>
#include <omp.h>
#include <iostream>

// CSimplePsDlg 对话框
class CSimplePsDlg : public CDialogEx
{
	// 构造
public:
	CSimplePsDlg(CWnd* pParent = nullptr);	// 标准构造函数

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLEPS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON m_hIcon;
	CString ImagePath;
	CString ImageName;
	std::stack<cv::Mat> ImageReviseList;
	std::queue<std::string> operLogs;
	cv::Mat ImageSrcMat;
	cv::Mat orginImg;
	cv::Mat ImageMat2;
	BOOL imgIsOpen;
	byte opers;
	int imgIntensity;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedOpenImage();
	void pushImageRevise(cv::Mat mat);
	void Display(const cv::Mat& cvImg);
	void ErrorMessage(byte errorCode);
	void SliderInit();
	void ShowLogs();
	void LightChange();
	void SaturationChange();
	void VagueChange();
	void MosaicChange();

	CStatic IMG_SHOW;
	afx_msg void OnClickedSaturation();
	CSliderCtrl SliderIntensity;
	CEdit LogsText;
	CStatic IntensityText;
	afx_msg void OnClickedLight();
	afx_msg void OnClickedMosaic();
	afx_msg void OnClickedVague();
	CButton vagueOperation;
	CButton mosaicOperation;
	CButton originOperation;
	CButton lightOperation;
	CButton saturationOperation;
	afx_msg void OnClickedOrigin();
	afx_msg void OnClickedSaveImage();
	afx_msg void OnClickedUndoOper();
	afx_msg void OnClickedRedoOper();
//	afx_msg void OnCustomdrawIntensity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
