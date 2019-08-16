
// Prototype(for_PC)Dlg.h: 헤더 파일
//

#pragma once
#include<iostream>
#include<dlib/dnn.h>
#include<dlib/data_io.h>
#include<dlib/image_processing.h>
#include<dlib/gui_widgets.h>
#include<dlib/image_io.h>
#include <dlib/opencv.h>
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace std;
using namespace dlib;
using namespace cv;


// CPrototypeforPCDlg 대화 상자
class CPrototypeforPCDlg : public CDialogEx
{
// 생성입니다.
public:
	CPrototypeforPCDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROTOTYPEFOR_PC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//DNN Func
	template <long num_filters, typename SUBNET> using con5d = con<num_filters, 5, 5, 2, 2, SUBNET>;
	template <long num_filters, typename SUBNET> using con5 = con<num_filters, 5, 5, 1, 1, SUBNET>;
	template <typename SUBNET> using downsampler = relu<affine<con5d<32, relu<affine<con5d<32, relu<affine<con5d<16, SUBNET>>>>>>>>>;
	template <typename SUBNET> using rcon5 = relu<affine<con5<45, SUBNET>>>;

	using net_type = loss_mmod<con<1, 9, 9, 1, 1, rcon5<rcon5<rcon5<downsampler<input_rgb_image_pyramid<pyramid_down<6>>>>>>>>;

	//Model
	net_type net;

	const char * modelName = "mmod_dog_hipsterizer.dat";
	//Var
	shape_predictor sp;
	matrix<rgb_alpha_pixel> glasses, mustache;
	rgb_alpha_pixel a;
//	image_window win_wireframe, win_hipster;
	image_window win_hipster;
	array2d<rgb_pixel> gl;
	matrix<rgb_pixel> img;

	void initModel(); 
	void CPrototypeforPCDlg::refreshFrame();
	void hipsterize();
	void imageLoad();
	
	afx_msg void OnBnClickedOk();
	VideoCapture capture;
	Mat frame;
	int m_framePerMs;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_targetImageDir;
//	afx_msg void OnBnClickedFileDlg();
	afx_msg void OnBnClickedCancel();
};
