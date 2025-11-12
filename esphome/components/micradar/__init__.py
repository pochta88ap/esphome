from esphome import automation
from esphome.automation import maybe_simple_id
import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PASSWORD, CONF_THROTTLE, CONF_TIMEOUT

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@pochta88.ap"]
MULTI_CONF = True

micradar_ns = cg.esphome_ns.namespace("micradar")
micradarComponent = micradar_ns.class_("MicradarComponent", cg.Component, uart.UARTDevice)

CONF_MICRADAR_ID = "micradar_id"