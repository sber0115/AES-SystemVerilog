

module aes_dumb_tb;

    initial begin
        logic[127:0] in = 128'h00112233445566778899aabbccddeeff;
        logic[127:0] key128 = 128'h000102030405060708090a0b0c0d0e0f;
        logic[127:0] expected128 = 128'h69c4e0d86a7b0430d8cdb78070b4c55a;
    end

    logic clock;
    logic rst_n;
    logic encrypt_load, encrypt_valid;

    initial begin 
        clock = 1'b0;
        forever #100 clock = ~clock;
    end

    initial begin
        $monitor($time,, {"rst_n: %b, valid: %b, encrypted128: %h"}, 
                            encrypt_valid, encrypted128);
        
    end

    initial begin
        rst_n = 1'b0;
        #100 rst_n = 1'b1;
        
        encrypt_load = 1'b1; 
        @(posedge encrypt_valid);
        @(posedge clock);
        @(posedge clock);
        @(posedge clock);
        @(posedge clock);
        $display("Finished!");
        $finish;       
    end

    aes_encrypt (.clk(clock), .rst_n, .pt(in), .key(key128), .ct(encrypted128), 
                            .valid(encrypt_valid), .load(encrypt_load));

endmodule: aes_dumb_tb