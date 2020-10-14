/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class CurrentSensor
 * @extends Device
 */
export declare class CurrentSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    CURRENT = 0
}
export declare const ModeMap: {
    [event: string]: number;
};
