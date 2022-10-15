
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
namespace octo_max31855 {

class OCTO_MAX31855Component : public Component {
 public:
  
  void setup() override;
  //void dump_config() override;
  float get_setup_priority() const override;

  void activate_ch(uint8_t pin);

  /// set the pin connected to multiplexer control pin 0
  void set_T0(InternalGPIOPin *pin) { this->T0_ = pin; }
  /// set the pin connected to multiplexer control pin 1
  void set_T1(InternalGPIOPin *pin) { this->T1_ = pin; }
  /// set the pin connected to multiplexer control pin 2
  void set_T2(InternalGPIOPin *pin) { this->T2_ = pin; }
  
  /// set the delay needed after an input switch
  void set_mux_delay(uint32_t mux_delay) { this->mux_delay_ = mux_delay; }

 private:
  InternalGPIOPin *T0_;
  InternalGPIOPin *T1_;
  InternalGPIOPin *T2_;
  uint32_t mux_delay_;
  
};

class OCTO_MAX31855Sensor : public sensor::Sensor,
                       public PollingComponent,
                       public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_HIGH,
                                             spi::CLOCK_PHASE_TRAILING, spi::DATA_RATE_4MHZ> {
 public:

  OCTO_MAX31855Sensor(OCTO_MAX31855Component *parent);

  void set_reference_sensor(sensor::Sensor *temperature_sensor) { temperature_reference_ = temperature_sensor; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void update() override;

  void set_channel(uint8_t channel){channel_ = channel;}

 protected:
  OCTO_MAX31855Component *parent_;
  void read_data_();
  sensor::Sensor *temperature_reference_{nullptr};
  uint8_t channel_;
  

};

}  // namespace octo_max31855
}  // namespace esphome
