/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "tensor/xtensor_lib.h"
#include "loader/dataset.h"
#include "list/listheader.h"

using namespace std;

template<typename DType, typename LType>
class DataLoader{
public:
    
private:
    DLinkedList<Batch<DType, LType>> batches;
    Dataset<DType, LType>* ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    int m_seed;
    /*TODO: add more member variables to support the iteration*/
public:
    DataLoader(Dataset<DType, LType>* ptr_dataset,
            int batch_size,
            bool shuffle=true,
            bool drop_last=false,
            int seed=-1) : ptr_dataset(ptr_dataset), batch_size(batch_size), shuffle(shuffle), drop_last(drop_last), m_seed(seed) {
        /*TODO: Add your code to do the initialization */

        int num_samples = ptr_dataset->len();
        xt::xarray <int> indices = xt::xarray<int>::from_shape({(unsigned long)num_samples});
        for (int i = 0; i < num_samples; i++) indices[i] = i;
        if (shuffle) {
            if (m_seed >= 0) {
                xt::random::seed(m_seed);
            }
            xt::random::shuffle(indices);
        }

        int num_batches = (num_samples / batch_size);

        for (int i = 0; i < num_batches; i++) {
            int start = i * batch_size;
            int end = -1;
            if (!drop_last && i == num_batches - 1) {
                end = num_samples;
            } else {
                end = start + batch_size;
            }
            int batch_size = end - start;
            
            xt::svector<unsigned long> data_shape = ptr_dataset->get_data_shape();
            xt::svector<unsigned long> label_shape = ptr_dataset->get_label_shape();
            data_shape[0] = batch_size;
            label_shape[0] = batch_size;

            xt::xarray<DType> data = xt::xarray<DType>::from_shape(data_shape);
            xt::xarray<LType> label;
            if (label_shape.size()) label  = xt::xarray<LType>::from_shape(label_shape);

            for (int j = start; j < end; j++) {
                DataLabel<DType, LType> item = ptr_dataset->getitem(indices[j]);
                xt::view(data, j - start) = item.getData();
                if (label_shape.size()) {
                    xt::view(label, j - start) = item.getLabel();
                }
            }
            batches.add(Batch<DType, LType>(data, label));
        }
    }
    virtual ~DataLoader(){}

    //New method: from V2: begin
    int get_batch_size(){ return batch_size; }
    int get_sample_count(){ return ptr_dataset->len(); }
    int get_total_batch(){return int(ptr_dataset->len()/batch_size); }
    
    //New method: from V2: end
    
    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// START: Section                                                     //
    /////////////////////////////////////////////////////////////////////////
    
    /*TODO: Add your code here to support iteration on batch*/
    class Iterator
    {
    private:
        int cursor;
        DataLoader<DType, LType> *pLoader;

    public:
        Iterator(DataLoader<DType, LType> *pLoader = 0, int index = 0)
        {
            this->pLoader = pLoader;
            this->cursor = index;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            cursor = iterator.cursor;
            pLoader = iterator.pLoader;
            return *this;
        }

        Batch<DType, LType> &operator*()
        {
            return pLoader->batches.get(cursor);
        }

        bool operator!=(const Iterator &iterator)
        {
            return pLoader == iterator.pLoader && cursor != iterator.cursor;
        }

        // Prefix ++ overload
        Iterator &operator++()
        {
            this->cursor++;
            return *this;
        }

        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }

        // Prefix -- overload
        Iterator &operator--()
        {
            this->cursor--;
            return *this;
        }

        // Postfix -- overload
        Iterator operator--(int)
        {
            Iterator iterator = *this;
            --*this;
            return iterator;
        }
    };

    Iterator begin()
    {
        return Iterator(this, 0);
    }

    Iterator end()
    {
        return Iterator(this, batches.size());
    }

    Iterator bbegin()
    {
        return Iterator(this, batches.size() - 1);
    }

    Iterator bend()
    {
        return Iterator(this, -1);
    }

    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// END: Section                                                       //
    /////////////////////////////////////////////////////////////////////////
};


#endif /* DATALOADER_H */

