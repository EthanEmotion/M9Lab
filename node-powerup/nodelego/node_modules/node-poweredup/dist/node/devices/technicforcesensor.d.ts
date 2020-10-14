/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class TechnicForceSensor
 * @extends Device
 */
export declare class TechnicForceSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    FORCE = 0,
    TOUCHED = 1,
    TAPPED = 2
}
export declare const ModeMap: {
    [event: string]: number;
};
