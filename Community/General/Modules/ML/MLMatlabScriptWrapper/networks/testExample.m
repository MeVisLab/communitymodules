% This external script illustrates how an XMarkerList is
% generated in Matlab and exported to MeVisLab.

% Generate 30 angles between 0 and 2*pi
phi = linspace(0,2*pi,30)';

% Make a circle of points and set the vector and type
% properties of the XMarkers.
OutputXMarkerList.pos = 10*[cos(phi) sin(phi)];
OutputXMarkerList.vec= 10*[-sin(phi) cos(phi)];
OutputXMarkerList.type = [1:size(OutputXMarkerList.pos,1)]';

fprintf('External script successfully executed!')

