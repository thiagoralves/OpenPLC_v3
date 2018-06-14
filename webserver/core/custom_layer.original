#include "ladder.h"

//-----------------------------------------------------------------------------
// DISCLAIMER: EDDITING THIS FILE CAN BREAK YOUR OPENPLC RUNTIME! IF YOU DON'T
// KNOW WHAT YOU'RE DOING, JUST DON'T DO IT. EDIT AT YOUR OWN RISK.
//
// PS: You can always restore original functionality if you broke something
// in here by clicking on the "Restore Original Code" button above.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// These are the ignored I/O vectors. If you want to override how OpenPLC
// handles a particular input or output, you must put them in the ignored
// vectors. For example, if you want to override %IX0.5, %IX0.6 and %IW3
// your vectors must be:
//     int ignored_bool_inputs[] = {5, 6}; //%IX0.5 and %IX0.6 ignored
//     int ignored_int_inputs[] = {3}; //%IW3 ignored
//
// Every I/O on the ignored vectors will be skipped by OpenPLC hardware layer
//-----------------------------------------------------------------------------
int ignored_bool_inputs[] = {-1};
int ignored_bool_outputs[] = {-1};
int ignored_int_inputs[] = {-1};
int ignored_int_outputs[] = {-1};

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures for your custom layer should be here.
//-----------------------------------------------------------------------------
void initCustomLayer()
{
}

//-----------------------------------------------------------------------------
// This function is called by OpenPLC in a loop. Here the internal input
// buffers must be updated with the values you want. Make sure to use the mutex 
// bufferLock to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateCustomIn()
{
    // Example Code - Overwritting %IW3 with a fixed value
    // If you want to have %IW3 constantly reading a fixed value (for example, 53)
    // you must add %IW3 to the ignored vectors above, and then just insert this 
    // single line of code in this function:
    //     if (int_input[3] != NULL) *int_input[3] = 53;
}

//-----------------------------------------------------------------------------
// This function is called by OpenPLC in a loop. Here the internal output
// buffers must be updated with the values you want. Make sure to use the mutex 
// bufferLock to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateCustomOut()
{
    // Example Code - Sending %QW5 value over I2C
    // If you want to have %QW5 output to be sent over I2C instead of the
    // traditional output for your board, all you have to do is, first add
    // %QW5 to the ignored vectors, and then define a send_over_i2c()
    // function for your platform. Finally you can call send_over_i2c() to 
    // send your %QW5 value, like this:
    //     if (int_output[5] != NULL) send_over_i2c(*int_output[5]);
    //
    // Important observation: If your I2C pins are used by OpenPLC I/Os, you
    // must also add those I/Os to the ignored vectors, otherwise OpenPLC
    // will try to control your I2C pins and your I2C message won't work.
}