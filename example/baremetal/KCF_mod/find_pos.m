function [pos, num_frame, target_sz] = find_pos(video_path, img_files)

sigma = 25;
threshold = 0.005;
s = 1.5;
N = 4;
im_mean = 0;
obj_pos = [];
rad = [];

for frame = 1:numel(img_files)
	%load image
	im = imread([video_path img_files{frame}]);
    im = rgb2gray(im);
    im = imresize(im, 0.5);
    im = double(im) / 255;
    
    if frame == 1
        %im_mean = mean(mean(im));
        orig_im = im;
        im_size = size(orig_im);
    else
        res_im = abs(im - orig_im);
        if frame > 115
            [obj_pos, rad] = detectBlob(res_im, sigma, N, threshold, 'green', 1, 1); 
        end
        
        if ~isempty(obj_pos)
            pos = obj_pos(1,:);
            target_sz = [2*rad, 2*rad];
            num_frame = frame;
            return
        end
        
        %{
        if frame == 150      
            pos = [380 500];
            target_sz = [180 200]; %width, height
            num_frame = 119;
            return
        end
        %}
    end
end