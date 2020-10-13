/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class TechnicDistanceSensor
 * @extends Device
 */
export declare class TechnicDistanceSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
    /**
     * Set the brightness (or turn on/off) of the lights around the eyes.
     * @method TechnicDistanceSensor#setBrightness
     * @param {number} topLeft Top left quadrant (above left eye). 0-100 brightness.
     * @param {number} bottomLeft Bottom left quadrant (below left eye). 0-100 brightness.
     * @param {number} topRight Top right quadrant (above right eye). 0-100 brightness.
     * @param {number} bottomRight Bottom right quadrant (below right eye). 0-100 brightness.
     * @returns {Promise} Resolved upon successful issuance of the command.
     */
    setBrightness(topLeft: number, bottomLeft: number, topRight: number, bottomRight: number): void;
}
export declare enum Mode {
    DISTANCE = 0,
    FAST_DISTANCE = 1
}
export declare const ModeMap: {
    [event: string]: number;
};
