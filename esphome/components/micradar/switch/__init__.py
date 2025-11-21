import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_SWITCH,
    ENTITY_CATEGORY_CONFIG,
    ICON_PULSE,
)

from .. import CONF_MICRADAR_ID, MicradarComponent, micradar_ns

HumanPresenceFunctionSwitch = micradar_ns.class_("HumanPresenceFunctionSwitch", switch.Switch)

CONF_HUMANPRESENCEFUNCTION = "human_presence_function"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MicradarComponent),
    cv.Optional(CONF_HUMANPRESENCEFUNCTION): switch.switch_schema(
        HumanPresenceFunctionSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon=ICON_PULSE,
    )
}

async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
    if human_presence_function_config := config.get(CONF_HUMANPRESENCEFUNCTION):
        s = await switch.new_switch(human_presence_function_config)
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_human_presence_function_switch(s))
