function [Blx, Bly, Blz] = MagneticFluxDensityComponent(N_T,m,n,p,a,b,c,x,y,z)
Rl = sqrt((x-a)^2+(y-b)^2+(z-c)^2);
comm = 3*(m*(x-a)+n*(y-b)+p*(z-c));
Blx = N_T*((comm*(x-a))/(Rl^5)-m/(Rl^3));
Bly = N_T*((comm*(y-b))/(Rl^5)-n/(Rl^3));
Blz = N_T*((comm*(z-c))/(Rl^5)-p/(Rl^3));
end
