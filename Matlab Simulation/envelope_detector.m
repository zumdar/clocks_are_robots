function [env_samples, env_time_s, env_freq, env_freq_axis_kHz] = envelope_detector(signal, time_s, tempo_resolution)
%ENVELOPE_DETECTOR Summary of this function goes here
%Envelope Detection based on Low pass filter and then FFT
lp1 = dsp.FIRFilter('Numerator',firpm(20,[0 0.03 0.1 1],[1 1 0 0]));

Fs = 22050;
numSamples = 10000;
DownsampleFactor = 10/tempo_resolution;
frameSize = 10*DownsampleFactor;

scope1 = dsp.TimeScope( ...
  'NumInputPorts',2, ...
  'Name','Envelope detection using Amplitude Modulation', ...
  'SampleRate',[Fs,Fs/DownsampleFactor], ...
  'TimeDisplayOffset',[(N/2+frameSize)/Fs,0], ...
  'TimeSpanSource','Property', ...
  'TimeSpan',0.45, ...
  'YLimits',[-2.5 2.5], ...
  'TimeSpanOverrunAction','Scroll');

sig_sq = 2 * signal .* signal;
env = sqrt(lp1(downsample(sig_sq,DownsampleFactor)));

%plot
scope1(signal, env);
release(scope1);

env_freq = fft(env);
env_freq_axis_kHz = env_freq/1000; %???
env_time_s=(0:numSamples-1)/time_s;
env_samples = downsample(env, numSamples);

end

