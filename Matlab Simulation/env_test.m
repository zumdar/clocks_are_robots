%% Envelope Detector Test
Fs = 3000;
end_time = 5;


time = 0:1/Fs:end_time;
signal = sin(800*2*pi*time).*cos(400*2*pi*time);
[env, env_time, freq, freq_axis] = envelope_detector(signal, time, 0.1);

subplot(131)
plot(time, signal)
subplot(132)
plot(env_time, abs(env))
subplot(133)
plot(freq_axis,abs(freq))
