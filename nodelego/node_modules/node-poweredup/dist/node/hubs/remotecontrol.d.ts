import { Peripheral } from "@abandonware/noble";
import { IBLEAbstraction } from "../interfaces";
import { LPF2Hub } from "./lpf2hub";
/**
 * The RemoteControl is emitted if the discovered device is a Remote Control.
 * @class RemoteControl
 * @extends LPF2Hub
 * @extends BaseHub
 */
export declare class RemoteControl extends LPF2Hub {
    static IsRemoteControl(peripheral: Peripheral): boolean;
    constructor(device: IBLEAbstraction);
    connect(): Promise<void>;
}
export declare const PortMap: {
    [portName: string]: number;
};
