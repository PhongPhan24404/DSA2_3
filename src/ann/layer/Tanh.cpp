/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Tanh.cpp
 * Author: ltsach
 * 
 * Created on September 1, 2024, 7:03 PM
 */

#include "layer/Tanh.h"
#include "sformat/fmt_lib.h"
#include "ann/functions.h"

Tanh::Tanh(string name) {
    if(trim(name).size() != 0) m_sName = name;
    else m_sName = "Tanh_" + to_string(++m_unLayer_idx);
}

Tanh::Tanh(const Tanh& orig) {
    m_sName = "Tanh_" + to_string(++m_unLayer_idx);
}

Tanh::~Tanh() {
}

xt::xarray<double> Tanh::forward(xt::xarray<double> X) {
    // Apply the tanh activation function: Y = tanh(X)
    xt::xarray<double> Y = xt::xarray<double>::from_shape(X.shape());
    for (size_t i = 0; i < X.size(); ++i) {
        Y.flat(i) = std::tanh(X.flat(i)); // Element-wise tanh calculation
    }
    
    // Cache the result for backward pass
    m_aCached_Y = Y;
    
    return Y;
}

xt::xarray<double> Tanh::backward(xt::xarray<double> DY) {
    // The derivative of tanh: dY/dX = 1 - tanh(X)^2
    xt::xarray<double> DX = xt::xarray<double>::from_shape(DY.shape());
    
    for (size_t i = 0; i < m_aCached_Y.size(); ++i) {
        double tanh_val = m_aCached_Y.flat(i);
        DX.flat(i) = DY.flat(i) * (1.0 - tanh_val * tanh_val); // Chain rule: DY * dY/dX
    }
    
    return DX;
}

string Tanh::get_desc(){
    string desc = fmt::format("{:<10s}, {:<15s}:",
                    "Tanh", this->getname());
    return desc;
}
