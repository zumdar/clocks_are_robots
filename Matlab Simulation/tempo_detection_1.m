function tempo = tempo_detection(env_samples, env_time, Threshold)
% Find tempo of input signal given envelope
% Note: Thresholding not implemented
%% Resample Amplitude
resolution = 4; % Will probably make a standalone function and parameterize
env_samples = round(abs(env_samples*1000)); % Integer version of envelope - may not need this

% Convert to a binary signal based on bit resolution
conversion_factor = (2^resolution - 1)/max(env_samples); 
resampled = floor(env_samples*conversion_factor);

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
for i = 2:length(resampled)
    if resampled(i) == 0
        zero_count = zero_count + 1;
    elseif zero_count
        if zero_count < min_count
            min_count = zero_count;
            location = i;
        end
        zero_count = 0;
    end
end

tempo = min_count*Ts; % The time for a beat in seconds is equal to the length of the shortest rest
  
