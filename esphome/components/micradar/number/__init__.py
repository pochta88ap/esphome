import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    
    CONF_FREQUENCY,
    
    ENTITY_CATEGORY_CONFIG,
    
    ICON_TIMELAPSE,
   
    UNIT_HERTZ,
)

from .. import CONF_MICRADAR_ID, MicradarComponent, micradar_ns

TrackFrequencyNumber = micradar_ns.class_("TrackFrequencyNumber", number.Number)


CONF_MAX_MOVE_DISTANCE_GATE = "max_move_distance_gate"
CONF_MAX_STILL_DISTANCE_GATE = "max_still_distance_gate"
CONF_LIGHT_THRESHOLD = "light_threshold"

FREQUENCY_GROUP = "frequency"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(cg.EntityBase),
        cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MicradarComponent),
        cv.Inclusive(CONF_FREQUENCY, FREQUENCY_GROUP): number.number_schema(
            TrackFrequencyNumber,
            unit_of_measurement=UNIT_HERTZ,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_TIMELAPSE,
        ),
        
    }
)

async def to_code(config):
   micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
   if frequency_config := config.get(CONF_FREQUENCY):
        n = await number.new_number(
            frequency_config, min_value=0.01, max_value=50.0, step=0.01
        )
        await cg.register_parented(n, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_track_frequency_number(n))