#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{

Mat image;
image = imread( "/user/eleves/mschneid/ros/src/ecn_sonar/img2test.png" , 1 );

if(!image.data){
    cout<<"No image data"<<endl;
    return -1;
}
/*
Size ksize(9,9);

Mat gray_image;
cvtColor( image, gray_image, CV_BGR2GRAY );

Mat gray_image_blurred;
blur(gray_image, gray_image_blurred, ksize, Point(-1,-1), BORDER_DEFAULT );

Mat gray_image_gaussian;
GaussianBlur(gray_image, gray_image_gaussian, ksize, 50, 0, BORDER_DEFAULT );

Mat gray_image_median;
medianBlur(gray_image, gray_image_median, 3);

// Bruit joli
Mat noise = Mat(gray_image.size(),CV_64F);
Mat gray_image_noise;
normalize(gray_image, gray_image_noise, 0.0, 2.0, CV_MINMAX, CV_64F);
randn(noise, 0, 0.1);
gray_image_noise = gray_image_noise + noise;

// namedWindow( "Initial", CV_WINDOW_AUTOSIZE );
namedWindow( "Gray", CV_WINDOW_AUTOSIZE );
namedWindow( "Blurred", CV_WINDOW_AUTOSIZE );
namedWindow( "Gaussian", CV_WINDOW_AUTOSIZE );
namedWindow( "Median", CV_WINDOW_AUTOSIZE );
namedWindow( "Noise", CV_WINDOW_AUTOSIZE );

// imshow( "Initial", image );
imshow( "Gray", gray_image );
imshow( "Blurred", gray_image_blurred );
imshow( "Gaussian", gray_image_gaussian );
imshow( "Median", gray_image_median );
imshow("Noise",gray_image_noise);
*/
// Adding noise
int sizey = 200;
float xmin = 25;
float xmax = 150;
Mat noise2 = Mat::zeros(1, sizey, CV_8UC3);
Mat K = Mat::zeros(1, sizey, CV_8UC3);
for (int m = 0 ; m<sizey ; m++){
    if (!(m<xmin || m>xmax)){
        int grey = rand() % 256;
        noise2.col(m)= Scalar(grey, grey, grey);
    }
}

namedWindow( "rline", CV_WINDOW_AUTOSIZE );
imshow("rline",noise2);

waitKey(0);

return 0;
}
