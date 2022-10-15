import esphome.codegen as cg
from esphome import pins
from esphome.components import octo_max31855
import esphome.config_validation as cv
from esphome.const import (
    CONF_DELAY,
    CONF_ID,
)

AUTO_LOAD = ["sensor"]
MULTI_CONF = True

octo_max31855_ns = cg.esphome_ns.namespace("octo_max31855")

OCTO_MAX31855Component = octo_max31855_ns.class_(
    "OCTO_MAX31855Component", cg.Component, cg.PollingComponent
)



#CONF_CHANNEL = "channel"
CONF_T0 = "T0"
CONF_T1 = "T1"
CONF_T2 = "T2"
CONF_MUX_DELAY = "mux_delay"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OCTO_MAX31855Component),
        cv.Required(CONF_T0): pins.internal_gpio_output_pin_schema,
        cv.Required(CONF_T1): pins.internal_gpio_output_pin_schema,
        cv.Required(CONF_T2): pins.internal_gpio_output_pin_schema,
        cv.Required(CONF_MUX_DELAY): cv.int_,
        #cv.Required(CONF_CHANNEL): validate_channel,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    T0 = await cg.gpio_pin_expression(config[CONF_T0])
    cg.add(var.set_T0(T0))
    T1 = await cg.gpio_pin_expression(config[CONF_T1])
    cg.add(var.set_T1(T1))
    T2 = await cg.gpio_pin_expression(config[CONF_T2])
    cg.add(var.set_T2(T2))
    cg.add(var.set_mux_delay(config[CONF_MUX_DELAY]))
    