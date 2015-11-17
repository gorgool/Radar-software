function test_case()
    % Points
    p = [ 
        2499583.9924921603;
        2492584.8913670862;
        2485355.1712997020;
        2477895.5131051424;
        2470206.6186932744;
        2462289.2110114861
        ];
    % Time, sec
    t = [0 10 20 30 40 50];
    plot(t,p,'o');
    % Compute params
    X = get_X6(t);
    Xp = precompute(X);
    disp('Precompute matrix (transposed)');
    disp(Xp');
    params = Xp * p;
    disp(params);
    % Check result
    t_check = 0:0.1:60;
    p_check = zeros(size(t_check));
    for i=1:size(t_check,2)
        p_check(i) = time_coef(t_check(i)) * params;
    end
    
    hold on;
    plot(t_check, p_check);
end