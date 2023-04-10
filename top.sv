`include "include/aes.svh"
`include "include/flops.svh"

/* Encrypt Args
 * Param Nk - Number of 32-bit words comprising the Cipher Key. For this standard, Nk = 4, 6, or 8. 
 * Param Nr - Number of rounds, which is a function of Nk and Nb (which is fixed). For this standard, Nr = 10, 12, or 14. 
 * Input clk, rst_n
 * Input pt[127:0]  - plain text 
 * Input load - plain text ready
 * key[32*Nk-1:0] - Cipher Key
 * Output ct[127:0]  - cipher text 
 * Output valid
 */

/* Decrypt Args
 * Param Nk - Number of 32-bit words comprising the Cipher Key. For this standard, Nk = 4, 6, or 8. 
 * Param Nr - Number of rounds, which is a function of Nk and Nb (which is fixed). For this standard, Nr = 10, 12, or 14. 
 * Input clk, rst_n
 * Input ct[127:0]  - cipher text 
 * Input load - cipher text ready
 * key[32*Nk-1:0] - Cipher Key
 * Output pt[127:0]  - plain text 
 * Output valid
 */


 //divides the fclk by 4
module clk_div(input logic fclk, output logic sclk, input logic reset_n);

  logic [3:0] div;

  always_ff @(posedge fclk, negedge reset_n) begin
    if (~reset_n) begin
      div <= 4'd1;
    end 
    else begin
      div <= {div[2:0], div[3]};
    end
  end

  assign sclk = div[3];

endmodule : clk_div


 module top #(parameter Nk = 4,
              parameter Nr = Nk + 6)
            (output logic[7:0] LEDR,
            input logic CLOCK_50, 
            input  logic [7:0] SW);

    logic encrypt_load, decrypt_load, rst_n, encrypt_valid, decrypt_valid;
    
    logic [127:0] in, decrypted128, encrypted128, expected128;

    logic [32*Nk-1:0] key128;

    logic correctEnc, correctDec;

    logic CLOCK_SLOW;

    clk_div (.fclk(CLOCK_50), .sclk(CLOCK_SLOW), .reset_n(1'b1));

    always_comb begin
        in = 128'h00112233445566778899aabbccddeeff;
        key128 = 128'h000102030405060708090a0b0c0d0e0f;
        expected128 = 128'h69c4e0d86a7b0430d8cdb78070b4c55a;

        correctEnc = (encrypted128 == expected128) ? 1'b1 : 1'b0;
        correctDec = (decrypted128 == in) ? 1'b1 : 1'b0;
    end

    assign encrypt_load = SW[0];
    assign decrypt_load = SW[1];
    
	 assign LEDR[0] = (correctEnc && encrypt_valid) ? 1'b1 : 1'b0;
	 assign LEDR[1] = (correctDec && decrypt_valid) ? 1'b1 : 1'b0;
     
    aes_encrypt #(Nk, Nr) encrypt_dut(.clk(CLOCK_SLOW), .rst_n(1'b1), .pt(in), .key(key128), .ct(encrypted128), .valid(encrypt_valid), .load(encrypt_load));
    

 endmodule: top