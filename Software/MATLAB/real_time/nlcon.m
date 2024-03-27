function [c,ceq] = nlcon(param)
c = [];

m = param(1);
n = param(2);
p = param(3);
ceq = m^2 + n^2 + p^2 - 1;
end