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

    //cout << gx << " " << gy << " " << rx << " " << ry;
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

    if (isPostprocessing) {
        if (postprocessingType == "--unsharp") {
            res = unsharp(res);
        } else if (postprocessingType == "--gray-world") {
            res = gray_world(res);
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
    Matrix<double> kernel = {{ -0.166666,  -0.666666,  -0.166666},
                             { -0.666666,  4.33333,  -0.666666},
                             {-0.166666, -0.666666, -0.166666}};
    return custom(src_image, kernel);
}



Image gray_world(Image src_image) {

    double sumB = 0;
    double sumG = 0;
    double sumR = 0;
    Image res(src_image.n_rows, src_image.n_cols);
    for(uint i = 0; i < src_image.n_rows; i++) {
        for(uint j = 0; j < src_image.n_cols; j++) {
            long double add = static_cast<long double>(std::get<0>(src_image(i, j))) / (src_image.n_rows * src_image.n_cols);
            sumB += add;
            add = static_cast<long double>(std::get<1>(src_image(i, j))) / (src_image.n_rows * src_image.n_cols);
            sumG += add;
            add = static_cast<long double>(std::get<2>(src_image(i, j))) / (src_image.n_rows * src_image.n_cols);
            sumR += add;
            
        }
    }
    long double S = min((sumB + sumG + sumR) / 3, 255.0);

    for(uint i = 0; i < src_image.n_rows; i++) {
        for(uint j = 0; j < src_image.n_cols; j++) {
            std::get<0>(res(i, j)) = min(static_cast<uint>(255), std::get<0>(src_image(i, j)) * static_cast<int>(S) / static_cast<int>(sumB));
            std::get<1>(res(i, j)) = min(static_cast<uint>(255), std::get<1>(src_image(i, j)) * static_cast<int>(S) / static_cast<int>(sumG));
            std::get<2>(res(i, j)) = min(static_cast<uint>(255), std::get<2>(src_image(i, j)) * static_cast<int>(S) / static_cast<int>(sumR));
            
        }
    }
    return res;
}

Image resize(Image src_image, double scale) {
    return src_image;
}




Image custom(Image src_image, Matrix<double> kernel) {
    // Function custom is useful for making concrete linear filtrations
    // like gaussian or sobel. So, we assume that you implement custom
    // and then implement other filtrations using this function.
    // sobel_x and sobel_y are given as an example.
    double sumK = 0;
    for(uint k = 0; k < kernel.n_rows; k++) {
        for(uint l = 0; l < kernel.n_cols; l++) {
            sumK += kernel(k, l);
        }
    }
    std::cout << 1;
    Image res(src_image.n_rows, src_image.n_cols);
    for(uint i = 0; i < src_image.n_rows - kernel.n_rows + 1; i++) {
        for(uint j = 0; j < src_image.n_cols - kernel.n_cols + 1; j++) {
            double sumB = 0;
            double sumG = 0;
            double sumR = 0;

            for(uint k = 0; k < kernel.n_rows; k++) {
                for(uint l = 0; l < kernel.n_cols; l++) {
                    sumB += kernel(k, l) * std::get<0>(src_image(i + k, j + l));
                    sumG += kernel(k, l) * std::get<1>(src_image(i + k, j + l));
                    sumR += kernel(k, l) * std::get<2>(src_image(i + k, j + l));

                }
            }
            std::get<0>(res(i, j)) = static_cast<uint>(max(0.0, (min(255.0, sumB))));
            std::get<1>(res(i, j)) = static_cast<uint>(max(0.0, (min(255.0, sumG))));
            std::get<2>(res(i, j)) = static_cast<uint>(max(0.0, (min(255.0, sumR))));
        }
    }

    return res;
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
