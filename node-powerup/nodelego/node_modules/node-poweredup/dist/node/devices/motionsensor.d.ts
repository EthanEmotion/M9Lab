/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class MotionSensor
 * @extends Device
 */
export declare class MotionSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    DISTANCE = 0
}
export declare const ModeMap: {
    [event: string]: number;
};
