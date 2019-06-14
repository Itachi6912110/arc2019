
%
%  High-Speed Tracking with Kernelized Correlation Filters
%
%  Joao F. Henriques, 2014
%  http://www.isr.uc.pt/~henriques/
%
%  Main interface for Kernelized/Dual Correlation Filters (KCF/DCF).
%  This function takes care of setting up parameters, loading video
%  information and computing precisions. For the actual tracking code,
%  check out the TRACKER function.
%
%  RUN_TRACKER
%    Without any parameters, will ask you to choose a video, track using
%    the Gaussian KCF on HOG, and show the results in an interactive
%    figure. Press 'Esc' to stop the tracker early. You can navigate the
%    video using the scrollbar at the bottom.
%
%  RUN_TRACKER VIDEO
%    Allows you to select a VIDEO by its name. 'all' will run all videos
%    and show average statistics. 'choose' will select one interactively.
%
%  RUN_TRACKER VIDEO KERNEL
%    Choose a KERNEL. 'gaussian'/'polynomial' to run KCF, 'linear' for DCF.
%
%  RUN_TRACKER VIDEO KERNEL FEATURE
%    Choose a FEATURE type, either 'hog' or 'gray' (raw pixels).
%
%  RUN_TRACKER(VIDEO, KERNEL, FEATURE, SHOW_VISUALIZATION, SHOW_PLOTS)
%    Decide whether to show the scrollable figure, and the precision plot.
%
%  Useful combinations:
%  >> run_tracker choose gaussian hog  %Kernelized Correlation Filter (KCF)
%  >> run_tracker choose linear hog    %Dual Correlation Filter (DCF)
%  >> run_tracker choose gaussian gray %Single-channel KCF (ECCV'12 paper)
%  >> run_tracker choose linear gray   %MOSSE filter (single channel)
%


function [precision, fps] = run_tracker_mod(base_path, video, show_visualization, show_plots)

	%path to the videos (you'll be able to choose one with the GUI).
% 	base_path = './data/Benchmark/';
   

	%default settings
	kernel_type = 'gaussian';
	show_visualization = ~strcmp(video, 'all');
	show_plots = ~strcmp(video, 'all');


	%parameters according to the paper. at this point we can override
	%parameters based on the chosen kernel or feature type
	kernel.type = kernel_type;
	
    feature_type = 'gray';
	features.gray = false;
	features.hog = false;
	
	padding = 1.5;  %extra area surrounding the target
    %padding = 2;  %extra area surrounding the target

	lambda = 1e-4;  %regularization
	output_sigma_factor = 0.1;  %spatial bandwidth (proportional to target)
	interp_factor = 0.075;  %linear interpolation factor for adaptation

	kernel.sigma = 0.2;  %gaussian kernel bandwidth
		
	kernel.poly_a = 1;  %polynomial kernel additive term
	kernel.poly_b = 7;  %polynomial kernel exponent
	
	features.gray = true;
	cell_size = 1;
    %[img_files, pos, target_sz, ground_truth, video_path] = load_video_info_mod(base_path, video);
    img_files = dir([base_path '*.jpg']);
	assert(~isempty(img_files), 'No image files to load.')
	img_files = sort({img_files.name});
    
    [pos, num_frame, target_sz] = find_pos(base_path, img_files)
    
	%call tracker function with all the relevant parameters
	[positions, time] = tracker_mod(num_frame, base_path, img_files, pos, target_sz, ...
                                padding, kernel, lambda, output_sigma_factor, interp_factor, ...
                                cell_size, features, show_visualization);

	clear results
    results{1}.type = 'rect';
    x = positions(:,2);
    y = positions(:,1);
    w = target_sz(2) * ones(size(x, 1), 1);
    h = target_sz(1) * ones(size(x, 1), 1);
    results{1}.res = [x-w/2, y-h/2, w, h];
        
    results{1}.startFame = 1;
    results{1}.annoBegin = 1;
    results{1}.len = size(x, 1);
        
    frames = {'David', 300, 465;%770
            'Football1', 1, 74;
            'Freeman3', 1, 460;
            'Freeman4', 1, 283};
        
    idx = find(strcmpi(video, frames(:,1)));
        
    if ~isempty(idx),
        results{1}.startFame = frames{idx, 2};
        results{1}.len = frames{idx, 3} - frames{idx, 2}; +1;
    end
    res_path = ['KCF_results_' feature_type '/'];
    if ~isdir(res_path)
        mkdir(res_path);
    end
    save([res_path lower(video) '_kcf_' feature_type '8.mat'], 'results');
        
        
		%calculate and show precision plot, as well as frames-per-second
    precisions = precision_plot(positions, ground_truth, video, show_plots);
	fps = numel(img_files) / time;

	fprintf('%12s - Precision (20px):% 1.3f, FPS:% 4.2f\n', video, precisions(20), fps)

	if nargout > 0,
		%return precisions at a 20 pixels threshold
		precision = precisions(20);
    end
	
