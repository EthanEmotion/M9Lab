/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class TechnicMediumHubGyroSensor
 * @extends Device
 */
export declare class TechnicMediumHubGyroSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    GYRO = 0
}
export declare const ModeMap: {
    [event: string]: number;
};
