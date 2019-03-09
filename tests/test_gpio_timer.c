#include "../assert.h"
#include "../gpio.h"
#include "../timer.h"

void test_gpio_set_get_function() {
    gpio_init();

    // Test get pin function
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_INPUT );
    assert( gpio_get_function(GPIO_PIN30) == GPIO_FUNC_INPUT );
    assert( gpio_get_function(GPIO_PIN19) == GPIO_FUNC_INPUT );

    // Test set pin to output
    gpio_set_output(GPIO_PIN2);
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_OUTPUT );
    gpio_set_output(GPIO_PIN30);
    assert( gpio_get_function(GPIO_PIN30) == GPIO_FUNC_OUTPUT );
    gpio_set_output(GPIO_PIN51);
    assert( gpio_get_function(GPIO_PIN51) == GPIO_FUNC_OUTPUT );
    gpio_set_output(GPIO_PIN20);
    assert( gpio_get_function(GPIO_PIN20) == GPIO_FUNC_OUTPUT );
    gpio_set_output(GPIO_PIN19);
    assert( gpio_get_function(GPIO_PIN19) == GPIO_FUNC_OUTPUT );

    //Test set pin to gpio_set_input
    gpio_set_input(GPIO_PIN30);
    assert( gpio_get_function(GPIO_PIN30) == GPIO_FUNC_INPUT );
    gpio_set_input(GPIO_PIN50);
    assert( gpio_get_function(GPIO_PIN50) == GPIO_FUNC_INPUT );
    gpio_set_input(GPIO_PIN5);
    assert( gpio_get_function(GPIO_PIN5) == GPIO_FUNC_INPUT );
    gpio_set_input(GPIO_PIN0);
    assert( gpio_get_function(GPIO_PIN0) == GPIO_FUNC_INPUT );
    gpio_set_input(GPIO_PIN9);
    assert( gpio_get_function(GPIO_PIN9) == GPIO_FUNC_INPUT );
    gpio_set_input(GPIO_PIN19);
    assert( gpio_get_function(GPIO_PIN19) == GPIO_FUNC_INPUT );

    //Test alternate functions
    gpio_set_function(GPIO_PIN9, GPIO_FUNC_ALT0);
    assert( gpio_get_function(GPIO_PIN9) == GPIO_FUNC_ALT0 );
    gpio_set_function(GPIO_PIN12, GPIO_FUNC_ALT1);
    assert( gpio_get_function(GPIO_PIN12) == GPIO_FUNC_ALT1 );

}

void test_gpio_read_write() {
    gpio_init();
    gpio_set_function(GPIO_PIN20, GPIO_FUNC_OUTPUT);

    // Test gpio_write low, then gpio_read
    gpio_write(GPIO_PIN20, 0);
    assert( gpio_read(GPIO_PIN20) ==  0 );

   // Test gpio_write high, then gpio_read
    gpio_write(GPIO_PIN20, 1);
    assert( gpio_read(GPIO_PIN20) ==  1 );

    gpio_init();
    gpio_set_function(GPIO_PIN30, GPIO_FUNC_OUTPUT);

    // Test gpio_write low, then gpio_read
    gpio_write(GPIO_PIN30, 0);
    assert( gpio_read(GPIO_PIN30) ==  0 );

   // Test gpio_write high, then gpio_read
    gpio_write(GPIO_PIN30, 1);
    assert( gpio_read(GPIO_PIN30) ==  1 );

    gpio_init();
    gpio_set_function(GPIO_PIN9, GPIO_FUNC_OUTPUT);

    // Test gpio_write low, then gpio_read
    gpio_write(GPIO_PIN9, 0);
    assert( gpio_read(GPIO_PIN9) ==  0 );

   // Test gpio_write high, then gpio_read
    gpio_write(GPIO_PIN9, 1);
    assert( gpio_read(GPIO_PIN9) ==  1 );

    gpio_init();
    gpio_set_function(GPIO_PIN33, GPIO_FUNC_OUTPUT);

    // Test gpio_write low, then gpio_read
    gpio_write(GPIO_PIN33, 0);
    assert( gpio_read(GPIO_PIN33) ==  0 );

   // Test gpio_write high, then gpio_read
    gpio_write(GPIO_PIN33, 1);
    assert( gpio_read(GPIO_PIN33) ==  1 );

    // Test invalid
     gpio_write(GPIO_PIN33, 1);
     assert(gpio_read(100) ==  -1);
}

void test_timer() {
    timer_init();

    // Test timer tick count incrementing
    unsigned int start = timer_get_ticks();
    for( int i=0; i<10; i++ ) { /* Spin */ }
    unsigned int finish = timer_get_ticks();
    assert( finish > start );

    // Test timer delay
    int usecs = 100;
    start = timer_get_ticks();
    timer_delay_us(usecs);
    finish = timer_get_ticks();
    assert( finish >= start + usecs );
}

// Uncomment each call below when you have implemented the functions
// and are ready to test them

void main(void) {
    test_gpio_set_get_function();
    test_gpio_read_write();
    test_timer();
}
