#ifndef __NEURAL_NETWORK_HPP__
#define __NEURAL_NETWORK_HPP__

#include <vector>
#include <string>
#include "Matrix.hpp"

class NeuralNetwork {
public:
    NeuralNetwork(std::vector<uint> layers, double (*f)(double), double (*df)(double), double learningRate = 0.01);
    NeuralNetwork(const std::string& file, double (*f)(double), double (*df)(double), double learningRate = 0.01); // load a neural netwrok from a file
    NeuralNetwork(const NeuralNetwork& a, const NeuralNetwork& b); // construct a neural network from a crossover
    NeuralNetwork(const NeuralNetwork& o);
    void operator = (const NeuralNetwork& o);
    ~NeuralNetwork();
    void toFile(const std::string& file) const;
    std::vector<double> guess(const std::vector<double>& inputs) const;
    void train(const std::vector<double>& inputs, const std::vector<double>& outputs);
    double _learningRate;
private:
    double (*_f) (double);
    double (*_df) (double);
    uint _numberOfLayers;
    uint* _layers;
    Matrix* _weights;
    Matrix* _biases;
};

#endif