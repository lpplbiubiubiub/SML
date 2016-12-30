#ifndef WRITEFEATURE_H
#define WRITEFEATURE_H
#include <vector>
#include <opencv2/core/core.hpp>
#include "EM.h"
#include "MulGauss.h"
using namespace cv;
int DataWrite(const char *fileName, vector<MeanCovWeight> data);
int DataRead(const char *fileName, vector<MeanCovWeight>& data);
#endif