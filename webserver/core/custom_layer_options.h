//-----------------------------------------------------------------------------
// DISCLAIMER: EDDITING THIS FILE CAN BREAK YOUR OPENPLC RUNTIME! IF YOU DON'T
// KNOW WHAT YOU'RE DOING, JUST DON'T DO IT. EDIT AT YOUR OWN RISK.
//
// PS: You can always restore original functionality if you broke something
// in here by clicking on the "Restore Original Code" button above.
//-----------------------------------------------------------------------------

struct custom_layer_options {
    uint8_t rpi_modbus_rts_pin;     // If <> 0, expect hardware RTS to be used with this pin
};
