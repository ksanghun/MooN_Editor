#pragma once
#ifdef YOLODLL_EXPORTS
#if defined(_MSC_VER)
#define YOLODLL_API __declspec(dllexport) 
#else
#define YOLODLL_API __attribute__((visibility("default")))
#endif
#else
#if defined(_MSC_VER)
#define YOLODLL_API __declspec(dllimport) 
#else
#define YOLODLL_API
#endif
#endif

#include <memory>
#include <vector>
#include <deque>
#include <algorithm>


struct next_char {
	unsigned char next;
	float prob;					// confidence - probability that the object was found correctly	
};

class Detector {
	std::shared_ptr<void> detector_gpu_ptr;
	const int cur_gpu_id;
public:
	float nms = .4;
	bool wait_stream;

	YOLODLL_API Detector(std::string cfg_filename, std::string weight_filename, int gpu_id = 0);
	YOLODLL_API ~Detector();

	YOLODLL_API std::vector<next_char> predict_next(unsigned char* str, int len);

	YOLODLL_API int get_net_width() const;
	YOLODLL_API int get_net_height() const;
};

