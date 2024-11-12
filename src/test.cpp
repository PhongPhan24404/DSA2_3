#include "iostream"
#include "tensor/xtensor_lib.h"

using namespace std;

int main() {
    xt::xarray<double> X = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    xt::xarray<double> W = {
       {1, 2, 3},
       {4, 5, 6}
    };
    
    // xt::xarray<double> b = {1, 2};
    xt::xarray<double> Y = xt::linalg::dot(X, xt::transpose(W));
    cout << "Y: " << Y << endl;
    return 0;
}