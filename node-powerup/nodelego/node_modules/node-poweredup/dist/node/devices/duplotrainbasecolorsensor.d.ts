/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class DuploTrainBaseColorSensor
 * @extends Device
 */
export declare class DuploTrainBaseColorSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    COLOR = 0,
    REFLECTIVITY = 2,
    RGB = 3
}
export declare const ModeMap: {
    [event: string]: number;
};
