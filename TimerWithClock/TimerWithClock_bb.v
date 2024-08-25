
module TimerWithClock (
	buttons_export,
	clk_clk,
	leds_export,
	sseg_hour_tens_export,
	sseg_hour_units_export,
	sseg_min_units_export,
	sseg_mins_tens_export,
	sseg_sec_tens_export,
	sseg_sec_units_export,
	switch_export,
	buzzer_export);	

	input	[3:0]	buttons_export;
	input		clk_clk;
	output	[9:0]	leds_export;
	output	[6:0]	sseg_hour_tens_export;
	output	[6:0]	sseg_hour_units_export;
	output	[6:0]	sseg_min_units_export;
	output	[6:0]	sseg_mins_tens_export;
	output	[6:0]	sseg_sec_tens_export;
	output	[6:0]	sseg_sec_units_export;
	input		switch_export;
	output		buzzer_export;
endmodule
