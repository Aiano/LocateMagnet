N_T = 0.1570; % 系数

sensor_n = 5; % 传感器行列数
sensor_interval = 0.005; % 传感器间隔
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
Bl_compensate = zeros(sensor_N,3);
init = [0 0 1 0 0 0.02]; % 优化初值
lb = [-1 -1 -1 -0.05 -0.05 0]; % 下界
ub = [1 1 1 0.05 0.05 0.1]; % 上界
draw_trace_flag = false; % 轨迹绘制标志位
max_loop_times = 1000; % 最大循环次数
trace = zeros(max_loop_times,6); % 轨迹点 m n p x y z
trace_cnt = 0; % 轨迹点数

delete(instrfindall);
ser = serialport('COM19',115200); % 打开串口

Bl_compensate = serial_parse(ser, sensor_N);

cnt = 0;

while cnt < max_loop_times
    cnt = cnt + 1;

    Bl = serial_parse(ser, sensor_N); % 读取磁场值
    
    Bl(:,1) = - Bl(:,1) + Bl_compensate(:,1); % 地磁场补偿
    Bl(:,2) = Bl(:,2) - Bl_compensate(:,2);
    Bl(:,3) = Bl(:,3) - Bl_compensate(:,3); % Z轴与X、Y相反
    
    quiver3(sensor_pos(:,1),sensor_pos(:,2),sensor_pos(:,3),Bl(:,1),Bl(:,2),Bl(:,3),'-b','LineWidth',1.5);
    hold on;
    xlim([-0.05 0.05]);
    ylim([-0.05 0.05]);
    zlim([-0.01 0.1]);
    xlabel('X');
    ylabel('Y');
    zlabel('Z');
    set(gca,'YDir','reverse');
    
    result = lsqnonlin(@(param) target_function(param,sensor_pos,Bl,N_T),init,lb,ub,[],[],[],[],@nlcon);
    init = result; % 以上次的结果作为下次优化的初值，加快优化速度

    if draw_trace_flag % 绘制轨迹
        trace_cnt = trace_cnt + 1;
        trace(trace_cnt,:) = result;

        plot3(trace(1:trace_cnt,4),trace(1:trace_cnt,5),trace(1:trace_cnt,6),'-r','LineWidth',1);
    end

    plot3(result(4),result(5),result(6),'or','MarkerSize',10,'LineWidth',1.5);
    quiver3(result(4),result(5),result(6),result(1),result(2),result(3),0.01,'-r','LineWidth',1.5)
    hold off;

    fprintf("x: %.4f m\ty: %.4f m\tz: %.4f m\tm: %.4f\tn: %.4f\tp: %.4f\tcnt: %d\t",result(4),result(5),result(6),result(1),result(2),result(3),cnt);

    switch get(gcf,'CurrentCharacter')
        case 'q' % 退出
            close all;
            break
        case 't' % 绘制轨迹
            draw_trace_flag = 1;
        case 'c' % 取消绘制轨迹
            draw_trace_flag = 0;
            trace_cnt = 0;
    end

    %pause(0.1);
end

delete(ser); % 关闭串口