# Stop-Watch
*Stop Watch system using ATmega32 , six Common Anode 7-segments, and a 7447 decoder.
--> Timer1 is used with CTC mode to count the Stop Watch time.
--> The six 7-segments in the project using the multiplexed technique.
--> First 6-pins in PORTA as the enable/disable pins for the six 7-segments.
--> External Interrupt INT0 with falling edge. Push button with the internal pull-up resistor. If a falling edge detected. the Stop Watch time reset.
--> External Interrupt INT1 with raising edge. Push button with the external pull-down resistor. If a raising edge detected the Stop Watch time pause.
--> External Interrupt INT2 with falling edge. Push button with the internal pull-up resistor. If a falling edge detected the Stop Watch time resume.
--> The SET button swtich to alarm mode.
--> + buttom increase the digit that you are selecting.
--> - buttom decrease the digit that you are selecting.
--> arrow button for selecting the digit.


