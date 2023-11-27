// ESP32 FrequencyMeter with MCPWM Capture module
// Author: Tales-sv

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "soc/rtc.h"
#include <Arduino.h>

#define GPIO_CAP0_IN   23   //Set GPIO 23 as CAP0 input

//--- mcpwm variables
uint32_t cap_count= 0;  //cycles between cap interrupts
uint32_t last_count= 0; //last cap_count value
uint32_t cap_calc= 0;   //calculated frequency

bool cap_ISR_cb(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata,void *user_data){ //this function need to be in that format to be recognized as cap_isr_cb_t type

// cap_count = (uint32_t) edata->cap_value; //same as mcpwm_capture_signal_get_value()
cap_count = mcpwm_capture_signal_get_value(MCPWM_UNIT_0, MCPWM_SELECT_CAP0);

// calculating the frequency: 
cap_calc = 1/((cap_count - last_count)*0.0000000125); //Count freq = 80Mhz, Period= 0.0000000125
// cap_calc = cap_count - last_count; //Count freq = 80Mhz, Period= 0.0000000125

last_count = cap_count;

digitalWrite(2,HIGH); //sinalize an interrupt call

return 0; //Whether a task switch is needed after the callback function returns, this is usually due to the callback wakes up some high priority task.

}

mcpwm_capture_config_t MCPWM_cap_config = { //Capture channel configuration
  .cap_edge = MCPWM_POS_EDGE,               /*!<Set capture edge*/
  .cap_prescale = 1,                        /*!<Prescale of capture signal, ranging from 1 to 256 */
  .capture_cb = cap_ISR_cb,                 /*!<User defined capture event callback, running under interrupt context */
  .user_data = nullptr,                     /*!<User defined ISR callback function args*/
};

void setup(){
  Serial.begin(115200); 

  pinMode(2 ,OUTPUT);    //set led buit in as output, for debug

  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, GPIO_CAP0_IN); //set the cap input pin
  gpio_pulldown_en((gpio_num_t)GPIO_CAP0_IN);               //Enable pull down on CAP0 signal input

  ESP_ERROR_CHECK(mcpwm_capture_enable_channel(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, &MCPWM_cap_config)); //enable and config capture channel. Inside an ESP_ERROR_CHECK() to avoid trouble

  Serial.println("end setup().");

}//end setup()

void loop(){
  // Serial.print(esp_timer_get_time()); //get the measurement time in ms 

  Serial.print("  frequency: ");
  Serial.print(cap_calc); // print the frequency in Hz with less than 0,5% of error

  Serial.println("hz");

  digitalWrite(2,LOW);  // built in led sinalize an interrupt call
  delay(50);

}//end loop()