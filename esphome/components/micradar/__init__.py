from esphome import automation
from esphome.automation import maybe_simple_id
import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

AUTO_LOAD = ["ld24xx"]
DEPENDENCIES = ["uart"]
CODEOWNERS = ["@pochta88.ap"]
MULTI_CONF = True

micradar_ns = cg.esphome_ns.namespace("micradar")
MicradarComponent = micradar_ns.class_("MicradarComponent", cg.Component, uart.UARTDevice)

CONF_MICRADAR_ID = "micradar_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(MicradarComponent),
        
    }
)

CONFIG_SCHEMA = cv.All(
    CONFIG_SCHEMA.extend(uart.UART_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "micradar",
    require_tx=True,
    require_rx=True,
    parity="NONE",
    stop_bits=1,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)