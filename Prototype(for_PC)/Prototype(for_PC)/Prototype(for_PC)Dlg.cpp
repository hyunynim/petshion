﻿
// Prototype(for_PC)Dlg.cpp: 구현 파일
//
#include "stdafx.h"
#include "Prototype(for_PC).h"
#include "Prototype(for_PC)Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define TIMER_START 100000
#define FRAME_TIMER (TIMER_START + 1)

// CPrototypeforPCDlg 대화 상자



CPrototypeforPCDlg::CPrototypeforPCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROTOTYPEFOR_PC_DIALOG, pParent)
	, m_framePerMs(1)
	, m_targetImageDir(_T(""))
	, m_sourceFilePath(_T(""))
	, m_sourceCamera(FALSE)
	, m_sourceImage(FALSE)
	, m_sourceVideo(FALSE)
	, hwndDesktop(::GetDesktopWindow())
	, m_posX(10)
	, m_posY(200)
	, m_sizeX(900)
	, m_sizeY(500)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrototypeforPCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FRAMEPERMS, m_framePerMs);
	DDV_MinMaxInt(pDX, m_framePerMs, 1, 15000);
	//	DDX_Text(pDX, IDC_TARGET_IMAGE_DIR, m_targetImageDir);
	DDX_Control(pDX, IDC_GLASSES_LIST, m_glassesList);
	DDX_Text(pDX, IDC_SOURCE_FILE_PATH, m_sourceFilePath);
	DDX_Radio(pDX, IDC_SOURCE_CAMERA, m_sourceCamera);
	DDX_Radio(pDX, IDC_SOURCE_IMAGE, m_sourceImage);
	DDX_Radio(pDX, IDC_SOURCE_VIDEO, m_sourceVideo);
	DDX_Text(pDX, IDC_DESKTOP_POS_X, m_posX);
	DDX_Text(pDX, IDC_DESKTOP_POS_Y, m_posY);
	DDX_Text(pDX, IDC_DESKTOP_SIZE_X, m_sizeX);
	DDX_Text(pDX, IDC_DESKTOP_SIZE_Y, m_sizeY);
}

BEGIN_MESSAGE_MAP(CPrototypeforPCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CPrototypeforPCDlg::OnBnClickedOk)
	ON_WM_TIMER()
ON_BN_CLICKED(IDCANCEL, &CPrototypeforPCDlg::OnBnClickedCancel)
ON_BN_CLICKED(ID_GLASSES_PREVIEW, &CPrototypeforPCDlg::OnBnClickedGlassesPreview)
ON_BN_CLICKED(ID_GLASSES_SET, &CPrototypeforPCDlg::OnBnClickedGlassesSet)
ON_BN_CLICKED(ID_GLASSES_SET_DEFAULT, &CPrototypeforPCDlg::OnBnClickedGlassesSetDefault)
ON_BN_CLICKED(IDC_SOURCE_FILE_DLG, &CPrototypeforPCDlg::OnBnClickedSourceFileDlg)
ON_BN_CLICKED(IDC_EXIT, &CPrototypeforPCDlg::OnBnClickedExit)
ON_BN_CLICKED(IDC_DESKTOP_WINDOW_APPLY, &CPrototypeforPCDlg::OnBnClickedDesktopWindowApply)
ON_BN_CLICKED(IDC_SOURCE_CAMERA, &CPrototypeforPCDlg::OnBnClickedSourceCamera)
ON_BN_CLICKED(IDC_SOURCE_IMAGE, &CPrototypeforPCDlg::OnBnClickedSourceImage)
ON_BN_CLICKED(IDC_SOURCE_VIDEO, &CPrototypeforPCDlg::OnBnClickedSourceVideo)
END_MESSAGE_MAP()


// CPrototypeforPCDlg 메시지 처리기

BOOL CPrototypeforPCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	initModel();
	initGlassesList();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.
void CPrototypeforPCDlg::cameraInitialize() {
	if (!capture.open(0)) {
		MessageBox("Camera initialize fail!");
		KillTimer(FRAME_TIMER);
		return;
	}
	imageInit = videoInit = 0;
	cameraInit = 1;
}
void CPrototypeforPCDlg::videoInitialize() {
	UpdateData(TRUE);
	char path[1010];
	sprintf(path, "%s", m_sourceFilePath);
	if (!capture.open(path)) {
		MessageBox("Video initialize fail!");
		KillTimer(FRAME_TIMER);
		return;
	}
	imageInit = cameraInit = 0;
	videoInit = 1;
}
void CPrototypeforPCDlg::imageInitialize() {
	UpdateData(TRUE);
	char path[1010];
	sprintf(path, "%s", m_sourceFilePath);
	frame = imread(path);
	if (frame.empty()) {
		MessageBox("Image initialize fail!");
		KillTimer(FRAME_TIMER);
		return;
	}
	cameraInit = videoInit = 0;
	imageInit = 1;
}
void CPrototypeforPCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPrototypeforPCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPrototypeforPCDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	UpdateData(TRUE);
	SetTimer(FRAME_TIMER, m_framePerMs, 0);
	if (m_sourceImage)
		KillTimer(FRAME_TIMER);
//	CDialogEx::OnOK();
}


void CPrototypeforPCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent) {
	case FRAME_TIMER:		//프레임 갱신시간 
		refreshFrame();
		hipsterize();
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CPrototypeforPCDlg::refreshFrame() {
	if (m_sourceCamera && !cameraInit)
		cameraInitialize();
	if (m_sourceVideo && !videoInit)
		videoInitialize();
	if (m_sourceImage && !imageInit)
		imageInitialize();

	if (m_sourceCamera || m_sourceVideo)
		capture >> frame;
	else if (m_sourceDesktopWindow) {
		Mat tmp = hwnd2mat(hwndDesktop);
		frame = tmp(Range(m_posY, m_posY + m_sizeY), Range(m_posX, m_posX + m_sizeX));
		tmp.release();
	}
	imshow("Source", frame);
	cvtColor(frame, frame, COLOR_BGR2RGB);
	cv_image<rgb_pixel> tmp(frame);
	assign_image(img, tmp);
}
void CPrototypeforPCDlg::hipsterize() {
	auto dets = net(img);
	// 강아지 얼굴의 landmark 찾아와서 선으로 이어주는 작업
	// shape_predictor가 찾아줌
	win_wireframe.clear_overlay();
	win_wireframe.set_image(img);
	std::vector<image_window::overlay_line> lines;
	for (auto&& d : dets) {
		// Landmark 찾기
		auto shape = sp(img, d.rect);

		const rgb_pixel color(0, 255, 0);
		auto top = shape.part(0);
		auto lear = shape.part(1);
		auto leye = shape.part(2);
		auto nose = shape.part(3);
		auto rear = shape.part(4);
		auto reye = shape.part(5);

		// 수염의 양쪽 끝 위치 계산
		auto lmustache = 1.3 * (leye - reye) / 2 + nose;
		auto rmustache = 1.3 * (reye - leye) / 2 + nose;

		// 이미지 상에 안경 그리기
		std::vector<point> from = { 2 * point(176,36), 2 * point(59,35) }, to = { leye, reye };
		auto tform = find_similarity_transform(from, to);
		for (long r = 0; r < glasses.nr(); ++r) {
			for (long c = 0; c < glasses.nc(); ++c) {
				point p = tform(point(c, r));
				if (get_rect(img).contains(p))
					assign_pixel(img(p.y(), p.x()), glasses(r, c));
			}
		}

		// 이미지 상에 수염 그리기 >> 코 우측 하단
		auto mrect = get_rect(mustache);
		from = { mrect.tl_corner(), mrect.tr_corner() };
		to = { rmustache, lmustache };
		tform = find_similarity_transform(from, to);
		for (long r = 0; r < mustache.nr(); ++r) {
			for (long c = 0; c < mustache.nc(); ++c) {
				point p = tform(point(c, r));
				if (get_rect(img).contains(p))
					assign_pixel(img(p.y(), p.x()), mustache(r, c));
			}
		}

		// Landmark lines
		lines.push_back(image_window::overlay_line(leye, nose, color));
		lines.push_back(image_window::overlay_line(nose, reye, color));
		lines.push_back(image_window::overlay_line(reye, leye, color));
		lines.push_back(image_window::overlay_line(reye, rear, color));
		lines.push_back(image_window::overlay_line(rear, top, color));
		lines.push_back(image_window::overlay_line(top, lear, color));
		lines.push_back(image_window::overlay_line(lear, leye, color));
		}
	win_wireframe.add_overlay(lines);
	win_hipster.set_image(img);
}
void CPrototypeforPCDlg::imageLoad() {
	img.set_size(frame.rows, frame.cols);
	auto it = img.begin();
	for (int i = 0; i < frame.rows; ++i) {
		for (int j = 0; j < frame.cols; ++j) {
			auto cur = frame.at<rgb_pixel>(i, j);
			it->blue = cur.blue;
			it->red = cur.red;
			it->green = cur.green;
		}
	}
	pyramid_up(img);
}

void CPrototypeforPCDlg::initModel() {
	deserialize(modelName) >> net >> sp >> glasses >> mustache;
	pyramid_up(glasses);
	pyramid_up(mustache);
}

void CPrototypeforPCDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	KillTimer(FRAME_TIMER);
	capture.release();
//	CDialogEx::OnCancel();
}

void CPrototypeforPCDlg::initGlassesList() {
	char name[1010];
	CRect r;
	m_glassesList.GetWindowRect(&r);
	m_glassesList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_glassesList.InsertColumn(0, "파일명", r.Width());
	system("cd ");
	system("dir glasses /b >> ./glasses/list.txt");
	FILE * fp = freopen("./glasses/list.txt", "r", stdin);
	while (~scanf("%s", name)) {
		glassesImage.push_back(name);
		if (glassesImage.size() && glassesImage.back().back() != 'g')
			glassesImage.pop_back();
		else
			m_glassesList.InsertItem(glassesImage.size() - 1, name);
	}
	fclose(fp);
	system("del .\\glasses\\list.txt");
	UpdateData(FALSE);
}

void CPrototypeforPCDlg::OnBnClickedGlassesPreview()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto cur = m_glassesList.GetFirstSelectedItemPosition();
	int idx = m_glassesList.GetNextSelectedItem(cur);
	char path[1010];
	sprintf(path, "./glasses/%s", glassesImage[idx].c_str());
	Mat tmp = imread(path, IMREAD_UNCHANGED);
	imshow("Glasses Preview", tmp);
	waitKey(0);
	tmp.release();
}


void CPrototypeforPCDlg::OnBnClickedGlassesSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto cur = m_glassesList.GetFirstSelectedItemPosition();
	int idx = m_glassesList.GetNextSelectedItem(cur);
	char path[1010];
	sprintf(path, "./glasses/%s", glassesImage[idx].c_str());
	Mat tmp = imread(path, IMREAD_UNCHANGED);
	cvtColor(tmp, tmp, COLOR_BGR2RGBA);
	cv_image<rgb_alpha_pixel> res(tmp);
	assign_image(glasses, res);
	tmp.release();
}


void CPrototypeforPCDlg::OnBnClickedGlassesSetDefault()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	initModel();
}


void CPrototypeforPCDlg::OnBnClickedSourceFileDlg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	static TCHAR BASED_CODE szFilter[] = "이미지 파일(*.BMP) | *.BMP;*.bmp |모든파일(*.*)|*.*||";
	CFileDialog dlg(TRUE, "*.png", "image", OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal()) {
		m_sourceFilePath = dlg.GetPathName();
		UpdateData(FALSE);
	}
}


void CPrototypeforPCDlg::OnBnClickedExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	exit(0);
}

Mat CPrototypeforPCDlg::hwnd2mat(HWND hwnd) {
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = ::GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	::GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)& bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	::ReleaseDC(hwnd, hwindowDC);

	return src;
}

void CPrototypeforPCDlg::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CPrototypeforPCDlg::OnBnClickedDesktopWindowApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_sourceDesktopWindow = 1;
	m_sourceImage = m_sourceCamera = m_sourceVideo = 0;
	UpdateData(FALSE);
}


void CPrototypeforPCDlg::OnBnClickedSourceCamera()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sourceCamera = TRUE;
	m_sourceDesktopWindow = FALSE;
	m_sourceImage = FALSE;
	m_sourceVideo = FALSE;
	UpdateData(FALSE);
}


void CPrototypeforPCDlg::OnBnClickedSourceImage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sourceCamera = 0;
	m_sourceDesktopWindow = 0;
	m_sourceImage = 1;
	m_sourceVideo = 0;
	UpdateData(FALSE);
}


void CPrototypeforPCDlg::OnBnClickedSourceVideo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sourceCamera = 0;
	m_sourceDesktopWindow = 0;
	m_sourceImage = 0;
	m_sourceVideo = 1;
	UpdateData(FALSE);
}
