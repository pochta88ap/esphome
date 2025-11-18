import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_MAC_ADDRESS,
    CONF_VERSION,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_CHIP,
)

from . import CONF_MICRADAR_ID, MicradarComponent

DEPENDENCIES = ["micradar"]

CONF_PRODUCT_MODEL = "product_model"
CONF_PRODUCT_ID = "product_id"
CONF_HARDWARE_MODEL = "hardware_model"
CONF_FIRMWARE_VERSION = "firmware_version"
CONF_HUMAN_PRESENCE_INFORMATION = "HumanPresenceInformation"
CONF_NOVEMENT_INFORMATION = "MovementInformation"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MicradarComponent),
    cv.Optional(CONF_PRODUCT_MODEL): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
    ),
     cv.Optional(CONF_PRODUCT_ID): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
    ),
    cv.Optional(CONF_HARDWARE_MODEL): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
    ),
    cv.Optional(CONF_FIRMWARE_VERSION): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
    ),
    cv.Optional(CONF_HUMAN_PRESENCE_INFORMATION): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
    ),
     cv.Optional(CONF_NOVEMENT_INFORMATION): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
    ),
    
    
}

async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
    if product_model_config := config.get(CONF_PRODUCT_MODEL):
        sens = await text_sensor.new_text_sensor(product_model_config)
        cg.add(micradar_component.set_product_model_text_sensor(sens))
        
    if product_id_config := config.get(CONF_PRODUCT_ID):
        sens = await text_sensor.new_text_sensor(product_id_config)
        cg.add(micradar_component.set_product_id_text_sensor(sens))

    if hardware_model_config := config.get(CONF_HARDWARE_MODEL):
        sens = await text_sensor.new_text_sensor(hardware_model_config)
        cg.add(micradar_component.set_hardware_model_text_sensor(sens))

    if firmware_version_config := config.get(CONF_FIRMWARE_VERSION):
        sens = await text_sensor.new_text_sensor(firmware_version_config)
        cg.add(micradar_component.set_firmware_version_text_sensor(sens))

    if human_presence_information_config := config.get(CONF_FIRMWARE_VERSION):
        sens = await text_sensor.new_text_sensor(human_presence_information_config)
        cg.add(micradar_component.set_human_presence_information_text_sensor(sens))

    if movement_information_config := config.get(CONF_FIRMWARE_VERSION):
        sens = await text_sensor.new_text_sensor(movement_information_config)
        cg.add(micradar_component.set_movement_information_text_sensor(sens))