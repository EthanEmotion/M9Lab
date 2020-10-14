/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class VoltageSensor
 * @extends Device
 */
export declare class VoltageSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    VOLTAGE = 0
}
export declare const ModeMap: {
    [event: string]: number;
};
