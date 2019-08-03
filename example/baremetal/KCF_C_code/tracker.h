#include "include.h"


typedef struct pair{
    double x, y; // y for row, x for column
}pair;


typedef struct double_array{
    double** data;
    pair size;
}double_array;

struct object_tracker
{
    int first_frame;
    char name[50];
    double ker_sigma;
    double_array model_alphaf_r;
    double_array model_alphaf_i;
    double_array model_xf_r;
    double_array model_xf_i;
    double_array cos_window;
    double_array yf;
    pair pos;
    pair target_sz;
    pair img_sz;
    char img_path[100];
    
};

void InitArray(double_array* _arr, pair size);
void DeleteArray(double_array* _arr);
void Tracker(struct object_tracker* _target_obj);//, double_array* _cos_window, double_array* _yf);
void GaussianShapedLabel(double sigma, double_array* label, pair tar_size);
void CWindow(double_array* _cos_window);
void ZeroInit(double_array* _arr);
void GetVideo(char im_path[100], double_array* img_arr);
void GetSubwindow(double_array* img_arr, pair pos, pair target_sz, double_array* patch);
void GetFeature(double_array* _patch, double_array* _cos_window);
void CplxConj(double_array* _arr);
void GaussianCorrelation(double_array* _xf_r, double_array* xf_i, double_array* _yf_r, double_array* _yf_i, double sigma, double_array* _kf_r, double_array* _kf_i, pair tar_size);
pair FindMax(double_array* _arr);

void FFT2(double_array* _in_r, double_array* _in_i, double_array* _out_r, double_array* _out_i);
void IFFT2(double_array* _in_r, double_array* _in_i, double_array* _out_r, double_array* _out_i);
void FFT1D(double* t_r, double* t_i, int len, double* f_r, double* f_i);
void IFFT1D(double* f_r, double* f_i, int len, double* t_r, double* t_i);
void P2FFT(double* t_r, double* t_i, int m, int twopm, double* f_r, double* f_i);
int Powerof2(int n,int* m);
int BitReverse(int num, int bit_num);
void TwiddleFactor(double k, double N, double* re, double* im);
void PrintArray(double_array* _arr);
void CheckArray(double_array* _arr, double thres);


//tar 175, 175
//img 240, 320