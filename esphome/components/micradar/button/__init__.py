import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    CONF_RESTART,
    DEVICE_CLASS_RESTART,
    ENTITY_CATEGORY_CONFIG,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_DATABASE,
    ICON_RESTART,
    ICON_RESTART_ALERT,
)

from .. import CONF_MICRADAR_ID, MicradarComponent, micradar_ns

QueryButton = micradar_ns.class_("QueryButton", button.Button)
RestartButton = micradar_ns.class_("RestartButton", button.Button)
InitProgressButton = micradar_ns.class_("InitProgressButton", button.Button)
HumanPresenceQueryButton = micradar_ns.class_("HumanPresenceQueryButton", button.Button)
TrackQueryButton = micradar_ns.class_("TrackQueryButton", button.Button)

CONF_QUERY_PARAMS = "query_params"
CONF_INITIALIZATION_PROGRESS_QUERY = "initialization_progress_query"
CONF_HUMAN_PRESENCE_QUERY = "human_presence_query"
CONF_TRACK_INFORMATION_QUERY = "track_information_query"




CONFIG_SCHEMA = {
    cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MicradarComponent),
    cv.Optional(CONF_RESTART): button.button_schema(
        RestartButton,
        device_class=DEVICE_CLASS_RESTART,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_RESTART,
    ),
    cv.Optional(CONF_QUERY_PARAMS): button.button_schema(
        QueryButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_DATABASE,
    ),
     cv.Optional(CONF_INITIALIZATION_PROGRESS_QUERY): button.button_schema(
        InitProgressButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_DATABASE,
    ),
    cv.Optional(CONF_HUMAN_PRESENCE_QUERY): button.button_schema(
        HumanPresenceQueryButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_DATABASE,
    ),
     cv.Optional(CONF_TRACK_INFORMATION_QUERY): button.button_schema(
        TrackQueryButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_DATABASE,
    ),
}


async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
    if restart_config := config.get(CONF_RESTART):
        b = await button.new_button(restart_config)
        await cg.register_parented(b, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_restart_button(b))
    if query_params_config := config.get(CONF_QUERY_PARAMS):
        b = await button.new_button(query_params_config)
        await cg.register_parented(b, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_query_button(b))
    if initialization_progress_query_config := config.get(CONF_INITIALIZATION_PROGRESS_QUERY):
        b = await button.new_button(initialization_progress_query_config)
        await cg.register_parented(b, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_init_progress_query_button(b))
    if human_presence_query_config := config.get(CONF_HUMAN_PRESENCE_QUERY):
        b = await button.new_button(human_presence_query_config)
        await cg.register_parented(b, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_human_query_button(b))
    if track_info_query_config := config.get(CONF_TRACK_INFORMATION_QUERY):
        b = await button.new_button(track_info_query_config)
        await cg.register_parented(b, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_track_query_button(b))