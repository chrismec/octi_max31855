#include "octo_max31855.h"

#include "esphome/core/log.h"

namespace esphome {
namespace octo_max31855 {

static const char *const TAG = "octo_max31855";

float OCTO_MAX31855Component::get_setup_priority() const { return setup_priority::DATA; }



void OCTO_MAX31855Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up OCTO_MAX31855Component...");
  
  this->T0_->setup();
  this->T1_->setup();
  this->T2_->setup();
  //this->mux_delay_ = 125;
  this->activate_ch(0);
}

void OCTO_MAX31855Component::activate_ch(uint8_t ch) {
   ESP_LOGD(TAG, "switch to channel %d, with %d msec settling time", ch, this->mux_delay_);
   static int mux_channel[8][3] = {
       {0, 0, 0},  // channel 0
       {1, 0, 0},  // channel 1
       {0, 1, 0},  // channel 2
       {1, 1, 0},  // channel 3
       {0, 0, 1},  // channel 4
       {1, 0, 1},  // channel 5
       {0, 1, 1},  // channel 6
       {1, 1, 1}   // channel 7
   };
   this->T0_->digital_write(mux_channel[ch][0]);
   this->T1_->digital_write(mux_channel[ch][1]);
   this->T2_->digital_write(mux_channel[ch][2]);
   // small delay is needed to let the multiplexer switch
   delay(this->mux_delay_);
}

OCTO_MAX31855Sensor::OCTO_MAX31855Sensor(OCTO_MAX31855Component *parent) : parent_(parent) {}

float OCTO_MAX31855Sensor::get_setup_priority() const { return this->parent_->get_setup_priority() - 1.0f; }

void OCTO_MAX31855Sensor::setup() {
  //this->parent_->setup();

  ESP_LOGCONFIG(TAG, "Setting up OCTO_MAX31855Sensor '%s'...", this->name_.c_str());
  this->spi_setup();
}

void OCTO_MAX31855Sensor::update() {
  this->enable();
  delay(1);
  // conversion initiated by rising edge
  this->disable();

  // Conversion time typ: 170ms, max: 220ms
  auto f = std::bind(&OCTO_MAX31855Sensor::read_data_, this);
  this->set_timeout("value", 220, f);
}

void OCTO_MAX31855Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "octo_MAX31855:");
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Thermocouple", this);
  if (this->temperature_reference_) {
    LOG_SENSOR("  ", "Reference", this->temperature_reference_);
  } else {
    ESP_LOGCONFIG(TAG, "  Reference temperature disabled.");
  }
}

void OCTO_MAX31855Sensor::read_data_() {
  this->parent_->activate_ch(this->channel_);
  this->enable();
  delay(1);
  uint8_t data[4];
  this->read_array(data, 4);
  this->disable();

  const uint32_t mem = encode_uint32(data[0], data[1], data[2], data[3]);

  // Verify we got data
  if (mem != 0xFFFFFFFF) {
    this->status_clear_error();
  } else {
    ESP_LOGE(TAG, "No data received from OCTO_MAX31855 (0x%08X). Check wiring!", mem);
    this->publish_state(NAN);
    if (this->temperature_reference_) {
      this->temperature_reference_->publish_state(NAN);
    }
    this->status_set_error();
    return;
  }

  // Internal reference temperature always works
  if (this->temperature_reference_) {
    int16_t val = (mem & 0x0000FFF0) >> 4;
    if (val & 0x0800) {
      val |= 0xF000;  // Pad out 2's complement
    }
    //const float t_ref = float(val) * 0.0625f; //C
    const float t_ref = (float(val) * 0.0625f) * (9.0/5.0) + 32.0; // F
    ESP_LOGD(TAG, "Got reference temperature: %.4f°F", t_ref);
    this->temperature_reference_->publish_state(t_ref);
  }

  // Check thermocouple faults
  if (mem & 0x00000001) {
    ESP_LOGW(TAG, "Thermocouple open circuit (not connected) fault from octo_MAX31855 (0x%08X)", mem);
    this->publish_state(NAN);
    this->status_set_warning();
    return;
  }
  if (mem & 0x00000002) {
    ESP_LOGW(TAG, "Thermocouple short circuit to ground fault from octo_MAX31855 (0x%08X)", mem);
    this->publish_state(NAN);
    this->status_set_warning();
    return;
  }
  if (mem & 0x00000004) {
    ESP_LOGW(TAG, "Thermocouple short circuit to VCC fault from octo_MAX31855 (0x%08X)", mem);
    this->publish_state(NAN);
    this->status_set_warning();
    return;
  }
  if (mem & 0x00010000) {
    ESP_LOGW(TAG, "Got faulty reading from octo_MAX31855 (0x%08X)", mem);
    this->publish_state(NAN);
    this->status_set_warning();
    return;
  }

  // Decode thermocouple temperature
  int16_t val = (mem & 0xFFFC0000) >> 18;
  if (val & 0x2000) {
    val |= 0xC000;  // Pad out 2's complement
  }
  //const float t_sense = float(val) * 0.25f; // C
  const float t_sense = (float(val) * 0.25f) * (9.0/5.0) + 32.0; // F
  ESP_LOGD(TAG, "Got thermocouple temperature: %.2f°F", t_sense);
  this->publish_state(t_sense);
  this->status_clear_warning();
}

}  // namespace octo_max31855
}  // namespace esphome
