	TimerWithClock u0 (
		.clk_clk                              (<connected-to-clk_clk>),                              //                           clk.clk
		.led_external_connection_export       (<connected-to-led_external_connection_export>),       //       led_external_connection.export
		.sseg_hour_external_connection_export (<connected-to-sseg_hour_external_connection_export>), // sseg_hour_external_connection.export
		.sseg_min_external_connection_export  (<connected-to-sseg_min_external_connection_export>),  //  sseg_min_external_connection.export
		.sseg_sec_external_connection_export  (<connected-to-sseg_sec_external_connection_export>),  //  sseg_sec_external_connection.export
		.switch_external_connection_export    (<connected-to-switch_external_connection_export>),    //    switch_external_connection.export
		.buttons_external_connection_export   (<connected-to-buttons_external_connection_export>)    //   buttons_external_connection.export
	);

