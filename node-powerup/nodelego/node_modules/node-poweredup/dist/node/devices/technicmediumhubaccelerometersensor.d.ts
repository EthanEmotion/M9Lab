/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class TechnicMediumHubAccelerometerSensor
 * @extends Device
 */
export declare class TechnicMediumHubAccelerometerSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    ACCEL = 0
}
export declare const ModeMap: {
    [event: string]: number;
};
