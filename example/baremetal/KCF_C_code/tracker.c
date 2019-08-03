#include "tracker.h"

void InitArray(double_array* _arr, pair arr_size)
{
    (_arr->size).y = arr_size.y;
    (_arr->size).x = arr_size.x;

    _arr->data = (double **)malloc(arr_size.y * sizeof(double*));

    for(int i = 0; i < arr_size.y; i++)
    {
        _arr->data[i] = (double *)malloc(arr_size.x * sizeof(double));
    }

}

void DeleteArray(double_array* _arr)
{
    for(int i = 0; i < _arr->size.y; i++)
    {
        free(_arr->data[i]);
    }

    free(_arr->data);
}

void Tracker(struct object_tracker* _target_obj)//, double_array* _cos_window, double_array* _yf)
{
    double padding = 1.5;
    double cell_size = 1;
    double lambda = 0.0001;
    double interp_factor = 0.075;

    pair img_size;
    //printf("%f\n", target_size.x);

    img_size.y = 240;
    img_size.x = 320;

    double_array img_arr;
    InitArray(&img_arr, img_size);

    double_array patch, zf_r, zf_i;
    double_array kzf_r, kzf_i;
    double_array zero_arr;
    //double_array alpha_arr;

    InitArray(&patch, _target_obj->target_sz);
    InitArray(&zf_r, _target_obj->target_sz);
    InitArray(&zf_i, _target_obj->target_sz);
    InitArray(&kzf_r, _target_obj->target_sz);
    InitArray(&kzf_i, _target_obj->target_sz);
    InitArray(&zero_arr, _target_obj->target_sz);
    //InitArray(&alpha_arr, _target_obj->target_sz);

    // Parameter Initialization
    ZeroInit(&zero_arr);
    
    // Get video image
    //printf("Get video image!\n");
    GetVideo(_target_obj->img_path, &img_arr);
    
    if(_target_obj->first_frame == 0)
    {
        // Obtain a subwindow for detection at the position from last frame, 
        // and conert to Fourier domain (its size is unchanged)
        GetSubwindow(&img_arr, _target_obj->pos, _target_obj->target_sz, &patch);
        GetFeature(&patch, &(_target_obj->cos_window));
        FFT2(&patch, &zero_arr, &zf_r, &zf_i);
        GaussianCorrelation(&zf_r, &zf_i, &(_target_obj->model_xf_r), &(_target_obj->model_xf_i), _target_obj->ker_sigma, &kzf_r, &kzf_i, _target_obj->target_sz);

        //PrintArray(&(kzf_r));

        // Equation for fast detection
        double_array conv_r, conv_i;
        double_array resp_r, resp_i;

        InitArray(&conv_r, _target_obj->target_sz);
        InitArray(&conv_i, _target_obj->target_sz);
        InitArray(&resp_r, _target_obj->target_sz);
        InitArray(&resp_i, _target_obj->target_sz);

        for(int i = 0; i < (conv_r.size).y; i++)
        {
            for(int j = 0; j <(conv_r.size).x; j = j + 1)
            {
                //(conv_r.data)[i][j] = ((_target_obj->model_alphaf_r).data)[i][j] * (kzf_r.data)[i][j] - ((_target_obj->model_alphaf_i).data)[i][j] * (kzf_i.data)[i][j]; 
                //(conv_i.data)[i][j] = ((_target_obj->model_alphaf_r).data)[i][j] * (kzf_i.data)[i][j] + ((_target_obj->model_alphaf_i).data)[i][j] * (kzf_r.data)[i][j]; 
                (conv_r.data)[i][j] = ((_target_obj->model_alphaf_r).data)[i][j] * (kzf_r.data)[i][j]; 
                (conv_i.data)[i][j] = ((_target_obj->model_alphaf_r).data)[i][j] * (kzf_i.data)[i][j]; 
            }
        }


        IFFT2(&conv_r, &conv_i, &resp_r, &resp_i);
        //response = real(ifft2(model_alphaf .* kzf));  %equation for fast detection  

        // Target location is at the maximum response. we must take into account the fact that, 
        // if the target doesn't move, the peak will appear at the top-left corner, not at the center 
        // (this is discussed in the paper). The responses wrap around cyclically.

        pair resp_pos = FindMax(&resp_r);

        //printf("%f\n", resp_pos.x);
        //[vert_delta, horiz_delta] = find(response == max(response(:)), 1);
        
        // Wrap around to negative half-space of vertical axis
        if(resp_pos.y > zf_r.size.y / 2.0)
        {
            resp_pos.y = resp_pos.y - zf_r.size.y;
        }
        // Same for horizontal axis
        if(resp_pos.x > zf_r.size.x / 2.0)
        {
            resp_pos.x = resp_pos.x - zf_r.size.x;
        }

        (_target_obj->pos).y = (_target_obj->pos).y + cell_size * (resp_pos.y);
        (_target_obj->pos).x = (_target_obj->pos).x + cell_size * (resp_pos.x);

        DeleteArray(&conv_r);
        DeleteArray(&conv_i);
        DeleteArray(&resp_r);
        DeleteArray(&resp_i);
    }

    //printf("Get subwindows and features!\n");
    GetSubwindow(&img_arr, _target_obj->pos, _target_obj->target_sz, &patch);
    DeleteArray(&img_arr);
    GetFeature(&patch, &(_target_obj->cos_window));

    //printf("Get gaussian correlation!\n");
    FFT2(&patch, &zero_arr, &zf_r, &zf_i);
    DeleteArray(&patch);
    DeleteArray(&zero_arr);
    
    GaussianCorrelation(&zf_r, &zf_i, &zf_r, &zf_i, _target_obj->ker_sigma, &kzf_r, &kzf_i, _target_obj->target_sz);


    //printf("Update parameter!\n");
    for(int i = 0; i < (kzf_r.size).y; i++)
    {
        for(int j = 0; j <(kzf_r.size).x; j = j + 1)
        {
            (kzf_r.data)[i][j] = ((_target_obj->yf).data)[i][j] / ((kzf_r.data)[i][j] + lambda); 
        }
    }


    if(_target_obj->first_frame == 1)
    {
        for(int i = 0; i < ((_target_obj->model_alphaf_r).size).y; i++)
        {
            for(int j = 0; j <((_target_obj->model_alphaf_r).size).x; j = j + 1)
            {
                ((_target_obj->model_alphaf_r).data)[i][j] = (kzf_r.data)[i][j]; 
                ((_target_obj->model_xf_r).data)[i][j] = (zf_r.data)[i][j]; 
                ((_target_obj->model_xf_i).data)[i][j] = (zf_i.data)[i][j]; 
            }
        }
    }
    else
    {
        for(int i = 0; i < ((_target_obj->model_alphaf_r).size).y; i++)
        {
            for(int j = 0; j <((_target_obj->model_alphaf_r).size).x; j = j + 1)
            {
                ((_target_obj->model_alphaf_r).data)[i][j] = (1 - interp_factor) * ((_target_obj->model_alphaf_r).data)[i][j] + interp_factor * (kzf_r.data)[i][j]; 
                ((_target_obj->model_xf_r).data)[i][j] = (1 - interp_factor) * ((_target_obj->model_xf_r).data)[i][j] + interp_factor * (zf_r.data)[i][j]; 
                ((_target_obj->model_xf_i).data)[i][j] = (1 - interp_factor) * ((_target_obj->model_xf_i).data)[i][j] + interp_factor * (zf_i.data)[i][j]; 
            }
        }
    }

    DeleteArray(&zf_r);
    DeleteArray(&zf_i);
    DeleteArray(&kzf_r);
    DeleteArray(&kzf_i);
    //DeleteArray(&alpha_arr);

    //PrintArray(&(_target_obj->model_alphaf_r));
}


void GaussianShapedLabel(double sigma, double_array* _label, pair tar_size)
{
    double_array rs, cs;
    double_array tmp_label;
    
    InitArray(&rs, tar_size);
    InitArray(&cs, tar_size);
    InitArray(&tmp_label, tar_size);

    int half_height = floor((_label->size).y / 2);
    int half_width  = floor((_label->size).x / 2);

    for(int i = 0; i < (_label->size).y; i++)
    {
        for(int j = 0; j < (_label->size).x; j++)
        {
            (rs.data)[i][j] = i + 1 - half_height;
            (cs.data)[i][j] = j + 1 - half_width;
        }
    }

    for(int i = 0; i < (_label->size).y; i++)
    {
        for(int j = 0; j < (_label->size).x; j++)
        {
            (tmp_label.data)[i][j] = exp(-0.5 / (sigma * sigma) * ((rs.data)[i][j] * (rs.data)[i][j] + (cs.data)[i][j] * (cs.data)[i][j]));
        }
    }

    DeleteArray(&rs);
    DeleteArray(&cs);
    
    for(int i = 0; i < (_label->size).y; i++)
    {
        for(int j = 0; j < (_label->size).x; j++)
        {
            int tmp_i, tmp_j;
            if(i + half_height - 1 >= (_label->size).y)
            {
                tmp_i = i + half_height - 1 - (_label->size).y;
            }
            else
            {
                tmp_i = i + half_height - 1;
            }
            if(j + half_width - 1 >= (_label->size).x)
            {
                tmp_j = j + half_width - 1 - (_label->size).x;
            }
            else
            {
                tmp_j = j + half_width - 1;
            }
            (_label->data)[i][j] = (tmp_label.data)[tmp_i][tmp_j];
        }
    }

    DeleteArray(&tmp_label);

}

// Create cosine window by hanning window
void CWindow(double_array* _cos_window)
{
    
    for(int i = 0; i < (_cos_window->size).y; i++)
    {
       for(int j = 0; j <(_cos_window->size).x; j = j + 1)
       {
            (_cos_window->data)[i][j] = 0.25 * (1 - cos(2 * PI * i / ((_cos_window->size).y - 1))) * (1 - cos(2 * PI * j / ((_cos_window->size).x - 1)));
       }
    }

    return;
}

// Initialize all the value in the array to zero
void ZeroInit(double_array* _arr)
{
    for(int i = 0; i < (_arr->size).y; i++)
    {
       for(int j = 0; j <(_arr->size).x; j = j + 1)
       {
            (_arr->data)[i][j] = 0.0;
       }
    }

    return;
}

// Get the video image
void GetVideo(char img_path[100], double_array* img_arr)
{
    //char file_name[] = img_path;
    FILE *fp;
    fp = fopen(img_path, "r");
    
    if (!fp) {
        fprintf(stderr, "failed to open file for reading\n");
        return;
    }
    
    
    int idx = 0;
    size_t len = 0;
    char * line = NULL;
    char * element;
    
    size_t lines_size = 240;
    size_t items_size = 320;
    
    //double array[240][320];
 
    while ((getline (&line, &len, fp)) != -1) {
        
        element = strtok(line, ",");
        for(int j=0; j<items_size; j++){
            (img_arr->data)[idx][j] = atof(element);
            element = strtok(NULL, ",");
        }
        idx++;
    }
    
    fclose (fp);
    return;
}

// Get the image window of the target
void GetSubwindow(double_array* img_arr, pair pos, pair target_sz, double_array* patch)
{
    // Give the boundary of the patch
    int y_start, y_end;
    int x_start, x_end;
    y_start = floor(pos.y) - floor(target_sz.y/2) + 1;
    y_end   = floor(pos.y) - floor(target_sz.y/2) + target_sz.y;
    x_start = floor(pos.x) - floor(target_sz.x/2) + 1;
    x_end   = floor(pos.x) - floor(target_sz.x/2) + target_sz.x;

    //printf("%d\t%d\t%d\t%d\n", y_start, y_end, x_start, x_end);

    for(int row = y_start; row <= y_end; row++)
    {
        for(int col = x_start; col <= x_end; col++)
        {
            int i_y, i_x;
            int i, j;

            // Check for out-of-bounds coordinates, and set them to the value at the borders.
            // for row
            if(row < 0)
            {
                i_y = 0;
            }
            else if(row > (img_arr->size).y - 1)
            {
                i_y = (img_arr->size).y - 1;
            }
            else
            {
                i_y = row;
            }
            // for column
            if(col < 0)
            {
                i_x = 0;
            }
            else if(col > (img_arr->size).x - 1)
            {
                i_x = (img_arr->size).x - 1;
            }
            else
            {
                i_x = col;
            }

            // Extract image
            i = row - y_start;
            j = col - x_start;
            (patch->data)[i][j] = (img_arr->data)[i_y][i_x];
        }
    }

    return;
}

// Get the feature of the target by normalization and windowing
void GetFeature(double_array* _patch, double_array* _cos_window)
{

    double total_sum = 0;
    double mean;

    // Normalize
    for(int i = 0; i < (_patch->size).y; i++)
    {
       for(int j = 0; j < (_patch->size).x; j++)
       {
            (_patch->data)[i][j] = (_patch->data)[i][j] / 255.0;
            total_sum += (_patch->data)[i][j];
       }
    }

    // Subtract mean then multiply cosine window
    mean = total_sum / ((_patch->size).y * (_patch->size).x);

    for(int i = 0; i < (_patch->size).y; i++)
    {
       for(int j = 0; j < (_patch->size).x; j++)
       {
            (_patch->data)[i][j] = (_cos_window->data)[i][j] * ((_patch->data)[i][j] - mean);
       }
    }

    return;
}

// 2D Fourier Transform
/*
void FFT2(double_array* _in_r, double_array* _in_i, double_array* _out_r, double_array* _out_i)
{
    printf("Calculate 2D FFT!\n");
    for(int i = 0;i < (_in_r->size).x; i++)
    {
        for(int j = 0; j < (_in_r->size).y; j++)
        {
            double ak = 0; 
            double bk = 0;
            for(int ii = 0; ii < (_in_r->size).x; ii++)
            {
                for(int jj = 0;jj < (_in_r->size).y; jj++)
                {  

                    double x = -2.0 * PI * i * ii / (double)(_in_r->size).x;
                    double y = -2.0 * PI * j * jj / (double)(_in_r->size).y;
                    ak += (_in_r->data)[ii][jj] * cos(x+y) - (_in_i->data)[ii][jj] * 1.0 * sin(x+y);
                    bk += (_in_r->data)[ii][jj] * 1.0 * sin(x+y) + (_in_i->data)[ii][jj] * cos(x+y);
                }
            }

            //printf("y: %d\tx: %d\n", i, j);

            (_out_r->data)[i][j] = ak;
            (_out_i->data)[i][j] = bk;
        }
    }
}
*/



void CplxConj(double_array* _arr)
{
    for(int i = 0; i < (_arr->size).y; i++)
    {
       for(int j = 0; j <(_arr->size).x; j = j + 1)
       {
            (_arr->data)[i][j] = -(_arr->data)[i][j];
       }
    }

    return;
}

void GaussianCorrelation(double_array* _xf_r, double_array* _xf_i, double_array* _yf_r, double_array* _yf_i, double sigma, double_array* _kf_r, double_array* _kf_i, pair tar_size)
{

    // Calculate norm in Fourier domain
    //printf("Calculate norm!\n");
    double N = (_xf_r->size).x * (_xf_r->size).y;
    double xf_norm = 0;
    double yf_norm = 0;
    //printf("HiHi I'm Josh.\n");
    
    for(int i = 0; i < (_xf_r->size).y; i++)
    {
       for(int j = 0; j < (_xf_r->size).x; j++)
       {
            xf_norm += (_xf_r->data)[i][j] * (_xf_r->data)[i][j] + (_xf_i->data)[i][j] * (_xf_i->data)[i][j];
            yf_norm += (_yf_r->data)[i][j] * (_yf_r->data)[i][j] + (_yf_i->data)[i][j] * (_yf_i->data)[i][j];
            //printf("row: %d\tcol: %d\n", i, j);
            //printf("%f\t%f\n", xf_norm, yf_norm);
       }
    }

    xf_norm = xf_norm / N;
    yf_norm = yf_norm / N;

    
    // Cross-correlation term in Fourier domain
    //printf("Calculate cross-correlation!\n");
    double_array xyf_r, xyf_i;
    InitArray(&xyf_r, tar_size);
    InitArray(&xyf_i, tar_size);
    ZeroInit(&xyf_r);
    ZeroInit(&xyf_i);

    for(int i = 0; i < (_xf_r->size).y; i++)
    {
       for(int j = 0; j < (_yf_r->size).x; j++)
       {
            (xyf_r.data)[i][j] += (_xf_r->data)[i][j] * (_yf_r->data)[i][j] + (_xf_i->data)[i][j] * (_yf_i->data)[i][j];
            (xyf_i.data)[i][j] += (_xf_i->data)[i][j] * (_yf_r->data)[i][j] - (_xf_r->data)[i][j] * (_yf_i->data)[i][j];
       }
    }


    // Turn into spatial domain
    // printf("Turn into spatial domain!\n");
    double_array xy_r, xy_i;
    InitArray(&xy_r, tar_size);
    InitArray(&xy_i, tar_size);
    IFFT2(&xyf_r, &xyf_i, &xy_r, &xy_i);

    DeleteArray(&xyf_r);
    DeleteArray(&xyf_i);

    // Calculate gaussian response for all positions, then go back to the Fourier domain
    //printf("Calculate gaussian response!\n");
    for(int i = 0; i < (xy_r.size).y; i++)
    {
       for(int j = 0; j < (xy_r.size).x; j++)
       {
            (xy_r.data)[i][j] = exp(-1 / (sigma * sigma) * fmax(0, (xf_norm + yf_norm - 2 * xy_r.data[i][j]) / N));
       }
    }

    ZeroInit(&xy_i);
    FFT2(&xy_r, &xy_i, _kf_r, _kf_i);
    //PrintArray(_kf_i);

    DeleteArray(&xy_r);
    DeleteArray(&xy_i);

    return;
}

pair FindMax(double_array* _arr)
{
    double max_val = -100000.0;
    pair pos = {0,0};
    for(int i = 0; i < (_arr->size).y; i++)
    {
       for(int j = 0; j < (_arr->size).x; j++)
       {
            if((_arr->data)[i][j] >= max_val)
            {
                max_val = (_arr->data)[i][j];
                pos.y = i;
                pos.x = j;
            }
       }
    }

    return pos;

}

/*-------------------------------------------------------------------------
   Perform a 2D FFT inplace given a complex 2D array
   The direction dir, 1 for forward, -1 for reverse
   The size of the array (nx,ny)
   Return false if there are memory problems or
      the dimensions are not powers of 2
*/

/*-------------------------------------------------------------------------
   This computes an in-place complex-to-complex FFT
   x and y are the real and imaginary arrays of 2^m points.
   dir =  1 gives forward transform
   dir = -1 gives reverse transform

     Formula: forward
                  N-1
                  ---
              1   \          - j k 2 pi n / N
      X(n) = ---   >   x(k) e                    = forward transform
              N   /                                n=0..N-1
                  ---
                  k=0

      Formula: reverse
                  N-1
                  ---
                  \          j k 2 pi n / N
      X(n) =       >   x(k) e                    = forward transform
                  /                                n=0..N-1
                  ---
                  k=0
*/

void FFT2(double_array* _in_r, double_array* _in_i, double_array* _out_r, double_array* _out_i)
{
    //printf("Calculate 2D FFT!\n");
    double* tmp_r;
    double* tmp_i;
    /* Transform the rows */
    tmp_r = (double *)malloc((_in_r->size).x * sizeof(double));
    tmp_i = (double *)malloc((_in_r->size).x * sizeof(double));

    for(int i = 0; i < (_in_r->size).y; i++)
    {
        for(int j = 0; j < (_in_r->size).x; j++)
        {
            tmp_r[j] = (_in_r->data)[i][j];
            tmp_i[j] = (_in_i->data)[i][j];
        }

        FFT1D(tmp_r, tmp_i, (_in_r->size).x, tmp_r, tmp_i);

        for(int j = 0; j < (_in_r->size).x; j++)
        {
            (_out_r->data)[i][j] = tmp_r[j];
            (_out_i->data)[i][j] = tmp_i[j];
        }
    }
    

    free(tmp_r);
    free(tmp_i);
    tmp_r = (double *)malloc((_in_r->size).y * sizeof(double));
    tmp_i = (double *)malloc((_in_r->size).y * sizeof(double));

    for(int j = 0; j < (_in_r->size).x; j++)
    {
        for(int i = 0; i < (_in_r->size).y; i++)
        {
            tmp_r[i] = (_out_r->data)[i][j];
            tmp_i[i] = (_out_i->data)[i][j];
        }

        FFT1D(tmp_r, tmp_i, (_in_r->size).y, tmp_r, tmp_i);

        for(int i = 0; i < (_in_r->size).y; i++)
        {
            (_out_r->data)[i][j] = tmp_r[i];
            (_out_i->data)[i][j] = tmp_i[i];
        }
    }

    free(tmp_r);
    free(tmp_i);
}

// 2D Inverse Fourier Transform
void IFFT2(double_array* _in_r, double_array* _in_i, double_array* _out_r, double_array* _out_i)
{
    double* tmp_r;
    double* tmp_i;
    /* Transform the rows */
    tmp_r = (double *)malloc((_in_r->size).x * sizeof(double));
    tmp_i = (double *)malloc((_in_r->size).x * sizeof(double));

    for(int i = 0; i < (_in_r->size).y; i++)
    {
        for(int j = 0; j < (_in_r->size).x; j++)
        {
            tmp_r[j] = (_in_r->data)[i][j];
            tmp_i[j] = (_in_i->data)[i][j];
        }

        IFFT1D(tmp_r, tmp_i, (_in_r->size).x, tmp_r, tmp_i);

        for(int j = 0; j < (_in_r->size).x; j++)
        {
            (_out_r->data)[i][j] = tmp_r[j];
            (_out_i->data)[i][j] = tmp_i[j];
        }
    }

    free(tmp_r);
    free(tmp_i);
    tmp_r = (double *)malloc((_in_r->size).y * sizeof(double));
    tmp_i = (double *)malloc((_in_r->size).y * sizeof(double));

    for(int j = 0; j < (_in_r->size).x; j++)
    {
        for(int i = 0; i < (_in_r->size).y; i++)
        {
            tmp_r[i] = (_out_r->data)[i][j];
            tmp_i[i] = (_out_i->data)[i][j];
        }

        IFFT1D(tmp_r, tmp_i, (_in_r->size).y, tmp_r, tmp_i);

        for(int i = 0; i < (_in_r->size).y; i++)
        {
            (_out_r->data)[i][j] = tmp_r[i];
            (_out_i->data)[i][j] = tmp_i[i];
        }
    }

    free(tmp_r);
    free(tmp_i);

}

void FFT1D(double* t_r, double* t_i, int len, double* f_r, double* f_i)
{
    int m, twopm;
    twopm = Powerof2(len, &m);



    if(twopm == len)
    {
        P2FFT(t_r, t_i, m, len, f_r, f_i);
    }
    else
    {
        m = m + 1;
        twopm = twopm * 2;
        double ext_t_r[twopm];
        double ext_t_i[twopm];
        double ext_twid_r[twopm];
        double ext_twid_i[twopm];

        double ext_f_r[twopm];
        double ext_f_i[twopm];
        double ext_twidf_r[twopm];
        double ext_twidf_i[twopm];

        for(int idx = 0; idx < twopm; idx++)
        {
            if(idx < len)
            {
                double W_r, W_i;
                TwiddleFactor(idx * idx / 2., (double)len, &W_r, &W_i);
                ext_t_r[idx] = W_r * t_r[idx] - W_i * t_i[idx];
                ext_t_i[idx] = W_i * t_r[idx] + W_r * t_i[idx];
            }
            else
            {
                ext_t_r[idx] = 0;
                ext_t_i[idx] = 0;
            }
        }

        for(int idx = 0; idx < twopm; idx++)
        {
            double W_r, W_i;
            if(idx < len)
            {
                TwiddleFactor(-idx * idx / 2., (double)len, &W_r, &W_i);
                ext_twid_r[idx] = W_r;
                ext_twid_i[idx] = W_i;
            }
            else if(idx > twopm - len)
            {
                TwiddleFactor(-(twopm - idx) * (twopm - idx) / 2., (double)len, &W_r, &W_i);
                ext_twid_r[idx] = W_r;
                ext_twid_i[idx] = W_i;
            }
            else
            {
                ext_twid_r[idx] = 0;
                ext_twid_i[idx] = 0;
            }
        }

        P2FFT(ext_t_r, ext_t_i, m, twopm, ext_f_r, ext_f_i);
        P2FFT(ext_twid_r, ext_twid_i, m, twopm, ext_twidf_r, ext_twidf_i);

        double conv_f_r[twopm];
        double conv_f_i[twopm];

        for(int idx = 0; idx < twopm; idx++)
        {
            conv_f_r[idx] = ext_twidf_r[idx] * ext_f_r[idx] - ext_twidf_i[idx] * ext_f_i[idx];
            conv_f_i[idx] = -(ext_twidf_i[idx] * ext_f_r[idx] + ext_twidf_r[idx] * ext_f_i[idx]);
        }

        double out_r[twopm];
        double out_i[twopm];

        //CplxConj(conv_f_i);
        P2FFT(conv_f_r, conv_f_i, m, twopm, out_r, out_i);
        //CplxConj(_out_i) // Do it in the following for loop   

        for(int idx = 0; idx < twopm; idx++)
        {
            out_r[idx] = out_r[idx] / twopm;
            out_i[idx] = -out_i[idx] / twopm;
        }

        for(int idx = 0; idx < len; idx++)
        {
            double W_r, W_i;
            TwiddleFactor(idx * idx / 2., (double)len, &W_r, &W_i);
            f_r[idx] = W_r * out_r[idx] - W_i * out_i[idx];
            f_i[idx] = W_i * out_r[idx] + W_r * out_i[idx];
        }

    }
    
}

void IFFT1D(double* f_r, double* f_i, int len, double* t_r, double* t_i)
{
    for(int idx=0; idx < len; idx++)
    {
        f_i[idx] = -f_i[idx];
    }

    FFT1D(f_r, f_i, len, t_r, t_i);
    //CplxConj(_out_i) // Do it in the following for loop

    for(int idx = 0; idx < len; idx++)
    {
        t_r[idx] = t_r[idx] / len;
        t_i[idx] = -t_i[idx] / len;
    }
}


void P2FFT(double* t_r, double* t_i, int m, int twopm, double* f_r, double* f_i)
{

    double tmp_r[twopm];
    double tmp_i[twopm];

    for(int idx = 0; idx < twopm; idx++)
    {
        tmp_r[idx] = t_r[idx];
        tmp_i[idx] = t_i[idx];
    }

    /*
    for(int idx = 0; idx < twopm; idx++)
    {
        printf("Index %d\t", idx);
        printf("Real: %f\t", tmp_r[idx]);
        printf("Imag: %f\n", tmp_i[idx]);
    }
    */

    double W_r, W_i;
    double G_r, G_i;
    double H_r, H_i;
    int max_num, sublength;

    for(int iter = 0; iter < m; iter++)
    {
        max_num = twopm / (pow(2, (m - iter)));
        sublength = twopm / (max_num*2);
        for(int idx = 0; idx < sublength; idx++)
        {
            TwiddleFactor((double)idx, (double)sublength*2, &W_r, &W_i);

            /*
            printf("Info: %d\t", sublength);
            printf("%d\n", idx);
            printf("Real: %f\t", W_i);
            printf("Imag: %f\n", W_r);
            */
            for(int num = 0; num < max_num; num++)
            {
                G_r = tmp_r[2 * num * sublength + idx];
                G_i = tmp_i[2 * num * sublength + idx];
                H_r = tmp_r[(2 * num + 1) * sublength + idx];
                H_i = tmp_i[(2 * num + 1) * sublength + idx];

                //printf("Info: %d\t", idx);
                //printf("%d\n", num);
                //printf("%f\n", H_r);

                tmp_r[2 * num * sublength + idx] = G_r + H_r;
                tmp_i[2 * num * sublength + idx] = G_i + H_i;
                tmp_r[(2 * num + 1) * sublength + idx] = W_r * (G_r - H_r) - W_i * (G_i - H_i);
                tmp_i[(2 * num + 1) * sublength + idx] = W_r * (G_i - H_i) + W_i * (G_r - H_r);
            }
        }
    }



    for(int idx = 0; idx < twopm; idx++)
    {
        f_r[idx] = tmp_r[BitReverse(idx, m)];
        f_i[idx] = tmp_i[BitReverse(idx, m)];
    }
}

/*-------------------------------------------------------------------------
   Calculate the closest but higher power of two of a number
   twopm = 2**m >= n
*/
int Powerof2(int n,int* m)
{
    int twopm = 2;
    if(n <= 1)
    {
        *m = 0;
        return 1;
    }

    *m = 1;

    while(twopm < n)
    {
        (*m)++;
        twopm *= 2;
    }

    return twopm;
}

int BitReverse(int num, int bit_num) 
{  
    int reverse_num = 0;
    int temp; 
  
    for (int i = 0; i < bit_num; i++) 
    { 
        temp = (num & (1 << i)); 
        if(temp) 
            reverse_num |= (1 << ((bit_num - 1) - i)); 
    } 
   
    return reverse_num; 
}

void TwiddleFactor(double k, double N, double* re, double* im)
{
    *re = cos(-2.0 * PI * k / N);
    *im = sin(-2.0 * PI * k / N);
}

void PrintArray(double_array* _arr)
{
    for(int i = 0; i < 10; i++)
    {
       for(int j = 0; j < 10; j++)
       {
            printf("%e\t", (_arr->data)[i][j]);
       }
       printf("\n");
    }
}

void CheckArray(double_array* _arr, double thres)
{
    double sum = 0;
    for(int i = 0; i < (_arr->size).y; i++)
    {
       for(int j = 0; j < (_arr->size).x; j++)
       {
            if((_arr->data)[i][j] > thres)
            {
                sum = sum + (_arr->data)[i][j];
                //printf("%d\t%d\n", i, j);
                //printf("%e\t", (_arr->data)[i][j]);
            }
       }
    }
    printf("The sum of the array is: %e\n", sum);
}