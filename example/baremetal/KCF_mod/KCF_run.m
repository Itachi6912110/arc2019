clc
data_path =  './data/Benchmark/';

files = dir(data_path);

for id = 1:length(files)
    if ~isdir([data_path files(id).name]) || strcmp('.', files(id).name) || strcmp('AE_train_Deer', files(id).name) || strcmp('..', files(id).name)
        continue;
    end
    close all;
    run_tracker(files(id).name, 'gaussian', 'gray');
end