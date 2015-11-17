function t_coef = time_coef(t)
    t_coef = [1 t t.^2./2 t.^3./6 t.^4./24 t.^5./120];
end