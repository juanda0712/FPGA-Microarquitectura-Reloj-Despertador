	TimerWithClock u0 (
		.buttons_export         (<connected-to-buttons_export>),         //         buttons.export
		.clk_clk                (<connected-to-clk_clk>),                //             clk.clk
		.leds_export            (<connected-to-leds_export>),            //            leds.export
		.sseg_hour_units_export (<connected-to-sseg_hour_units_export>), // sseg_hour_units.export
		.sseg_min_units_export  (<connected-to-sseg_min_units_export>),  //  sseg_min_units.export
		.sseg_sec_tens_export   (<connected-to-sseg_sec_tens_export>),   //   sseg_sec_tens.export
		.sseg_sec_units_export  (<connected-to-sseg_sec_units_export>),  //  sseg_sec_units.export
		.sseg_mins_tens_export  (<connected-to-sseg_mins_tens_export>),  //  sseg_mins_tens.export
		.sseg_hour_tens_export  (<connected-to-sseg_hour_tens_export>),  //  sseg_hour_tens.export
		.switch_export          (<connected-to-switch_export>)           //          switch.export
	);

