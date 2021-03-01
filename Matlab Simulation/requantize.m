function quantized = requantize(signal,resolution)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

signal = round(abs(signal*1000)); % Integer version of envelope - may not need this

% Convert to a binary signal based on bit resolution
conversion_factor = (2^resolution - 1)/max(signal); 
quantized = floor(signal*conversion_factor);

end

