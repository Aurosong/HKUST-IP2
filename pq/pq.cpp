#ifndef PQ_CPP
#define PQ_CPP

#include "pq.h"

float getVectorDist(std::vector<double> vec1, std::vector<double> vec2) {
    if(vec1.size() != vec2.size()) std::cerr << "Error, different dimension vectors!" << std::endl;

    float dist = 0.0;
    for(int i = 0; i < vec1.size(); i++) {
        float d = std::pow((vec1[i] - vec2[i]), 2);
        dist += d;
    }
    return std::sqrt(dist);
}

void initSampler() {
    acceleration_samples.reserve(sample_nums);
    magneticField_samples.reserve(sample_nums);
    rawAcceleration_samples.reserve(sample_nums);
    rawRotationRate_samples.reserve(sample_nums);
    rotationRate_samples.reserve(sample_nums);
}

void sample_to_str() {
    std::cout << "acceleration samples" << std::endl;
    for (auto data : acceleration_samples) {
        std::cout << "[" << data[0] << ", " << data[1] << ", " << data[2] << "]" << std::endl;
    }

    std::cout << std::endl << "magneticField samples" << std::endl;
    for (auto data : magneticField_samples) {
        std::cout << "[" << data[0] << ", " << data[1] << ", " << data[2] << "]" << std::endl;
    }

    std::cout << std::endl << "rawAcceleration samples" << std::endl;
    for (auto data : rawAcceleration_samples) {
        std::cout << "[" << data[0] << ", " << data[1] << ", " << data[2] << "]" << std::endl;
    }

    std::cout << std::endl << "rawRotationRate samples" << std::endl;
    for (auto data : rawRotationRate_samples) {
        std::cout << "[" << data[0] << ", " << data[1] << ", " << data[2] << "]" << std::endl;
    }

    std::cout << std::endl << "rotationRate samples" << std::endl;
    for (auto data : rotationRate_samples) {
        std::cout << "[" << data[0] << ", " << data[1] << ", " << data[2] << "]" << std::endl;
    }
}

void doSample() {
    std::ifstream   file(srcpath);
    std::string     line;
    int             counter = 0;

    if(!file.is_open()) {
        std::cerr << "Error: Failed to open file " << srcpath << std::endl;
        return;
    }

    while(std::getline(file, line)) {
        auto begin = std::sregex_iterator(line.begin(), line.end(), pattern);
        auto end = std::sregex_iterator();

        for(auto it = begin; it != end; it++) {
            std::smatch match = *it;
            std::vector<double> data;
            data.reserve(3);
            for(int i = 1; i < match.size() - 1; i++) {
                data.push_back(std::stod(match[i]));
                // std::cout << "i: " << i << ", data: " << match[i] << std::endl;
                if(i % 3 == 0) {
                    if((i - 1) / 3 == 0) {
                        acceleration_samples.push_back(data);
                    } else if ((i - 1) / 3 == 1) {
                        magneticField_samples.push_back(data);
                    } else if ((i - 1) / 3 == 2) {
                        rawAcceleration_samples.push_back(data);
                    } else if ((i - 1) / 3 == 3) {
                        rawRotationRate_samples.push_back(data);
                    } else if ((i - 1) / 3 == 4) {
                        rotationRate_samples.push_back(data);
                    }
                    data.clear();
                }
            }
            counter += 1;
            if(counter > sample_nums) break;
        }
        if(counter > sample_nums) break;
    }

    file.close();

    doKmeans(magneticField_samples);

    return;
}

void sample_to_csv() {
    std::string exportPath = "data/csv/";
    std::vector<std::string> files = {"acceleration_samples.csv", "magneticField_samples.csv", "rawAcceleration_samples.csv", "rawRotationRate_samples.csv", "rotationRate_samples.csv"};

    for(int i = 0; i < files.size(); i++) {
        std::string exportFile = exportPath + files[i];
        std::ofstream outfile(exportFile);

        if (!outfile.is_open())  {
            std::cerr << "无法打开文件: " << exportFile << std::endl;
            return;
        }

        outfile << "x,y,z,vector_magnitude\n";

        switch(i) {
            case 0: {
                for (const auto& vec : acceleration_samples) {
                    if (vec.size() != 3) continue;
                    double mag = std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
                    outfile << vec[0] << "," << vec[1] << "," << vec[2] << "," << mag << "\n";
                }
                break;
            }
            case 1: {
                for (const auto& vec : magneticField_samples) {
                    if (vec.size() != 3) continue;
                    double mag = std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
                    outfile << vec[0] << "," << vec[1] << "," << vec[2] << "," << mag << "\n";                }
                break;
            }
            case 2: {
                for (const auto& vec : rawAcceleration_samples) {
                    if (vec.size() != 3) continue;
                    double mag = std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
                    outfile << vec[0] << "," << vec[1] << "," << vec[2] << "," << mag << "\n";                }
                break;
            }
            case 3: {
                for (const auto& vec : rawRotationRate_samples) {
                    if (vec.size() != 3) continue;
                    double mag = std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
                    outfile << vec[0] << "," << vec[1] << "," << vec[2] << "," << mag << "\n";                }
                break;
            }
            case 4: {
                for (const auto& vec : rotationRate_samples) {
                    if (vec.size() != 3) continue;
                    double mag = std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
                    outfile << vec[0] << "," << vec[1] << "," << vec[2] << "," << mag << "\n";                }
                break;
            }
            default: break;
        }
        outfile.close();
    }
}

void kmeans_initCenter(std::vector<std::vector<double>>& datasets, std::vector<std::vector<double>>& centers) {
    std::random_device  rd;
    std::mt19937        gen(rd());

    centers.clear();
    while(centers.size() < kmeans_subCenter_nums) {
        int idx = gen() % datasets.size();
        centers.push_back(datasets[idx]);
    }
}

void kmeans_assignCluster(std::vector<std::vector<double>>& datasets, std::vector<std::vector<double>>& centers, std::vector<int> assign_idx) {
    for(int j = 0; j < datasets.size(); j++) {
        float min_dist = std::numeric_limits<float>::max();
        int   min_idx = -1;
        for(int i = 0; i < centers.size(); i++) {
            float dist = getVectorDist(datasets[j], centers[i]);
            if(dist < min_dist) {
                min_dist = dist;
                min_idx = i;
            }
        }
        assign_idx[j] = min_idx;
    }
}

void kmeans_updateCenter(std::vector<std::vector<double>>& datasets, std::vector<std::vector<double>>& centers, std::vector<int> assign_idx) {
    centers.clear();
    for(int i = 0; i < kmeans_subCenter_nums; i++) {
        std::vector<double> newCenter(datasets[0].size(), 0.0);
        int counter = 0;
        for(int j = 0; j < sample_nums; j++) {
            if(assign_idx[j] == i) {
                for(int k = 0; k < datasets[j].size(); k++) {
                    newCenter[k] += datasets[j][k];
                    counter += 1;
                }
            }
        }

        if(counter != 0) {
            for(auto& num : newCenter) {
                num /= counter;
            }
        } else {
            std::cerr << "K-means Error, no elements assigned in cluster!" << std::endl;
        }
        centers.push_back(newCenter);
    }
}

void kmeans_to_csv(std::vector<std::vector<double>>& datasets, std::vector<std::vector<double>>& centers, std::vector<int> assign_idx) {
    std::string clustered_vectors = "data/pq/cluster.csv";
    std::string clustered_centers = "data/pq/centers.csv";

    std::ofstream datafile(clustered_vectors);
    
    if(!datafile.is_open()) {
        std::cerr << "Error, fail to open " << clustered_vectors << std::endl;
        return;
    }

    for(int i = 0; i < sample_nums; i++) {
        for(int j = 0; j < datasets[i].size(); j++) {
            datafile << datasets[i][j];
            datafile << ", ";
        }
        datafile << assign_idx[i];
        datafile << "\n";
    }
    datafile.close();

    std::ofstream centerfile(clustered_centers);
    if(!centerfile.is_open()) {
        std::cerr << "Error, fail to open " << clustered_centers << std::endl;
        return;
    }

    for(int i = 0; i < centers.size(); i++) {
        for(int j = 0; j < centers[i].size(); j++) {
            centerfile << centers[i][j];
            if(j < centers[i].size() - 1) {
                centerfile << ", ";
            }
        }
        centerfile << "\n";
    }
    centerfile.close();
}

void doKmeans(std::vector<std::vector<double>> datasets) {
    int iter = 0;
    std::vector<std::vector<double>> centers;
    std::vector<int>                 assign_idx;
    centers.reserve(kmeans_subCenter_nums);
    assign_idx.reserve(sample_nums);

    kmeans_initCenter(datasets, centers);
    while(iter < kmeans_iter) {
        kmeans_assignCluster(datasets, centers, assign_idx);
        kmeans_updateCenter(datasets, centers, assign_idx);
    }

    kmeans_to_csv(datasets, centers, assign_idx);
}

int main() {
    initSampler();
    doSample();
    // sample_to_str();
    // sample_to_csv();
    return 0;
}

#endif