/// <reference types="node" />
import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class RemoteControlButton
 * @extends Device
 */
export declare class RemoteControlButton extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    receive(message: Buffer): void;
}
export declare enum Mode {
    BUTTON_EVENTS = 0
}
export declare const ModeMap: {
    [event: string]: number;
};
export declare const ButtonState: {
    [state: string]: number;
};
