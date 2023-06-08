
// SimplePsDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SimplePs.h"
#include "SimplePsDlg.h"
#include "afxdialogex.h"
#include "ToolUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 保存的最大修改图片次数
#define MAX_IMAGE_REVISE_SIZE 10
// 用户所有操作
#define OPER_LIGHT            0b00000001
#define OPER_SATUR            0b00000010
#define OPER_VAGUE            0b00000100
#define OPER_MOSAIC           0b00001000

// 所有用户的错误操作
enum
{
	USER_FILE_NOT_OPEN  = 1,
	USER_AREA_ERROR     = 2,
	USER_CANT_UNDO      = 3,
	USER_CANT_RECOVER   = 4,
	USER_OPER_NOT_END   = 5,
};


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSimplePsDlg 对话框



CSimplePsDlg::CSimplePsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLEPS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimplePsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_SHOW, IMG_SHOW);
	DDX_Control(pDX, IDC_INTENSITY, SliderIntensity);
	DDX_Control(pDX, IDC_OPER_LOGS, LogsText);
	DDX_Control(pDX, IDC_INTENSITY_TEXT, IntensityText);
	DDX_Control(pDX, IDC_VAGUE, vagueOperation);
	DDX_Control(pDX, IDC_MOSAIC, mosaicOperation);
	DDX_Control(pDX, IDC_ORIGIN, originOperation);
	DDX_Control(pDX, IDC_LIGHT, lightOperation);
	DDX_Control(pDX, IDC_SATURATION, saturationOperation);
}

BEGIN_MESSAGE_MAP(CSimplePsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN_IMAGE, &CSimplePsDlg::OnClickedOpenImage)
	ON_BN_CLICKED(IDC_SATURATION, &CSimplePsDlg::OnClickedSaturation)
	ON_BN_CLICKED(IDC_LIGHT, &CSimplePsDlg::OnClickedLight)
	ON_BN_CLICKED(IDC_MOSAIC, &CSimplePsDlg::OnClickedMosaic)
	ON_BN_CLICKED(IDC_VAGUE, &CSimplePsDlg::OnClickedVague)
	ON_BN_CLICKED(IDC_ORIGIN, &CSimplePsDlg::OnClickedOrigin)
	ON_BN_CLICKED(IDC_SAVE_IMAGE, &CSimplePsDlg::OnClickedSaveImage)
	ON_BN_CLICKED(IDC_UNDO_OPER, &CSimplePsDlg::OnClickedUndoOper)
	ON_BN_CLICKED(IDC_REDO_OPER, &CSimplePsDlg::OnClickedRedoOper)
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_INTENSITY, &CSimplePsDlg::OnCustomdrawIntensity)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CSimplePsDlg 消息处理程序

BOOL CSimplePsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	ImagePath = _T("");
	ImageName = _T("");
	imgIsOpen = FALSE;
	SliderInit();
	GetDlgItem(IDC_INTENSITY)->ShowWindow(FALSE);
	GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSimplePsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSimplePsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSimplePsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSimplePsDlg::OnClickedOpenImage()
{
	// 所有操作重置
	opers = 0;
	GetDlgItem(IDC_INTENSITY)->ShowWindow(FALSE);
	GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(FALSE);
	
	CFileDialog ImageDlg(TRUE, _T("png"), NULL, 0,
		_T("image Files(*.bmp; *.jpg;*.png)|*.JPG;*.PNG;*.BMP|All Files (*.*) |*.*|"), this);
	if (ImageDlg.DoModal() != IDOK) return;

	ImagePath = ImageDlg.GetPathName();           // 文件路径
	ImageName = ImageDlg.GetFileName();           // 文件名
	

	if (ImagePath == _T("")) return;
	std::string imgPath;
	imgPath = CT2A(ImagePath.GetString());
	
	if (!ImageSrcMat.empty()) {
		ImageSrcMat.release();
	}

	ImageSrcMat = cv::imread(imgPath);
	if (ImageSrcMat.empty()) {
		return;
	}
	orginImg = ImageSrcMat.clone();
	ImageMat2 = ImageSrcMat.clone();
	pushImageRevise(ImageSrcMat);

	imgIsOpen = TRUE; // 图片已显示
	Display(ImageMat2);
}

void CSimplePsDlg::Display(const cv::Mat& mat)
{
	/*
	CImage* mfcImg = nullptr;
	GetDlgItem(IDC_IMAGE_SHOW)->ShowWindow(FALSE);
	GetDlgItem(IDC_IMAGE_SHOW)->ShowWindow(TRUE);
	CRect r;
	IMG_SHOW.GetClientRect(r);
	cv::Size winSize(r.right, r.bottom);
	cv::Mat cvImgTmp(winSize, CV_8UC3);
	if (cvImg.size() != winSize)
	{
		cv::resize(cvImg, cvImgTmp, winSize);
	}
	else
	{
		cvImgTmp = cvImg.clone();
	}
	cv::flip(cvImgTmp, cvImgTmp, 0);
	if (mfcImg)
	{
		mfcImg->ReleaseDC();
		delete mfcImg;
		mfcImg = nullptr;
	}

	mfcImg = new CImage();
	mfcImg->Create(winSize.width, winSize.height, 24);
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 24;
	bitInfo.bmiHeader.biWidth = winSize.width;
	bitInfo.bmiHeader.biHeight = winSize.height;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;
	StretchDIBits(mfcImg->GetDC(), 0, 0,
		winSize.width, winSize.height, 0, 0,
		winSize.width, winSize.height,
		cvImgTmp.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY
	);
	mfcImg->BitBlt(::GetDC(IMG_SHOW.m_hWnd), 0, 0);
	if (mfcImg)
	{
		mfcImg->ReleaseDC();
		delete mfcImg;
		mfcImg = nullptr;
	}
	*/
//	GetDlgItem(IDC_IMAGE_SHOW)->ShowWindow(FALSE);
//	GetDlgItem(IDC_IMAGE_SHOW)->ShowWindow(TRUE);
	CRect rectControl;
	IMG_SHOW.GetClientRect(rectControl);
	CDC* pDc = IMG_SHOW.GetDC();
	int W = rectControl.Width(), H = rectControl.Height();
	IMG_SHOW.SetBitmap(NULL);
	CImage img;
	ToolUtility::Mat2CImage(mat, img);   //将mat转CImage
	//不执行该语句会导致窗口绘制出的图片有很多噪点，严重失真
	SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);
	img.Draw(pDc->m_hDC, 0, 0, W, H, 0, 0, mat.cols, mat.rows);
	IMG_SHOW.ReleaseDC(pDc);
}

void CSimplePsDlg::ErrorMessage(byte errorCode)
{
	switch (errorCode) {
		case USER_FILE_NOT_OPEN:
			AfxMessageBox(_T("非法操作: 未打开文件或文件错误!"));
			break;
		case USER_AREA_ERROR:
			AfxMessageBox(_T("非法操作: 范围错误!"));
			break;
		case USER_CANT_UNDO:
			AfxMessageBox(_T("非法操作: 无法撤回!"));
			break;
		case USER_CANT_RECOVER:
			AfxMessageBox(_T("非法操作: 无法恢复!"));
			break;
		case USER_OPER_NOT_END:
			AfxMessageBox(_T("非法操作: 上一步操作未结束!"));
			break;
		default:
			break;
	}
}

// 滚动条初始化
void CSimplePsDlg::SliderInit()
{
	// 强度设置
	SliderIntensity.SetRange(0, 100);
	SliderIntensity.SetTicFreq(10);
}

void CSimplePsDlg::ShowLogs()
{
	std::string text = "", tmp;
	while (operLogs.size() > MAX_IMAGE_REVISE_SIZE) operLogs.pop();

	int logsSize = int(operLogs.size());
	for (int i = 0; i < logsSize; ++i) {
		tmp = operLogs.front();
		text += tmp+"\r\n";
		operLogs.pop();
		operLogs.push(tmp);
	}
	CString cText;
	cText = text.c_str();
	LogsText.SetWindowTextW(cText);
}

void CSimplePsDlg::LightChange()
{
	cv::Mat& src = ImageSrcMat;
	int height = src.rows;
	int width = src.cols;
	cv::Mat dst;
	dst = Mat::zeros(src.size(), src.type());
	float alpha = (imgIntensity+1)*2/100.0;
	float beta = 10;
	omp_set_num_threads(8);
#pragma omp parallel for
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			if (src.channels() == 3)
			{
				int b = src.at<cv::Vec3b>(row, col)[0];
				int g = src.at<cv::Vec3b>(row, col)[1];
				int r = src.at<cv::Vec3b>(row, col)[2];

				dst.at<cv::Vec3b>(row, col)[0] = cv::saturate_cast<uchar>(b * alpha + beta);
				dst.at<cv::Vec3b>(row, col)[1] = cv::saturate_cast<uchar>(g * alpha + beta);
				dst.at<cv::Vec3b>(row, col)[2] = cv::saturate_cast<uchar>(r * alpha + beta);

			}
			else if (src.channels() == 1)
			{
				float v = src.at<uchar>(row, col);
				dst.at<uchar>(row, col) = cv::saturate_cast<uchar>(v * alpha + beta);
			}
		}
	}
	ImageMat2 = dst.clone();
	dst.release();
	Display(ImageMat2);
}

void CSimplePsDlg::SaturationChange()
{
	auto max3 = [](int a, int b, int c) {
		int ret = a;
		ret = std::max(ret, b);
		ret = std::max(ret, c);
		return ret;
	};
	auto min3 = [](int a, int b, int c) {
		int ret = a;
		ret = std::min(ret, b);
		ret = std::min(ret, c);
		return ret;
	};
	cv::Mat& src = ImageSrcMat;
	float Increment = (imgIntensity-50)*0.05;
	cv::Mat dst = src.clone();
	int row = src.rows;
	int col = src.cols;
	omp_set_num_threads(8);
#pragma omp parallel for
	for (int i = 0; i < row; ++i)
	{
		uchar* t = dst.ptr<uchar>(i);
		uchar* s = src.ptr<uchar>(i);
		for (int j = 0; j < col; ++j)
		{
			uchar b = s[3 * j];
			uchar g = s[3 * j + 1];
			uchar r = s[3 * j + 2];
			float max = max3(r, g, b);
			float min = min3(r, g, b);
			float delta, value;
			float L, S, alpha;
			delta = (max - min) / 255;
			if (delta == 0)
				continue;
			value = (max + min) / 255;
			L = value / 2;
			if (L < 0.5)
				S = delta / value;
			else
				S = delta / (2 - value);
			if (Increment >= 0)
			{
				if ((Increment + S) >= 1)
					alpha = S;
				else
					alpha = 1 - Increment;
				alpha = 1 / alpha - 1;
				t[3 * j + 2] = static_cast<uchar>(r + (r - L * 255) * alpha);
				t[3 * j + 1] = static_cast<uchar>(g + (g - L * 255) * alpha);
				t[3 * j] = static_cast<uchar>(b + (b - L * 255) * alpha);
			}
			else
			{
				alpha = Increment;
				t[3 * j + 2] = static_cast<uchar>(L * 255 + (r - L * 255) * (1 + alpha));
				t[3 * j + 1] = static_cast<uchar>(L * 255 + (g - L * 255) * (1 + alpha));
				t[3 * j] = static_cast<uchar>(L * 255 + (b - L * 255) * (1 + alpha));
			}
		}
	}
	ImageMat2 = dst.clone();
	dst.release();
	Display(ImageMat2);
}

void CSimplePsDlg::VagueChange()
{
	cv::Mat src = ImageSrcMat.clone();
	cv::Mat dst = src.clone();
	int rate = (imgIntensity/2) * 2 + 1;
	cv::GaussianBlur(src, dst, cv::Size(rate, rate), 0, 0);
	ImageMat2 = dst.clone();
	dst.release();
	Display(ImageMat2);
}

void CSimplePsDlg::MosaicChange()
{
	cv::Mat dst = ImageSrcMat.clone();
	int width = dst.rows;	//图片的长度
	int height = dst.cols;	//图片的宽度
	//10*10的像素点进行填充
	int arr = imgIntensity+1;
	// i和j代表了矩形区域的左上角的像素坐标
	omp_set_num_threads(16);
#pragma omp parallel for
	for (int i = 0; i < width; i += arr)
	{
		for (int j = 0; j < height; j += arr)
		{
			// 对矩形区域内的每一个像素值进行遍历
			for (int k = i; k < arr + i && k < width; k++)
			{
				for (int m = j; m < arr + j && m < height; m++) {
					// 在这里进行颜色的修改
					dst.at<cv::Vec3b>(k, m)[0] = dst.at<cv::Vec3b>(i, j)[0];
					dst.at<cv::Vec3b>(k, m)[1] = dst.at<cv::Vec3b>(i, j)[1];
					dst.at<cv::Vec3b>(k, m)[2] = dst.at<cv::Vec3b>(i, j)[2];
				}
			}
		}
	}
	ImageMat2 = dst.clone();
	dst.release();
	Display(ImageMat2);
}

void CSimplePsDlg::pushImageRevise(cv::Mat mat)
{
	ImageReviseList.push(mat.clone());
	if (ImageReviseList.size() > MAX_IMAGE_REVISE_SIZE) {
		ImageReviseList.pop();
	}
}

// 饱和度
void CSimplePsDlg::OnClickedSaturation()
{
	if (opers & ~OPER_SATUR) {
		ErrorMessage(USER_OPER_NOT_END);
		return;
	}
	if (!imgIsOpen) {
		ErrorMessage(USER_FILE_NOT_OPEN);
		return;
	}
	GetDlgItem(IDC_INTENSITY)->ShowWindow(TRUE);
	GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(TRUE);

	if (opers & OPER_SATUR) {
		ImageSrcMat = ImageMat2;
		pushImageRevise(ImageSrcMat);
		operLogs.push("饱和度变化:S * " + std::to_string(imgIntensity + 50) + "%");
		ShowLogs();
//		GetDlgItem(IDC_OPER_LOGS)->ShowWindow(FALSE);
		GetDlgItem(IDC_INTENSITY)->ShowWindow(FALSE);
		GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(FALSE);
		opers ^= OPER_SATUR;
	}
	else {
		SliderIntensity.SetPos(50);
		opers |= OPER_SATUR;
		saturationOperation.SetFocus();
	}
}

// 亮度按钮
void CSimplePsDlg::OnClickedLight()
{
	if (opers & ~OPER_LIGHT) {
		ErrorMessage(USER_OPER_NOT_END);
		return;
	}
	if (!imgIsOpen) {
		ErrorMessage(USER_FILE_NOT_OPEN);
		return;
	}
	GetDlgItem(IDC_INTENSITY)->ShowWindow(TRUE);
	GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(TRUE);

	if (opers & OPER_LIGHT) {
		ImageSrcMat = ImageMat2;
		pushImageRevise(ImageSrcMat);
		operLogs.push("亮度变化\u3000:L * " + std::to_string(imgIntensity + 50) + "%");
		ShowLogs();
		GetDlgItem(IDC_INTENSITY)->ShowWindow(FALSE);
		GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(FALSE);
		opers ^= OPER_LIGHT;
	}
	else {
		SliderIntensity.SetPos(50);
		opers |= OPER_LIGHT;
		lightOperation.SetFocus();
	}
}


void CSimplePsDlg::OnClickedMosaic()
{
	if (opers & ~OPER_MOSAIC) {
		ErrorMessage(USER_OPER_NOT_END);
		return;
	}
	if (!imgIsOpen) {
		ErrorMessage(USER_FILE_NOT_OPEN);
		return;
	}
	GetDlgItem(IDC_INTENSITY)->ShowWindow(TRUE);
	GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(TRUE);

	if (opers & OPER_MOSAIC) {
		ImageSrcMat = ImageMat2;
		pushImageRevise(ImageSrcMat);
		operLogs.push("马赛克程度:    " + std::to_string(imgIntensity / 10 + 1));
		ShowLogs();
		GetDlgItem(IDC_INTENSITY)->ShowWindow(FALSE);
		GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(FALSE);
		opers ^= OPER_MOSAIC;
	}
	else {
		SliderIntensity.SetPos(0);
		opers |= OPER_MOSAIC;
		mosaicOperation.SetFocus();
	}
}

// 模糊按钮
void CSimplePsDlg::OnClickedVague()
{
	if (opers & ~OPER_VAGUE) {
		ErrorMessage(USER_OPER_NOT_END);
		return;
	}
	if (!imgIsOpen) {
		ErrorMessage(USER_FILE_NOT_OPEN);
		return;
	}
	GetDlgItem(IDC_INTENSITY)->ShowWindow(TRUE);
	GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(TRUE);

	if (opers & OPER_VAGUE) {
		ImageSrcMat = ImageMat2;
		pushImageRevise(ImageSrcMat);
		operLogs.push("模糊程度\u3000:V * " + std::to_string((imgIntensity / 2) * 2 + 1));
		ShowLogs();
		GetDlgItem(IDC_INTENSITY)->ShowWindow(FALSE);
		GetDlgItem(IDC_INTENSITY_TEXT)->ShowWindow(FALSE);
		opers ^= OPER_VAGUE;
	}
	else {
		SliderIntensity.SetPos(0);
		opers |= OPER_VAGUE;
		vagueOperation.SetFocus();
	}
}


void CSimplePsDlg::OnClickedOrigin()
{
	if (!imgIsOpen) {
		ErrorMessage(USER_FILE_NOT_OPEN);
		return;
	}
	Display(orginImg);
	operLogs.push("显示原图");
	ShowLogs();
}


void CSimplePsDlg::OnClickedSaveImage()
{
	if (!imgIsOpen) {
		ErrorMessage(USER_FILE_NOT_OPEN);
	}
	CString  strFilter =
		_T("位图文件(*.bmp)|*.bmp|JPEG 图像文件|*.jpg|PNG图像文件 |*.png|其他格式(*.*)| *.* ||");
	CFileDialog dlg(FALSE, _T("png"), _T("Picture01.png"), NULL, strFilter);
	if (dlg.DoModal() != IDOK) return;
	CString strFileName; //文件扩展名
	CString strExtension;
	strFileName = dlg.m_ofn.lpstrFile;

	if (dlg.m_ofn.nFileExtension == 0)
	{
		switch (dlg.m_ofn.nFilterIndex)
		{
		case 1:
			strExtension = "bmp"; break;
		case 2:
			strExtension = "jpg"; break;
		case 3:
			strExtension = "png"; break;
		default:
			break;
		}
		strFileName = strFileName + "." + strExtension;
	}
	std::string str = CT2A(strFileName);
	cv::imwrite(str, ImageSrcMat);
}


void CSimplePsDlg::OnClickedUndoOper()
{
	if (opers & 0b11111111) {
		ErrorMessage(USER_OPER_NOT_END);
		return;
	}
	if (!imgIsOpen) {
		ErrorMessage(USER_FILE_NOT_OPEN);
		return;
	}
	if (ImageReviseList.size() <= 1) { // 图片为空
		ErrorMessage(USER_CANT_UNDO);
		return;
	}
	// 回撤图片
	ImageReviseList.pop();
	ImageSrcMat = ImageReviseList.top().clone();
	// 显示图片
	Display(ImageSrcMat);

	operLogs.push("撤销");
	ShowLogs();
}


void CSimplePsDlg::OnClickedRedoOper()
{
	if (opers & 0b11111111) {
		ErrorMessage(USER_OPER_NOT_END);
		return;
	}
	if (!imgIsOpen) {
		ErrorMessage(USER_FILE_NOT_OPEN);
		return;
	}
	// 清空修改表, 只保留原始图片
	while (ImageReviseList.size() >= 1) ImageReviseList.pop();
	// 显示原始图片
	ImageSrcMat = orginImg.clone();
	ImageMat2 = ImageSrcMat.clone();
	Display(ImageSrcMat);
	// 显示操作
	operLogs.push("还原");
	ShowLogs();
}

void CSimplePsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar != NULL) {
		CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
		if (pSlider->GetDlgCtrlID() == IDC_INTENSITY) { // 对改变强度的滚动条操作
			imgIntensity = SliderIntensity.GetPos();
			switch (opers) {
				case OPER_LIGHT:
					LightChange();
					break;
				case OPER_SATUR:
					SaturationChange();
					break;
				case OPER_VAGUE:
					VagueChange();
					break;
				case OPER_MOSAIC:
					MosaicChange();
					break;
			}
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
