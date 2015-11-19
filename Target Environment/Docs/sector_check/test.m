function test()
    % Working area description
    r = 1;
    elev_ang = 45;
    elev_width = 25;
    azim_ang = 0;
    azim_width = 30;
    % Plot area
    drow_sector(elev_ang, elev_width, azim_ang, azim_width, r);
    % Generate test points
    N = 300; % Number of points
    test_points = zeros(N,1)*zeros(1,3);
    for j = 1 : N
        test_points(j,:) = rand(1,3)*2 - 0.5; 
    end
    test_points = zeros(N,1)*zeros(1,3);
    for j = 1 : N
        test_points(j,:) = rand(1,3)*2 - 0.5; 
    end
    
    
    res = zeros(size(test_points(:,1)));
    % Check points 
    for i = 1 : size(test_points, 1)
        res(i) = sector_check(test_points(i, :), elev_ang, elev_width, azim_ang, azim_width, r);
    end
    
    % Plot test points
    % blue - in sector, red - out of sector
    hold on;
    
    bp_idx = find(res(:,1));
    blue_points = test_points(bp_idx, :);
    
    hold on;
    plot3(blue_points(:,1),blue_points(:,2),blue_points(:,3),'LineStyle','none','MarkerEdgeColor','b','Marker','o');
    
    rp_idx = find(~res(:,1));
    red_points = test_points(rp_idx, :);
    plot3(red_points(:,1),red_points(:,2),red_points(:,3),'LineStyle','none','MarkerEdgeColor','r','Marker','o');
    
return