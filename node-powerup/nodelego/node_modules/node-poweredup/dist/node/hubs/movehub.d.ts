import { Peripheral } from "@abandonware/noble";
import { IBLEAbstraction } from "../interfaces";
import { LPF2Hub } from "./lpf2hub";
/**
 * The MoveHub is emitted if the discovered device is a Move Hub.
 * @class MoveHub
 * @extends LPF2Hub
 * @extends BaseHub
 */
export declare class MoveHub extends LPF2Hub {
    static IsMoveHub(peripheral: Peripheral): boolean;
    constructor(device: IBLEAbstraction);
    connect(): Promise<void>;
    protected _checkFirmware(version: string): void;
}
export declare const PortMap: {
    [portName: string]: number;
};
