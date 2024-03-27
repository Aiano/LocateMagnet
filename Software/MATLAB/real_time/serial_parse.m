function Bl = serial_parse(ser, N)
Bl = [];
err_cnt = 1;

while true
    flush(ser);
    stringLine = readline(ser);
    if(strncmp(stringLine,"U",1))
        newStr = strip(stringLine,"U");
        U = sscanf(newStr,"%d");


        stringLine = readline(ser);
        if(strncmp(stringLine,"V",1))
            newStr = strip(stringLine,"V");
            V = sscanf(newStr,"%d");
        end

        stringLine = readline(ser);
        if(strncmp(stringLine,"W",1))
            newStr = strip(stringLine,"W");
            W = sscanf(newStr,"%d");
        end

        if(length(U)==N && length(V)==N && length(W)==N)
            Bl(:,1) = U;
            Bl(:,2) = V;
            Bl(:,3) = W;
        end
    end

    if isempty(Bl)
        err_cnt = err_cnt + 1;
    else
        fprintf("Try read serialport times: %d", err_cnt);
        break
    end

    if err_cnt >= 10
        error("Fail too many times.")
    end
end

end
