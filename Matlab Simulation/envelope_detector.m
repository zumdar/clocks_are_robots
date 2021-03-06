function [env_samples, env_time_s, env_freq, env_freq_axis_kHz] = envelope_detector(signal, time_s, tempo_resolution)
%ENVELOPE_DETECTOR Envelope Detection based on Low pass filtering and downsampling
%Envelope Detection based on Low pass filter and then FFT
lp1 = dsp.FIRFilter('Numerator',firpm(20,[0 0.015 0.3 1],[1 1 0 0]));

Ts = 1/(length(time_s)-1)*(sum(time_s(2:end)-time_s(1:end-1))); % Average sampling period - in case there is some error
Fs = 1/Ts;
DownsampleFactor = 10/tempo_resolution;



sig_sq = 2 * signal .* signal;
% env = sqrt(lp1(downsample(sig_sq,DownsampleFactor)));;
env = downsample(lowpass(abs(sig_sq), 20, Fs), DownsampleFactor);

env_freq = fft(env);
env_freq = env_freq(1:floor(length(env_freq)/2)); % FFT is symmetric -> take positive side
env_freq_axis_kHz = linspace(0, Fs/2000, length(env_freq)); % X-axis for frequency response
env_time_s = linspace(0, time_s(end), length(env));
env_samples = env;

subplot(121)
plot(env_time_s, env_samples)
xlabel("Time (sec)")
ylabel("Input Envelope")
subplot(122)
plot(env_freq_axis_kHz, abs(env_freq))
xlabel("Frequency (kHz)")
ylabel("Magnitude Response of Envelope")

% % Visualization Code
% frameSize = 10*DownsampleFactor;
% scope1 = dsp.TimeScope( ...
%   'NumInputPorts',2, ...
%   'Name','Envelope detection using Amplitude Modulation', ...
%   'SampleRate',[Fs,Fs/DownsampleFactor], ...
%   'TimeDisplayOffset',[(frameSize)/Fs,0], ...
%   'TimeSpanSource','Property', ...
%   'TimeSpan',0.45, ...
%   'YLimits',[-2.5 2.5], ...
%   'TimeSpanOverrunAction','Scroll');
% % plot
% scope1(signal, env);
% release(scope1);
end

