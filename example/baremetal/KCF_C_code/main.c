#include "include.h"
#include "tracker.h"

struct object_tracker* FirstTrack(double tar_size_x, double tar_size_y, double tar_pos_x, double tar_pos_y, double padding, double k_sigma, char video_path[30]);
void TrackObj(struct object_tracker* _obj, char video_path[30]);

int main()
{

    /*
    double padding = 1.5;
    pair target_sz;

    target_sz.y = 70;
    target_sz.x = 70; 


    struct object_tracker obj;
    obj.pos.y = 205 - 1;
    obj.pos.x = 283 - 1;
    obj.target_sz.y = floor((target_sz.y) * (1 + padding));
    obj.target_sz.x = floor((target_sz.x) * (1 + padding)); 
    obj.first_frame = 1;
    obj.ker_sigma = 0.2;
    double output_sigma = 7.0;
    
    InitArray(&(obj.model_alphaf_r), obj.target_sz);
    InitArray(&(obj.model_alphaf_i), obj.target_sz);
    InitArray(&(obj.model_xf_r), obj.target_sz);
    InitArray(&(obj.model_xf_i), obj.target_sz);
    ZeroInit(&(obj.model_alphaf_r));
    ZeroInit(&(obj.model_alphaf_i));
    ZeroInit(&(obj.model_xf_r));
    ZeroInit(&(obj.model_xf_i));

    double_array cos_window;
    InitArray(&cos_window, obj.target_sz);
    CWindow(&cos_window);

    double_array yf;
    double_array label_r;
    double_array label_i;
    double_array labelf_i;

    InitArray(&yf, obj.target_sz);
    InitArray(&label_r, obj.target_sz);
    InitArray(&label_i, obj.target_sz);
    InitArray(&labelf_i, obj.target_sz);
    
    ZeroInit(&(label_i));

    GaussianShapedLabel(output_sigma, &label_r, obj.target_sz);
    FFT2(&label_r, &label_i, &yf, &labelf_i);

    DeleteArray(&label_r);
    DeleteArray(&label_i);
    DeleteArray(&labelf_i);

    //PrintArray(&yf);

    for(int num = 117; num <= 383; num++)
    {
        time_t start_seconds, end_seconds; 
   
        // Stores time seconds 

        char video_path[30] = "video/"; 
        char snum[4];
        sprintf(snum, "%d", num);
        strcat(video_path, snum);
        strcat(video_path, ".csv");
        strncpy(obj.img_path, video_path, 50);
        //printf("%s\n", obj.img_path);
        time(&start_seconds);
        Tracker(&obj, &cos_window, &yf);
        time(&end_seconds);
        printf("Time cost: %ld\n", end_seconds - start_seconds);
        if(num == 117)
        {
            obj.first_frame = 0;
        }
        printf("New position: [%f, %f]\n", obj.pos.y, obj.pos.x);
    }

    DeleteArray(&yf);

    DeleteArray(&cos_window);
    DeleteArray(&(obj.model_alphaf_r));
    DeleteArray(&(obj.model_alphaf_i));
    DeleteArray(&(obj.model_xf_r));
    DeleteArray(&(obj.model_xf_i));
    */

    int num = 117;
    char first_frame[30] =  "video/"; 
    char fnum[4];
    sprintf(fnum, "%d", num);
    strcat(first_frame, fnum);
    strcat(first_frame, ".csv");

    double tar_size_y = 70;
    double tar_size_x = 70; 
    double tar_pos_y  = 205 - 1;
    double tar_pos_x  = 283 - 1;
    double k_sigma    = 0.2;
    double padding    = 1.5; 

    struct object_tracker* _new_obj = FirstTrack(tar_size_x, tar_size_y, tar_pos_x, tar_pos_y, padding, k_sigma, first_frame);

    time_t start_seconds, end_seconds; 
    time(&start_seconds);
    for(num = 118; num <= 383; num++)
    {
   
        // Stores time seconds 

        char video_path[30] = "video/"; 
        char snum[4];
        sprintf(snum, "%d", num);
        strcat(video_path, snum);
        strcat(video_path, ".csv");
        strncpy(_new_obj->img_path, video_path, 50);
        //printf("%s\n", obj.img_path);
        
        TrackObj(_new_obj, video_path);
        
        printf("New position: [%f, %f]\n", _new_obj->pos.y, _new_obj->pos.x);
    }

    time(&end_seconds);
    printf("Time cost: %ld\n", end_seconds - start_seconds);



    return 0;
}

struct object_tracker* FirstTrack(double tar_size_x, double tar_size_y, double tar_pos_x, double tar_pos_y, double padding, double k_sigma, char video_path[30])
{
    //padding = 1.5;
    //pair target_sz;

    //target_sz.y = 70;
    //target_sz.x = 70; 
    //obj.pos.y = 205 - 1;
    //obj.pos.x = 283 - 1;


    struct object_tracker* _obj;
    _obj = (struct object_tracker*)malloc(sizeof(struct object_tracker));
    _obj->pos.y = tar_pos_y;
    _obj->pos.x = tar_pos_x;
    _obj->target_sz.y = floor((tar_size_y) * (1 + padding));
    _obj->target_sz.x = floor((tar_size_x) * (1 + padding)); 
    _obj->first_frame = 1;
    _obj->ker_sigma = 0.2;
    strncpy(_obj->img_path, video_path, 50);
    double output_sigma = 7.0;
    
    InitArray(&(_obj->model_alphaf_r), _obj->target_sz);
    InitArray(&(_obj->model_alphaf_i), _obj->target_sz);
    InitArray(&(_obj->model_xf_r), _obj->target_sz);
    InitArray(&(_obj->model_xf_i), _obj->target_sz);
    InitArray(&(_obj->cos_window), _obj->target_sz);
    InitArray(&(_obj->yf), _obj->target_sz);
    ZeroInit(&(_obj->model_alphaf_r));
    ZeroInit(&(_obj->model_alphaf_i));
    ZeroInit(&(_obj->model_xf_r));
    ZeroInit(&(_obj->model_xf_i));

    
    CWindow(&(_obj->cos_window));

    double_array label_r;
    double_array label_i;
    double_array labelf_i;

    InitArray(&label_r, _obj->target_sz);
    InitArray(&label_i, _obj->target_sz);
    InitArray(&labelf_i, _obj->target_sz);
    
    ZeroInit(&(label_i));

    GaussianShapedLabel(output_sigma, &label_r, _obj->target_sz);
    FFT2(&label_r, &label_i, &(_obj->yf), &labelf_i);

    DeleteArray(&label_r);
    DeleteArray(&label_i);
    DeleteArray(&labelf_i);

    Tracker(_obj);

    return _obj;
}

void TrackObj(struct object_tracker* _obj, char video_path[30])
{
    _obj->first_frame = 0;
    strncpy(_obj->img_path, video_path, 50);
    Tracker(_obj);
}










