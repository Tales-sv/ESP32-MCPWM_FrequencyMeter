# ESP32-MCPWM_FrequencyMeter
ESP32 frequency Meter made with MCPWM Capture module. 
- Tested with 1hz to 100khz, should work with higher values too.
- The time of measurement is near the period of the frequency measured.
- Have an aproximated +- 0,5% error. (with lower frequency it's even lower)

It uses the **new method** to use the MCPWM Capture, and not the deprecated one.

New Capture enable function:
```cpp
esp_err_t mcpwm_capture_enable_channel(mcpwm_unit_t mcpwm_num, mcpwm_capture_channel_id_t cap_channel, const mcpwm_capture_config_t *cap_conf);
```
Deprecated Capture enable function: (Not used)
```cpp
esp_err_t mcpwm_capture_enable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig, mcpwm_capture_on_edge_t cap_edge, uint32_t num_of_pulse);
```
# Setting MCPWM Capture:
- ## Config Object
Object type with all the capture configs. Pointed in enable function, to effectively configurate the channel. Notice that the inputs have some restrictions, these are listed below:
```cpp
mcpwm_capture_config_t MCPWM_cap_config = { //Tha's my capture config, for example
  .cap_edge = MCPWM_POS_EDGE,               /*!<Set capture edge*/
  .cap_prescale = 1,                        /*!<Prescale of capture signal, ranging from 1 to 256 */
  .capture_cb = cap_ISR_cb,                 /*!<User defined capture event callback, running under interrupt context */
  .user_data = nullptr,                     /*!<User defined ISR callback function args*/
};
```
 - ### The Input Values:
    - `cap_edge`: The Interrupt edge of the signal, in `mcpwm_capture_on_edge_t` type. Can be:
      - MCPWM_NEG_EDGE
      - MCPWM_POS_EDGE
      - MCPWM_BOTH_EDGE

    - `cap_prescale`: The preescale of the capture timer.
      - `uint32_t` 1 to 256 

    - `capture_cb`: The callback function, which will be called every successful capture. Need to be in `cap_isr_cb_t` type, describe in the ISR section below.

    - `user_data`: A pointer with the user defined function args. Passed to ISR function.

- ## ISR
The Callback function. Must be in `cap_isr_cb_t` type, so it'll only recognized in the correct type with that format, need to have all these exact arguments, and return the bool value.
```cpp
bool cap_ISR_cb(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata,void *user_data);
```
The type definition inside library:
```cpp
typedef bool (*cap_isr_cb_t)(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data);
```
The `bool` _return_: Whether a task switch is needed after the callback function returns, this is usually due to the callback wakes up some high priority task.

 - ### The arguments:
    - `mcpwm_unit_t mcpwm`: MCPWM unit(0-1).

    - `mcpwm_capture_channel_id_t cap_channel`: The capture channel ID.

    - `const cap_event_data_t *edata`: Capture event data, contains capture edge and capture value.
        `cap_event_data_t` definition:
       ```cpp
        typedef struct {
       mcpwm_capture_on_edge_t cap_edge; /*!<Which signal edge is detected*/
       uint32_t cap_value;               /*!<Corresponding timestamp when event occurs. Clock rate = APB(usually 80M)*/
       } cap_event_data_t;
        ```
       Notice that is'nt the only way to get capture value. The function below also returns that value:
       ```cpp
       uint32_t mcpwm_capture_signal_get_value(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig);
       ``` 
    - `void *user_data`:  User registered data, passed from `mcpwm_capture_config_t`.
  
- ## GPIO Set
  Setting the gpio as Capture input. That function can init the gpio for any MCPWM Module (selected in `mcpwm_io_signals_t io_signal`).
```cpp
esp_err_t mcpwm_gpio_init(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, int gpio_num);
```
  `esp_err_t` _returns_: Error flag, can be checked with `ESP_ERROR_CHECK()`

   - ### The arguments:
     - `mcpwm_unit_t mcpwm_num`: MCPWM unit(0-1).
     - `mcpwm_io_signals_t io_signal`: Select wich MCPWM module io will be set. For capture:
       - MCPWM_CAP_0
       - MCPWM_CAP_1
       - MCPWM_CAP_2
     - `int gpio_num`: The gpio number.
- ## Capture Enable
  That function enable the capture with the settings in `mcpwm_capture_config_t` given.
```cpp
esp_err_t mcpwm_capture_enable_channel(mcpwm_unit_t mcpwm_num, mcpwm_capture_channel_id_t cap_channel, const mcpwm_capture_config_t *cap_conf);
```
  `esp_err_t` _returns_: Error flag, can be checked with `ESP_ERROR_CHECK()`

   - ### The arguments:
     - `mcpwm_unit_t mcpwm_num`: MCPWM unit(0-1).
     - `mcpwm_capture_channel_id_t cap_channel`: Capture channel to enable.
       - MCPWM_SELECT_CAP0
       - MCPWM_SELECT_CAP1
       - MCPWM_SELECT_CAP2
     - `const mcpwm_capture_config_t *cap_conf`: Point to object type with all the capture configs.
