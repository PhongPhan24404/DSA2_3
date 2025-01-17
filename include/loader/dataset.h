/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataset.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 3:59 PM
 */

#ifndef DATASET_H
#define DATASET_H
#include "tensor/xtensor_lib.h"
using namespace std;

template<typename DType, typename LType>
class DataLabel{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
public:
    DataLabel(xt::xarray<DType> data,  xt::xarray<LType> label):
    data(data), label(label){
    }
    xt::xarray<DType> getData() const{ return data; }
    xt::xarray<LType> getLabel() const{ return label; }
};

template<typename DType, typename LType>
class Batch{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
public:
    Batch() {};
    Batch(xt::xarray<DType> data,  xt::xarray<LType> label):
    data(data), label(label){
    }
    virtual ~Batch(){}
    xt::xarray<DType>& getData(){return data; }
    xt::xarray<LType>& getLabel(){return label; }
};


template<typename DType, typename LType>
class Dataset{
private:
public:
    Dataset(){};
    virtual ~Dataset(){};
    
    virtual int len()=0;
    virtual DataLabel<DType, LType> getitem(int index)=0;
    virtual xt::svector<unsigned long> get_data_shape()=0;
    virtual xt::svector<unsigned long> get_label_shape()=0;
    
};

//////////////////////////////////////////////////////////////////////
template<typename DType, typename LType>
class TensorDataset: public Dataset<DType, LType>{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
    xt::svector<unsigned long> data_shape, label_shape;
    
public:
    /* TensorDataset: 
     * need to initialize:
     * 1. data, label;
     * 2. data_shape, label_shape
    */
    TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label) : data(data), label(label) {
        /* TODO: your code is here for the initialization
         */
        auto d_shape = data.shape();
        auto l_shape = label.shape();
        data_shape = xt::svector<unsigned long>(d_shape.begin(), d_shape.end());
        label_shape = xt::svector<unsigned long>(l_shape.begin(), l_shape.end());
    }
    /* len():
     *  return the size of dimension 0
    */
    int len() {
        /* TODO: your code is here to return the dataset's length
         */
        if (data_shape.size() == 0) return 0;
        return data_shape[0];
    }
    
    /* getitem:
     * return the data item (of type: DataLabel) that is specified by index
     */
    DataLabel<DType, LType> getitem(int index) {
        /* TODO: your code is here
         */
        if (index < 0 || index >= len()) {
            throw out_of_range("Index is out of range!");
        } 

        xt::xarray<DType> data_item = xt::view(data, index);

        if (label_shape.size() == 0) {
            return DataLabel<DType, LType>(data_item, xt::xarray<LType>());
        }

        return DataLabel<DType, LType>(data_item, xt::view(label, index));
    }
    
    xt::svector<unsigned long> get_data_shape() {
        /* TODO: your code is here to return data_shape
         */
        return data_shape;
    }
    xt::svector<unsigned long> get_label_shape() {
        /* TODO: your code is here to return label_shape
         */
        return label_shape;
    }
};


template<typename DType, typename LType>
class ImageFolderDataset : public Dataset<xt::xarray<float>, int> {
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
    xt::svector<unsigned long> data_shape, label_shape;

public:
    ImageFolderDataset(std::string folder_path) 
    {
        
    }

    int len() {
        return data_shape[0];
    }

    DataLabel<xt::xarray<float>, int> getitem(int index) {
        return DataLabel<xt::xarray<float>, int>(xt::view(data, index), xt::view(label, index));
    }

    xt::svector<unsigned long> get_data_shape() {
        return data_shape;
    }

    xt::svector<unsigned long> get_label_shape() {
        return label_shape;
    }
};


#endif /* DATASET_H */

