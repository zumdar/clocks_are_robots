function tempo = tempo_detection_1(env_samples, env_time, Threshold)
%TEMPO_DETECTION Find tempo of input signal given envelope
% Note: Thresholding not implemented
% Note: Some delays cause the min silence count to be off by one sample
%% Resample Amplitude
resampled = requantize(env_samples, 4);

% Dumb code: will trash
% num_bits = 1;
% while 2^num_bits < max(env_samples)
%     num_bits = num_bits + 1;
% end
% limit = 2^num_bits;
% breakpoints = linspace(0,max(resampled),resolution+1);
% for i = 1:length(env_samples)
%     for j = 2:length(breakpoints)
%         if env_samples(i) < breakpoints(j)
%            resampled(i) = breakpoints(j-1);
%            break
%         end
%     end
% end

%% Detect Tempo by zero-length

Ts = 1/(length(env_time)-1)*(sum(env_time(2:end)-env_time(1:end-1))); % Average sampling period - in case there is some error
zero_count = 0;
min_count = inf;
% silence at the beginning could be a cut off rest
start_counting = 0; 
for i = 1:length(resampled)
    if resampled(i) == 0
        if start_counting
            zero_count = zero_count + 1;
        end
    elseif zero_count
        if zero_count < min_count
            min_count = zero_count;
            location = i;
        end
        zero_count = 0;
    else
        start_counting = 1; % start counting after first rising edge
    end
end

tempo = min_count*Ts; % The time for a beat in seconds is equal to the length of the shortest rest
  
