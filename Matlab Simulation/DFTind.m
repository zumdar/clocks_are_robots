function [closestF, closestInd] = DFTind(N, T, freq)
% Find DFT indices closest to frequencies given DFT size N and sampling
% period T

closestF = inf*ones(1, length(freq));
closestInd = zeros(1, length(freq));

for k = 0:1:N-1
  f = k/(N*T);
  for i = 1:length(freq)
      if abs(freq(i) - f) < abs(freq(i) - closestF(i))
          closestF(i) = f;
          closestInd(i) = k;
      end
  end
end