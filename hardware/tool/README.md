# Tool

## Extruder parts

* [Direct drive extruder](https://www.3dprima.com/spare-parts-accessories/upgrades/hot-end-upgrades/primacreator-direct-drive-extruder-for-cr10-ender-3-series_27215_8676)
* [E3D Standard Heater Cartridge - 24V - 40W](https://www.3dprima.com/spare-parts-accessories/manufacturers/e3d/e3d-standard-heater-cartridge-24v-40w_28152_9637)
* [E3D Thermistor Cartridge](https://www.3dprima.com/spare-parts-accessories/manufacturer/e3d/e3d-thermistor-cartridge_28155_9640)
* [PrimaCreator MK8 brass nozzle 0.8 mm](https://www.3dprima.com/se/parts/spare-parts/general/mk8-brass-nozzle-0-8-x-1_22708_3747)

## Heater and temperature control

* [Controllino Mini](https://www.controllino.com/product/controllino-mini/)
* [Stepper motor driver]()


## Bracket

* [fdm_extruder_bracket.step](./fdm_extruder_bracket.step)

## Toolchanger

Attach after 30 degree adapter, see [30_degree_robot_tool_adapter.obj](30_degree_robot_tool_adapter.obj)

[Tool changer TC20-4 - Robot System Products](https://robotsystemproducts.com/product/tool-changer-tc20-4/)

# Tool defintion in RAPID

The tool center point (TCP) can be found using the 4 point calibration method. (`Jogging` > `Tool` > Select tool > `Edit` > `Define` on the FlexPendant).

The orientation needs to be considered as well. The bracket points the extruder in 90 degress and then there's the 30 degree adapter to account for as well.

Copy this into your tool definition (that should preferably be in the `user` module or your RAPID script from grasshopper)

```
TASK PERS tooldata *TOOLNAME*:=[TRUE,[[*X*, *Y*, *Z*],[0.866025,0,-0.5,0]],[2,[0,0,100],[1,0,0,0],1,1,1]];
```
The relevant part is the [quaternion]() `[0.866025,0,-0.5,0]`.
