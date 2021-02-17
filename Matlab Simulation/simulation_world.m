%% Simulation World
% Engineer: Tim Brothers
% Overview
%    This is a simulation for the Robotic Orchestra
% Design Name:   The Conductor
% File Name:     simulation_world.m
%
% Inputs: 
%		Tempo: controls the tempo of the song. This is a number between 0 and 1
%		Octave: The ocive of the song (normally 4)
%		fs_kHz: sampling rate in kHz.
%       tempo_resolution: this is a value between 0 and 1. 1 being full 
%           resolution. Be careful setting this too high. 1 will lock up
%           your computer.
%       time_offset: This is the delay to the start of the song. This is an
%           index to the time scale, so the actual delay amount is
%           determined by multiple factors
%
% History:       4 January 2020 File created
%
%-----------------------------------------------------

%% Clear the Variables
clc
clear
close all

%% Set up the world
fs_kHz = 5;

% Set the tempo and the octave for the conductor
octave = 4;
tempo_s = 0.6; %This is the time in seconds for each note

%% Parameters for code simulation
tempo_resolution = .05; %this is a value between 0 and 1. 1 being full resolution
time_offset = 60;   % This is the delay to the start of the song.
  
%% Create the song given the tempo
[song_freq_Hz, song_duration_s] = conductor_simulation(tempo_s,octave);

% form up the cumulative durations
cumulative_duration_s = zeros(1, length(song_duration_s));
cumulative_duration_s(1) =  song_duration_s(1);
for i=2:length(song_duration_s)
	cumulative_duration_s(i) = song_duration_s(i) + cumulative_duration_s(i-1);
end

total_duration_s = max(cumulative_duration_s);
fs_Hz = fs_kHz*1000; %Convert kHz to Hz
time_s = 0:(1/fs_Hz):total_duration_s;	%create a time vector

% There is some rounding error happening sometimes, so we are going to make
% the end value exact
time_s(length(time_s)) = total_duration_s;


%% Create the digital signal
min_time_index = 1; %start the min time index at the start
% loop through all the freqs and generate a sin wave for each freq
for i=1:length(song_freq_Hz)
	max_time_index = find(time_s >= cumulative_duration_s(i),1); %find the times that correspond to this duration of time
	digital_note = sin(2*pi*song_freq_Hz(i)*time_s(min_time_index:max_time_index)); %create the actual note
	min_time_index = max_time_index + 1; %shift to the next time region.
	if(i == 1)
        digital = digital_note;
    else
        digital = [digital, digital_note];
    end
    
end

%% Create a signal to test the system
%%Put two copies of the song together
signal = [digital, digital];
signal = circshift(signal, time_offset);


%% Plot the Time Domain
plot(time_s,digital)
xlabel("time (s)")
ylabel("Amplitude")
title("Row Row Row Your Boat")

%% Plot the frequency
figure()
freq_digital = fft(digital);
freq_digital = freq_digital(1:floor(length(digital)/2));
freq_axis_kHz = linspace(0, fs_kHz/2, length(freq_digital));
plot(freq_axis_kHz, abs(freq_digital))
xlabel("Frequency (kHz)")
ylabel("Amplitude")
title("Spectrum of Row Row Row Your Boat")

%% Do the envelope detection
[env_samples, env_time_s, env_freq, env_freq_axis_kHz] = envelope_detector(signal, linspace(0,2*time_s(end), 2*length(time_s)), tempo_resolution);

%% Find the Tempo
%[measured_tempo] = tempo_detection(env_freq, env_freq_axis_kHz)
[measured_tempo] = tempo_detection_1(env_samples, env_time_s, 1e-2);

%% Sync the time
%[measured_time_offset_s] = sync_time(env_samples, digital, time_s, tempo_resolution, fskHz)
actual_time_offset_s = time_s(time_offset) %print out the actual start time to compare.