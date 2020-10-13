/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class MoveHubTiltSensor
 * @extends Device
 */
export declare class MoveHubTiltSensor extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    TILT = 0
}
export declare const ModeMap: {
    [event: string]: number;
};
