
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
	, m_framePerMs(0)
	, m_targetImageDir(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrototypeforPCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FRAMEPERMS, m_framePerMs);
	DDV_MinMaxInt(pDX, m_framePerMs, 10, 5000);
//	DDX_Text(pDX, IDC_TARGET_IMAGE_DIR, m_targetImageDir);
}

BEGIN_MESSAGE_MAP(CPrototypeforPCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CPrototypeforPCDlg::OnBnClickedOk)
	ON_WM_TIMER()
//	ON_BN_CLICKED(IDC_FILE_DLG, &CPrototypeforPCDlg::OnBnClickedFileDlg)
ON_BN_CLICKED(IDCANCEL, &CPrototypeforPCDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPrototypeforPCDlg 메시지 처리기

BOOL CPrototypeforPCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	if (!capture.open(0)) {
		MessageBox("Camera initialize fail!");
		return 1;
	}
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	initModel();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

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
	UpdateData(TRUE);
	SetTimer(FRAME_TIMER, m_framePerMs, 0);
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
	capture >> frame;
	imshow("Camera1", frame);
	cvtColor(frame, frame, COLOR_BGR2RGB);
	cv_image<rgb_pixel> tmp(frame);
	assign_image(img, tmp);
}
void CPrototypeforPCDlg::hipsterize() {
	auto dets = net(img);
//	win_wireframe.clear_overlay();
//	win_wireframe.set_image(img);
	// 강아지 얼굴의 landmark 찾아와서 선으로 이어주는 작업
	// shape_predictor가 찾아줌
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
//	win_wireframe.add_overlay(lines);
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
/*
void CPrototypeforPCDlg::OnBnClickedFileDlg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static TCHAR BASED_CODE szFilter[] = "이미지 파일(*.BMP) | *.BMP";
	CFileDialog dlg(TRUE, "*.bmp", "image", OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal()) {
		m_targetImageDir = dlg.GetPathName();
		UpdateData(FALSE);
		imageLoad();
	}
}
*/

void CPrototypeforPCDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	KillTimer(FRAME_TIMER);
	CDialogEx::OnCancel();
}
