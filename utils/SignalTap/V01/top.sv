// module top(
//     (* chip_pin = "T2" *) input CLK_50M,
//     // --
//     (* chip_pin = "AA13" *) input wire TXD_OUT, 
//     (* chip_pin = "AA14" *) input wire RXD_IN, 
//     (* chip_pin = "AA15" *) input wire IRQ0, 
//     (* chip_pin = "AA16" *) input wire DBG_PIN2,
//     (* chip_pin = "AA17" *) input wire nBLO0, 
//     // --
//     output wire clk_1M, clk_10M, clk_50M, clk_100M, clk_200M
// );

// // wire clk_1M, clk_10M, clk_50M, clk_100M, clk_200M;

// pll pll_inst( .inclk0(CLK_50M), .c0(clk_1M), .c1(clk_10M), .c2(clk_50M), .c3(clk_100M), .c4(clk_200M) );

// endmodule

module top(
    (* chip_pin = "T2" *) input CLK_50M,
    // --
    (* chip_pin = "AA13" *) input wire SCL, 
    (* chip_pin = "AA14" *) input wire SDA, 
    (* chip_pin = "AA15" *) input wire rsrv_0, 
    (* chip_pin = "AA16" *) input wire rsrv_1,
    (* chip_pin = "AA17" *) input wire rsrv_2, 
    // --
    output wire clk_1M, clk_10M, clk_50M, clk_100M, clk_200M
);

// wire clk_1M, clk_10M, clk_50M, clk_100M, clk_200M;

pll pll_inst( .inclk0(CLK_50M), .c0(clk_1M), .c1(clk_10M), .c2(clk_50M), .c3(clk_100M), .c4(clk_200M) );

endmodule

// module top(
//     (* chip_pin = "T2" *) input CLK_50M,
//     // --
//     (* chip_pin = "AA13" *) input wire CONFIG_DONE,     // cabel [1]
//     (* chip_pin = "AA14" *) input wire INIT_DONE,       // cabel [2]
//     (* chip_pin = "AA15" *) input wire DATA0,           // cabel [3]
//     (* chip_pin = "AA16" *) input wire nCONFIG,         // cabel [4]
//     (* chip_pin = "AA17" *) input wire nSTATUS,         // cabel [5]
//     // --
//     output wire clk_1M, clk_10M, clk_50M, clk_100M, clk_200M
// );

// wire clk_1M, clk_10M, clk_50M, clk_100M, clk_200M;

// pll pll_inst( .inclk0(CLK_50M), .c0(clk_1M), .c1(clk_10M), .c2(clk_50M), .c3(clk_100M), .c4(clk_200M) );

// endmodule

