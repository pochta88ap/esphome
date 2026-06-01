import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    
    CONF_MOVING_DISTANCE,
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_ILLUMINANCE,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_MOTION_SENSOR,
    ICON_SIGNAL,
    UNIT_CENTIMETER,
    UNIT_MILLIMETER,
    UNIT_METER_PER_SECOND,
    UNIT_PERCENT,
)
from . import CONF_MICRADAR_ID, MicradarComponent

DEPENDENCIES = ["micradar"]

CONF_MOVING_ENERGY = "moving_energy"
CONF_X_COORD = "x_coord"
CONF_Y_COORD = "y_coord"
CONF_DIST = "distance"
CONF_VEL = "velocity"
CONF_MOVE_ENERGY = "move_energy"
MAX_TARGETS = 3


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MicradarComponent),
        cv.Optional(CONF_MOVING_ENERGY): sensor.sensor_schema(
            filters=[{"throttle_with_priority": cv.TimePeriod(milliseconds=200)}],
            icon=ICON_MOTION_SENSOR,
            unit_of_measurement=UNIT_PERCENT,
        ),
    }
)

CONFIG_SCHEMA = CONFIG_SCHEMA.extend(
    {
        cv.Optional(f"target_{x}"): cv.Schema(
            {
                cv.Optional(CONF_MOVE_ENERGY): sensor.sensor_schema(
                    entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                    filters=[
                        {"throttle_with_priority": cv.TimePeriod(milliseconds=500)}
                    ],
                    icon=ICON_MOTION_SENSOR,
                    unit_of_measurement=UNIT_PERCENT,
                ),
                cv.Optional(CONF_X_COORD): sensor.sensor_schema(
                    entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                    filters=[
                        {"throttle_with_priority": cv.TimePeriod(milliseconds=500)}
                    ],
                    icon=ICON_MOTION_SENSOR,
                    unit_of_measurement=UNIT_MILLIMETER,
                ),
                cv.Optional(CONF_Y_COORD): sensor.sensor_schema(
                    entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                    filters=[
                        {"throttle_with_priority": cv.TimePeriod(milliseconds=500)}
                    ],
                    icon=ICON_MOTION_SENSOR,
                    unit_of_measurement=UNIT_MILLIMETER,
                ),
                cv.Optional(CONF_DIST): sensor.sensor_schema(
                    entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                    filters=[
                        {"throttle_with_priority": cv.TimePeriod(milliseconds=500)}
                    ],
                    icon=ICON_MOTION_SENSOR,
                    unit_of_measurement=UNIT_MILLIMETER,
                ),
                cv.Optional(CONF_VEL): sensor.sensor_schema(
                    entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                    filters=[
                        {"throttle_with_priority": cv.TimePeriod(milliseconds=500)}
                    ],
                    icon=ICON_MOTION_SENSOR,
                    unit_of_measurement=UNIT_METER_PER_SECOND,
                ),
            }
        )
        for x in range(MAX_TARGETS)

    }
)

async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
    if moving_energy_config := config.get(CONF_MOVING_ENERGY):
        sens = await sensor.new_sensor(moving_energy_config)
        cg.add(micradar_component.set_moving_target_energy_sensor(sens))
    for x in range(MAX_TARGETS):
        if target_conf := config.get(f"target_{x}"):
            if move_config := target_conf.get(CONF_MOVE_ENERGY):
                sens = await sensor.new_sensor(move_config)
                cg.add(micradar_component.set_move_energy_sensor(x, sens))
            if x_config := target_conf.get(CONF_X_COORD):
                sens = await sensor.new_sensor(x_config)
                cg.add(micradar_component.set_x_coord_sensor(x, sens))
            if y_config := target_conf.get(CONF_Y_COORD):
                sens = await sensor.new_sensor(y_config)
                cg.add(micradar_component.set_y_coord_sensor(x, sens))
            if dist_config := target_conf.get(CONF_DIST):
                sens = await sensor.new_sensor(dist_config)
                cg.add(micradar_component.set_dist_sensor(x, sens))
            if vel_config := target_conf.get(CONF_VEL):
                sens = await sensor.new_sensor(vel_config)
                cg.add(micradar_component.set_velocity_sensor(x, sens))
