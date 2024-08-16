
module TimerWithClock (
	clk_clk,
	led_external_connection_export,
	sseg_hour_external_connection_export,
	sseg_min_external_connection_export,
	sseg_sec_external_connection_export,
	switch_external_connection_export,
	buttons_external_connection_export);	

	input		clk_clk;
	output	[9:0]	led_external_connection_export;
	output	[13:0]	sseg_hour_external_connection_export;
	output	[13:0]	sseg_min_external_connection_export;
	output	[13:0]	sseg_sec_external_connection_export;
	input		switch_external_connection_export;
	input	[3:0]	buttons_external_connection_export;
endmodule
