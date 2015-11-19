function drow_sector(el_ang, el_width, az_ang, az_width, r)
    % Number of polygons
    N = 5;
    
    r_spaced = linspace(0,r,N);
    theta_spaced = linspace(degtorad(az_ang - az_width),degtorad(az_ang + az_width),N);
    phi_spaced = linspace(degtorad(el_ang - el_width),degtorad(el_ang + el_width),N);
    
    % Front mesh 
    [theta,phi] = meshgrid(theta_spaced, phi_spaced);
    x1 = r.*cos(theta).*cos(phi);
    y1 = r.*sin(theta).*cos(phi);
    z1 = r.*sin(phi);
    
    % Left side mesh
    [r,phi] = meshgrid(r_spaced,phi_spaced);
    theta = degtorad(az_ang + az_width);
    x2 = r.*cos(theta).*cos(phi);
    y2 = r.*sin(theta).*cos(phi);
    z2 = r.*sin(phi);
    % Right side mesh
    theta = degtorad(az_ang - az_width);
    x3 = r.*cos(theta).*cos(phi);
    y3 = r.*sin(theta).*cos(phi);
    z3 = r.*sin(phi);
    
    % Top mesh
    [r,theta] = meshgrid(r_spaced, theta_spaced);
    phi = degtorad(el_ang + el_width);
    x4 = r.*cos(theta).*cos(phi);
    y4 = r.*sin(theta).*cos(phi);
    z4 = r.*sin(phi);
    
    % Bottom mesh
    phi = degtorad(el_ang - el_width);
    x5 = r.*cos(theta).*cos(phi);
    y5 = r.*sin(theta).*cos(phi);
    z5 = r.*sin(phi);
    
    % Combine meshes
    x = [x1 x2 x3 x4 x5];
    y = [y1 y2 y3 y4 y5];
    z = [z1 z2 z3 z4 z5];
    
    % Drow meshes
    surf(x,y,z, 'FaceColor', 'none');
    axis square
end