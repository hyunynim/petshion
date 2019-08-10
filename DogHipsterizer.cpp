#include<iostream>
#include<dlib/dnn.h>
#include<dlib/data_io.h>
#include<dlib/image_processing.h>
#include<dlib/gui_widgets.h>
#include<dlib/image_io.h>
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace std;
using namespace dlib;
using namespace cv;

template <long num_filters, typename SUBNET> using con5d = con<num_filters, 5, 5, 2, 2, SUBNET>;
template <long num_filters, typename SUBNET> using con5  = con<num_filters, 5, 5, 1, 1, SUBNET>;

template <typename SUBNET> using downsampler  = relu<affine<con5d<32, relu<affine<con5d<32, relu<affine<con5d<16, SUBNET>>>>>>>>>;
template <typename SUBNET> using rcon5  = relu<affine<con5<45, SUBNET>>>;

using net_type = loss_mmod<con<1, 9, 9, 1, 1, rcon5<rcon5<rcon5<downsampler<input_rgb_image_pyramid<pyramid_down<6>>>>>>>>;

/*
 Func CV2Matrix (2019.08.01.)
 Convert cv matrix to dlib matrix
 des: result of function
*/
void CV2Matrix(matrix<rgb_alpha_pixel> & des) {
	Mat img = imread("001.png", IMREAD_UNCHANGED);
	resize(img, img, Size(des.nc(), des.nr()), 0, 0, 1);\
	auto it = des.begin();
	for (int i = 0; i < img.rows; ++i) {
		for (int j = 0; j < img.cols; ++j) {
			auto cur = img.at<rgb_alpha_pixel>(i, j);
			it->alpha = cur.alpha;
			it->blue = cur.blue;
			it->green = cur.green;
			it->red = cur.red;
			++it;
		}
	}
}
int main(int argc, char** argv) try {
	if (argc < 3) {
		cout << "Call this program like this:" << endl;
		cout << "train_data target_image" << endl;
		return 0;
	}

	// 모델 로딩
	net_type net;

	shape_predictor sp;
	matrix<rgb_alpha_pixel> glasses, mustache;
	rgb_alpha_pixel a;
	deserialize(argv[1]) >> net >> sp >> glasses >> mustache;
	pyramid_up(glasses);
	pyramid_up(mustache);

	/*
	 학습되지 않은 이미지를 합성하기 위해
	 cv로 이미지를 읽어와서 dlib용 행렬로 변환
	*/
	//CV2Matrix(mustache);
	
	image_window win1(glasses);
	image_window win2(mustache);
	image_window win_wireframe, win_hipster;
	array2d<rgb_pixel> gl;

	// Argument로 들어온 image들에 대해 작업(수염 + 안경)
	for (int i = 2; i < argc; ++i) {
		matrix<rgb_pixel> img;
		load_image(img, argv[i]);

		// Image upsampling
		// Face detection acc 올라가는 듯
		pyramid_up(img); 

		auto dets = net(img);
		win_wireframe.clear_overlay();
		win_wireframe.set_image(img);
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

		win_wireframe.add_overlay(lines);
		win_hipster.set_image(img);
		cout << "Hit enter to process the next image." << endl;
		cin.get();
	}
}
catch (std::exception& e) {
	cout << e.what() << endl;
	system("pause");
}
