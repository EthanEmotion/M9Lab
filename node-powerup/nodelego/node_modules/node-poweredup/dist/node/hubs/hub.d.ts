import { Peripheral } from "@abandonware/noble";
import { IBLEAbstraction } from "../interfaces";
import { LPF2Hub } from "./lpf2hub";
/**
 * The Hub is emitted if the discovered device is a Hub.
 * @class Hub
 * @extends LPF2Hub
 * @extends BaseHub
 */
export declare class Hub extends LPF2Hub {
    static IsHub(peripheral: Peripheral): boolean;
    protected _currentPort: number;
    constructor(device: IBLEAbstraction);
    connect(): Promise<void>;
    protected _checkFirmware(version: string): void;
}
export declare const PortMap: {
    [portName: string]: number;
};
