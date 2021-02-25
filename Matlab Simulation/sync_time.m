function [measured_time_offset_s] = sync_time(env_samples, digital, time_s, tempo, fskHz)
%SYNC_TIME Summary of this function goes here
%   Detailed explanation goes here

time_end = time_s(end);
fs = fskHz*1000;
fs_env = round(length(env_samples)/time_end);
binary_env = requantize(env_samples, 1);


%%  Search for C to determine octave
C = 16.3516;
rest = 0;

freq_resp = abs(fft(digital));
freq_resp = freq_resp(1:floor(length(freq_resp)/2));
relevant_freq = [C*2, C*2^2, C*2^3, C*2^4, C*2^5, C*2^6];
% Get the DFT indices closest to the given frequencies
[~, DFTindices] = DFTind(length(digital), 1/fs, relevant_freq);
[~, maxInd] = max(freq_resp(DFTindices)); % Strongest frequency tells octave
octave = maxInd;

%  Assign notes based on octave
C = 16.3516*2^octave;
D = 18.35405*2^octave;
E = 20.60172*2^octave;
F = 21.82676*2^octave;
G = 24.49971*2^octave;
A = 27.5*2^octave;
B = 30.86771*2^octave;
high_C = 32.70320*2^octave;

%% Generate template of non-delayed signal
notes = [C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest];
beats = [2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1];
ref_signal = [];
env_ref = [];
t = 0:1/fs:sum(beats)*tempo;
start_time = 0;
for i = 1:length(beats)
    time = start_time:1/fs:start_time+beats(i)*tempo;
    start_time = time(end) + 1/fs;
    ref_signal = [ref_signal, sin(2*pi*notes(i)*time)];
%     if mod(i,2)
%         env_ref = [env_ref, ones(
end

% ref_signal = [ref_signal, ref_signal];
correlation = xcorr(ref_signal, digital);
correlation = (correlation - mean(correlation))/std(correlation);
% autocorrelation = autocorrelation(1:floor(length(autocorrelation)/2));
[~, ind] = max(abs(correlation));
plot(correlation)

subplot(121)
plot(digital)
subplot(122)
plot(ref_signal)

while ind/fs > 0
    ind = ind - length(digital);
end
measured_time_offset_s = -ind/fs;
end

