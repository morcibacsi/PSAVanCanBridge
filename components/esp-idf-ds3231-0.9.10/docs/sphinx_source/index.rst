.. esp-idf-ds3221 documentation master file, created by
   sphinx-quickstart on Wed Aug 28 19:33:10 2024.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

============================
esp-idf-ds3221 Documentation
============================

esp-idf-ds3231 is a component for the esp-idf. The component abstracts all the features in to get/set functions. 
The component is 100% feature complete. All time keeping, alarm and control function are part of the comoment, 
including alarm interrupt functionality.

This my first fully documented and shared open source project. I welcome
comments, suggestions and corrections.

The areas I could use help in are the documentation generation and
configurion on readthedics.io.

Usage
=====

Adding to Project
-----------------

To use esp-idf-ds3231, simply use **idf.py** to import the component in to you project:

.. code-block:: console

   idf.py add-dependency "jschwefel/esp-idf-ds3231"

All publically accessable structs, enums, defines and functions are available through *esp-idf-ds3231.h*.

.. code-block:: C

   #include "esp-idf-ds3231.h"


Initialization
--------------

The esp-idf-ds3231 component provides two methof of Initialization:

1. Full Initialization, including I2C bus. 

.. code-block:: C

   i2c_master_dev_handle_t* rtc_handle = ds3231_init_full(GPIO_NUM_X, GPIO_NUM_Y);

2. Partial Initialization, where the IC2 bus is initialized serparately. 

.. code-block:: C

   // Allocte memory for the pointer of i2c_master_bus_handle_t
   i2c_master_bus_handle_t* bus_handle = (i2c_master_bus_handle_t*)malloc(sizeof(i2c_master_bus_handle_t));
   // Create the i2c_master_bus_config_t struct and assign values.
   i2c_master_bus_config_t i2c_mst_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = -1,
      .scl_io_num = GPIO_NUM_X,
      .sda_io_num = GPIO_NUM_Y,
      .glitch_ignore_cnt = 7,
      // The DS3231 **requires** pullup resistors on all of its I/O pins. Note: Some DS3231 boards have pullup resistors as part
      // of their design.
      .flags.enable_internal_pullup = true,
   };
   i2c_new_master_bus(&i2c_mst_config, bus_handle);
   rtc_handle_t* rtc_handle = ds3231_init(bus_handle);


Use method 1 if the DS3231 is the only I2C device conneced to that I2C bus, otherwise use method 2.




.. toctree::
   :maxdepth: 1
   :caption: Contents:

.. doxygenindex::
 