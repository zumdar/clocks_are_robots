function [env_samples, env_time_s, env_freq, env_freq_axis_kHz] = envelope_detector(signal, time_s, tempo_resolution)
%ENVELOPE_DETECTOR Summary of this function goes here
%Envelope Detection based on Low pass filter and then FFT
sig_sq = 2*signal.*signal;% squaring
sig_sq = filter(a,b,sig_sq); %applying LPF
sig_sq = sqrt(sig_sq);%taking Square root
% N=2*2048;
% env_freq = abs(fft(sig_sq(1:N)',N));
% sig_n = env_freq/(norm(env_freq));
env_time_s=(0:N-1)/time_s;

plot(time_s, sig_sq)
hold on
plot(time_s,[-1;1]*env,plot_param{:})
hold off
title('Envelope Detection')

end

