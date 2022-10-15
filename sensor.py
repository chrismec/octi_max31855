import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, spi
from esphome.const import (
    CONF_ID,
    CONF_REFERENCE_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_DEGREES,
)

from . import octo_max31855_ns, OCTO_MAX31855Component

DEPENDENCIES = ["octo_max31855"]

OCTO_MAX31855Sensor = octo_max31855_ns.class_(
    "OCTO_MAX31855Sensor",
    sensor.Sensor,
    cg.PollingComponent,
    spi.SPIDevice
)

CONF_OCTO_MAX31855_ID = "octo_max31855_id"

CONF_CHANNEL = "channel"

def validate_channel(value):
    if isinstance(value, int):
        if (value >= 0) and (value <= 7):
            return value
        else:
            raise cv.Invalid(f'invalid channel "{value}"')


CONFIG_SCHEMA = (
    sensor.sensor_schema(
        OCTO_MAX31855Sensor,
        unit_of_measurement=UNIT_DEGREES,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_TEMPERATURE,
    )
    .extend(
        {
            cv.GenerateID(CONF_OCTO_MAX31855_ID): cv.use_id(OCTO_MAX31855Component),
            cv.Required(CONF_CHANNEL): validate_channel,
            cv.Optional(CONF_REFERENCE_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_DEGREES,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(spi.spi_device_schema())
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OCTO_MAX31855_ID])

    var = await sensor.new_sensor(config, parent)
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)
    cg.add(var.set_channel(config[CONF_CHANNEL]))
    
    if CONF_REFERENCE_TEMPERATURE in config:
        tc_ref = await sensor.new_sensor(config[CONF_REFERENCE_TEMPERATURE])
        cg.add(var.set_reference_sensor(tc_ref))
