/*******************************************************************************************************************
Copyright 2017 Dajiang Innovations Technology Co., Ltd (DJI)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files(the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and 
to permit persons to whom the Software is furnished to do so, subject to the following conditions : 

The above copyright notice and this permission notice shall be included in all copies or substantial portions of
the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*******************************************************************************************************************/

#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "define.hpp"
#include "MnistRecognizer.h"
#include "DigitRecognizer.h"
#include "Settings.hpp"
#include "Voter.hpp"

#include <vector>
#include <utility>


class RuneDetector {
public:
	struct Point2fWithIdx {
		cv::Point2f p;
		size_t idx;
		Point2fWithIdx(const cv::Point2f _p, size_t _idx) :p(_p), idx(_idx){}
	};

    typedef enum {RUNE_ORB, RUNE_GRAD, RUNE_CANNY} Methed_Type;
	typedef enum {RUNE_S, RUNE_B} RuneType;

public:
/*
    RuneDetector(int cell_width = 127, int cell_height = 71, bool perspective = false, Methed_Type m_type = RUNE_ORB) : mnistRecognizer("LeNet-model"){
		sudoku_width = cell_width;
		sudoku_height = cell_height;
        use_perspective = perspective;
        type = m_type;
	}
	*/
	RuneDetector(const Settings& settings):runeSetting(settings.runeSetting),lightSetting(settings.lightSetting),digitRecognizer(settings.lightSetting),voteSetting(settings.voteSetting)
	{
		sudoku_width = runeSetting.CellWidth * runeSetting.CellRatio;
		sudoku_height = runeSetting.CellHeight * runeSetting.CellRatio;
		digit_width = runeSetting.DigitWidth * runeSetting.DigitRatio;
		digit_height = runeSetting.DigitHeight * runeSetting.DigitRatio;
		one_height = runeSetting.OneHeight * runeSetting.DigitRatio;
		contour_threshold = runeSetting.ContourThreshold;
		mnist_threshold = runeSetting.MnistThreshold;
        use_perspective = true;
        type = (Methed_Type)runeSetting.RuneSType;
		sudoku_indexs.resize(10);
		digit_results.resize(5);
	}
	cv::RotatedRect getRotateRect(int idx){ return sudoku_rects.at(idx); }
	const cv::RotatedRect & getRotateRect(int idx) const { return sudoku_rects.at(idx); }

	cv::Mat getSudokuImgs(int idx){ return sudoku_imgs.at(idx); }
	const cv::Mat & getSudokuImgs(int idx) const { return sudoku_imgs.at(idx); }
	int getSudokuIndex(int num) {return sudoku_indexs.at(num);}
    /**
     * @brief getTarget
     * @param image
     * @return ret.first is the index of the vector of sudoku rectangle,
     *         ret.second is the index of the obsolute position of target cell
     */
	std::pair<int, int> getTarget(const cv::Mat & image, RuneType runeType);

protected:
	std::pair<int, int> chooseTargetPerspective(const cv::Mat & image, const std::vector<cv::RotatedRect> & sudoku_rects);
    std::pair<int, int> chooseTarget(const cv::Mat & image, const std::vector<cv::RotatedRect> & sudoku_rects);
	std::pair<int, int> chooseMnistTarget(const cv::Mat & image, const std::vector<cv::RotatedRect> & sudoku_rects);

	int findTargetORB(cv::Mat * cells);
    int findTargetEdge(cv::Mat * cells);
    int findTargetCanny(cv::Mat * cells);

	cv::RotatedRect adjustRotatedRect(const cv::RotatedRect & rect);
	bool checkSudoku(const std::vector<std::vector<cv::Point2i> > & contours,std::vector<cv::RotatedRect> & sudoku_rects, RuneType rune_type);
	cv::Mat transformSudokuPerspective(const cv::Mat & image, const std::vector<cv::RotatedRect> & sudoku_rects);

private:
	void getUniqueResult(vector<vector<pair<double, int> > >& results, vector<int>& mnistResult);
	std::vector<cv::RotatedRect> sudoku_rects; // before perspective transform
	std::vector<cv::RotatedRect> digit_rects; // before perspective transform
	std::vector<cv::RotatedRect> one_digit_rects; // before perspective transform
	std::vector<cv::Mat> sudoku_imgs;
	std::vector<int> sudoku_indexs; // use to find the index of img of num
	std::vector<int> digit_results;
	int sudoku_width;	 // pixel
	int sudoku_height;   // pixel
	int digit_width;	 // pixel
	int digit_height;   // pixel
	int one_height; 	//pixel
	int contour_threshold;
	int mnist_threshold;
    bool use_perspective;
	int oneIndex;
	bool oneConfirmed;
    Methed_Type type;
	cv::Mat src;
	MnistRecognizer mnistRecognizer[9]; // 9 for 9 sudoku_img, cannot share due to multi thread
	DigitRecognizer digitRecognizer;
	Settings::RuneSetting runeSetting;
	Settings::LightSetting lightSetting;
	Settings::VoteSetting voteSetting;
};

