module OnChipRAMWriter(
    input cs,
    input UDS_L,
    input LDS_L,
    input WE_L,
    input AS_L,
    output U_OE,
    output L_OE,
    output U_WREN,
    output L_WREN
);

wire ram_selector;

assign ram_selector = AS_L ? 1'b0 : (WE_L ? 1'b0 : cs);

assign U_WREN = ram_selector & ~UDS_L;
assign L_WREN = ram_selector & ~LDS_L;

assign U_OE = ~UDS_L & WE_L & cs;
assign L_OE = ~LDS_L & WE_L & cs;

endmodule