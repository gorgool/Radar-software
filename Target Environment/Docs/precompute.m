function X_coef = precompute(X)
  X_coef = ((X'*X)^-1)*X'
end
