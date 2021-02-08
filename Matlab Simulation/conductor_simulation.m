function [freq_out_Hz,duration_out_s] = conductor_simulation(tempo_s,Octive)
%% Conductor Simulation
% Engineer: Tim Brothers
% Overview
%    simulation of the conductor.
%    When this device is turned on it plays the song Row
%    Row Row Your Boat.
% Design Name:   The Conductor
% File Name:     conductor_simulation.m
%
% Inputs: 
%		Tempo_s: controls the tempo of the song. This is the time for each
%		note in seconds.
%		Octive: The ocive of the song (normally 4)
% Outputs: 
%		freq_out_Hz: Frequency of the output tone in Hz
%		duration_out_s: the duration of the note in seconds
%
% History:       4 January 2020 File created
%
%-----------------------------------------------------

%% Music Notes based on Octive--
C = 16.3516 *(2^Octive);
D = 18.35405*(2^Octive);
E = 20.60172*(2^Octive);
F = 21.82676*(2^Octive);
G = 24.49971*(2^Octive);
A = 27.5    *(2^Octive);
B = 30.86771*(2^Octive);
high_C = 32.70320*(2^Octive);
rest = 0;

% Row Row Row Your Boat
songLength = 54;  
notes = [C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest];
beats = [2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1];

%% Create arrays for the outputs
freq_out_Hz = zeros(1, songLength);
duration_out_s = zeros(1, songLength);


%% Play the song
for i_note_index = 1:songLength
  % Create an array of output notes
  freq_out_Hz(i_note_index) = notes(i_note_index);
  duration_out_s(i_note_index) = beats(i_note_index) * tempo_s;
  end

end