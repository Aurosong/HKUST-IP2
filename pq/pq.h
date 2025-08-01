#ifndef PQ_H
#define PQ_H

#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <limits>

static const long           sample_nums =  10000;
static const int            kmeans_subCenter_nums = 100;
static const int            kmeans_iter = 100;
static const std::string    srcpath = "data/output.txt";

static std::vector<std::vector<double>>     acceleration_samples;
static std::vector<std::vector<double>>     magneticField_samples;
static std::vector<std::vector<double>>     rawAcceleration_samples;
static std::vector<std::vector<double>>     rawRotationRate_samples;
static std::vector<std::vector<double>>     rotationRate_samples;

std::regex pattern(
    R"("acceleration"\s*:\s*\[\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?)\s*\],\s*)"
    R"("magneticField"\s*:\s*\[\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?)\s*\],\s*)"
    R"("rawAcceleration"\s*:\s*\[\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?)\s*\],\s*)"
    R"("rawRotationRate"\s*:\s*\[\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?)\s*\],\s*)"
    R"("rotationRate"\s*:\s*\[\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?),\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?)\s*\],\s*)"
    R"("ts"\s*:\s*(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?))"
);

void initSampler();
void doSample();
void sample_to_str();
void sample_to_csv();
void doKmeans(std::vector<std::vector<double>> datasets);

#endif