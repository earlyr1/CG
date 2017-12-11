#include "align.h"
#include <string>

using std::string;
using std::cout;
using std::endl;
using std::pair;

int dx = 20;  //bounds
int dy = 17;

template <class T> const T& max (const T& a, const T& b) {
  return (a<b)?b:a;   
}

template <class T> const T& min (const T& a, const T& b) {
  return (a>b)?b:a;   
}

double I(std::tuple<int, int, int> a) {
    return std::get<2>(a);
}

double MSE(Image srcImage1, Image srcImage2, int x, int y, int S) {

    double sum = 0;
    for(uint i = dx + max(-x, 0); i < srcImage1.n_rows - dx - max(x, 0); i++) {
        for (uint j = dy + max(0, -y); j < srcImage1.n_cols - dy - max(0, y); j++)   {
            sum += (I(srcImage2(i, j)) - I(srcImage1(i + x, j + y)) ) * (I(srcImage2(i, j)) - I(srcImage1(i + x, j + y)));
                    }
    }
    return sum / S;
}  


std::pair<int, int> align2(Image srcImage1, Image srcImage2, int S) {
    double metr = 9999999;
    int x_ans = -16;
    int y_ans = -16;
    for(int x = -15; x < 16; x++) {
        for(int y = -15; y < 16; y++) {
            double mse = MSE(srcImage1, srcImage2, x, y, S);
            if (mse < metr) {
                metr = mse;
                x_ans = x;
                y_ans = y;
            }
        }
    }
    return std::pair<int, int> (x_ans, y_ans);
}

Image align(Image srcImage, bool isPostprocessing, std::string postprocessingType, double fraction, bool isMirror, 
            bool isInterp, bool isSubpixel, double subScale) {

    int third = srcImage.n_rows / 3;
    int S = third * srcImage.n_cols;
    Image B = srcImage.submatrix(0, 0, third, srcImage.n_cols);
    Image G = srcImage.submatrix(third, 0, third, srcImage.n_cols);
    Image R = srcImage.submatrix(2 * third, 0, third, srcImage.n_cols);

    std::pair<int, int> bg = align2(B, G ,S);
    int gx = bg.first;
    int gy = bg.second;

    std::pair<int, int> br = align2(R, G, S);
    int rx = br.first;
    int ry = br.second;

    cout << gx << " " << gy << " " << rx << " " << ry;
    Image res(G.n_rows + 40, srcImage.n_cols + 40);

    for(int i = dx + max(-gx, 0); i < third - dx - max(0, gx); i++) {
        for (uint j = dy + max(-gy, 0); j < G.n_cols - dy - max(0, gy); j++) {
            std::get<2>(res(i, j)) = std::get<2>( B(i+gx, j+gy) );
            std::get<1>(res(i, j)) = std::get<1>( G(i, j) );
        }
    }

    for (int i = dx + max(-rx, 0); i < third - dx - max(0, rx); i++) {
        for(uint j = dy + max(-ry, 0); j < G.n_cols - dy - max(0, ry); j++) {
            std::get<0>(res(i, j)) = std::get<0>(R(i+rx, j+ry) );
        }
    }   

    return res;
}


Image sobel_x(Image src_image) {
    Matrix<double> kernel = {{ -1, 0, 1},
                             { -2, 0, 2},
                             { -1, 0, 1}};
    return custom(src_image, kernel);
}

Image sobel_y(Image src_image) {
    Matrix<double> kernel = {{ 1,  2,  1},
                             { 0,  0,  0},
                             {-1, -2, -1}};
    return custom(src_image, kernel);
}

Image unsharp(Image src_image) {
    return src_image;
}

Image gray_world(Image src_image) {
    return src_image;
}

Image resize(Image src_image, double scale) {
    return src_image;
}

Image custom(Image src_image, Matrix<double> kernel) {
    // Function custom is useful for making concrete linear filtrations
    // like gaussian or sobel. So, we assume that you implement custom
    // and then implement other filtrations using this function.
    // sobel_x and sobel_y are given as an example.
    return src_image;
}

Image autocontrast(Image src_image, double fraction) {
    return src_image;
}

Image gaussian(Image src_image, double sigma, int radius)  {
    return src_image;
}

Image gaussian_separable(Image src_image, double sigma, int radius) {
    return src_image;
}

Image median(Image src_image, int radius) {
    return src_image;
}

Image median_linear(Image src_image, int radius) {
    return src_image;
}

Image median_const(Image src_image, int radius) {
    return src_image;
}

Image canny(Image src_image, int threshold1, int threshold2) {
    return src_image;
}
