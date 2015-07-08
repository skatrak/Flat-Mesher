function [ result ] = IsInside( p, x )
%ISINSIDE Summary of this function goes here
%   Detailed explanation goes here

if size(p, 2)>size(p, 1)
    p = p';
end

figure;
hold on;
line([p(:,1); p(1,1)],[p(:,2); p(1,2)]);
plot(x(1),x(2),'o');

q_weights = [ 0.173927422568727
    0.326072577431273
    0.326072577431273
    0.173927422568727 ];

q_points = [ 0.930568155797026
    0.669990521792428
    0.330009478207572
    0.069431844202974 ];

n_points = length(q_points);

n = length(p);

integral = 0.0;
for i = 1 : n
    y_start = p(i, :);
    if i == n
        y_end = p(1, :);
    else
        y_end = p(i+1, :);
    end
    
    segment = y_end - y_start;
    h = norm(segment, 2);
    
    normal = [segment(2), -segment(1)] / h;
    
    local_integral = 0.0;
    for j = 1 : n_points
        y = y_start + q_points(j) * segment;
        norm_xy = norm(x-y,2);
        local_integral = local_integral + (-1.0/(2*pi)*(x-y)*normal'/ ...
            norm_xy^2) * q_weights(j);
    end
    integral = integral + local_integral * h;
    
end

if (integral > 0.5)
    result = true;
else
    result = false;
end

end
