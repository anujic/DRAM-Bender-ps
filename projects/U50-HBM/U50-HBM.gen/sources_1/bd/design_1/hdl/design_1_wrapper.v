//Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2022.1 (lin64) Build 3526262 Mon Apr 18 15:47:01 MDT 2022
//Date        : Fri Nov 22 16:59:33 2024
//Host        : tardis-b09 running 64-bit Debian GNU/Linux 10 (buster)
//Command     : generate_target design_1_wrapper.bd
//Design      : design_1_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module design_1_wrapper
   (Clk,
    M_AXIS_0_tdata,
    M_AXIS_0_tkeep,
    M_AXIS_0_tlast,
    M_AXIS_0_tready,
    M_AXIS_0_tvalid,
    S_AXIS_0_tdata,
    S_AXIS_0_tready,
    S_AXIS_0_tvalid,
    aresetn_0,
    reset_rtl,
    uart_msp_0_rxd,
    uart_msp_0_txd);
  input Clk;
  output [255:0]M_AXIS_0_tdata;
  output [31:0]M_AXIS_0_tkeep;
  output M_AXIS_0_tlast;
  input M_AXIS_0_tready;
  output M_AXIS_0_tvalid;
  input [255:0]S_AXIS_0_tdata;
  output S_AXIS_0_tready;
  input S_AXIS_0_tvalid;
  input aresetn_0;
  input reset_rtl;
  input uart_msp_0_rxd;
  output uart_msp_0_txd;

  wire Clk;
  wire [255:0]M_AXIS_0_tdata;
  wire [31:0]M_AXIS_0_tkeep;
  wire M_AXIS_0_tlast;
  wire M_AXIS_0_tready;
  wire M_AXIS_0_tvalid;
  wire [255:0]S_AXIS_0_tdata;
  wire S_AXIS_0_tready;
  wire S_AXIS_0_tvalid;
  wire aresetn_0;
  wire reset_rtl;
  wire uart_msp_0_rxd;
  wire uart_msp_0_txd;

  design_1 design_1_i
       (.Clk(Clk),
        .M_AXIS_0_tdata(M_AXIS_0_tdata),
        .M_AXIS_0_tkeep(M_AXIS_0_tkeep),
        .M_AXIS_0_tlast(M_AXIS_0_tlast),
        .M_AXIS_0_tready(M_AXIS_0_tready),
        .M_AXIS_0_tvalid(M_AXIS_0_tvalid),
        .S_AXIS_0_tdata(S_AXIS_0_tdata),
        .S_AXIS_0_tready(S_AXIS_0_tready),
        .S_AXIS_0_tvalid(S_AXIS_0_tvalid),
        .aresetn_0(aresetn_0),
        .reset_rtl(reset_rtl),
        .uart_msp_0_rxd(uart_msp_0_rxd),
        .uart_msp_0_txd(uart_msp_0_txd));
endmodule
