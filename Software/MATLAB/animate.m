% 所有单位均采用国际单位制（SI)，坐标系采用右手系
pic_num=1;

for frame = -15:15
    
mag_L = 0.003; % 永磁体圆柱长度
mag_b = 0.003; % 永磁体圆柱半径


N_T=1; % 系数
% 永磁体位置
a = frame * 0.001;       % x
b = 0;       % y
c = 0.03;    % z
% 永磁体朝向
m = 0;
n = 0;
p = 1;
H_0 = [m;n;p];
H_0 = normalize(H_0,"norm"); % 归一化向量（按L2范数缩放）
% 传感器
sensor_n = 5; % 传感器行列数
sensor_interval = 0.01; % 传感器间隔
sensor_N = sensor_n^2; % 传感器数量N
sensor_pos = zeros(sensor_N,3); % 传感器位置
for i = 1:sensor_n % y
    for j = 1:sensor_n % x
        sensor_pos((i-1)*sensor_n + j,1)=((j-1)-(sensor_n-1)/2)*sensor_interval;   % x
        sensor_pos((i-1)*sensor_n + j,2)=((i-1)-(sensor_n-1)/2)*sensor_interval;   % y
        sensor_pos((i-1)*sensor_n + j,3)=0;                                        % z
    end
end

Bl = zeros(sensor_N,3);
% 磁通密度
for i = 1:sensor_N
   [Bl(i,1),Bl(i,2),Bl(i,3)] = MagneticFluxDensityComponent(N_T,H_0(1),H_0(2),H_0(3),a,b,c,sensor_pos(i,1),sensor_pos(i,2),sensor_pos(i,3)); 
end

% 画出各个传感器获得的磁通密度矢量
%figure;
quiver3(sensor_pos(:,1),sensor_pos(:,2),sensor_pos(:,3),Bl(:,1),Bl(:,2),Bl(:,3));
hold on;
[X,Y,Z]=cylinder(mag_b/2);
X=X+a;
Y=Y+b;
Z=Z*mag_L+c;
s = surf(X,Y,Z);
view([-38.7551 10.6989]);
xlabel('X')
ylabel('Y')
zlabel('Z')
hold off;
drawnow;

% 保存动画
F=getframe(gcf);
I=frame2im(F);
[I,map]=rgb2ind(I,256);

if pic_num == 1
        imwrite(I,map,'test.gif','gif', 'Loopcount',inf,'DelayTime',0.05);
else
        imwrite(I,map,'test.gif','gif','WriteMode','append','DelayTime',0.05);
end

pic_num=pic_num+1;

end;