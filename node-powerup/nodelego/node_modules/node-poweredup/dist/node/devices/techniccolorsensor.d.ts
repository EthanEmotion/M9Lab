/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class TechnicColorSensor
 * @extends Device
 */
export declare class TechnicColorSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
    /**
     * Set the brightness (or turn on/off) of the lights around the sensor.
     * @method TechnicColorSensor#setBrightness
     * @param {number} firstSegment First light segment. 0-100 brightness.
     * @param {number} secondSegment Second light segment. 0-100 brightness.
     * @param {number} thirdSegment Third light segment. 0-100 brightness.
     * @returns {Promise} Resolved upon successful issuance of the command.
     */
    setBrightness(firstSegment: number, secondSegment: number, thirdSegment: number): void;
}
export declare enum Mode {
    COLOR = 0,
    REFLECTIVITY = 1,
    AMBIENT_LIGHT = 2
}
export declare const ModeMap: {
    [event: string]: number;
};
