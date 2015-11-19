function is_in = sector_check(point, elev_ang, elev_width, azim_ang, azim_width, r)
    is_in = false;
    
    % Range check
    if (norm(point) > r)
        return;
    end

    % Convert point into Spheric Coordinate System
    theta = atan2d(sqrt(point(1)^2 + point(2)^2), point(3));
    phi = atan2d(point(2), point(1));
    
    % Angle check
    if (abs(phi - azim_ang) < azim_width && abs(theta - elev_ang) < elev_width)
        is_in = true;
    end
end