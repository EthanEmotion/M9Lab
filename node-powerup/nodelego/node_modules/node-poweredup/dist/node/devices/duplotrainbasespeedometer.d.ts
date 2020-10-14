/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class DuploTraniBaseSpeedometer
 * @extends Device
 */
export declare class DuploTrainBaseSpeedometer extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    SPEED = 0
}
export declare const ModeMap: {
    [event: string]: number;
};
